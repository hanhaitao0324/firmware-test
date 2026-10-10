#include "const.h"
#include "global.h"
#include "pins.h"
#include "platform.h"
#include "rs232.h"
#include "timer.h"

TWord RS232_BAUDRATE[DEVICE_BAUD_LAST]={1200, 9600};

char RS232_IBuffer[RS232_BUFFER_SIZE];
//char rs232OBuffer[RS232_BUFFER_SIZE];
TByte RS232_IHead;
//volatile TByte rs232OHead,rs232OTail;

//------------------------------------------------------------------------------
// RS232_ResetRx - reset input buffer（复位输入缓存）
//------------------------------------------------------------------------------
void RS232_ResetRx(void)
{
  globalDeviceStatus.gotRS232Package=false;
  RS232_IHead=0;
}
  
//------------------------------------------------------------------------------
// RS232_SetRx - set receiving（设置接收使能，act=0禁止，1使能）
//------------------------------------------------------------------------------
void RS232_SetRx(TBool act)
{
  if (act)
    UCSRB|=(1<<RXEN);
  else
    UCSRB&=~(1<<RXEN);
}

//------------------------------------------------------------------------------
// RS232_Init - init external connection（初始化）
//------------------------------------------------------------------------------
void RS232_Init(TByte Baudrate)
{
  //baud rate
  UBRRH=0;//((RS_BaudRate(RS232_BAUDRATE[Baudrate])>>8)&0x7F);
  if (Baudrate<DEVICE_BAUD_LAST)
    UBRRL=(TByte)RS_BaudRate(RS232_BAUDRATE[Baudrate]);
  else
    UBRRL=(TByte)RS_BaudRate(RS232_BAUDRATE[DEVICE_BAUD_1200]);
  //enable Rx/Tx
  UCSRB=((1<<RXEN)|(1<<TXEN)|(1<<RXCIE));//|(1<<TXCIE));
  //set frame format: 8data, 1stop bit
  UCSRC=((1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0));
}

//------------------------------------------------------------------------------
// RxD_IntHandler - RxD interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART_RXC_vect
__interrupt void RxD_IntHandler(void)
{                       
  TByte data;
    
  //check for errors
  if (UCSRA&((1<<FE)|(1<<DOR)))
  {
    UDR;
    return;
  }
  //get byte
  data=UDR;
  if (data=='\n') 						// LF，换行
    return;
  if (data==0x1B) 						// ESC，返回键
  {
    RS232_IHead=0;
    return;
  }
  if (data==0x08) 						// BS，退格键
  {
    if (RS232_IHead)
      RS232_IHead--;
    return;
  }
  RS232_IBuffer[RS232_IHead++]=data;
  if (RS232_IHead>=RS232_BUFFER_SIZE)
    RS232_IHead=0;
  if (data=='\r') 						// CR，回车键
  {
    globalDeviceStatus.gotRS232Package=true;
    RS232_IHead=0;
  }
}

//------------------------------------------------------------------------------
// TxD_IntHandler - TxD interrupt handler
//------------------------------------------------------------------------------
/*#pragma vector=USART_TXC_vect
__interrupt void TxD_IntHandler(void)
{
  if (rs232OHead==rs232OTail)
  {
    rs232OHead=rs232OTail=0;
    return;
  }
  UDR=rs232OBuffer[rs232OTail++];
  if (rs232OTail>=RS232_BUFFER_SIZE)
    rs232OTail=0;
}*/

//------------------------------------------------------------------------------
// RS232_PutByte - put a byte to RS-232
//------------------------------------------------------------------------------
/*void RS232_PutByte(TByte data)
{                
  if (rs232OHead==0)
  {
    //wait for empty transmit buffer
    while (!(UCSRA&(1<<UDRE)))
      continue;
    //put data into UART buffer, sends the data
    UDR=data;
    rs232OHead=rs232OTail=1;
  } else
  {
    //output buffer is not empty
    //put byte to a waiting buffer
    rs232OBuffer[rs232OHead++]=data;
    if (rs232OHead>=RS232_BUFFER_SIZE)
    {
      while (rs232OHead&&rs232OTail);
      rs232OHead=0;
    }
  }
}*/
void RS232_PutByte(char c)
{
  //wait for empty transmit buffer
  while (!(UCSRA&(1<<UDRE)))
    continue;
  //put data into UART buffer, sends the data
  UDR=c;
}

//------------------------------------------------------------------------------
// RS232_PutString - put a string to RS-232
//------------------------------------------------------------------------------
void RS232_PutString(char *buffer)
{
  while(*buffer)
    RS232_PutByte(*buffer++);
}

int putchar(int c)
{
  RS232_PutByte(c);
  return 0;
}
