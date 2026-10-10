#include "defines.h"
#include "usb.h"

void delay(void)
{
  __no_operation();
  __no_operation();
  __no_operation();  
}

//------------------------------------------------------------------------------
void initbootuart(void)
{
  // установка портов на вход и выход
  USB_CTRL_OUT1_DIR = (1<<USB_WR_BIT);
  //USB_CTRL_OUT1_DIR |= (1<<USB_WR_BIT);
  USB_CTRL_OUT2_DIR = (1<<USB_RD_BIT);
  //USB_CTRL_OUT2_DIR |= (1<<USB_RD_BIT);
  //USB_CTRL_IN_DIR &= ~((1<<USB_RX_READY_BIT)|(1<<USB_TX_READY_BIT));
}

//------------------------------------------------------------------------------
void sendchar(char c)
{
  while (USB_CTRL_IN & (1<<USB_TXE_BIT));
  USB_DATA_DIR = 0xff;
  USB_DATA_OUT = c;
  USB_CTRL_OUT1 |= (1<<USB_WR_BIT);
  delay();
  USB_CTRL_OUT1 &= ~(1<<USB_WR_BIT);
}

//------------------------------------------------------------------------------
char recchar(void)
{
  char c;
  
  while (USB_CTRL_IN & (1<<USB_RXF_BIT));
  USB_DATA_OUT = 0x00;
  USB_DATA_DIR = 0x00;
  USB_CTRL_OUT2 &= ~(1<<USB_RD_BIT);
  delay();
  c = USB_DATA_IN;
  USB_CTRL_OUT2 |= (1<<USB_RD_BIT);  
  return c;
}

