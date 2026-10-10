#ifndef	_USB_H
#define _USB_H
/*****************************************************************************
* File              : usb.h
****************************************************************************/

#define USB_DATA_OUT    PORTC
#define USB_DATA_IN     PINC
#define USB_DATA_DIR    DDRC
#define USB_CTRL_OUT    PORTG
#define USB_RXF_IN      PIND
#define USB_TXE_IN      PING
#define USB_CTRL_DIR    DDRG
#define USB_RXF_DIR     DDRD
#define USB_TXE_DIR     DDRG

#define USB_RXF_BIT     1
#define USB_TXE_BIT     2

#define USB_WR_BIT      0
#define USB_RD_BIT      1

void initbootuart(void);
void sendchar(char);
char recchar(void);

#endif
