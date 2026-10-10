#include "confmenu.h"
#include "const.h"
#include "control.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "printrtc.h"
#include "report.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "test.h"
#include "testsetup.h"
#include "version.h"

enum CONFMENU_SVM
{
  CONFMENU_SVM_SOUND,
  CONFMENU_SVM_BRIGHTNESS,
  CONFMENU_SVM_PRINT,
  CONFMENU_SVM_PTYPE,
  CONFMENU_SVM_DATE,
  CONFMENU_SVM_OQ,
  CONFMENU_SVM_VERSION,
  CONFMENU_SVM_EKEY,
  CONFMENU_SVM_PWMG, 										// 增加密码管理菜单，v1.30，Gavin，2019-08-08 星期四 15:29:36
  CONFMENU_SVM_IVOL, 										// Gavin, 2020-05-19 星期二 15:32:36
  CONFMENU_SVM_EXIT,
  CONFMENU_SVM_LAST
};

TMenuItemStr CONF_MENU_TITLE="CONF";

TMenuItemStr CONF_MENU_SVM[CONFMENU_SVM_LAST]=
{
  "SND",
  "LED",
  "PRNT",
  "P.TYP",
  "DATE",
  "OQ",
  "VER",
  "EKEY",
  "PWMG", 															// 增加密码管理菜单，v1.30，Gavin，2019-08-08 星期四 15:29:36
  "I.VOL", 															// Gavin, 2020-05-19 星期二 15:31:23
  "EXIT"
};

TMenuItemStr CONF_LIST_SOUND[SOUND_MODE_LAST]=
{
  "OFF ",
  "ON  "
};

TMenuItemStr CONF_LIST_PRINT[PRINTER_MODE_LAST]=
{
  "OFF ",
  "AUTO",
  "ASK"
};

TMenuItemStr CONF_LIST_PTYPE[PRINTER_TYPE_LAST]=
{
  "NORM",
  "PORT"
};

TMenuItemStr CONF_LIST_DATE[DTIME_MODE_LAST]=
{
  "TIME",
  "DAY",
  "YEAR",
  "FRMT",
  "VER"
};

TMenuItemStr CONF_LIST_OQ[OQ_MODE_LAST]=
{
  "HGHT",
  "SPM ",
  "RTIM",
  "OQPR"
};

// 增加PWMG的子菜单，v1.30 Gavin，2019-08-07 星期三 15:33:24
TMenuItemStr CONF_LIST_PWMG[PWMG_MODE_LAST]=
{
	"PWST",
	"DATE"
};

// Gavin, 2020-05-19 星期二 15:37:43
TMenuItemStr CONF_LIST_IVOL[IVOL_MODE_LAST]=
{
	"  V0",
	" V10"
};

static __flash char heightFmt[]="%4.1f";

//------------------------------------------------------------------------------
// CTL_GetHeight - get stroke height (OQ)
//------------------------------------------------------------------------------
void CTL_GetHeight(void)
{
  TByte key, count, ix;
  TByte DevMode;
  TByte index=0;
  TBool Running;
  TBool Stopped;
  TBool Exit;
  TWord hValues[2]={SVM_OQ_HEIGHT_1, SVM_OQ_HEIGHT_2};
  TWord value;
  char str[10];

  LED_Print_P(LED_DISPLAY_LEFT, false, CONF_LIST_OQ[OQ_MODE_HGHT]);

  DevMode=EEPROM1.DevMode;
  
  switch (DevMode)
  {
    case DEVICE_MODE_121:
    case DEVICE_MODE_221:
      index=1;
      count=2;
      break;
    case DEVICE_MODE_122:
    case DEVICE_MODE_222:
      index=0;
      count=1;
      break;
    case DEVICE_MODE_223:
      index=0;
      count=2;
      break;
  }  
  
  ix=0;
  
  for (; index<count; index++)
  {
    Running=false;
    Stopped=false;
    Exit=false;

    sprintf_P(str, heightFmt, (TReal32)hValues[index]/10);
    LED_Print(LED_DISPLAY_RIGHT, Stopped, str);
    
    for (;;)
    {
      while(!KBD_KeyPressed())
        EXTERNAL_Process();

      key=KBD_GetKey();        
    
      switch(key)
      {
        case KBD_CODE_START:
          SND_Play(ssOk);
          Stopped=true;
          break;
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        default:  
          SND_Play(ssError);
      }  
      
      if (Stopped)
        break;
    }  
    
    Stopped=false;
    Running=true;
    value=hValues[index];
    
    for (;;)
    {        
      sprintf_P(str, heightFmt, (TReal32)value/10);
      LED_Print(LED_DISPLAY_RIGHT, Stopped, str);
      
      if ((Running)&&(!Stopped))
      {
        MOTOR_SetSpeed(MOTOR_MIN_SPEED);
        MOTOR_ResetRevolutions();
        MOTOR_Start();
      }
  
      while(!KBD_KeyPressed())
        EXTERNAL_Process();
  
      key=KBD_GetKey();        
  
      switch(key)
      {
        case KBD_CODE_START:
          if (Running)
            SND_Play(ssError);
          else 
          if (Stopped)
          {
            OQReport.RHeight[ix]=value;
            SND_Play(ssOk);
            Exit=true;
            break;
          } else
          {
            Running=true;
            SND_Play(ssOk);
          }
          break;
  
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          if (!Running)
            return;
          MOTOR_Stop();
          OQReport.Height[ix]=value;
          Running=false;
          Stopped=true;
          break;
  
        case KBD_CODE_FAST_UP:
        case KBD_CODE_SLOW_UP:
          if ((Stopped)&&(TESTSETUP_IncInt(SVM_CHANGE_STEP, &value, SVM_OQ_MIN_HEIGHT, SVM_OQ_MAX_HEIGHT)))
            SND_Play(ssSlowUp);
          else
            SND_Play(ssError);
          break;
        case KBD_CODE_SLOW_DOWN:
        case KBD_CODE_FAST_DOWN:
          if ((Stopped)&&(TESTSETUP_IncInt(-SVM_CHANGE_STEP, &value, SVM_OQ_MIN_HEIGHT, SVM_OQ_MAX_HEIGHT)))
            SND_Play(ssSlowDown);
          else
            SND_Play(ssError);
          break;
        default:
          SND_Play(ssError);
      }       
  
      if (Exit)
        break;
    }
    
    ix++;
  }

  /*for (;;)
  {
    sprintf_P(str, heightFmt, (TReal32)hValues[index]/10);
    LED_Print(LED_DISPLAY_RIGHT, Stopped, str);

    while(!KBD_KeyPressed())
      EXTERNAL_Process();
    
    key=KBD_GetKey();        

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        Stopped=true;
        break;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return;
      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if (index>0)
        {
          index--;
          SND_Play(ssSlowUp);
        } else
        {
          SND_Play(ssError);
        } 
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        if (index<1)
        {
          index++;
          SND_Play(ssSlowDown);
        } else
        {
          SND_Play(ssError);
        } 
        break;        
    }
    if (Stopped)
      break;
  }  
  
  Stopped=false;
  Running=true;
  value=hValues[index];
  
  for (;;)
  {        
    sprintf_P(str, heightFmt, (TReal32)value/10);
    LED_Print(LED_DISPLAY_RIGHT, Stopped, str);
    
    if ((Running)&&(!Stopped))
    {
      MOTOR_SetSpeed(MOTOR_MIN_SPEED);
      MOTOR_ResetRevolutions();
      MOTOR_Start();
    }

    while(!KBD_KeyPressed())
      EXTERNAL_Process();

    key=KBD_GetKey();        

    switch(key)
    {
      case KBD_CODE_START:
        if (Running)
          SND_Play(ssError);
        else 
        if (Stopped)
        {
          OQReport.RHeight=value;
          SND_Play(ssOk);
          return;
        } else
        {
          Running=true;
          //MOTOR_SetSpeed(MOTOR_MIN_SPEED);
          //MOTOR_ResetRevolutions();
          //MOTOR_Start();
          SND_Play(ssOk);
        }
        break;

      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        if (!Running)
          return;
        MOTOR_Stop();
        OQReport.Height=value;
        Running=false;
        Stopped=true;
        break;

      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if ((Stopped)&&(TESTSETUP_IncInt(SVM_CHANGE_STEP, &value, SVM_OQ_MIN_HEIGHT, SVM_OQ_MAX_HEIGHT)))
          SND_Play(ssSlowUp);
        else
          SND_Play(ssError);
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        if ((Stopped)&&(TESTSETUP_IncInt(-SVM_CHANGE_STEP, &value, SVM_OQ_MIN_HEIGHT, SVM_OQ_MAX_HEIGHT)))
          SND_Play(ssSlowDown);
        else
          SND_Play(ssError);
        break;
      default:
        SND_Play(ssError);
    }       
  }*/
}

//------------------------------------------------------------------------------
// CTL_GetStrokes - get strokes number (OQ)
//------------------------------------------------------------------------------
void CTL_GetStrokes(void)
{
  TByte key;
  TBool Running=false;
  TBool Stopped=false;
  TWord value=SVM_OQ_STROKES;
            
  MOTOR_ResetRevolutions();
  for(;;)
  {        
    LED_Print_P(LED_DISPLAY_LEFT, false, CONF_LIST_OQ[OQ_MODE_SPM]);
    TEST_ShowRev(value, Stopped);
       
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      if (Running)
        TEST_ShowRev(value-MOTOR_GetRevolutions(), false);
      if (Stopped) continue;
      if (!(value-MOTOR_GetRevolutions()))
      {
        OQReport.Strokes=value;
        Running=false;
        Stopped=true;
        MOTOR_Stop();
        MOTOR_ResetRevolutions();
        break;
      }
    }

    if (KBD_KeyPressed())
    {
      key=KBD_GetKey();        

      switch(key)
      {
        case KBD_CODE_START:
          if (Running)
            SND_Play(ssError);
          else 
          if (Stopped)
          {
            OQReport.RStrokes=value;
            SND_Play(ssOk);
            return;
          } else
          {
            Running=true;
            MOTOR_SetSpeed(60);
            MOTOR_ResetRevolutions();
            MOTOR_Start();
            SND_Play(ssOk);
          }
          break;
              
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          if (!Running)
            return;
          MOTOR_Stop();
          OQReport.Strokes=value;
          Running=false;
          Stopped=true;
          return;

        case KBD_CODE_FAST_UP:
        case KBD_CODE_SLOW_UP:
          if ((Stopped)&&(TESTSETUP_IncInt(SVM_CHANGE_STEP, &value, SVM_REV_MIN, SVM_REV_MAX)))
            SND_Play(ssSlowUp);
          else
            SND_Play(ssError);
          break;
        case KBD_CODE_SLOW_DOWN:
        case KBD_CODE_FAST_DOWN:
          if ((Stopped)&&(TESTSETUP_IncInt(-SVM_CHANGE_STEP, &value, SVM_REV_MIN, SVM_REV_MAX)))
            SND_Play(ssSlowDown);
          else
            SND_Play(ssError);
          break;
        default:
          break;
      }
    }
  }
}

//------------------------------------------------------------------------------
// CTL_GetRTime - get runtime (OQ)
//------------------------------------------------------------------------------
void CTL_GetRTime(void)
{
  TByte key;
  TBool Running=false;
  TBool Stopped=false;
  TWord value, setValue;

  LED_Print_P(LED_DISPLAY_LEFT, false, CONF_LIST_OQ[OQ_MODE_RTIM]);
   
  setValue=SVM_OQ_TIME;
  
  for (;;)
  {
    TEST_ShowTime(setValue, Stopped);    

    while (!KBD_KeyPressed())
      EXTERNAL_Process();
    
    key=KBD_GetKey();        
    
    switch (key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        Stopped=true;
        break;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return;
      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if ((TESTSETUP_IncInt(SVM_OQ_TIME_STEP, &setValue, SVM_OQ_MIN_TIME, SVM_OQ_MAX_TIME)))
          SND_Play(ssSlowDown);
        else
          SND_Play(ssError);
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        if ((TESTSETUP_IncInt(-SVM_OQ_TIME_STEP, &setValue, SVM_OQ_MIN_TIME, SVM_OQ_MAX_TIME)))
          SND_Play(ssSlowDown);
        else
          SND_Play(ssError);
        break;
    }  
    
    if (Stopped)
      break;
  }  
  
  TIMER_ResetTick();
  Stopped=false;
  Running=true;
  value=0;

  for (;;)
  {        
    TEST_ShowTime(value, Stopped);
       
    while (!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      if (Running)
      {  
        if (TIMER_TimeTick()) 
          break;
      } else 
      {  
        TIMER_ResetTick();
      }  
    }
        
    if (TIMER_TimeTick())
    {
      TIMER_ResetTick();
      value++;
      if (value>=setValue)
      {
        //OQReport.Time=value;
        //Running=false;
        //Stopped=true;
        SND_Play(ssOk);
      } else
      {
        continue;
      }  
    } 
    
    if (KBD_KeyPressed())
      key=KBD_GetKey();        
    else
      key=KBD_CODE_STOP;

    switch(key)
    {
      case KBD_CODE_START:
        if (Running)
          SND_Play(ssError);
        else 
        if (Stopped)
        {
          OQReport.RTime=value;
          SND_Play(ssOk);
          return;
        } else
        {
          Running=true;
          TIMER_ResetTick();
          SND_Play(ssOk);
        }
        break;
              
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        if (!Running)
          return;
        OQReport.Time=value;
        Running=false;
        Stopped=true;
        break;

      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if (Stopped)
        {
          TESTSETUP_IncInt(1, &value, 0, SVM_TIME_CONVERT);
          SND_Play(ssOk);
        } else SND_Play(ssError);
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        if (Stopped)
        {
          TESTSETUP_IncInt(-1, &value, 0, SVM_TIME_CONVERT);
          SND_Play(ssOk);
        } else SND_Play(ssError);
        break;
      default:
        break;
    }
  }
}

//------------------------------------------------------------------------------
// CTL_Print - print OQ report
//------------------------------------------------------------------------------
void CTL_Print(void)
{
  REPORT_Print(REPORT_TYPE_OQ);//QReport);
}

//------------------------------------------------------------------------------
// CONF_Sound - configure sound
//------------------------------------------------------------------------------
TBool CONF_Sound(void)
{
  signed char value;
  TList list;
    
  list.numItems=SOUND_MODE_LAST;
  list.items=CONF_LIST_SOUND;
  //list.current=eepromSound;
  list.current=EEPROM1.Sound;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_SOUND];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromSound=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&value, sizeof(EEPROM1.Sound));
  if (value==SOUND_MODE_ON)
    SND_On();
  else
    SND_Off();
  return true;
}

//------------------------------------------------------------------------------
// CONF_Brightness - set LED's brightness
//------------------------------------------------------------------------------
void OnBrightness(TWord val)
{
  LED_SetBrightness(val);
  //eepromLED=val;
}

TBool CONF_Brightness(void)
{
  static __flash char title[]="LED ";
  static __flash char format[]="%4d";
  TWord value;
  
  if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_ALARM2))
  {  
    value=EEPROM1.LED;
    if (EDIT_Integer(title,format,&value,LED_BRIGHTNESS_0,LED_BRIGHTNESS_NUM-1,OnBrightness))
    {  
      EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&value, sizeof(EEPROM1.LED));
      SND_Play(ssOk);
    } else
    {  
      LED_SetBrightness(EEPROM1.LED);
      SND_Play(ssCancel);
    }
  } else
  {
    SND_Play(ssError);
  }  
  return true;
}

//------------------------------------------------------------------------------
// CONF_Print - configure print
//------------------------------------------------------------------------------
TBool CONF_Print(void)
{
  signed char value;
  TList list;
  
  list.numItems=PRINTER_MODE_LAST;
  list.items=CONF_LIST_PRINT;
  //list.current=eepromPrint;
  list.current=EEPROM1.Print;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_PRINT];
  value=MENU_RunList(&list);
  if ((value<0)||(value>=PRINTER_MODE_LAST))
    //return false;
    return true;
  //eepromPrint=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&value, sizeof(EEPROM1.Print));
  return true;
}

//------------------------------------------------------------------------------
// CONF_PrintType - configure printer type
//------------------------------------------------------------------------------
TBool CONF_PrintType(void)
{
  signed char value;
  TList list;
    
  list.numItems=PRINTER_TYPE_LAST;
  list.items=CONF_LIST_PTYPE;
  //list.current=eepromPrint;
  list.current=EEPROM1.PrinterType;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_PTYPE];
  value=MENU_RunList(&list);
  if ((value<0)||(value>PRINTER_TYPE_LAST))
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PrinterType, (TByte *)&value, sizeof(EEPROM1.PrinterType));
  return true;
}

//------------------------------------------------------------------------------
// CONF_PrinterVersion - show printer board firmware version
//------------------------------------------------------------------------------
TBool CONF_PrinterVersion(void)
{
  char str[10];
  char str2[5];
  TByte status;
  TByte Vers[2];

  status=PRN_GetVersion((TWord*)Vers);
  if (status!=stsOk)
  {
    SND_Play(ssError);
    return true;
  }  
  sprintf_P(str, prnVerFmt, Vers[0], Vers[1]);
  strcpy_P(str2, CONF_LIST_DATE[DTIME_MODE_VER]);
  EDIT_Show(str2, str, false);
  return true;
}

//------------------------------------------------------------------------------
// CONF_DateTime - configure date and time
//------------------------------------------------------------------------------
TBool CONF_DateTime(void)
{
  char title[]="DATE";
  signed char value;
  TList list;
  TWord value1 = 0; 										// v1.30, Gavin, 2019-08-15 星期四 16:23:28
    
  list.numItems=DTIME_MODE_LAST;
  list.items=CONF_LIST_DATE;
  list.current=0;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_DATE];
  
  // 密码检查，v1.30，Gavin，2019-08-08 星期四 14:18:24
  if(EEPROM1.DTPassword != 9999)
  {
  	if(EEPROM1.DTPassword == (EEPROM1.DTPWC^0xaaaa))
  	{
			if(CTL_CheckPW(title, EEPROM1.DTPassword) == -1)
				return true;
		}
		else
		{
			value1 = 9999;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DTPassword, (TByte *)&value1, sizeof(EEPROM1.DTPassword));
		}
	}
	
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case DTIME_MODE_TIME: // configure time
        CTL_SetTime();
        break;
      case DTIME_MODE_DAY: // configure date
        CTL_SetDate();
        break;
      case DTIME_MODE_YEAR: // configure year
        CTL_SetYear();
        break;
      case DTIME_MODE_FORMAT: // configure date/time format
        CTL_SetDFormat();
        break;
      case DTIME_MODE_VER:  // show printer board version
        CONF_PrinterVersion(); 
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// CONF_OQ - configure OQ
//------------------------------------------------------------------------------
TBool CONF_OQ(void)
{
  signed char value;
  TList list;
    
  list.numItems=OQ_MODE_LAST;
  list.items=CONF_LIST_OQ;
  list.current=0;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_OQ];
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case OQ_MODE_HGHT: // configure height
        CTL_GetHeight();
        break;
      case OQ_MODE_SPM: // configure
        CTL_GetStrokes();
        break;
      case OQ_MODE_RTIM: // configure
        CTL_GetRTime();
        break;
      case OQ_MODE_OQPR: // configure
        CTL_Print();
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// CONF_Version - show DT firmware version
//------------------------------------------------------------------------------
TBool CONF_Version(void)
{
  char str[10];
  char str2[5];
    
  strcpy_P(str, VERSION_Revision);
  strcpy_P(str2, CONF_MENU_SVM[CONFMENU_SVM_VERSION]);
  EDIT_Show(str2, str, false);
  return true;
}

//------------------------------------------------------------------------------
// CONF_Ekey - configure Ekey
//------------------------------------------------------------------------------
TBool CONF_Ekey(void)
{
  if (EEPROM1.Ekey)
  {  
    SND_Play(ssOk);            
    EKEY_Menu();
  } else SND_Play(ssError);

  return true;
}

//------------------------------------------------------------------------------
// CONF_Pwmg - configure Pwmg, v1.30, Gavin, 2019-08-08 星期四 15:34:19
//------------------------------------------------------------------------------
TBool CONF_Pwmg(void)
{
	char title[]="PWMG";
	signed char value;
  TList list;
  TWord value1 = 0; 										// v1.30，Gavin，2019-08-15 星期四 16:28:14
    
  list.numItems=PWMG_MODE_LAST;
  list.items=CONF_LIST_PWMG;
  list.current=0;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_PWMG];
  
  // 密码检查，v1.30，Gavin，2019-08-08 星期四 14:18:24
  if(EEPROM1.MGPassword != 9999)
  {
  	if(EEPROM1.MGPassword == (EEPROM1.MGPWC^0xaaaa))
  	{
			if(CTL_CheckPW(title, EEPROM1.MGPassword) == -1)
				return true;
		}
		else
		{
			value1 = 9999;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MGPassword, (TByte *)&value1, sizeof(EEPROM1.MGPassword));
		}
	}
	
  for (;;)
  {
  	value=MENU_RunList(&list);
    switch(value)
    {
      case PWMG_MODE_PWST: 						// 设置管理密码
        CTL_SetPwst();
        break;
      case PWMG_MODE_DATE: 						// 设置修改日期密码
        CTL_SetPwdt();
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// CONF_IVol - configure Vol, v1.30, Gavin, 2020-05-19 星期二 15:44:20
//------------------------------------------------------------------------------
TBool CONF_IVol(void)
{
  signed char value;
  TList list;
	
  list.numItems=IVOL_MODE_LAST;
  list.items=CONF_LIST_IVOL;
  list.current=EEPROM1.IVol;
  list.title=CONF_MENU_SVM[CONFMENU_SVM_IVOL];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.IVol, (TByte *)&value, sizeof(EEPROM1.IVol));
  
  return true;
}

//------------------------------------------------------------------------------
// CONF_Exit - exit menu
//------------------------------------------------------------------------------
TBool CONF_Exit(void)
{
  return false;
}

//------------------------------------------------------------------------------
// CONFIG_Menu - run configuration menu
//------------------------------------------------------------------------------
void CONFIG_Menu(void)
{
  TMenu menu;
  ptrTItemHandler handler[CONFMENU_SVM_LAST];

  menu.title=CONF_MENU_TITLE;
  menu.handler=handler;
  menu.numItems=CONFMENU_SVM_LAST;
  menu.items=CONF_MENU_SVM;
  handler[CONFMENU_SVM_SOUND]=CONF_Sound;
  handler[CONFMENU_SVM_BRIGHTNESS]=CONF_Brightness;
  handler[CONFMENU_SVM_PRINT]=CONF_Print;
  handler[CONFMENU_SVM_PTYPE]=CONF_PrintType;
  handler[CONFMENU_SVM_DATE]=CONF_DateTime;
  handler[CONFMENU_SVM_OQ]=CONF_OQ;
  handler[CONFMENU_SVM_VERSION]=CONF_Version;
  handler[CONFMENU_SVM_EKEY]=CONF_Ekey;
  handler[CONFMENU_SVM_PWMG]=CONF_Pwmg; 					// 增加密码管理菜单，v1.30，Gavin，2019-08-08 星期四 15:33:01
  handler[CONFMENU_SVM_IVOL]=CONF_IVol; 					// Gavin, 2020-05-19 星期二 15:57:10
  handler[CONFMENU_SVM_EXIT]=CONF_Exit;
  MENU_Run(&menu);
}
