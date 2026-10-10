#include <stdio.h>
#include "edit.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "platform.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "version.h"

//------------------------------------------------------------------------------
// EDIT_Bool - edit boolean
//------------------------------------------------------------------------------
TBool EDIT_Bool(char __flash *title)
{
  TByte key;
  char val_yes[]=" YES";
  char val_no[]="  NO";
  TBool val=true;
    
  //print title
  LED_Print_P(LED_DISPLAY_LEFT, false, title);

  for (;;)
  {
    if (val) LED_Print(LED_DISPLAY_RIGHT, false, val_yes);
      else LED_Print(LED_DISPLAY_RIGHT, false, val_no);
          
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
        
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        if (val)
          return true;
        else
          return false;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if (!val)
          val=true;
        SND_Play(ssSlowUp);//TMR0_INT_RATE/8);
        break;
      case KBD_CODE_FAST_DOWN:
      case KBD_CODE_SLOW_DOWN:
        if (val)
          val=false;
        SND_Play(ssSlowDown);//TMR0_INT_RATE/8);           
        break;
      default:
        SND_Play(ssError);//TMR0_INT_RATE/2);
        break;
    }       
  }
}

//------------------------------------------------------------------------------
// EDIT_Time - edit time in minutes
//------------------------------------------------------------------------------
TBool EDIT_Time(char __flash *title, TWord *minutes)
{
#define MAX_MINUTES   (60*24)

  char str[10];
  TWord val;
  TByte key;
  
  //print title
  LED_Print_P(LED_DISPLAY_LEFT, false, title);
  val=*minutes;
  for (;;)
  {
    //format and print value
    sprintf_P(str, timeFmt, val/60, val%60);
    LED_Print(LED_DISPLAY_RIGHT, false, str);

    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        //SND_Play(ssOk);
        *minutes=val;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      case KBD_CODE_SLOW_UP:
        val++;
        if (val==MAX_MINUTES) val=0;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_SLOW_DOWN:
        if (val)
          val--;
        else
          val=MAX_MINUTES-1;
        SND_Play(ssSlowDown);
        break;
      case KBD_CODE_FAST_UP:
        val+=60;
        if (val>=MAX_MINUTES) val-=MAX_MINUTES;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_FAST_DOWN:
        if (val>=60)
          val-=60;
        else
          val=val+MAX_MINUTES-60;
        SND_Play(ssSlowDown);
        break;
      default:
        SND_Play(ssError);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// EDIT_Date - edit date (day and month)
//------------------------------------------------------------------------------
TBool EDIT_Date(TTimeDate *Dt)
{
  char str[10];
  TByte key;
  TTimeDate tmpDt=*Dt;

  for (;;)
  {
    // Check date
    TIMEDATE_Validate(&tmpDt);

    LED_Print_P(LED_DISPLAY_LEFT, false, TIMEDATE_MonthName[tmpDt.Month]);
    sprintf_P(str, dayFmt, tmpDt.Day);
    LED_Print(LED_DISPLAY_RIGHT, false, str);

    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }

    key=KBD_GetKey();
    switch(key)
    {
      case KBD_CODE_START:
        //SND_Play(ssOk);
        *Dt=tmpDt;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      case KBD_CODE_FAST_UP:
        tmpDt.Month++;
        SND_Play(ssFastUp);
        break;
      case KBD_CODE_SLOW_UP:
        tmpDt.Day++;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_FAST_DOWN:
        if (tmpDt.Month)
        tmpDt.Month--;
        SND_Play(ssFastDown);
        break;
      case KBD_CODE_SLOW_DOWN:
        if (tmpDt.Day)
          tmpDt.Day--;
        SND_Play(ssSlowDown);           
        break;
      default:
        SND_Play(ssError);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// EDIT_Show - just show text
//------------------------------------------------------------------------------
TBool EDIT_Show(char *title, char *text, TBool blink)
{
  LED_Print(LED_DISPLAY_LEFT, false, title);
  LED_Print(LED_DISPLAY_RIGHT, blink, text);
  
  while(!KBD_KeyPressed())
  {
    EXTERNAL_Process();
  }
  
  KBD_GetKey();
  
  SND_Play(ssCancel);
  
  return true;
}

//------------------------------------------------------------------------------
// EDIT_Block - block keyboard
//------------------------------------------------------------------------------
void EDIT_Block(void)
{
  while (1)
  {  
    while(!KBD_KeyPressed())
      continue;
    KBD_GetKey();
    SND_Play(ssError);
  }
}

//------------------------------------------------------------------------------
// EDIT_Integer - edit integer
//------------------------------------------------------------------------------
TBool EDIT_Integer(char __flash *title,char __flash *format,TWord *value,
  TWord lo,TWord hi,TEditCallback *callback)
{
  char str[10];
  TWord val;
  TByte key;
    
  val=*value;
  for (;;)
  {
    //print title
    LED_Print_P(LED_DISPLAY_LEFT,false,title);
    //format and print value
    sprintf_P(str,format,val);
    LED_Print(LED_DISPLAY_RIGHT,false,str);
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        //SND_Play(ssOk);
        *value=val;
        return true;
      case KBD_CODE_STOP:
        //SND_Play(ssCancel);
        return false;
      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        val++;
        if (val>hi)
        {  
          val=hi;
          SND_Play(ssError);
        } else
        SND_Play(ssSlowUp);
        if (callback)
          callback(val);
        break;
      case KBD_CODE_FAST_DOWN:
      case KBD_CODE_SLOW_DOWN:
        if ((signed short)val-1>(signed short)lo)
        {  
          val--;
          SND_Play(ssSlowDown);
        } else
        {  
          val=lo;
          SND_Play(ssError);
        }
        if (callback)
          callback(val);
        break;
      default:
        SND_Play(ssError);
        break;
    }       
  }
}

//------------------------------------------------------------------------------
// EDIT_Advanced - advanced edit (serial number and password)
//------------------------------------------------------------------------------
TBool EDIT_Advanced(char *title, char *tStr, TByte len, TBool sound)
{
  TByte key;
  TByte initPos, curPos, stPos;
    
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  stPos = LED_NUM_DIGITS*2 - len;
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
    //print title
    memcpy(&title[2], tStr, initPos);
    LED_Print(LED_DISPLAY_LEFT,false,title);
    //print value
    LED_Print(LED_DISPLAY_RIGHT,false,&tStr[initPos]);
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        if (sound)
          SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        if (sound)
          SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      case KBD_CODE_SLOW_DOWN:
        tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_SLOW_UP:
        tStr[curPos]++;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_FAST_DOWN:
        if (!curPos) curPos=len-1;
          else curPos--;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);
        break;
      case KBD_CODE_FAST_UP:
        curPos++;
        if (curPos==len) curPos=0;
          ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);
        break;
      default:
        SND_Play(ssError);
        break;
    }       
    if (tStr[curPos]>0x39)
      tStr[curPos]=0x30;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x39;
  }
}

//------------------------------------------------------------------------------
// EDIT_AdvancedPW - advanced edit (serial number and password)£¬Gavin£¬2019-11-05 ÐÇÆÚ¶þ 17:30:28
//------------------------------------------------------------------------------
TBool EDIT_AdvancedPW(char *title, char *tStr, TByte len, TBool sound)
{
  TByte key;
  TByte initPos, curPos, stPos;
	char setStr[] = "****";
	
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  stPos = LED_NUM_DIGITS*2 - len;
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
  	setStr[0] = setStr[1] = setStr[2] = setStr[3] = '*';
  	setStr[curPos] = tStr[curPos];
  	
    //print title
    memcpy(&title[2], tStr, initPos);
    LED_Print(LED_DISPLAY_LEFT,false,title);
    //print value
    //LED_Print(LED_DISPLAY_RIGHT,false,&tStr[initPos]);
    LED_Print(LED_DISPLAY_RIGHT,false,setStr);
    
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        if (sound)
          SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        if (sound)
          SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      case KBD_CODE_SLOW_DOWN:
        tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_SLOW_UP:
        tStr[curPos]++;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_FAST_DOWN:
        if (!curPos) curPos=len-1;
          else curPos--;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);
        break;
      case KBD_CODE_FAST_UP:
        curPos++;
        if (curPos==len) curPos=0;
          ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);
        break;
      default:
        SND_Play(ssError);
        break;
    }       
    if (tStr[curPos]>0x39)
      tStr[curPos]=0x30;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x39;
  }
}

//------------------------------------------------------------------------------
// EDIT_Hex - edit HEX-Value
//------------------------------------------------------------------------------
TBool EDIT_Hex(char *title, char *tStr, TByte len)
{
  //char str[10];
  TByte key;
  TByte initPos, curPos, stPos;
    
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  stPos = LED_NUM_DIGITS*2 - len;
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
    //print title
    memcpy(&title[stPos], tStr, initPos);
    LED_Print(LED_DISPLAY_LEFT, false, title);
    //format and print value
    LED_Print(LED_DISPLAY_RIGHT, false, &tStr[initPos]);
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      case KBD_CODE_SLOW_DOWN:
        tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_SLOW_UP:
        tStr[curPos]++;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_FAST_DOWN:
        if (!curPos) curPos=len-1;
          else curPos--;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);       
        break;
      case KBD_CODE_FAST_UP:
        curPos++;
        if (curPos==len) curPos=0;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);    
        break;
      default:
        SND_Play(ssError);
        break;
    }       
    if (tStr[curPos]>0x46)
      tStr[curPos]=0x30;
    if ((tStr[curPos]>0x39)&&(tStr[curPos]<0x3D))
      tStr[curPos]=0x41;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x46;
    if ((tStr[curPos]<0x41)&&(tStr[curPos]>0x3D))
      tStr[curPos]=0x39;
  }
}

//------------------------------------------------------------------------------
// EDIT_Start - show logo and device id
//------------------------------------------------------------------------------
void EDIT_Start(void)
{
  TByte ix, jx, kx;
  TByte l_len, d_len;
  char logo[10];
  char device[10];
  char str[LED_NUM_DIGITS];
  char str2[LED_NUM_DIGITS];
  
  strcpy_P(str, DEVICE_TYPE[EEPROM1.DevType]);
  strcpy_P(str2, DEVICE_MODE[EEPROM1.DevType][EEPROM1.DevMode]);
  sprintf_P(device, devFmt, str, str2);
  //EEPROM_ReadString(logo, EEPROM1.FirmLogo, LOGO_MAX_LEN+1);
  strcpy_P(logo, DEVICE_LOGO[EEPROM1.FirmLogo]);

  for (ix=0; ix<LED_NUM_DIGITS; ix++)
  {  
    str[ix]=' ';
    str2[ix]=' ';
  }
  l_len=strlen(logo);
  d_len=strlen(device);
  if (d_len>l_len)
    ix=d_len;
  else
    ix=l_len;
  ix+=LED_NUM_DIGITS;
  
  for (jx=0; jx<=ix; jx++)
  {
    LED_Print(LED_DISPLAY_LEFT, false, str);
    LED_Print(LED_DISPLAY_RIGHT, false, str2);
    
    TIMER_Start(TIMER_GENERAL, TO_200MSEC);
    while(!TIMER_TimeOut(TIMER_GENERAL));
    TIMER_Stop(TIMER_GENERAL);
    for (kx=1; kx<LED_NUM_DIGITS; kx++)
    {  
      str[kx-1]=str[kx];
      str2[kx-1]=str2[kx];
    }  
    if (jx<l_len)
      str[LED_NUM_DIGITS-1]=logo[jx];
    else
      str[LED_NUM_DIGITS-1]=' ';
    if (jx<d_len)
      str2[LED_NUM_DIGITS-1]=device[jx];
    else
      str2[LED_NUM_DIGITS-1]=' ';
  }  
}
