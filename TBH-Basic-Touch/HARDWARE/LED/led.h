#ifndef _LED_H
#define _LED_H
#include "sys.h"


#define BEEP PAout(5) 	// BEEP
#define LED0 PBout(1)   // LED0
#define LED1 PBout(0)   // LED1

void LED_Init(void);
#endif
