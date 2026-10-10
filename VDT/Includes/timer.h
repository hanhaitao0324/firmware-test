#ifndef _TIMER_H
#define _TIMER_H

#include "defines.h"
#include "platform.h"

#define TIMER_FREQ						(2000000L)
//interrupt rate, Hz
#define TIMER_INT_RATE	 			(500L)

#define TIMER_VALUE						((uint16_t)(TIMER_FREQ/TIMER_INT_RATE))

#define TIMER_ONE_SEC_RATE		(1)
#define TIMER_ONE_SEC_COUNTER	(TIMER_INT_RATE/TIMER_ONE_SEC_RATE)

#define TO_50MSEC							(10*TIMER_INT_RATE/1000) 
#define TO_100MSEC						(100*TIMER_INT_RATE/1000)
#define TO_200MSEC						(200*TIMER_INT_RATE/1000)
#define TO_300MSEC      			(300*TIMER_INT_RATE/1000)
#define TO_500MSEC						(500*TIMER_INT_RATE/1000)
#define TO_1SEC   						(1000*TIMER_INT_RATE/1000)
#define TO_2SEC   						(2000*TIMER_INT_RATE/1000)
#define TO_5SEC   						(5000*TIMER_INT_RATE/1000)
#define TO_10SEC  						(10000*TIMER_INT_RATE/1000)

typedef struct
{
  uint8_t flgRunning;
  uint8_t flgTimeOut;
  uint16_t value;
} TTimer;

enum TTimerName
{
  TIMER_GENERAL,
	TIMER_RS232,
	TIMER_I2C,
	TIMER_Vinculum,
  TIMER_TIMEOUT,  
	TIMER_YMODEM,
  TIMER_NUM_TIMERS
};

extern void TIMER_Init(void);
extern void TIMER_Start(uint8_t index, uint16_t timeout);
extern void TIMER_Stop(uint8_t index);
extern uint8_t TIMER_TimeOut(uint8_t index);

#endif
