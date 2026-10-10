#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "rtc.h"
#include "INP.h"
#include "motor.h"
#include "timer.h"
#include "can.h"
#include "programm.h"



int i, crc16;
char buffer[BLOCK_SIZE];

extern void execute_user_code(void);
extern void enter_usb_isp(void);

extern u8  TIM9CH1_CAPTURE_STA;					// 输入捕获状态
extern u16 TIM9CH1_CAPTURE_VAL;					// 输入捕获值

extern u16 MotExpCapVal;
extern u16 MotorPwmVal;
extern u8 MotSw;

#define SCB_AIRCR (*(volatile unsigned long *)0xE000ED0C) 	// Reset control Address Register
#define SCB_RESET_VALUE 0x05FA0004 													// Reset value, write to SCB_AIRCR can reset cpu

void Reset_Cpu(void)
{
	unsigned int * ptr;
	
	// 清除内存中软件更新的标记 ----------
	ptr =(unsigned int *)RAM_START_ADDR;
	*ptr = 0;
	*(ptr+1) = 0;
	
	// 软件重启 --------------------------
	SCB_AIRCR = SCB_RESET_VALUE;
}

//下面主函数使用位带操作实现：
int main(void)
{
	unsigned char val, val2, status;
  unsigned int * ptr;
  unsigned int address, size;
  unsigned int crc32;
	
#ifdef DEBUG
	PROGRAMM_ExecuteCode();
#endif
	
 	ptr =(unsigned int *)RAM_START_ADDR; 						// 软件更新内存标记指针
	if(!(*ptr == BOOTLOADER_KEY1 && *(ptr+1) == BOOTLOADER_KEY2)) 	// 判断是否是来自应用程序的软件更新
	{
		if (PROGRAMM_CodeChecksum() ) 								// 如果启动不是来自应用，则判断应用程序区的校验码是否与保存的数值相同
		{
			PROGRAMM_ExecuteCode();
		}
	}

	//__disable_irq() ; 										// 关闭总中断
	HAL_Init(); 													// 初始化HAL库
	Stm32_Clock_Init(336,8,2,7); 					// 设置时钟,168Mhz
	delay_init(168); 											// 初始化延时函数
	uart_init(115200); 										// 初始化USART
	LED_Init(); 													// 初始化LED	
	TIM4_Init(5000-1,8400-1);
	//__enable_irq(); 											// 开启总中断
	
	while(1)
	{
		// UpData ----------------------------------------------
		if(SERIAL_GetChar() != COMM_CONF1)
			continue;
		
		if(SERIAL_GetChar() != COMM_CONF2) 
			continue;
		
		val = SERIAL_GetChar(); 						// Wait for command character.
    
    // Check echo
    //if(val==CMD_ECHO)
    //{
    //  SERIAL_SendChar(ANSW_ECHO);
    //}
		
    if(val==CMD_ERASE) 									// Chip erase
    {
      PROGRAMM_EraseFlash();
      SERIAL_SendChar(ANSW_OK); 				// Send OK back.
    }
		else 
    if(val==CMD_WRITE) 									// Start block load. 0x31
    {
      crc16 = 0;
      val = SERIAL_GetChar();
      val2 = SERIAL_GetChar();
      address = ((unsigned int)val<<16) | ((unsigned int)val2<<8) | (unsigned int)SERIAL_GetChar(); // Get address
      val = SERIAL_GetChar();
      size = ((unsigned int)val<<8) | (unsigned int)SERIAL_GetChar(); // Get block size.
      val = SERIAL_GetChar(); // Get memtype
      crc16 = val + (size>>8) + (size&0xff) + (address>>16) + ((address>>8)&0xff) + (address&0xff);
      //address <<= 1;

      for (i=0; i<size; i++)
      {
        buffer[i] = SERIAL_GetChar();
        crc16 += buffer[i];
      }

      status = SERIAL_GetChar();
      i = (unsigned int)status | ((unsigned int)SERIAL_GetChar() << 8);

      if (i!=crc16)
      {
        status = ANSW_ERROR;
      }
      else
      {  
  	    if (val=='F')
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
            if (status==CMD_SUCCESS)
              status = ANSW_OK;
            else
              status = ANSW_ERROR;
          }
        }
      }

      SERIAL_SendChar(status);
    }
		else 
		if(val==CMD_VER) 										// Return software version.
    {
      SERIAL_SendChar('0');
      SERIAL_SendChar('1');
    }
		else 
		if(val==CMD_ID) 										// Return signature bytes.
    {							
      SERIAL_SendChar(SIGNATURE_BYTE_1);
      SERIAL_SendChar(SIGNATURE_BYTE_2);
      SERIAL_SendChar(SIGNATURE_BYTE_3);
    }
		else 
		if(val==CMD_DEV) 										// Return device signature
    {
      SERIAL_SendChar(DEVICE_ID);
    }
		else 
		if(val==CMD_RESET) 									// Reset controller
    {
      //calculate checksum and write it to flash
      address = CHECKSUM_START;
      crc32 = PROGRAMM_CalcChecksum();
      status = PROGRAMM_WriteFlash((void*)&crc32, (void*)address, sizeof(unsigned int));

      //PROGRAMM_ExecuteCode();
      Reset_Cpu();
    }
    else 
    if(val==CMD_STOP) 									// stop bootloader, execute user code
    {
      //PROGRAMM_ExecuteCode();
      Reset_Cpu();
    }
		else 																// Unknown command
    {
      SERIAL_SendChar(ANSW_UNKNOWN);
		}
	  
  }
}

