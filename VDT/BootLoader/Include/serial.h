#ifndef _SERIAL_H
#define _SERIAL_H

#include "stm32f10x.h"

#define SERIAL_BAUDRATE      (115200)

extern void SERIAL_Init();
extern void SERIAL_SendChar(char c);
extern char SERIAL_GetChar(void);

#endif
