#include "platform.h"
#include "global.h"
#include "pins.h"
#include "rs232.h"
#include "timer.h"

__flash TWord RS232_BAUDRATE[DEVICE_BAUD_LAST]={1200, 9600};

char RS232_IBuffer[RS232_BUFFER_I_SIZE];
char RS232_OBuffer[RS232_BUFFER_O_SIZE];
TByte RS232_IHead;
volatile TByte RS232_OHead, RS232_OTail;

//------------------------------------------------------------------------------
// RS232_ResetRx - reset input buffer
//------------------------------------------------------------------------------
void RS232_ResetRx(void)
{
  globalDeviceStatus.gotRS232Package=false;
  RS232_IHead=0;
}
  
//------------------------------------------------------------------------------
// RS232_Init - init external connection
//------------------------------------------------------------------------------
void RS232_Init(TByte Baudrate)
{
  //baud rate
  if (Baudrate<DEVICE_BAUD_LAST)
  {  
    UBRR0H=((RS_BaudRate(RS232_BAUDRATE[Baudrate])>>8)&0x7F);
    UBRR0L=(TByte)RS_BaudRate(RS232_BAUDRATE[Baudrate]);
  } else
  {  
    UBRR0H=((RS_BaudRate(RS232_BAUDRATE[DEVICE_BAUD_1200])>>8)&0x7F);
    UBRR0L=(TByte)RS_BaudRate(RS232_BAUDRATE[DEVICE_BAUD_1200]);
  }
  
  //UBRR0H=((RS_BaudRate(RS232_BAUDRATE)>>8)&0x7F);
  //UBRR0L=(TByte)RS_BaudRate(RS232_BAUDRATE);
  //enable Rx/Tx
  UCSR0B=((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0));//|(1<<TXCIE0);
  if (Baudrate==DEVICE_BAUD_1200)
    UCSR0B|=(1<<TXCIE0);
  //set frame format: 8data, 1stop bit
  UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);
  
  RS232_IHead=0;
}

//------------------------------------------------------------------------------
// RxD0_IntHandler - RxD0 interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART0_RXC_vect
__interrupt void RxD0_IntHandler(void)
{                       
  TByte data;
    
  //check for errors
  if (UCSR0A&((1<<FE0)|(1<<DOR0)))
  {
    UDR0;
    return;
  }
  //get byte
  data=UDR0;
  if (data==0x1b)
  {
    RS232_IHead=0;
    return;
  }
  if (data==0x08)
  {
    if (RS232_IHead)
      RS232_IHead--;
    return;
  }
  RS232_IBuffer[RS232_IHead++]=data;
  if (RS232_IHead>=RS232_BUFFER_I_SIZE)
    RS232_IHead=0;
  if (data=='\r') 
  {
    RS232_IBuffer[RS232_IHead]=0;
    globalDeviceStatus.gotRS232Package=true;
    RS232_IHead=0;
  }
}

//------------------------------------------------------------------------------
// TxD0_IntHandler - RxD0 interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART0_TXC_vect
__interrupt void TxD0_IntHandler(void)
{
  TByte i=RS232_OTail;
  
  if (RS232_OHead>=i)
  {
    RS232_OHead=RS232_OTail=0;
    return;
  }
  UDR0=RS232_OBuffer[RS232_OHead++];
  /*if (rs232OHead>=RS232_BUFFER_O_SIZE)
    rs232OHead=0;*/
}

//------------------------------------------------------------------------------
// RS232_SendChar - send a char to RS-232
//------------------------------------------------------------------------------
void RS232_SendChar(char data)
{                
  /*if (rs232OHead==0)
  {
    //wait for empty transmit buffer
    while (!(UCSR0A&(1<<UDRE0)))
      continue;
    //put data into UART buffer, sends the data
    UDR0=data;
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
  }*/
  macBitOn(UCSR0A,TXC0);
  //put data into UART buffer, sends the data
  UDR0=data;
  //wait for transmition to end
  while (!(UCSR0A&(1<<TXC0)))
    continue;                  
}

//------------------------------------------------------------------------------
// RS232_PutString - put a string to RS-232
//------------------------------------------------------------------------------
void RS232_PutString(char *buffer)
{
  RS232_OHead=0;
  RS232_OTail=0;
  while(*buffer)
  {
    /*//TIMER_Start(TIMER_RS232, 1);//TO_RS232BYTE);
    //while (!TIMER_TimeOut(TIMER_RS232)) continue;
    //TIMER_Stop(TIMER_RS232);*/
    //RS232_PutByte(*buffer++);
    RS232_OBuffer[RS232_OTail++]=*buffer++;
  }
  UDR0=RS232_OBuffer[RS232_OHead++];
}

//------------------------------------------------------------------------------
// RS232_WaitReply - wait for reply on RS232
//------------------------------------------------------------------------------
TBool RS232_WaitReply(void)
{
  RS232_ResetRx();
  TIMER_Start(TIMER_RS232, TO_RS232);
  while((!globalDeviceStatus.gotRS232Package)&&(!TIMER_TimeOut(TIMER_RS232)))
    continue;
  TIMER_Stop(TIMER_RS232);
  if(globalDeviceStatus.gotRS232Package)
    return true;
  return false;        
}
