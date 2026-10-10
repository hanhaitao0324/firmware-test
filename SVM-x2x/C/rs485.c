#include "global.h"
#include "pins.h"
#include "platform.h"
#include "rs485.h"
#include "timer.h"

//I/O buffers
TByte iBuffer[RS485_BUFFER_I_SIZE];
TByte oBuffer[RS485_BUFFER_O_SIZE];

TByte RS485_Status;
//device address
TByte RS485_Address;
volatile TByte RS485_OHead, RS485_OTail;

TWord RS485_Errors=0;

//------------------------------------------------------------------------------
// RS485_SetStatus - set bus status
//------------------------------------------------------------------------------
void RS485_SetStatus(TByte status)
{
  RS485_Status = status;  
}

//------------------------------------------------------------------------------
// RS485_GetStatus - get bus status
//------------------------------------------------------------------------------
TByte RS485_GetStatus(void)
{
  return RS485_Status;
}

//------------------------------------------------------------------------------
// RS485_Init - init module
//------------------------------------------------------------------------------
void RS485_Init(void)
{
  //baud rate
  UBRR0H=((RS485_BaudRate(RS485_BAUDRATE)>>8)&0x7F);
  UBRR0L=RS485_BaudRate(RS485_BAUDRATE);
  //enable Rx/Tx
  UCSR0B=((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0));
  //set frame format: 8data, 1stop bit
  UCSR0C=((1<<UCSZ01)|(1<<UCSZ00));
  //set address
  RS485_Address=RS485_ADDR_MASTER;
  RS485_Status=RS485_STATUS_FREE;
}

//------------------------------------------------------------------------------
// RS485_Stop - stop bus actions
//------------------------------------------------------------------------------
void RS485_Stop(void)
{
  UCSR0B&=~((1<<RXCIE0)|(1<<TXCIE0));
}

//------------------------------------------------------------------------------
// RS485_PutByte - put a byte on RS-485
//------------------------------------------------------------------------------
void RS485_PutByte(TByte data)
{
  //wait for empty transmit buffer
  while (!(UCSR0A&(1<<UDRE0)))
    continue;
  //put data into buffer, sends the data
  UDR0=data;
}

//------------------------------------------------------------------------------
// RS485_GetByte - get a byte from RS-485
//------------------------------------------------------------------------------
TBool RS485_GetByte(TByte *data)
{
  if (UCSR0A&(1<<RXC0))
  {
    *data=UDR0;
    return true;
  } 
  return false;
}

//------------------------------------------------------------------------------
// RS485_PutByte - put a byte on RS-485
//------------------------------------------------------------------------------
void RS485_PutByte2(TByte data)
{
  macBitOn(PORT_RS485_DIR, pin485TR);
  macBitOn(UCSR0A, TXC0);
  //put data into buffer, sends the data
  UDR0=data;
  while (!(UCSR0A&(1<<TXC0)))
    continue;
  macBitOff(PORT_RS485_DIR, pin485TR);
}

//------------------------------------------------------------------------------
// RS485_PutPackage
//------------------------------------------------------------------------------
void RS485_PutPackage(TByte address, TByte command, TByte len, TByte *buffer)
{
  TByte i;
  TByte checkSum;

  //select transmition mode
  macBitOn(PORT_RS485_DIR, pin485TR);
  macBitOn(UCSR0A, TXC0);
  //put sync sequence
  RS485_PutByte(PKG_HEAD0);
  RS485_PutByte(PKG_HEAD1);
  RS485_PutByte(PKG_HEAD2);
  //put package with address, command, data len, data
  RS485_PutByte(address);
  RS485_PutByte(command);
  RS485_PutByte(len);
  checkSum=address+command+len;
  for (i=0; i<len; i++)
  {
    RS485_PutByte(buffer[i]);
    checkSum+=buffer[i];
  }
  //put check sum
  RS485_PutByte(checkSum);
  //wait for transmition to end
  while (!(UCSR0A&(1<<TXC0)))
    continue;                  
  //macBitOn(UCSRA, TXC);
  //reception mode
  macBitOff(PORT_RS485_DIR, pin485TR);
}

//------------------------------------------------------------------------------
// TxD1_IntHandler - RxD0 interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART0_TXC_vect
__interrupt void TxD0_IntHandler(void)
{
  TByte i=RS485_OTail;
  
  if (RS485_OHead==i)
  {
    RS485_OHead=RS485_OTail=0;
    macBitOff(PORT_RS485_DIR, pin485TR);
    RS485_Status=RS485_STATUS_WAITING;
    return;
  }
  UDR0=oBuffer[RS485_OHead++];
  /*if (RS485_OHead>=RS485_BUFFER_O_SIZE)
    RS485_OHead=0;*/
}

//------------------------------------------------------------------------------
// RS485_SendPackage - build package and start transmitting
//------------------------------------------------------------------------------
void RS485_SendPackage(TByte address, TByte command, TByte len, TByte *buffer)
{
  TByte checkSum, i;
  
  RS485_Status=RS485_STATUS_SENDING;
  
  oBuffer[0]=PKG_HEAD0;
  oBuffer[1]=PKG_HEAD1;
  oBuffer[2]=PKG_HEAD2;
  oBuffer[3+PKG_OFS_ADDRESS]=address;
  oBuffer[3+PKG_OFS_CMD_STS]=command;
  oBuffer[3+PKG_OFS_DATA_LEN]=len;
  memcpy(&oBuffer[3+PKG_OFS_DATA], buffer, len);
  checkSum=address+command+len;
  for (i=0; i<len; i++)
  {
    oBuffer[i+3+PKG_OFS_DATA]=*buffer;
    checkSum+=*buffer++;
  }
  oBuffer[i+3+PKG_OFS_DATA]=checkSum;
  RS485_OHead=0;
  RS485_OTail=3+PKG_OFS_DATA+len+1;
  //select transmition mode
  macBitOn(PORT_RS485_DIR, pin485TR);
  macBitOn(UCSR0A, TXC0);
  UDR0=oBuffer[RS485_OHead++];
}

//------------------------------------------------------------------------------
// RS485_WaitPackage - wait for package
//------------------------------------------------------------------------------
TByte RS485_WaitPackage(TWord timeOut)
{
  TIMER_Start(TIMER_RS485, timeOut);
  while ((!globalStatus.gotPackage)&&(!TIMER_TimeOut(TIMER_RS485)))
    continue;
  TIMER_Stop(TIMER_RS485);
  if (globalStatus.gotPackage)
  {
    RS485_Status=RS485_STATUS_FREE;
    globalStatus.gotPackage=false;
    RS485_Errors=0;
    return true;
  }
  RS485_Status=RS485_STATUS_FREE;
  RS485_Errors++;
  return false;
}

//------------------------------------------------------------------------------
// RxD1_IntHandler - RxD interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART0_RXC_vect
__interrupt void RxD_IntHandler(void)
{
  static TByte pkgHead0=0x00;
  static TByte pkgHead1=0x00;
  static TByte pkgHead2=0x00;
  static TByte rxIndex=0x00;
  static TByte dataLen=0x00;
  static TByte data=0x00;
  static TByte rejectPackage=false;
  TByte i;

  //check for errors
  if (UCSR0A&((1<<FE0)|(1<<DOR0)))
  {
    UDR0;
    return;
  }
  //get byte
  data=UDR0;

  //external device sent a sync sequence?
  pkgHead0=pkgHead1;
  pkgHead1=pkgHead2;
  pkgHead2=data;

  if ((pkgHead0==PKG_HEAD0)&&(pkgHead1==PKG_HEAD1)&&(pkgHead2==PKG_HEAD2))
  {
    //init buffers for data reception
    rxIndex=0x00;
    dataLen=0x00;
    rejectPackage=false;
    return;
  }
  if (rxIndex>=RS485_BUFFER_I_SIZE)
    rxIndex=0x00;
  //add data to data buffer
  iBuffer[rxIndex]=data;
  //Is this an address byte?
  if (rxIndex==PKG_OFS_ADDRESS)
    if (data!=RS485_Address)
    {
      rejectPackage=true;
      return;
    }
  //Is this a Data Length byte?
  if (rxIndex==PKG_OFS_DATA_LEN)
    dataLen=data;
  //Is this a CheckSum?
  if (rxIndex==(dataLen+PKG_OFS_DATA))
  {
    //are we rejecting current package?
    if (rejectPackage)
      return;
    //calc CheckSum - use Data as a temp var
    data=0;
    for (i=0;i<rxIndex;i++)
      data+=iBuffer[i];
    //Check sum is Ok?
    globalStatus.gotPackage=false;
    if (data==iBuffer[i])
      globalStatus.gotPackage=true;
  }
  rxIndex++;
}
