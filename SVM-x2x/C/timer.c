#include "defines.h"
#include "eeprom.h"
#include "global.h"
#include "pins.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "platform.h"
#include "sound.h"
#include "timer.h"

volatile TTimer timers[TIMER_NUM_TIMERS];
volatile TWord counterOneSec=0;
volatile TBool timeTick=false;

//------------------------------------------------------------------------------
// TIMER_Init - init timer
//------------------------------------------------------------------------------
void TIMER_Init(void)
{
  TByte i;

  TCNT0=0x00;
  OCR0=TMR0_RELOAD_VALUE;
  //init TMR0 interrupt: 1:256
  TCCR0=(1<<CS02)|(1<<CS01)|(1<<WGM01);
  //enable interrupt
  //TIMSK|=(1<<TOIE0);
  TIMSK|=(1<<OCIE0);

  for(i=0;i<TIMER_NUM_TIMERS;i++)
    timers[i].flgRunning=false;
}

//------------------------------------------------------------------------------
// TIMER_Start - start a timer
//------------------------------------------------------------------------------
void TIMER_Start(TByte index, TWord timeout)
{
  timers[index].flgTimeOut=false;
  timers[index].value=timeout;
  timers[index].flgRunning=true;
}
  
//------------------------------------------------------------------------------
// TIMER_Stop - stop a timer
//------------------------------------------------------------------------------
void TIMER_Stop(TByte index)
{
  timers[index].flgTimeOut=false;
  timers[index].flgRunning=false;
}
  
//------------------------------------------------------------------------------
// TIMER_TimeOut - check if timeout
//------------------------------------------------------------------------------
TByte TIMER_TimeOut(TByte index)
{
  return timers[index].flgTimeOut;
}

//------------------------------------------------------------------------------
// TIMER_ResetTick - reset 1 second ticks
//------------------------------------------------------------------------------
void TIMER_ResetTick(void)
{
  __disable_interrupt();
  timeTick=false;
  counterOneSec=0;
  __enable_interrupt();
}

//------------------------------------------------------------------------------
// TIMER_TimeTick - return 1 second tick status
//------------------------------------------------------------------------------
TBool TIMER_TimeTick(void)
{
  return timeTick;
}

//------------------------------------------------------------------------------
// Timer 0 interrupt handler - 800Hz rate
//------------------------------------------------------------------------------
#pragma vector=TIMER0_COMP_vect
__interrupt void IntHandlerT0(void)
{
  TByte i;
    
  //TCNT0=TMR0_RELOAD;

  for(i=0;i<TIMER_NUM_TIMERS;i++)
    if (timers[i].flgRunning)
      if (timers[i].value)
        timers[i].value--;
      else
        timers[i].flgTimeOut=true;

  counterOneSec++;
  clockCounter++;

  if ((!flgSND_Off)&&(OCR2)) 
  {
    if (SNDDelay)
    {
      SNDDelay--;
    } else
    {
      TCNT2=0;
      SNDDelay=SNDStream[SNDPos].time;
      OCR2=SNDStream[SNDPos].freq;
      if ((OCR2==0)&&(SNDDelay==0))
        TCCR2&=~((1<<CS21)|(1<<CS20)|(1<<COM20)); //COM20 added!!!
      else 
        SNDPos++;
    }
  }
    
  if (counterOneSec>=TMR0_ONE_SEC_COUNTER)
  {
    timeTick=true;
    counterOneSec=0;
  }

  LED_IntHandler();
  KBD_IntHandler();
  MOTOR_IntHandler();
}
