#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "stmflash.h"
//#include "iap.h"
#include "can.h"
#include "programm.h"

int i, j, crc16;
char buffer[BLOCK_SIZE];
u8 can_buf[8];

unsigned char val, val2, val3, status;
//u8 t;
//u16 oldcount = 0; 										// 老的串口接收数据值
u32 applenth = 0; 										// 接收到的app代码长度
//u8 clearflag = 0;
unsigned int * ptr;
unsigned int address, size, dat_cnt;
unsigned int crc32;


int main(void)
{
	
	HAL_Init(); 													// 初始化HAL库   
	Stm32_Clock_Init(360,25,2,8); 				// 设置时钟,180Mhz
	delay_init(180); 											// 初始化延时函数
	uart_init(115200); 										// 初始化USART
	LED_Init(); 													// 初始化LED
	SDRAM_Init(); 												// 初始化SDRAM
	LCD_Init(); 													// 初始化LCD
	
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_8TQ,6,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	
	ptr =(unsigned int *)RAM_START_ADDR;
	if(!(*ptr == BOOTLOADER_KEY1 && *(ptr+1) == BOOTLOADER_KEY2))
	{
		if (PROGRAMM_CodeChecksum() )
		{
			PROGRAMM_ExecuteCode();
		}
	}
	
	POINT_COLOR = BLUE; //RED;
	//LCD_ShowString(30,50,200,24,24,"Apollo STM32F4/F7");
	LCD_ShowString(30,75,200,24,24,"FLASH UPDATA TEST");
	//LCD_ShowString(30,100,200,24,24,"ATOM@ALIENTEK");
	LCD_ShowString(30,125,200,24,24,"2020/11/12");
	//LCD_ShowString(30,130,200,16,16,"KEY_UP:Copy APP2FLASH");
	//LCD_ShowString(30,150,200,16,16,"KEY1:Erase SRAM APP");
	//LCD_ShowString(30,170,200,16,16,"KEY0:Run SRAM APP");
	//LCD_ShowString(30,190,200,16,16,"KEY2:Run FLASH APP");
	
	//显示提示信息
	POINT_COLOR = RED; //BLUE; 									// 设置字体为蓝色
	
	while(1)
	{
		// UpData ----------------------------------------------
		if(CAN1_RecDat() != COMM_CONF1) 		// 0x75,'u'
			continue;
		
		if(CAN1_RecDat() != COMM_CONF2) 		// 0x55,'U'
			continue;
		
		val = CAN1_RecDat();
		
		if(val == CMD_ERASE) 								// Chip erase, 0x30, '0'
		{
			LCD_ShowString(30,200,200,24,24,"uU0");
			PROGRAMM_EraseFlash();
			CAN1_SenDat(ANSW_OK); 						// Send OK back., 0x21,'!'
			applenth = 0;
			dat_cnt = 0;
		}
		else
		if(val == CMD_WRITE) 								// Start block load. 0x31,'1'
		{
			LCD_ShowString(120,200,200,24,24,"uU1");
			crc16 = 0;
			
			val = CAN1_RecDat();
			val2 = CAN1_RecDat();
			val3 = CAN1_RecDat();
			address = ((unsigned int)val<<16) | ((unsigned int)val2<<8) | (unsigned int)val3; // Get address
			//LCD_ShowxNum(220,200,address,6,24,0);
			
			val = CAN1_RecDat();
			val2 = CAN1_RecDat();
			size = ((unsigned int)val<<8) | (unsigned int)val2; // Get block size.
			dat_cnt += size;
			//LCD_ShowxNum(320,200,dat_cnt,6,24,0);
			
			status = CAN1_RecDat(); 								// Get memtype
			crc16 = status + (size>>8) + (size&0xff) + (address>>16) + ((address>>8)&0xff) + (address&0xff);
			
			for(i=0;i<size;i++)
			{
				buffer[i] = CAN1_RecDat();
				crc16 += buffer[i];
			}
			
			val = CAN1_RecDat();
			val2 = CAN1_RecDat();
			i = (unsigned int)val | ((unsigned int)val2 << 8);
			crc16 &= 0x0000ffff;
			if(i != crc16)
			{
				status = ANSW_ERROR; 						// 0x2D,'-'
			}
			else
			{
				if(status == 'F')
				{
					address += USER_FLASH_ADD;
					if (address<USER_FLASH_START)
					{
						status = ANSW_OK;
					}
					else
					{
						//write flash
						status = PROGRAMM_WriteFlash(buffer, (void*)address, size);
						if (status == CMD_SUCCESS)
						{
							status = ANSW_OK;
							applenth += size;
							LCD_ShowxNum(420,200,applenth,6,24,0);
						}
						else
							status = ANSW_ERROR;
					}
				}
			}
			
			CAN1_SenDat(status);
		}
		else
		if(val == CMD_VER) 									// Return software version.,0x56,'V'
		{
			LCD_ShowString(30,225,200,24,24,"uUV");
			CAN1_SenDat('0');
			CAN1_SenDat('1');
		}
		else
		if(val==CMD_ID) 										// Return signature bytes.,0x49,'I'
		{
			LCD_ShowString(30,250,200,24,24,"uUI");
			CAN1_SenDat(SIGNATURE_BYTE_1);
			CAN1_SenDat(SIGNATURE_BYTE_2);
			CAN1_SenDat(SIGNATURE_BYTE_3);
		}
		else
		if(val==CMD_DEV) 										// Return device signature,0x44,'D'
		{
			LCD_ShowString(30,275,200,24,24,"uUD");
			CAN1_SenDat(DEVICE_ID);
		}
		else
		if(val==CMD_RESET) 									// Reset controller, 0x52,'R'
		{
			LCD_ShowString(30,300,200,24,24,"uUR");
			CAN1_SenDat(ANSW_OK); 						// Send OK back., 0x21,'!'
			delay_ms(1000);
			
			//calculate checksum and write it to flash
			address = CHECKSUM_START;
			crc32 = PROGRAMM_CalcChecksum();
			status = PROGRAMM_WriteFlash((void*)&crc32, (void*)address, sizeof(unsigned int));
			
			PROGRAMM_ExecuteCode();
		}
		else
		if(val==CMD_STOP) 									// stop bootloader, execute user code, 0x53,'S'
		{
			LCD_ShowString(30,325,200,24,24,"uUS");
			PROGRAMM_ExecuteCode();
		}
		else 																// Unknown command, 0x3F,'?'
		{
			LCD_ShowString(30,350,200,24,24,"ANSW_UNKNOWN");
			CAN1_SenDat(ANSW_UNKNOWN);
		}
	}
}
