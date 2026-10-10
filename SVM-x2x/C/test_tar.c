#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "strings.h"
#include "sound.h"
#include "syserror.h"
#include "test.h"

//TReal32 minSpeed, maxSpeed;
TReal32 avrgSpeed, prevSpeed;

TWord time;

//------------------------------------------------------------------------------
// TEST_Stat - statistics on speed
//------------------------------------------------------------------------------
void TEST_Stat(TReal32 current)
{
  TWord Speed=(TWord)(current*10);
  if (globalTestStatus.MinSpeed>Speed)
    globalTestStatus.MinSpeed=Speed;
  if (globalTestStatus.MaxSpeed<Speed)
    globalTestStatus.MaxSpeed=Speed;
  avrgSpeed=0.4*current+0.6*prevSpeed;
  //avrgSpeed=current;
  prevSpeed=avrgSpeed;
}

//------------------------------------------------------------------------------
// TEST_ShowSpeed - show speed on the display
//------------------------------------------------------------------------------
void TEST_ShowSpeed(TBool Led, TReal32 speed, TBool blink)
{
  sprintf_P(globalStr, speedFmt, speed);
  if (!Led)
    LED_Print(LED_DISPLAY_LEFT, blink, globalStr);
  else
    LED_Print(LED_DISPLAY_RIGHT, blink, globalStr);
}

//------------------------------------------------------------------------------
// TEST_ShowTime - show time on the display
//------------------------------------------------------------------------------
void TEST_ShowTime(TWord time,TBool blink)
{
  TByte sec,min,hour;
  static __flash char format2[]="h%01d.%02d";

  if (time>TAR_TIME_MAX)
  {
    strcpy_P(globalStr,timeOver);
    LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
    return;
  }
  sec=time%60;
  min=(time/60)%60;
  hour=(time/(60*60));
  if (time<TAR_TIME_CONVERT)
    sprintf_P(globalStr,timeFmt,min,sec);
  else
    sprintf_P(globalStr,format2,hour,min);
  LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
}

//------------------------------------------------------------------------------
//show strokes
//------------------------------------------------------------------------------
void TEST_ShowRev(TWord rev,TBool blink)
{
  if(rev<=TAR_REVOL_MAX)
    sprintf_P(globalStr,revFmt,rev);
  else
    strcpy_P(globalStr,revOver);  
  LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
}

//------------------------------------------------------------------------------
// TEST_ProcessKey - process key press during the test
//------------------------------------------------------------------------------
TBool TEST_ProcessKey(TBool *paused)
{
  TByte key;
    
  key=KBD_GetKey();
  if (KBD_CODE_START==key)
  {
    if (*paused)
    {
      *paused=false;
      SND_Play(ssOk);
      //MOTOR_GoForward(forward); !!!
      MOTOR_SetSpeed(EEPROM1.Speed);
      MOTOR_Start();
      MOTOR_SetReverse(false);
      //globalStatus.timeTick=false;
      TIMER_ResetTick();
      //while (!globalStatus.timeTick);
      while (!TIMER_TimeTick());
      //globalStatus.timeTick=false;
      TIMER_ResetTick();
    } else SND_Play(ssError);
  } else 
  if (KBD_CODE_STOP==key)
  {
    //if (EEPROM1.Speed>20)
    //  MOTOR_SetSpeed(20);
    //wait a second
    //__delay_cycles(XTAL/4);
    MOTOR_Stop();
    //MOTOR_SetReverse(true);
    SND_Play(ssOk);
    //MOTOR_SetSpeed(EEPROM1.Speed);
    if (*paused)
      return true;
    *paused=true;
  } else SND_Play(ssError);
  return false;
}

//------------------------------------------------------------------------------
// TEST_Done - stop test and do all needed procedures
//------------------------------------------------------------------------------
TBool TEST_Done(TBool error)
{
  TWord temp;
  
  //update screen
  TEST_ShowSpeed(true, (TReal32)EEPROM1.Speed, false);
  if (TIME_UNIT_REVOLUTIONS==EEPROM1.TimeUnits)
    TEST_ShowRev(globalTestStatus.Value,false);
  else
    TEST_ShowTime(globalTestStatus.Value,false);
  //if (EEPROM1.Speed>20)
  //  MOTOR_SetSpeed(20);
  //wait a second
  //__delay_cycles(XTAL/2);
  MOTOR_Stop();
  //MOTOR_SetReverse(true);
  //wait a second
  __delay_cycles(XTAL/2);
  globalStatus.testStart=false;
  globalTestStatus.State=TEST_STOPPED;
  if (error)
    return false;
  if ((EEPROM1.EmptyMode==EMPTY_MODE_MANUAL)|(EEPROM1.DevType==DEVICE_TYPE_TA))
  {
    return true;
  } else
  {
    MOTOR_SetSpeed(SERVICE_SPEED);
    MOTOR_ResetRevolutions();
    MOTOR_SetReverse(true);
    MOTOR_Start();
  }
  //emptying cycle    
  if (!EEPROM1.NumRevRevolutions)
    return true;
  strcpy_P(globalStr,emptying);
  LED_Print(LED_DISPLAY_LEFT,false,globalStr);
  //run
  for (;;)
  {
    while (!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      temp=MOTOR_GetRevolutions();
      //show messages
      sprintf_P(globalStr, revFmt, temp);
      LED_Print(LED_DISPLAY_RIGHT, false, globalStr);
      if (((temp>=EEPROM1.NumRevRevolutions)&&(MOTOR_SensorReady()))||(temp>=EEPROM1.NumRevRevolutions+1))
      {
        MOTOR_Stop();
        __delay_cycles(XTAL/2);
        MOTOR_SetReverse(false);
        return true;
      }
    }
    if (KBD_CODE_STOP==KBD_GetKey())
    {
      MOTOR_Stop();
      __delay_cycles(XTAL/4);
      MOTOR_SetReverse(false);
      SND_Play(ssOk);
      return true;
    } else
    {
      SND_Play(ssError);
    }
  }
}

//------------------------------------------------------------------------------
// TEST_Run - run test
//------------------------------------------------------------------------------
TBool TEST_Run(void)
{
  TBool paused,ledsUpdated,testRev,lastCycle;
  TReal32 speed;
  //number of revolutions when stat is disabled
  TWord statDisabled=3;
  TWord numRevs=0;
  TWord SpeedBuf_Period;
  TWord SpeedBuf_Value;
  TByte SpeedBuf_Index;
  TWord BeepTime;

  globalTestStatus.State=TEST_RUNNING;
  globalTestStatus.Unlimited=false; 
  globalTestStatus.Ready=false;
  if (TIME_UNIT_REVOLUTIONS==EEPROM1.TimeUnits)
  {
    if (0==EEPROM1.TestRevolutions)
    {
      globalTestStatus.Unlimited=true;
      globalTestStatus.Value=0;
    }
    testRev=true;
    globalTestStatus.Value=EEPROM1.TestRevolutions;
  } else
  {
    if (0==EEPROM1.TestTime)
    {
      globalTestStatus.Unlimited=true;
      globalTestStatus.Value=0;
    } else
    {  
      globalTestStatus.Value=EEPROM1.TestTime;
    }
    testRev=false;
  }
  // initialize Speed buffer
  /*for (SpeedBuf_Index=0; SpeedBuf_Index<SPEED_BUF_LEN; SpeedBuf_Index++)
    Speed_Buf[SpeedBuf_Index] = 0.0;
  SpeedBuf_Index = 0;
  if (testRev)
    SpeedBuf_Period=EEPROM1.TestRevolutions/10;
  else
    SpeedBuf_Period=EEPROM1.TestTime/10;
  SpeedBuf_Value=globalTestStatus.Value-SpeedBuf_Period;*/
  globalTestStatus.MinSpeed=TAR_SPEED_MAX*10;
  globalTestStatus.MaxSpeed=0;
  //in pause mode update LED's only one time - cont update prevents from blinking
  BeepTime=EEPROM1.Speed*TAR_TIME_BEEP/60; 
  MOTOR_Stop();
  MOTOR_SetSpeed(EEPROM1.Speed);
  MOTOR_ResetRevolutions();
  MOTOR_SetReverse(false);
  MOTOR_Start();
  KBD_Reset();
  paused=false;
  //time=0;
  //sync
  //globalStatus.timeTick=false;
  TIMER_ResetTick();
  //while (!globalStatus.timeTick);
  //globalStatus.timeTick=false;
  globalTestStatus.Ticker=0;
  //run
  speed=0.0;//(TReal32)MOTOR_GetSpeed()/10;
  TEST_ShowSpeed(true, speed, paused);
  
  globalTestStatus.NumStatRecords=0;
  
  for (;;)
  {
    //wait for key press
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      SYSERROR_Check();
      if (!globalStatus.testStart)
        return TEST_Done(true);
      
      if (numRevs<=TAR_REVOL_MAX) numRevs=MOTOR_GetRevolutions();
      
      if (!ledsUpdated)
      {
          if (testRev)
          {
            if (globalTestStatus.Unlimited)
              globalTestStatus.Value = numRevs;
            else
              globalTestStatus.Value=EEPROM1.TestRevolutions-numRevs;
            TEST_ShowRev(globalTestStatus.Value,paused);
          } else
          {
            TEST_ShowTime(globalTestStatus.Value,paused);
          }
      }

      //time tick?
      //if (globalStatus.timeTick)
      if (TIMER_TimeTick())
      {
        //globalStatus.timeTick=false;
        TIMER_ResetTick();
        //show test parameters
        if (!ledsUpdated)
        {
          if (!paused)
          {
            speed=(TReal32)MOTOR_GetSpeed()/10;
            if (globalTestStatus.Ticker>=statDisabled)
            {  
              TEST_Stat(speed);
            } else
            {  
              avrgSpeed=prevSpeed=speed;
              prevSpeed=EEPROM1.Speed;
            }
          } else
          {
            avrgSpeed=EEPROM1.Speed;
          }
          TEST_ShowSpeed(true, avrgSpeed, paused);

          //TEST_ShowTime(time, paused);
        }
        if (paused)
        {
          ledsUpdated=true;
            continue;
        } else
        {
          ledsUpdated=false;
        }

        if (globalTestStatus.Ticker<TAR_TIME_MAX) globalTestStatus.Ticker++;
        if (globalTestStatus.Unlimited) 
        {  
          if (!testRev)
            globalTestStatus.Value++;
        } else
        {
          if (!testRev)
            globalTestStatus.Value--;
          if ((globalTestStatus.Value<=SpeedBuf_Value))
          {
            if (SpeedBuf_Index<SPEED_BUF_LEN)
            {  
              Speed_Buf[SpeedBuf_Index++]=speed;
              globalTestStatus.NumStatRecords=SpeedBuf_Index;
              SpeedBuf_Value-=SpeedBuf_Period;
            }  
          }

          if (testRev)
          {
            if (globalTestStatus.Value<BeepTime)
              SND_Play(ssWarning);
          } else
          {
            if (globalTestStatus.Value<TAR_TIME_BEEP)
              SND_Play(ssWarning);
          }
        }
      }
      //check if test complete?
      if (globalTestStatus.Unlimited)
        continue;

      numRevs=MOTOR_GetRevolutions();
      if (testRev)
      {
        if (numRevs>=EEPROM1.TestRevolutions)
        {
          globalTestStatus.Ready=true;
          return TEST_Done(false);
        }  
        if (numRevs==(EEPROM1.TestRevolutions-1))
        {  
          //lastCycle=true;
        }
      } else
      {
        if (globalTestStatus.Value==0)
        {  
          globalTestStatus.Ready=true;
          return TEST_Done(false);
        }
        if (globalTestStatus.Value==1)
        {  
          //lastCycle=true;
        }
      }
    }
    
    if (TEST_ProcessKey(&paused))
    {
      /*MOTOR_Stop();
      //wait a second
      __delay_cycles(XTAL/2);
      if ((EEPROM1.DevType==DEVICE_TYPE_TAR)&&(EEPROM1.EmptyMode==EMPTY_MODE_AUTO))
      {
        //reverse
        MOTOR_SetSpeed(SERVICE_SPEED);
        MOTOR_ResetRevolutions();
        MOTOR_SetReverse(true);
        MOTOR_Start();
      }
      if (unlimitedTest)
        return true;
      return false;*/
      return TEST_Done(false);
    } else
    {
      ledsUpdated=false;
      prevSpeed=EEPROM1.Speed;
      statDisabled+=globalTestStatus.Ticker;
    }
  }
}                          
