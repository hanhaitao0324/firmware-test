#include "platform.h"
#include "global.h"
#include "pins.h"
#include "rs232.h"
#include "timer.h"

__flash TWord RS232_BAUDRATE[DEVICE_BAUD_LAST]={1200, 9600}; 					// 串口波特率

// ZT120串口变量----------------------------------
char RS232M_IBuffer[RS232M_BUFFER_I_SIZE]; 									// 串口接收缓存，40
//char RS232M_OBuffer[RS232M_BUFFER_O_SIZE]; 									// 串口发送缓存，16
//TByte RS232M_IHead; 																				// 接收堆栈头
//volatile TByte RS232M_OHead, RS232M_OTail; 									// 发送栈头，栈尾

// DHC1520串口变量--------------------------------
char RS232H_IBuffer[RS232H_BUFFER_SIZE]; 										// 串口接收缓存，32
//TByte RS232H_IHead;

//------------------------------------------------------------------------------
// RS232_ResetRx - reset input buffer
//------------------------------------------------------------------------------
/*void RS232_ResetRx(void)
{
  globalDeviceStatus.gotRS232Package=false; 								// 收到完整帧标志复位
  RS232_IHead=0; 																						// 栈头复位
}*/

//------------------------------------------------------------------------------
// RS232_PutString - put a string to RS-232
//------------------------------------------------------------------------------
void RS232M_PutString(char *buffer)
{
  //RS232_OHead=0;
  //RS232_OTail=0;
  TByte i=0;
  
  while(*buffer)
  {
    RS232H_IBuffer[i] = *buffer++;
    i++;
  }
  
  globalDeviceStatus.gotRS232HPackage=true;
}

//------------------------------------------------------------------------------
// RS232_PutString - put a string to RS-232
//------------------------------------------------------------------------------
void RS232H_PutString(char *buffer)
{
  TByte i=0;
  
  while(*buffer)
  {
  	RS232M_IBuffer[i] = *buffer++;
    i++;
  }
  
  globalDeviceStatus.gotRS232MPackage=true;
}

//------------------------------------------------------------------------------
// RS232_WaitReply - wait for reply on RS232（等待RS232回复）
//------------------------------------------------------------------------------
/*TBool RS232_WaitReply(void)
{
	RS232_ResetRx();
	TIMER_Start(TIMER_RS232, TO_RS232);
	
	while((!globalDeviceStatus.gotRS232Package)&&(!TIMER_TimeOut(TIMER_RS232)))
		continue;
	
	TIMER_Stop(TIMER_RS232);
	
	if(globalDeviceStatus.gotRS232Package)
		return true;
	
	return false;        
}*/

//------------------------------------------------------------------------------
// RS232_SendChar - send a char to RS-232               // HHT 240613   加上串口发送的函数
//------------------------------------------------------------------------------
void RS232_SendChar(char data)
{
  macBitOn(UCSR0A,TXC0);
  //put data into UART buffer, sends the data
  UDR0=data;
  //wait for transmition to end
  while (!(UCSR0A&(1<<TXC0)))
    continue;                  
}
