#include "updater.h"
#include "can.h"
#include "string.h"
#include "timer.h"

unsigned char device;
unsigned char device_code;
char upBuffer[BLOCK_SIZE];
char UPDATER_Busy = 0;
u8 can_buf[8];

void sendchar(char c)
{
  //USB_SendChar(c);
  RS232_SendChar(c);
}

char recchar(void)
{
	int c;
	
	do
	{
		c = com_getchar1();
	}while(c == -1);
  //return USB_RecChar();
  
	return (char)c;
}

//------------------------------------------------------------------------------
// UPDATER_BusRun - start bus update procedure
//------------------------------------------------------------------------------
//char tmp_flag = 0;
//int tmp_cnt = 0;
//char tmp_buf[270];
void UPDATER_BusRun(int device)
{
	char data;
	char b[3]={0,0,0};
	char res[3]={0x75, 0x55, 'R'};
	char reset = 0;
	int c;
	char cbip = 0;
	
	UPDATER_Busy = 1;
	
	switch (device)
	{
		case DEVICE_ID_DTAS:
			//RS485_SendPackage(RS485_ADDR_SAMPLER, cmdUpdate, 0, NULL, true);
			break;
		case DEVICE_ID_PRINTER:
			//RS485_SendPackage(RS485_ADDR_PRNRTC, cmdUpdate, 0, NULL, true);
			break;
		case DEVICE_ID_TFT:
			// USART1 波特率设置,升级程序时的波特率由9600升高到115200
			com_baudrate(USART1, UART1_BAUD2, NO_FLOW_CONTROL);
			CAN_ITConfig(CAN1,CAN_IT_FMP0,DISABLE); 										// FIFO0消息挂号中断禁止
			break;
		default:
			UPDATER_Busy = 0;
			return;
	}
	
	//while (RS485_GetStatus()!=RS485_STATUS_WAITING)
	//  continue;  
	
	//__disable_irq(); 											// 关闭总中断
	
	//RS485_Stop();
	//read byte from USB and transmit it to RS485, leave this mode after 5 seconds timeout
	cbip = 0;
	for (;;)
	{
		c = com_getchar1();
		if(c != -1)
		{
			data=(char)c;
			b[0]=b[1];
			b[1]=b[2];
			b[2]=data;
			if (!strncmp(b, res, 3))
				reset = 1;
			
			can_buf[cbip] = data;
			cbip++;
			upd_rxtover = 3;
		}
		
		if(upd_rxtover == 0)
		{
			if(cbip != 0)
			{
				cbip = 0;
				CAN1_SenDat(can_buf,8);
			}
		}
		else
		if(cbip >= 8)
		{
			cbip = 0;
			CAN1_SenDat(can_buf,8);
		}
		
		if(CAN1_Receive_Msg(can_buf)==1)
		{
			data = can_buf[0];
			sendchar(data);
			if (reset)
				break;
		}
	}
	
	//RS485_Init();
	UPDATER_Busy = 0;
	NVIC_SystemReset();
	
	//__enable_irq(); 											// 开启总中断
}

//TBool UPDATER_GetBusy(void)
//{
//  return UPDATER_Busy;
//}
