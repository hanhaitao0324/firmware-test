#include "platform.h"
#include "global.h"
#include "pins.h"
#include "rs485.h"
#include "timer.h"

//I/O buffers
TByte iBuffer[RS485_BUFFER_I_SIZE];
TByte oBuffer[RS485_BUFFER_O_SIZE];
//device address
TByte RS485_Address;
volatile TByte RS485_OHead, RS485_OTail;

TByte RS485_Errors=0;

TByte RS485_Status;
TBool RS485_Request;

TByte rxIndex=0x00;
TByte pkgHead0=0x00;
TByte pkgHead1=0x00;
TByte pkgHead2=0x00;

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
// RS485_SetRequest - set request status
//------------------------------------------------------------------------------
void RS485_SetRequest(TBool request)
{
  RS485_Request = request;  
}

//------------------------------------------------------------------------------
// RS485_GetRequest - get request status
//------------------------------------------------------------------------------
TBool RS485_GetRequest(void)
{
  return RS485_Request;
}

//------------------------------------------------------------------------------
// RS485_Init - init internal bus
//------------------------------------------------------------------------------
void RS485_Init(void)
{
  //baud rate
  UBRR1H=((RS_BaudRate(RS485_BAUDRATE)>>8)&0x7F);
  UBRR1L=RS_BaudRate(RS485_BAUDRATE);
  //enable Rx/Tx
  UCSR1B=((1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(1<<TXCIE1));
  //set frame format: 8data, 1stop bit
  UCSR1C=(1<<UCSZ11)|(1<<UCSZ10);
  //set address
  RS485_Address=RS485_ADDR_MASTER;
  RS485_Status=RS485_STATUS_FREE;
  macBitOn(UCSR1A, TXC1);
}

//------------------------------------------------------------------------------
// RS485_Stop - stop bus actions
//------------------------------------------------------------------------------
void RS485_Stop(void)
{
  UCSR1B&=~((1<<RXCIE1)|(1<<TXCIE1));
}

//------------------------------------------------------------------------------
// RS485_PutByte - put a byte on RS-485
//------------------------------------------------------------------------------
void RS485_PutByte(TByte data)
{
  //wait for empty transmit buffer
  while (!(UCSR1A&(1<<UDRE1)))
    continue;
  //put data into buffer, sends the data
  UDR1=data;
}

//------------------------------------------------------------------------------
// RS485_GetByte - get a byte from RS-485
//------------------------------------------------------------------------------
TBool RS485_GetByte(TByte *data)
{
  if (UCSR1A&(1<<RXC1))
  {
    *data=UDR1;
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
  macBitOn(UCSR1A, TXC1);
  //put data into buffer, sends the data
  UDR1=data;
  while (!(UCSR1A&(1<<TXC1)))
    continue;
  macBitOff(PORT_RS485_DIR, pin485TR);
}

//------------------------------------------------------------------------------
// RS485_PutPackage
//------------------------------------------------------------------------------
/*void RS485_PutPackage(TByte address, TByte command, TByte len, TByte *buffer)
{
  TByte i;
  TByte checkSum;

  //select transmition mode
  macBitOn(PORT_RS485_DIR,pin485TR);
  macBitOn(UCSR1A,TXC1);
  //put sync sequence
  RS485_PutByte(PKG_HEAD0);
  RS485_PutByte(PKG_HEAD1);
  RS485_PutByte(PKG_HEAD2);
  //put package with address, command, data len, data
  RS485_PutByte(address);
  RS485_PutByte(command);
  RS485_PutByte(len);
  checkSum=address+command+len;
  for (i=0;i<len;i++)
  {
    RS485_PutByte(buffer[i]);
    checkSum+=buffer[i];
  }
  //put check sum
  RS485_PutByte(checkSum);
  //wait for transmition to end
  while (!(UCSR1A&(1<<TXC1)))
    continue;                  
  //reception mode
  macBitOff(PORT_RS485_DIR,pin485TR);
}*/

//------------------------------------------------------------------------------
// TxD1_IntHandler - RxD0 interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART1_TXC_vect
__interrupt void TxD1_IntHandler(void)
{
  TByte i=RS485_OTail;
  
  if (RS485_OHead>=i)
  {
    RS485_OHead=RS485_OTail=0;
    macBitOff(PORT_RS485_DIR, pin485TR);
    RS485_Status=RS485_STATUS_WAITING;
    return;
  }
  UDR1=oBuffer[RS485_OHead++];
  /*if (RS485_OHead>=RS485_BUFFER_O_SIZE)
    RS485_OHead=0;*/
}

//------------------------------------------------------------------------------
// RS485_SendPackage - build package and start transmitting
//------------------------------------------------------------------------------
void RS485_SendPackage(TByte address, TByte command, TByte len, TByte *buffer, TBool request)
{
  TByte checkSum, i;
  
  //RS485_Request=request;
  while (RS485_Status!=RS485_STATUS_FREE)
    continue;
  RS485_Status=RS485_STATUS_SENDING;

  oBuffer[0]=PKG_HEAD0;
  oBuffer[1]=PKG_HEAD1;
  oBuffer[2]=PKG_HEAD2;
  oBuffer[3+PKG_OFS_ADDRESS]=address;
  oBuffer[3+PKG_OFS_CMD_STS]=command;
  oBuffer[3+PKG_OFS_DATA_LEN]=len;
  memcpy(&oBuffer[3+PKG_OFS_DATA], buffer, len);
  checkSum=address+command+len;
  for (i=0;i<len;i++)
  {
    oBuffer[i+3+PKG_OFS_DATA]=*buffer;
    checkSum+=*buffer++;
  }
  oBuffer[i+3+PKG_OFS_DATA]=checkSum;
  RS485_OHead=0;
  RS485_OTail=3+PKG_OFS_DATA+len+1;
  //select transmition mode
  macBitOn(PORT_RS485_DIR,pin485TR);
  macBitOn(UCSR1A,TXC1);
  UDR1=oBuffer[RS485_OHead++];
}

//------------------------------------------------------------------------------
// RS485_WaitPackage - wait for package
//------------------------------------------------------------------------------
TByte RS485_WaitPackage(TWord timeOut)
{
  TIMER_Start(TIMER_RS485,timeOut);
  while((!globalDeviceStatus.gotRS485Package)&&(!TIMER_TimeOut(TIMER_RS485)))
    continue;
  TIMER_Stop(TIMER_RS485);
  if(globalDeviceStatus.gotRS485Package)
  {
    globalDeviceStatus.gotRS485Package=false;
    RS485_Status=RS485_STATUS_FREE;
    RS485_Request=false;
    RS485_Errors=0;
    return true;
  }
  RS485_Errors++;
  if (RS485_Errors>=RS485_MAX_TRY)
  {
    globalDeviceStatus.gotRS485Package=false;
    RS485_Request=false;
  }
  RS485_Status=RS485_STATUS_FREE;
  return false;        
}

//------------------------------------------------------------------------------
// RxD1_IntHandler - RxD1 interrupt handler
//------------------------------------------------------------------------------
#pragma vector=USART1_RXC_vect
__interrupt void RxD1_IntHandler(void)
{
  //static TByte pkgHead0=0x00;
  //static TByte pkgHead1=0x00;
  //static TByte pkgHead2=0x00;
  //static TByte rxIndex=0x00;
  static TByte dataLen=0x00;
  static TByte data=0x00;
  static TByte rejectPackage=false;
  TByte i;

  //check for errors
  if (UCSR1A&((1<<FE1)|(1<<DOR1)))
  {
    UDR1;
    return;
  }
  //get byte
  data=UDR1;

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
      return;
  //Is this a Data Length byte?
  if (rxIndex==PKG_OFS_DATA_LEN)
    dataLen=data;
  //Is this a CheckSum?
  if (rxIndex==(dataLen+PKG_OFS_DATA))
  {
    //are we rejecting current package?
    if (rejectPackage)
    {
      return;
    }
    //calc CheckSum - use Data as a temp var
    data=0;
    for (i=0;i<rxIndex;i++)
      data+=iBuffer[i];
    //Check sum is Ok?
    globalDeviceStatus.gotRS485Package=false; 
    if (data==iBuffer[i])
    {
      globalDeviceStatus.gotRS485Package=true;
      if (RS485_Status==RS485_STATUS_WAITING)
      	RS485_Status=RS485_STATUS_READY;
    }
  }
  rxIndex++;
}
