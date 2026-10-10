#include "const.h"
#include "eeprom.h"
#include "ekey.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "pins.h"
#include "sound.h"
#include "strings.h"
#include "test.h"
#include "testsetup.h"
#include "version.h"

enum CHOOSEMENU_ITEMS
{
  CHOOSEMENU_REV,
  CHOOSEMENU_TIME,
  CHOOSEMENU_LAST
};

TMenuItemStr CHOOSE_LIST_TAR[CHOOSEMENU_LAST]=
{
  "TIME",
  "REV "
};

/*
//------------------------------------------------------------------------------
//inc speed
//------------------------------------------------------------------------------
TBool TESTSETUP_IncSpeed(signed int step,TWord *speed)
{
  TWord val;
    
  //try to increment
  val=*speed+step;
  if (val>(TWord)(TAR_SPEED_MAX))
    return false;
  if (val<(TWord)(TAR_SPEED_MIN))
    return false;
  *speed=val;
  return true;
}
*/

//------------------------------------------------------------------------------
//inc integer
//------------------------------------------------------------------------------
TBool TESTSETUP_IncInt(signed char step,TWord *value,TWord lo,TWord hi)
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
//show speed
//------------------------------------------------------------------------------
void TESTSETUP_ShowSpeed(TWord speed,TBool blink)
{
  sprintf_P(globalStr,setSpeedFmt,speed);
  LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
}

//------------------------------------------------------------------------------
//setup test
//------------------------------------------------------------------------------
void TESTSETUP_Run(void)
{
  TWord val;
  TByte key;
  TWord min,max;
  TWord speed;
  TByte mode;
  TBool start;
  TByte step;
  TByte step_id;
  TByte step_Rev[3]={1, 10, 100};
  TByte step_Time[3]={1, 10, 60};

  //CTL_SetTimeUnits();
  start=false;
  mode=EEPROM1.TimeUnits;
  globalTestStatus.Mode=mode;
  
  if (EEPROM1.Ekey)
  {
    if (globalStatus.ekeyStatus==EKEY_STATUS_BLOCKED)
      EKEY_Check(false);
    else
      globalStatus.ekeyStatus=EKEY_Check(true);
  }  

  LED_Print_P(LED_DISPLAY_LEFT, false, DEVICE_TYPE[EEPROM1.DevType]);

  for (;;)
  {
    LED_Print_P(LED_DISPLAY_RIGHT, false, CHOOSE_LIST_TAR[mode]);
    while(!KBD_KeyPressed())
    {
      //if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_LOCKED))
      EXTERNAL_Process();
      if (globalStatus.testStart)
        return;
      if (globalTestStatus.Mode!=mode)
      {
        mode=globalTestStatus.Mode;
        break;
      }
    }
    
    if (KBD_KeyPressed())
    {  
      key=KBD_GetKey();        

      if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_LOCKED))
      {  
        switch(key)
        {
          case KBD_CODE_START:
          case KBD_CODE_STOP:
            SND_Play(ssOk);
            if (mode!=EEPROM1.TimeUnits)
              EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&mode, sizeof(EEPROM1.TimeUnits));
            start=true;
            break;
  
          case KBD_CODE_FAST_UP:
          case KBD_CODE_SLOW_UP:
            if (mode)
            {  
              mode--;
              SND_Play(ssFastDown);     //repeatable
            } else
            {
              SND_Play(ssError);        //repeatable
            }
            globalTestStatus.Mode=mode;
            break;

          case KBD_CODE_FAST_DOWN:
          case KBD_CODE_SLOW_DOWN:
            mode++;
            if (mode>=CHOOSEMENU_LAST)
            {  
              mode--;
              SND_Play(ssError);        //repeatable
            } else
            {  
              SND_Play(ssSlowDown);     //repeatable
            }
            globalTestStatus.Mode=mode;
            break;
          
          default:
            SND_Play(ssError);
        }       
      } else
      {
        SND_Play(ssError);
      }  
    }
    if (start)
      break;
  }  
      
  //allow editing both speed(revolutions) and time
  mode=EEPROM1.TimeUnits;
  step_id=0;
  if (TIME_UNIT_REVOLUTIONS==mode)
  {
    val=EEPROM1.TestRevolutions;
    min=TAR_REVOL_MIN;
    max=TAR_REVOL_MAX;
  } else
  {
    val=EEPROM1.TestTime;
    min=TAR_TIME_MIN;
    max=TAR_TIME_MAX;
  }
  speed=EEPROM1.Speed;
  //run
  for (;;)
  {
    //show revolutions or time
    if (TIME_UNIT_REVOLUTIONS==mode)
    {  
      TEST_ShowRev(val,false);
      step=step_Rev[step_id];//TAR_CHANGE_STEP;
    } else
    {  
      TEST_ShowTime(val,false);
      if (val>=TAR_TIME_CONVERT)
        step=TAR_CHANGE_STEP_TIME;
      else
        step=step_Time[step_id];//;TAR_CHANGE_STEP;
    }
    
    //show speed
    TESTSETUP_ShowSpeed(speed,false);
    //wait for key press
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      if (globalStatus.valueUpdate)
      {
        mode=EEPROM1.TimeUnits;
        if (mode==TIME_UNIT_TIME)
          val=EEPROM1.TestTime;
        else
          val=EEPROM1.TestRevolutions;
        speed=EEPROM1.Speed;
        globalStatus.valueUpdate=false;
        step_id=0;
        break;
      }
      if (globalStatus.testStart)
        return;
    }

    if (KBD_KeyPressed())
    {  
      key=KBD_GetKey();
      switch(key)
      {
        case KBD_CODE_START: 						// 中下键
          //start the test - save presets
          if (EEPROM1.Speed!=speed)
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&speed, sizeof(EEPROM1.Speed));
          if (TIME_UNIT_REVOLUTIONS==EEPROM1.TimeUnits)
          {
            if (EEPROM1.TestRevolutions!=val)
              EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestRevolutions, (TByte *)&val, sizeof(EEPROM1.TestRevolutions));
          } else
          {
            if (val>=TAR_TIME_CONVERT)
              val=(val/60)*60;
            if (EEPROM1.TestTime!=val)  
              EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestTime, (TByte *)&val, sizeof(EEPROM1.TestTime));
          }
          globalStatus.testStart=true;
          SND_Play(ssOk);
          return;
        case KBD_CODE_SLOW_UP: 					// 右上键
          //increment speed
          if (DEVICE_TYPE_TAR!=EEPROM1.DevType)
          {
            SND_Play(ssError);                    //repeatable
            break;
          }
          if (TESTSETUP_IncInt(+TAR_SPEED_CHANGE_STEP,&speed,TAR_SPEED_MIN,TAR_SPEED_MAX))
            SND_Play(ssOk);                       //repeatable
          else
            SND_Play(ssError);                    //repeatable
          break;
        case KBD_CODE_SLOW_DOWN: 				// 右下键
          //decrement speed
          if (DEVICE_TYPE_TAR!=EEPROM1.DevType)
          {
            SND_Play(ssError);                    //repeatable
            break;
          }
          if (TESTSETUP_IncInt(-TAR_SPEED_CHANGE_STEP,&speed,TAR_SPEED_MIN,TAR_SPEED_MAX))
            SND_Play(ssOk);                       //repeatable
          else
            SND_Play(ssError);                    //repeatable
          break;
        case KBD_CODE_FAST_UP: 					// 左上键
          //increment time/revs
          if (TESTSETUP_IncInt(+step,&val,min,max))
            SND_Play(ssOk);                       //repeatable
          else
            SND_Play(ssError);                    //repeatable
          break;
        case KBD_CODE_FAST_DOWN: 				// 左下键
          //decrement time/revs
          if (TESTSETUP_IncInt(-step,&val,min,max))
            SND_Play(ssOk);                       //repeatable
          else
            SND_Play(ssError);                    //repeatable
          break;
        case KBD_CODE_STOP:
          step_id++;
          if (step_id>2)
            step_id=0;
          SND_Play(ssOk);                         //repeatable
          break;
        default:
          SND_Play(ssError);
          break;
      }   
    }
  }
}                          
