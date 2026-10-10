#ifndef _RS232_H
#define _RS232_H

#include "defines.h"

#define RS232_BUFFER_SIZE	(32)

//#define RS232_BAUDRATE	        (1200L)

//RS-232 buffer
extern char RS232_IBuffer[RS232_BUFFER_SIZE];
//extern char rs232OBuffer[RS232_BUFFER_SIZE];     
extern TByte RS232_IHead;

// RS232 init
extern void RS232_Init(TByte Baudrate);
// set receiving mode
extern void RS232_SetRx(TBool act);
// put string to RS232
extern void RS232_PutString(char *buffer);

#endif
