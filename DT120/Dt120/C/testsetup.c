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

//------------------------------------------------------------------------------
//inc integer
//------------------------------------------------------------------------------
TBool TESTSETUP_IncInt(signed char step, TWord *value, TWord lo, TWord hi)
{
  signed short val;
    
  //try to increment
  val=*value+step;
  if (val>hi)
    return false;
  if (val<lo)
    return false;
  *value+=step;
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
  LED_Print_P(LED_DISPLAY_SPEED, false, setTempStr);
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
        case KBD_CODE_TEMP_UP:
          //increment temperature
          if (TESTSETUP_IncInt(+TEMP_STEP, &valTemp, MinTemp, MaxTemp))
            SND_Play(ssSlowUp);
          else
            SND_Play(ssError);
          break;
        case KBD_CODE_TEMP_DOWN:
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
  valTime=(TWord)(EEPROM1.Time/60);
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
    CTL_ShowTime((TDWord)valTime*60, false);
    CTL_ShowTemp((TReal32)valTemp/10, false);
    CTL_ShowSpeed((TReal32)valSpeed, false, false);

    //wait for key press
    while(!KBD_KeyPressed())
    {
      //if ((!EEPROM1.Ekey)||(globalDeviceStatus.ekeyStatus<EKEY_STATUS_LOCKED))
        EXTERNAL_Process();
      ERROR_Check();
      if (globalDeviceStatus.valueUpdate)
      {
        valTime=(TWord)(EEPROM1.Time/60);
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
      
      //if ((!EEPROM1.Ekey)||(globalDeviceStatus.ekeyStatus<EKEY_STATUS_LOCKED))
      //{  
        switch(key)
        {
          case KBD_CODE_START:
            //start the test - save presets
            if (EEPROM1.Time!=(valTime*60))
            {  
              DWVal=(TDWord)valTime*60;
              EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Time, (TByte *)&DWVal, sizeof(EEPROM1.Time));
            }
            if (EEPROM1.Speed!=valSpeed)
              EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&valSpeed, sizeof(EEPROM1.Speed));
            if (EEPROM1.Temp!=((TReal32)valTemp/10))
            {
               FVal=(TReal32)valTemp/10;
               EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Temp, (TByte *)&FVal, sizeof(EEPROM1.Temp));
            }
            SND_Play(ssOk);
            return;
          case KBD_CODE_HOUR_UP:
            if (TESTSETUP_IncInt(+TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_HOUR_DOWN:
            if (TESTSETUP_IncInt(-TIME_HOUR_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_MIN_UP:
            if (TESTSETUP_IncInt(+TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_MIN_DOWN:
            if (TESTSETUP_IncInt(-TIME_MIN_STEP, &valTime, MIN_TIME_VALUE, MAX_TIME_VALUE/60))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_SPEED_UP:
            if (TESTSETUP_IncInt(+SPEED_STEP, &valSpeed, MIN_SPEED_VALUE, MAX_SPEED_VALUE))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_SPEED_DOWN:
            if (TESTSETUP_IncInt(-SPEED_STEP, &valSpeed, MIN_SPEED_VALUE, MAX_SPEED_VALUE))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_TEMP_UP:
            //increment time/strokes
            if (TESTSETUP_IncInt(+TEMP_STEP, &valTemp, MinTemp, MaxTemp))
              SND_Play(ssSlowUp);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_TEMP_DOWN:
            //increment time/strokes
            if (TESTSETUP_IncInt(-TEMP_STEP, &valTemp, MinTemp, MaxTemp))
              SND_Play(ssSlowDown);
            else
              SND_Play(ssError);
            break;
          case KBD_CODE_MENU:
            SND_Play(ssOk);
            CONFIG_Menu();
            break;
          case KBD_CODE_STOP:
            default:
            SND_Play(ssError);
            break;
        }
      /*} else
      {
        if (globalDeviceStatus.ekeyStatus!=EKEY_STATUS_BLOCKED)
        {  
          if (key==KBD_CODE_MENU)
          {
            SND_Play(ssOk);
            CONFIG_Menu();
          } else
          {
            SND_Play(ssError);
          }
        } else
        {
          SND_Play(ssError);
        }  
      }*/
    }
  }
}                          
