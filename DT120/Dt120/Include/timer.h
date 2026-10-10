#ifndef _TIMER_H
#define _TIMER_H

#include "defines.h"
#include "platform.h"

//interrupt rate, Hz
#define TMR0_INT_RATE	    (400L)
//#define TMR0_RELOAD_VALUE   ((TByte)(256-(XTAL/TMR0_INT_RATE/256L)))
#define TMR0_RELOAD_VALUE   ((TByte)(XTAL/TMR0_INT_RATE/256L)-1)

#define TMR0_ONE_SEC_RATE	(1)
#define TMR0_ONE_SEC_COUNTER	(TMR0_INT_RATE/TMR0_ONE_SEC_RATE)

//KBD interrupt handler rate, Hz
#define TMR0_KBD_RATE	(10)

#define TO_ADC    	(500*TMR0_INT_RATE/1000)
#define TO_RS485  	(500*TMR0_INT_RATE/1000)
//#define TO_MOTOR  	(2000*TMR0_INT_RATE/1000)
#define TO_RS232    	(300*TMR0_INT_RATE/1000)
#define TO_RS232BYTE   	(10*TMR0_INT_RATE/1000)
#define TO_USB    	(50*TMR0_INT_RATE/1000)
#define TO_HEATER    	(1000*TMR0_INT_RATE/1000)
#define TO_PRINTER	(100*TMR0_INT_RATE/1000)

#define TO_20MSEC	(20*TMR0_INT_RATE/1000)
#define TO_100MSEC	(100*TMR0_INT_RATE/1000)
#define TO_200MSEC	(200*TMR0_INT_RATE/1000)
#define TO_300MSEC	(300*TMR0_INT_RATE/1000)
#define TO_500MSEC	(500*TMR0_INT_RATE/1000)
#define TO_1SEC   	(1000*TMR0_INT_RATE/1000)
#define TO_2SEC   	(2000*TMR0_INT_RATE/1000)
#define TO_3SEC   	(3000*TMR0_INT_RATE/1000)
#define TO_5SEC   	(5000*TMR0_INT_RATE/1000)
#define TO_10SEC  	(10000*TMR0_INT_RATE/1000)
#define TO_15SEC  	(15000*TMR0_INT_RATE/1000)
#define TO_30SEC  	(30000*TMR0_INT_RATE/1000)

/* a2k - heating phase 10 min time-out */
#define TO_10MIN	(10*60)

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
    TIMER_RS232,
    TIMER_USB,
    TIMER_ADC,
    TIMER_STEPPER,
    TIMER_MOTOR,
    TIMER_NUM_TIMERS
  };


extern void TIMER_Init(void);

extern void TIMER_Start(TByte index, TWord timeout);
  
extern void TIMER_Stop(TByte index);
  
extern TByte TIMER_TimeOut(TByte index);

extern __monitor void TIMER_TickReset(void);

extern TBool TIMER_Ticked(void);

#endif
