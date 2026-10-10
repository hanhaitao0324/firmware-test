#ifndef _TIMER_H
#define _TIMER_H

#include "defines.h"
//#include "sys.h"
#include "system.h"

extern volatile uint16_t PowOnDelay;
extern volatile uint16_t clockCounter;
extern volatile u8 upd_rxtover; 								// 升级程序时的接收超时判断

extern u8 flash_flag;
extern u8 flash_cont;
extern u8 dtFlash;

//interrupt rate, Hz
//#define TIMER_INT_RATE	 			(500L)
#define TIMER_INT_RATE	 			(1000L)

//#define TO_50MSEC							(10*TIMER_INT_RATE/1000) 
#define TO_50MSEC							(50*TIMER_INT_RATE/1000) 
#define TO_100MSEC						(100*TIMER_INT_RATE/1000)
#define TO_200MSEC						(200*TIMER_INT_RATE/1000)
#define TO_300MSEC      			(300*TIMER_INT_RATE/1000)
#define TO_500MSEC						(500*TIMER_INT_RATE/1000)
#define TO_1SEC   						(1000*TIMER_INT_RATE/1000)
#define TO_2SEC   						(2000*TIMER_INT_RATE/1000)
#define TO_5SEC   						(5000*TIMER_INT_RATE/1000)
#define TO_10SEC  						(10000*TIMER_INT_RATE/1000)
#define TO_20SEC 							(20000*TIMER_INT_RATE/1000)

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

extern void TIM3_Int_Init(uint16_t arr,uint16_t psc);
extern void TIM4_Int_Init(uint16_t arr,uint16_t psc);
extern void TIMER_Start(uint8_t index, uint16_t timeout);
extern void TIMER_Stop(uint8_t index);
extern uint8_t TIMER_TimeOut(uint8_t index);

void TIM5_CH1_Cap_Init(u32 arr,u16 psc);

#endif
