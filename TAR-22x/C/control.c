#include "const.h"
#include "edit.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "printrtc.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "version.h"

TMenuItemStr CONF_LIST_DFORMAT[DFORMAT_MODE_LAST]=
{
  "EUR ",
  "USA "
};

//------------------------------------------------------------------------------
//show test time
//------------------------------------------------------------------------------
void CTL_ShowTime(TWord time, TBool blink)
{
  char str[10];
  
  sprintf_P(str, timeFmt, (int)time/60, (int)time%60);
  LED_Print(LED_DISPLAY_RIGHT, blink, str);
}

//------------------------------------------------------------------------------
//show speed
//------------------------------------------------------------------------------
void CTL_ShowSpeed(TReal32 speed, TBool blink, TBool run)
{
  char str[10];
  
  if (run)
    sprintf_P(str, speedFmt, speed);
  else 
    sprintf_P(str, setSpeedFmt, (TWord)speed);
  LED_Print(LED_DISPLAY_RIGHT, blink, str);
}

//------------------------------------------------------------------------------
// CTL_SetTime - set time
//------------------------------------------------------------------------------
void CTL_SetTime(void)
{
  //TByte key;
  //char str[10];
  //TTimeDate tmpDt;                                 
  TWord Minutes;
            
  RTC_GetDateTime(&curDTime);
  Minutes=curDTime.Hour*60 + curDTime.Minute;
  
  if (EDIT_Time(timeStr, &Minutes))
  {
    curDTime.Hour=Minutes/60;
    curDTime.Minute=Minutes%60;
    curDTime.Second=0;
    if (RTC_SetDateTime(curDTime))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  }
}

//------------------------------------------------------------------------------
// CTL_SetDate - set date
//------------------------------------------------------------------------------
void CTL_SetDate(void)
{
  //TByte key;
  //char str[10];
  TTimeDate tmpDt;                                 
            
  RTC_GetDateTime(&curDTime);
  tmpDt = curDTime;
  if (EDIT_Date(&tmpDt))
  {
    curDTime.Day=tmpDt.Day;
    curDTime.Month=tmpDt.Month;
    if (RTC_SetDateTime(curDTime))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);                          
  }
}
                
//------------------------------------------------------------------------------
// CTL_SetYear - set year
//------------------------------------------------------------------------------
void CTL_SetYear(void)
{
  //TByte key;
  //char str[10];
  //TTimeDate tmpDt;
  TWord year;
            
  RTC_GetDateTime(&curDTime);
  year=curDTime.Year;
  if (EDIT_Integer(yearStr, yearFmt, &year, 0, 99, NULL))
  {
    //set year
    curDTime.Year=year;
    if (RTC_SetDateTime(curDTime))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  } else SND_Play(ssCancel);
}

//------------------------------------------------------------------------------
// CTL_SetDFormat - set date/time format
//------------------------------------------------------------------------------
void CTL_SetDFormat(void)
{
  signed char value;
  TList list;
    
  list.numItems=DFORMAT_MODE_LAST;
  list.items=CONF_LIST_DFORMAT;
  //list.current=eepromDFormat;
  list.current=EEPROM1.DFormat;
  list.title=formatStr;

  value=MENU_RunList(&list);
  if ((value>=0)&&(value<DFORMAT_MODE_LAST))
    //eepromDFormat=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DFormat, (TByte *)&value, sizeof(EEPROM1.DFormat));
}

//------------------------------------------------------------------------------
// CTL_SetPwst - set pwst, v2.20, Gavin, 2019-08-08 星期四 09:00:19
//------------------------------------------------------------------------------
void CTL_SetPwst(void)
{
	char title[]="PWST";
	TWord value=0;
	TWord value1 = 0;
	char tStr[PASSWORD_LEN];
	//TByte i;
	
	value = EEPROM1.MGPassword;
	tStr[0] = 0x30 + value/1000; 	value %= 1000;
	tStr[1] = 0x30 + value/100; 	value %= 100;
	tStr[2] = 0x30 + value/10;
	tStr[3] = 0x30 + value%10;
	      
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, false))
	{
		value = (TWord)(tStr[0]-0x30)*1000 + (TWord)(tStr[1]-0x30)*100 + (TWord)(tStr[2]-0x30)*10 + (TWord)(tStr[3]-0x30);
		
		if(EEPROM1.MGPassword != value)
		{
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MGPassword, (TByte *)&value, sizeof(EEPROM1.MGPassword));
			value1 = value^0xaaaa;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MGPWC, (TByte *)&value1, sizeof(EEPROM1.MGPWC));
		}
		
		SND_Play(ssOk);
  }
  else
	{
		SND_Play(ssCancel);
	}
}

//------------------------------------------------------------------------------
// CTL_SetPwdt - set pwdt, v2.20, Gavin, 2019-08-08 星期四 09:00:19
//------------------------------------------------------------------------------
void CTL_SetPwdt(void)
{
	char title[]="DATE";
	TWord value=0;
	TWord value1 = 0;
	char tStr[PASSWORD_LEN];
	//TByte i;
	
	value = EEPROM1.DTPassword;
	tStr[0] = 0x30 + value/1000; 	value %= 1000;
	tStr[1] = 0x30 + value/100; 	value %= 100;
	tStr[2] = 0x30 + value/10;
	tStr[3] = 0x30 + value%10;
	      
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, false))
	{
		value = (TWord)(tStr[0]-0x30)*1000 + (TWord)(tStr[1]-0x30)*100 + (TWord)(tStr[2]-0x30)*10 + (TWord)(tStr[3]-0x30);
		
		if(EEPROM1.DTPassword != value)
		{
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DTPassword, (TByte *)&value, sizeof(EEPROM1.DTPassword));
			value1 = value^0xaaaa;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DTPWC, (TByte *)&value1, sizeof(EEPROM1.DTPWC));
		}
		
		SND_Play(ssOk);
  }
  else
	{
		SND_Play(ssCancel);
	}
}

//------------------------------------------------------------------------------
// CTL_CheckPW - Check password, v2.20, Gavin, 2019-08-08 星期四 09:00:19
//------------------------------------------------------------------------------
signed char CTL_CheckPW(char *title, TWord code)
{
	//char title[]="PWMG";
	TWord value=0;
	char tStr[] = "****";
	//TByte i;
	signed char rt = 0;
	
	//value = EEPROM1.DTPassword;
	
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, false))
	{
		SND_Play(ssOk);
		
		value = (TWord)(tStr[0]-0x30)*1000 + (TWord)(tStr[1]-0x30)*100 + (TWord)(tStr[2]-0x30)*10 + (TWord)(tStr[3]-0x30);
		
		if(value == code)
			rt = 0;
		else
			rt = -1;
	}
  else
	{
		SND_Play(ssCancel);
		rt = -1;
	}
	
	return rt;
}
