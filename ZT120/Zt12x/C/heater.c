//#include <math.h>
#include <stdlib.h>
#include "defines.h"
#include "eeprom.h"
#include "global.h"
#include "heater.h"
#include "pins.h"
#include "platform.h"
#include "temp.h"
#include "timer.h"

TWord Heater_Target=0;

TByte Heater_Power=0;

TsWord Heater_Integral=0;

TBool Heater_Cycle=false;

TBool Heater_On=false;

TBool Heater_Flag=false;

void HEATER_Init(void)
{
  TIMER_Start(TIMER_INTEGRAL, TO_5SEC);
}

void HEATER_TimeTick(void)
{
  static TByte Heater_Phase = 0;
  static TByte Heater_Power_Latch = 0;
  
  if (!TIMER_TimeOut(TIMER_HEATER))
    return;
  
  TIMER_Start(TIMER_HEATER, TO_50MSEC);

  if (Heater_Phase < 99)
    Heater_Phase++;
  else
  {
    Heater_Cycle = true;
    Heater_Phase = 0;
    Heater_Power_Latch = Heater_Power;
  }

  if (Heater_Power_Latch <= Heater_Phase)
  {
    macBitOff(PORT_RELAY, pinRelay);
    //macBitOn(PORT_LEDS, pinLED_R);
    Heater_Flag=false;
  }
  else
  {  
    macBitOn(PORT_RELAY, pinRelay);
    //macBitOff(PORT_LEDS, pinLED_R);
    Heater_Flag=true;
  }
}

void HEATER_Regulate(void)
{
  TWord t;
  TReal32 p;
  //TsWord diff;  2017-11-3 星期五 11:12:31, Gavin

  if (!TIMER_TimeOut(TIMER_INTEGRAL))
    return;

  t=(TWord)(TEMP_GetValue(ADC_CHNL_INTERNAL)*100);
  
  TIMER_Start(TIMER_INTEGRAL, TO_5SEC);

  if (globalDeviceStatus.sensorInsError)
    Heater_On=false;
  
  if(!Heater_On)
  {
    Heater_Power = 0;
    Heater_Integral = 0;
    Heater_Flag=false;
    return;
  }

  if(!Heater_Cycle)
    return;
  
  /*if (t>Heater_Target)
  {
    Heater_Power=0;
    Heater_Integral = 0;
    return;
  }*/

  Heater_Cycle = false;
  
  //diff = Heater_Target - t;  2017-11-3 星期五 11:12:18, Gavin

  if (abs(Heater_Target - t) < 100)
    Heater_Integral += (Heater_Target - t);
  else
    Heater_Integral = 0;

  if (Heater_Integral > 25000)
    Heater_Integral = 25000;

  if (Heater_Integral < 0)
    Heater_Integral = 0;

  //p = ((TReal32)Heater_Target - t) / 4 + Heater_Integral / 500;
	//p = ((TReal32)Heater_Target - t) / 2 + Heater_Integral / 500;
	p = ((TReal32)Heater_Target - t) / 2 + Heater_Integral / 100;
	
  if (p>100)
    p = 100;
  if (p<0)
    p = 0;

  Heater_Power = (TByte)p; 		// 2017-11-3 星期五 11:13:01, Gavin
}

void HEATER_SetOff(void)
{
  Heater_On=false;
  Heater_Flag=false;
  TIMER_Stop(TIMER_HEATER);
  macBitOff(PORT_RELAY, pinRelay);
  //macBitOn(PORT_LED_BRD, pinLED_Brd);
  //macBitOn(PORT_LEDS, pinLED_R);
}

void HEATER_SetTemp(TReal32 *temp)
{
  TWord Value;
  TReal32 fltValue = *temp;
  
  if ((TByte)*temp==0)
  {
    HEATER_SetOff();
    return;
  }
  
  //fltValue+=(TReal32)EEPROM1.TempOffset[ADC_CHNL_INTERNAL]/10; 			// 2018-1-9 星期二 14:12:32，带入函数前已在test.c文件中加入了偏移量
  Value=(TWord)(fltValue*100);
  if (Value!=Heater_Target)
  {  
    Heater_Target=Value;
    Heater_Integral=0;
  }  
  Heater_On=true;
  TIMER_Start(TIMER_HEATER, TO_50MSEC);
}

TBool HEATER_On(void)
{
  return Heater_On;
}

TByte HEATER_GetPower(void)
{
  if (Heater_On)
    return Heater_Power;
  else
    return 0;
}

TBool HEATER_Flag(void)
{
  return Heater_Flag;
}
