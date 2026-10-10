#ifndef _TIMER_H
  #define _TIMER_H

#include "defines.h"
#include "platform.h"   

//interrupt rate, Hz
#define TMR0_INT_RATE	      (400L)
#define TMR0_RELOAD	      ((TByte)(256-(XTAL/TMR0_INT_RATE/256L)))
#define TMR0_RELOAD_VALUE     ((TByte)(XTAL/TMR0_INT_RATE/256L)-1)

//KBD interrupt handler rate, Hz
#define TMR0_KBD_RATE	      (10)
#define TMR0_KBD_COUNTER      (TMR0_INT_RATE/TMR0_KBD_RATE)

#define TMR0_ONE_SEC_RATE     (1)
#define TMR0_ONE_SEC_COUNTER  (TMR0_INT_RATE/TMR0_ONE_SEC_RATE)

#define TO_RS485  	      (500*TMR0_INT_RATE/1000)
#define TO_USB    	      (50*TMR0_INT_RATE/1000)

#define TO_100MSEC	      (100*TMR0_INT_RATE/1000)
#define TO_200MSEC	      (200*TMR0_INT_RATE/1000)
#define TO_500MSEC	      (500*TMR0_INT_RATE/1000)
#define TO_1SEC   	      (1000*TMR0_INT_RATE/1000)
#define TO_2SEC   	      (2000*TMR0_INT_RATE/1000)
#define TO_5SEC   	      (5000*TMR0_INT_RATE/1000)
#define TO_10SEC  	      (10000*TMR0_INT_RATE/1000)
#define TO_EKEY               (100*TMR0_INT_RATE/1000)

typedef struct
  {
    TByte flgRunning;
    TByte flgTimeOut;
    TWord value;
  } TTimer;

enum TTimerName
{
  TIMER_GENERAL,
  TIMER_RS485,
  TIMER_USB,
  TIMER_NUM_TIMERS
};

// TIMER_Init - init timer
extern void TIMER_Init(void);

extern void TIMER_Start(TByte index, TWord timeout);
  
extern void TIMER_Stop(TByte index);
  
extern TByte TIMER_TimeOut(TByte index);

extern void TIMER_ResetTick(void);

extern TBool TIMER_TimeTick(void);

#endif
