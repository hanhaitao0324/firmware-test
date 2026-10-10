#include "confmenu.h"
#include "control.h"
#include "const.h"
#include "dhc.h"
#include "eeprom.h"
#include "ekey.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "pins.h"
#include "sound.h"
#include "stepper.h"
#include "strings.h"
#include "syserror.h"
#include "test.h"
#include "testsetup.h"

volatile TBool TT_s = true; 				// 时分秒设置标志，true(0)，设置小时/分钟；false(1)，设置秒/温度；

//------------------------------------------------------------------------------
//inc integer
//------------------------------------------------------------------------------
//TBool TESTSETUP_IncInt(signed char step, TWord *value, TWord lo, TWord hi)
TBool TESTSETUP_IncInt(signed int step, TWord *value, TWord lo, TWord hi) 			// 显示秒，2017-11-9 星期四 11:37:35
{
  signed short val;
  
  //try to increment
  val = *value + step;
  
  if (val>hi)
    return false;
  
  if (val<lo)
    return false;
  
  *value += step;
  
  return true;
}

//------------------------------------------------------------------------------
//set temperature before test
//------------------------------------------------------------------------------
void TESTSETUP_SetTemp(void)
{
  TWord valTemp;
  TByte key;
  TReal32 FVal;
  TWord MinTemp, MaxTemp;
  
  MinTemp=EEPROM1.MinTemp;
  MaxTemp=EEPROM1.MaxTemp;
  
  valTemp=(TWord)(EEPROM1.Temp*10);
  //LED_Print_P(LED_DISPLAY_SPEED, false, setTempStr);
  LED_Print_P(LED_DISPLAY_TIME, false, setTempStr);
  for (;;)
  {
    CTL_ShowTemp((TReal32)valTemp/10, false);

    while(!KBD_KeyPressed())
    {
      if (EXTERNAL_Process())
        return;
    }
    
    if (KBD_KeyPressed())
    {  
      key=KBD_GetKey();

      switch(key)
      {
        case KBD_CODE_START:
          //save temperature
          FVal=(TReal32)valTemp/10;
          if (EEPROM1.Temp!=((TReal32)valTemp/10))
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Temp, (TByte *)&FVal, sizeof(EEPROM1.Temp));
            //set DHC temperature
          SND_Play(ssOk);
          DHC_SetTemp(&FVal);
          return;
        case KBD_CODE_UP:
          //increment temperature
          if (TESTSETUP_IncInt(+TEMP_STEP, &valTemp, MinTemp, MaxTemp))
            SND_Play(ssSlowUp);
          else
            SND_Play(ssError);
          break;
        case KBD_CODE_DOWN:
          //decrement temperature
          if (TESTSETUP_IncInt(-TEMP_STEP, &valTemp, MinTemp, MaxTemp))
            SND_Play(ssSlowDown);
          else
            SND_Play(ssError);
          break;
        default:
          SND_Play(ssError);
          break;
      }   
    }
  }
}

//------------------------------------------------------------------------------
//setup test
//------------------------------------------------------------------------------
void TESTSETUP_Run(void)
{
  TWord valTime, valSpeed, valTemp;
  TByte key;
  TDWord DWVal;
  TReal32 FVal;
  TWord MinTemp, MaxTemp;
  
  globalDeviceStatus.valueUpdate=false;
  globalTestStatus.State=TEST_STOPPED;

  MinTemp=EEPROM1.MinTemp;
  MaxTemp=EEPROM1.MaxTemp;

  //initial values
  //valTime=(TWord)(EEPROM1.Time/60);
  valTime=(TWord)EEPROM1.Time; 					// 客户确认需要显示秒，2017-11-9 星期四 11:24:14
  
  valSpeed=EEPROM1.Speed;
  valTemp=(TWord)(EEPROM1.Temp*10);
  
  /*if (EEPROM1.Ekey)
  {
    globalDeviceStatus.ekeyStatus=EKEY_Check();
  }*/  

  //run
  for (;;)
  {
    //show current time/speed/temperature values
    //CTL_ShowTime((TDWord)valTime*60, false);
    CTL_ShowTime((TDWord)valTime, false); 				// 显示秒，2017-11-9 星期四 11:25:10
    
    CTL_ShowTemp((TReal32)valTemp/10, false);
    //CTL_ShowSpeed((TReal32)valSpeed, false, false);   // Gavin，ZT120不显示转速

    //wait for key press
    while(!KBD_KeyPressed())
    {
      //if ((!EEPROM1.Ekey)||(globalDeviceStatus.ekeyStatus<EKEY_STATUS_LOCKED))
        EXTERNAL_Process();
      ERROR_Check();
      if (globalDeviceStatus.valueUpdate)
      {
        //valTime=(TWord)(EEPROM1.Time/60);
        valTime=(TWord)EEPROM1.Time; 							// 显示秒，2017-11-9 星期四 11:26:13
        valSpeed=EEPROM1.Speed;
        valTemp=(TWord)(EEPROM1.Temp*10);
        globalDeviceStatus.valueUpdate=false;
        break;
      }
            
      if (globalTestStatus.State==TEST_RUNNING)
        return;
    }
          
    if (KBD_KeyPressed())
    {  
      key=KBD_GetKey();
      
      switch(key)
      {
        case KBD_CODE_START:
        {
          //start the test - save presets
          //if (EEPROM1.Time!=(valTime*60))
          if (EEPROM1.Time!=valTime) 						// 显示秒，2017-11-9 星期四 11:26:59
          {  
            //DWVal=(TDWord)valTime*60;
            DWVal=(TDWord)valTime; 							// 显示秒，2017-11-9 星期四 11:27:42
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Time, (TByte *)&DWVal, sizeof(EEPROM1.Time));
          }
          
          if (EEPROM1.Speed!=valSpeed)
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&valSpeed, sizeof(EEPROM1.Speed));
          
          if (EEPROM1.Temp!=((TReal32)valTemp/10))
          {
             FVal=(TReal32)valTemp/10;
             EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Temp, (TByte *)&FVal, sizeof(EEPROM1.Temp));
          }
          
          TT_s = true; 								// 秒设置标志复位
          
          SND_Play(ssOk);
          return;
        }break;
        case KBD_CODE_QUP:
        {
        	if(TT_s)
        	{
        		//if (TESTSETUP_IncInt(+TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
            if (TESTSETUP_IncInt(+TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
        	}
        	else
        	{
            if (TESTSETUP_IncInt(+TIME_SEC_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
          }
        }break;
        case KBD_CODE_QDOWN:
        {
        	if(TT_s)
        	{
            //if (TESTSETUP_IncInt(-TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
            if (TESTSETUP_IncInt(-TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
          }
          else
          {
          	if (TESTSETUP_IncInt(-TIME_SEC_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
          }
        }break;
        case KBD_CODE_UP:
        {
        	if(TT_s)
        	{
            //if (TESTSETUP_IncInt(+TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
            if (TESTSETUP_IncInt(+TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
          }
          else
          {
          	if (TESTSETUP_IncInt(+TEMP_STEP, &valTemp, MinTemp, MaxTemp))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
          }
        }break;
        case KBD_CODE_DOWN:
        {
        	if(TT_s)
        	{
            //if (TESTSETUP_IncInt(-TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
            if (TESTSETUP_IncInt(-TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
          }
          else
          {
          	if (TESTSETUP_IncInt(-TEMP_STEP, &valTemp, MinTemp, MaxTemp))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
          }
        }break;
        case KBD_CODE_STOP:
        {
        	if(TT_s)
        		TT_s = false;
        	else
        		TT_s = true;
        	
        	SND_Play(ssOk);
        }break;
        default:
        {
          SND_Play(ssError);
        }break;
      }
    }
  }
}                          
