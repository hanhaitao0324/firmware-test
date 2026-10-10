#include "platform.h"
#include "global.h"
#include "pins.h"
#include "sound.h"
#include "timer.h"
#include "led.h"
#include "spi.h"
#include "kbd.h"
#include "adc.h"
#include "fram.h"
#include "dhc.h"
#include "sampler.h"
#include "stepper.h"
#include "motor.h"
#include "heater.h"

volatile TTimer timers[TIMER_NUM_TIMERS];

TWord TIMER_counterOneSec=0;

TBool TIMER_timeTick;

//------------------------------------------------------------------------------
// TIMER_Init - init module
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
  {  
    timers[i].flgRunning=false;
    timers[i].flgTimeOut=false;
  }
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
// TIMER_TimeOut - check if timeout（超时检查）
//------------------------------------------------------------------------------
TByte TIMER_TimeOut(TByte index)
{
  return timers[index].flgTimeOut;
}

//------------------------------------------------------------------------------
// TIMER_TickReset - reset 1s ticks counter
//------------------------------------------------------------------------------
__monitor void TIMER_TickReset(void)
{
  TIMER_timeTick=false;
  TIMER_counterOneSec=0;
}
  
//------------------------------------------------------------------------------
// TIMER_Ticked - check 1s tick
//------------------------------------------------------------------------------
TBool TIMER_Ticked(void)
{
  return TIMER_timeTick;
}

//------------------------------------------------------------------------------
// TIMER_IntHandler - interrupt handler
//------------------------------------------------------------------------------
//#pragma vector=TIMER0_OVF_vect
#pragma vector=TIMER0_COMP_vect
__interrupt __nested void TIMER_IntHandler(void)
{
  TByte i;
	
  //__enable_interrupt();
  
  for(i=0;i<TIMER_NUM_TIMERS;i++)
  {
		if (timers[i].flgRunning)
		{
			if (timers[i].value)
				timers[i].value--;
			else
				timers[i].flgTimeOut=true;
		}
  }

  TIMER_counterOneSec++;

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
      {
        TCCR2&=~((1<<CS21)|(1<<CS20));
      } else SNDPos++;
    }
  }

  if (TIMER_counterOneSec>=TMR0_ONE_SEC_COUNTER)
  {
    globalDeviceStatus.timeTick=true;
    TIMER_timeTick=true;
    TIMER_counterOneSec=0;
  }
  clockCounter++;

  //if (!SPI_Busy)
  LED_IntHandler(); 										// 数码管中断处理程序
  KBD_IntHandler(); 										// 按键中断处理程序
  //if (!SPI_Busy)
  ADC0_IntHandler(); 										// 模数控制中断处理程序
  ADC1_IntHandler();
  MOTOR_IntHandler(); 									// 电机中断处理程序
  DHC_IntHandler(); 										// 加热板中断处理程序
  
  HEATER_Regulate(); 										// 加热器调整，2017-11-7 星期二 20:07:15
  HEATER_TimeTick(); 										// 加热中断处理程序
  
  SAMPLER_IntHandler();
  STEPPER_IntHandler();
  FRAM_IntHandler();
}
