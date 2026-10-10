#ifndef _LED_H
  #define _LED_H

#include "const.h"
#include "defines.h"
#include "platform.h"

#define TIMER_BLINK_RELOAD      (TMR0_INT_RATE/2)

#define LED_MASK_DEC_POINT  (SEG_DP)

//blink position
extern TByte ledBlinkPos;
extern TBool ledBlinkPosYes;

// LED_Init - init module
extern void LED_Init(void);
// LED_Empty_Time - print nothing on the time display
extern __monitor void LED_Empty_Time(void);
// LED_Print_Time - print time on the display
extern __monitor void LED_Print_Time(TBool blink, TDWord* time);
// LED_Print - print a string on the display
extern __monitor void LED_Print(TByte led,TBool blink,char *str);
// LED_Print_P - print a string from the Flash on the display
extern __monitor void LED_Print_P(TByte led,TBool blink,char __flash *str);
// LED interrupt handler
extern void LED_IntHandler(void);
// LED_SetBrightness - set LED brightness
extern TBool LED_SetBrightness(TByte brightness);
// LED_GetBrightness - get LED brightness
extern TByte LED_GetBrightness(void);
// LED_Check - check LEDs
extern void LED_Check(TByte Position);
// LED_SwitchOff - switch off LEDs
extern void LED_SwitchOff(void);
// LED_Store - store LEDs values
extern void LED_Store(void);
// LED_Restore - restore LEDs values
extern void LED_Restore(void);

#endif
