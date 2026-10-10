#ifndef _IO_H
#define _IO_H

#include "stm32f10x.h"    // Device header

extern void IO_Init(void);
extern void IO_SetDcPump(BitAction val);
extern void IO_SetAcPump(BitAction val);
extern void IO_SetVentil(BitAction val);

#endif
