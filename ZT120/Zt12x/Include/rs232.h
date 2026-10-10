#ifndef _RS232_H
  #define _RS232_H

#include "defines.h"

#define RS232M_BUFFER_I_SIZE	(40)
#define RS232M_BUFFER_O_SIZE	(16)

#define RS232H_BUFFER_SIZE	(32)

//#define RS232_BAUDRATE	        (1200L)

//RS-232 buffer
extern char RS232M_IBuffer[RS232M_BUFFER_I_SIZE];
//extern char RS232M_OBuffer[RS232M_BUFFER_O_SIZE];     
//extern TByte RS232_IHead;
//extern volatile TByte RS232_OTail;

extern char RS232H_IBuffer[RS232H_BUFFER_SIZE];

// RS232 init
//extern void RS232_Init(TByte Baudrate);
// put char
extern void RS232_SendChar(char data);  // HHT 240613   加上串口发送的函数

// put string
extern void RS232M_PutString(char *buffer);
extern void RS232H_PutString(char *buffer);

// wait for reply on RS232
//extern TBool RS232_WaitReply(void);

#endif
