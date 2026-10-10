#ifndef _USB_H
#define _USB_H

#include "const.h"
#include "defines.h"

#define USB_BUFFER_SIZE	64

//USB buffers
extern char USB_IBuffer[USB_BUFFER_SIZE];
//extern char USBOBuffer[USB_BUFFER_SIZE];     
extern TByte USB_IHead;

// init USB
extern void USB_Init(void);
// send one char to USB
extern void USB_SendChar(char c);
// send string to USB
extern void USB_SendString(char *buf, TByte len);
// receive one char from USB
extern char USB_RecChar(void);
// process USB activity
//extern void USB_Process(void);

#endif
