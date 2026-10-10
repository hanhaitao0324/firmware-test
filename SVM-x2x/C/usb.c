#include "eeprom.h"
#include "global.h"
#include "pins.h"
#include "platform.h"
#include "timer.h"
#include "usb.h"

char USB_IBuffer[USB_BUFFER_SIZE];
//char USBOBuffer[USB_BUFFER_SIZE];
TByte USB_IHead;
//volatile TByte USBOHead, USBOTail;

//------------------------------------------------------------------------------
// USB_Delay - simple delay function for USB module
//------------------------------------------------------------------------------
void USB_Delay(void)
{
  __no_operation();
  __no_operation();
  __no_operation();
}

//------------------------------------------------------------------------------
// USB_Init - initialize USB module
//------------------------------------------------------------------------------
void USB_Init(void)
{
  // init USB
  DIR_USB_CTRL |= ((1<<pinUSBWR)|(1<<pinUSBRD));
  DIR_USB_RXF &= ~(1<<pinUSBRXF);
  DIR_USB_TXE &= ~(1<<pinUSBTXE);
  //USBOHead=USBOTail=0;
  // set interrupt for receiving byte
  USB_IHead=0;
  EICRA|=(1<<ISC11);
  EIFR|=(1<<INTF1);
  EIMSK|=(1<<INT1);
}

//------------------------------------------------------------------------------
// USB_SendChar - send one byte to USB
//------------------------------------------------------------------------------
void USB_SendChar(char c)
{
  TIMER_Start(TIMER_USB,TO_USB);
  while ((PIN_USB_TXE & (1<<pinUSBTXE))&&(!TIMER_TimeOut(TIMER_USB)));
  TIMER_Stop(TIMER_USB);
  if (PIN_USB_TXE & (1<<pinUSBTXE))
  {
    //USB error
    globalStatus.usbFailure=1;
    return;
  }
  //while (PIN_USB_TXE & (1<<pinUSBTXE));
  DIR_USB_DATA = 0xff;
  PORT_USB_DATA = c;
  PORT_USB_CTRL |= (1<<pinUSBWR);
  USB_Delay();
  PORT_USB_CTRL &= ~(1<<pinUSBWR);
}

int putchar(int c)
{
  USB_SendChar(c);
  return 0;
}

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
// USB_RecChar - read one byte from USB
//------------------------------------------------------------------------------
char USB_RecChar(void)
{
  char c;
  
  while (PIN_USB_RXF & (1<<pinUSBRXF));
  PORT_USB_DATA = 0x00;
  DIR_USB_DATA = 0x00;
  PORT_USB_CTRL &= ~(1<<pinUSBRD);
  USB_Delay();
  c = PIN_USB_DATA;
  PORT_USB_CTRL |= (1<<pinUSBRD);  
  return c;
}

//------------------------------------------------------------------------------
// USB_Process - main USB function, receive bytes
//------------------------------------------------------------------------------
#pragma vector=INT1_vect
__interrupt void USB_Process(void)
{
  TByte data;

  if (!(PIN_USB_RXF & (1<<pinUSBRXF)))
  {
    //get byte
    data=USB_RecChar();
    if (data=='\n') 
      return;
    if ((data==0xff)||(data=='\x1b'))
    {
      USB_IHead=0;
      return;
    }  
    if (data==0x08)
    {
      if (USB_IHead)
        USB_IHead--;
      return;
    }
    USB_IBuffer[USB_IHead++]=data;
    if (USB_IHead>=USB_BUFFER_SIZE)
      USB_IHead=0;
    if (data=='\r') 
    {
      globalStatus.gotPackage=true;
      USB_IHead=0;
    }
  }
}



