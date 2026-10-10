#include "platform.h"
#include "pins.h"
#include "global.h"
#include "eeprom.h"
#include "timer.h"
#include "usb.h"

char USBIBuffer[USB_BUFFER_SIZE];
TByte USBIHead;

//------------------------------------------------------------------------------
// USB_Delay - small delay for USB actions
//------------------------------------------------------------------------------
void USB_Delay(void)
{
  __no_operation();
  __no_operation();
  __no_operation();
}

//------------------------------------------------------------------------------
// USB_Init - intialize USB unit
//------------------------------------------------------------------------------
void USB_Init(void)
{
  // инициализация USB
  DIR_USB_WR |= (1<<pinUSBWR);
  DIR_USB_RD |= (1<<pinUSBRD);
  DIR_USB_STS &= ~((1<<pinUSBRXF)|(1<<pinUSBTXE));
  USBIHead=0;
  // set interrupt for receiving byte
  //EICRB|=(1<<ISC51);
  //EIFR|=(1<<INTF5);
  //EIMSK|=(1<<INT5);
  
  globalDeviceStatus.enableUSB=true;
}

//------------------------------------------------------------------------------
// USB_SendChar - send one char to USB
//------------------------------------------------------------------------------
void USB_SendChar(char c)
{
  TIMER_Start(TIMER_USB,TO_USB);
  while ((PIN_USB_STS & (1<<pinUSBTXE))&&(!TIMER_TimeOut(TIMER_USB)));
  TIMER_Stop(TIMER_USB);
  if (PIN_USB_STS & (1<<pinUSBTXE))
  {
    //USB error
    globalDeviceStatus.USBFailure=1;
    return;
  }
  DIR_USB_DATA = 0xff;
  PORT_USB_DATA = c;
  PORT_USB_WR |= (1<<pinUSBWR);
  USB_Delay();
  PORT_USB_WR &= ~(1<<pinUSBWR);
}

//int putchar(int c)
//{
//  USB_SendChar(c);
//  return 0;
//}

//------------------------------------------------------------------------------
// USB_SendString - send string to USB
//------------------------------------------------------------------------------
void USB_SendString(char *buf, TByte len)
{
  TByte ix;
  
  for (ix=0; ix<len; ix++)
    USB_SendChar(*buf++);
}

//------------------------------------------------------------------------------
// USB_RecChar - receive one char from USB
//------------------------------------------------------------------------------
char USB_RecChar(void)
{
  char c;
  
  /*TIMER_Start(TIMER_USB,TO_USB);
  while ((PIN_USB_STS & (1<<pinRXF))&&(!TIMER_TimeOut(TIMER_USB)));
  TIMER_Stop(TIMER_USB);
  if (PIN_USB_STS & (1<<pinRXF))
  {
    //USB error
    globalDeviceStatus.USBFailure=1;
    return 0xff;
  }*/
  PORT_USB_DATA = 0x00;
  DIR_USB_DATA = 0x00;
  PORT_USB_RD &= ~(1<<pinUSBRD);
  USB_Delay();
  c = PIN_USB_DATA;
  PORT_USB_RD |= (1<<pinUSBRD);  
  return c;
}

//------------------------------------------------------------------------------
// USB_Process - USB interrupt routine, receive chars
//------------------------------------------------------------------------------
//#pragma vector=INT5_vect
//__interrupt void USB_Process(void)
void USB_Process(void)
{
  TByte data;

  //EIMSK&=~(1<<INT5);
  //__enable_interrupt();
  if (!(PIN_USB_STS & (1<<pinUSBRXF)))
  {
    //get byte
    data=USB_RecChar();
    if (data=='\n')
      return;  
    if (data==0x08)
    {
      if (USBIHead)
        USBIHead--;
      //__disable_interrupt();
      //EIMSK|=(1<<INT5);
      return;
    }
    if ((data==0x1B)||(data==0xff))
    {
      USBIHead=0;
      //__disable_interrupt();
      //EIMSK|=(1<<INT5);
      return;
    }
    USBIBuffer[USBIHead++]=data;
    if (USBIHead>=USB_BUFFER_SIZE)
      USBIHead=0;
    if (data=='\r') 
    {
      globalDeviceStatus.gotUSBPackage=true;
      USBIHead=0;
    }
  }
  //__disable_interrupt();
  //EIMSK|=(1<<INT5);
}
