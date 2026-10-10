#ifndef	_USB_H
#define _USB_H
/*****************************************************************************
* File              : usb.h
****************************************************************************/

#define USB_DATA_OUT      PORTA
#define USB_DATA_IN       PINA
#define USB_DATA_DIR      DDRA
#define USB_CTRL_OUT1     PORTC
#define USB_CTRL_OUT2     PORTG

//#define USB_CTRL_IN       PINE
#define USB_CTRL_IN       PINC

#define USB_CTRL_OUT1_DIR DDRC
#define USB_CTRL_OUT2_DIR DDRG

//#define USB_CTRL_IN_DIR   DDRE
#define USB_CTRL_IN_DIR   DDRC

//#define USB_TXE_BIT       2
//#define USB_RXF_BIT       5
#define USB_TXE_BIT       5
#define USB_RXF_BIT       6

#define USB_RD_BIT        2
#define USB_WR_BIT        7

void initbootuart(void);
void sendchar(char);
char recchar(void);

#endif
