#ifndef _RS232_H
  #define _RS232_H

#include "defines.h"

#define RS232_BUFFER_I_SIZE	(40)
#define RS232_BUFFER_O_SIZE	(16)

//#define RS232_BAUDRATE	        (1200L)

//RS-232 buffer
extern char RS232_IBuffer[RS232_BUFFER_I_SIZE];
extern char RS232_OBuffer[RS232_BUFFER_O_SIZE];     
extern TByte RS232_IHead;
extern volatile TByte RS232_OTail;

// RS232 init
extern void RS232_Init(TByte Baudrate);
// put char
extern void RS232_SendChar(char data);
// put string
extern void RS232_PutString(char *buffer);
// wait for reply on RS232
extern TBool RS232_WaitReply(void);

#endif
