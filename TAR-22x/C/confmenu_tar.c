#include "confmenu.h"
#include "control.h"
#include "const.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "global.h"
#include "led.h"
#include "menu.h"
#include "printrtc.h"
#include "sound.h"
#include "strings.h"
#include "version.h"

enum CONFMENU_TAR
{
  CONFMENU_TAR_UNITS,
  CONFMENU_TAR_RPM,
  CONFMENU_TAR_EMPTYING,
  CONFMENU_TAR_SOUND,
  CONFMENU_TAR_PRINT,
  CONFMENU_TAR_PTYPE,
  CONFMENU_TAR_DATE,
  CONFMENU_TAR_BRIGHTNESS,
  CONFMENU_TAR_VERSION,
  CONFMENU_TAR_EKEY,
  CONFMENU_TAR_PWMG, 										// 密码管理菜单，Gavin，v2.20，2019-08-07 星期三 15:30:20
  CONFMENU_TAR_EXIT,
  CONFMENU_TAR_LAST
};

enum CONFMENU_TA
{
  CONFMENU_TA_UNITS,
  CONFMENU_TA_SOUND,
  CONFMENU_TA_PRINT,
  CONFMENU_TA_PTYPE,
  CONFMENU_TA_DATE,
  CONFMENU_TA_BRIGHTNESS,
  CONFMENU_TA_VERSION,
  CONFMENU_TA_EKEY,
  CONFMENU_TA_EXIT,
  CONFMENU_TA_LAST
};

enum CONFMENU_FT
{
  CONFMENU_FT_UNITS,
  CONFMENU_FT_EMPTYING,
  CONFMENU_FT_SOUND,
  CONFMENU_FT_PRINT,
  CONFMENU_FT_PTYPE,
  CONFMENU_FT_DATE,
  CONFMENU_FT_BRIGHTNESS,
  CONFMENU_FT_VERSION,
  CONFMENU_FT_EKEY,
  CONFMENU_FT_EXIT,
  CONFMENU_FT_LAST
};
  
TMenuItemStr CONF_MENU_TITLE="CONF";

TMenuItemStr CONF_MENU_TAR[CONFMENU_TAR_LAST]=
{
  "UNIT",
  "RPM",
  "EMPT",
  "SND ",
  "PRNT",
  "P.TYP",
  "DATE",
  "LED",
  "VER",
  "EKEY",
  "PWMG", 															// 密码管理菜单，Gavin，v2.20，2019-08-07 星期三 15:30:20
  "EXIT"
};

TMenuItemStr CONF_MENU_TA[CONFMENU_TA_LAST]=
{
  "UNIT",
  "SND ",
  "PRNT",
  "P.TYP",
  "DATE",
  "LED",
  "VER",
  "EKEY",
  "EXIT"
};

TMenuItemStr CONF_MENU_FT[CONFMENU_FT_LAST]=
{
  "UNIT",
  "EMPT",
  "SND ",
  "PRNT",
  "P.TYP",
  "DATE",
  "LED",
  "VER",
  "EKEY",
  "EXIT"
};

TMenuItemStr CONF_LIST_UNITS[TIME_UNIT_LAST]=
{
  "TIME",
  "REV "
};
  
TMenuItemStr CONF_LIST_EMPTY[EMPTY_MODE_LAST]=
{
  "AUTO",
  "MAN "
};

TMenuItemStr CONF_LIST_SOUND[SOUND_MODE_LAST]=
{
  "OFF ",
  "ON  "
};

TMenuItemStr CONF_LIST_PRINT[PRINTER_MODE_LAST]=
{
  "OFF ",
  "ON  ",
  "ASK "
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

// 增加PWMG的子菜单，v2.20 Gavin，2019-08-07 星期三 15:33:24
TMenuItemStr CONF_LIST_PWMG[PWMG_MODE_LAST]=
{
	"PWST",
	"DATE"
};

//------------------------------------------------------------------------------
// CONF_Units - configure units
//------------------------------------------------------------------------------
TBool CONF_Units(void)
{
  signed char value;
  TList list;
    
  list.numItems=TIME_UNIT_LAST;
  list.items=CONF_LIST_UNITS;
  list.current=EEPROM1.TimeUnits;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_UNITS];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&value, sizeof(EEPROM1.TimeUnits));
  return true;
}

//------------------------------------------------------------------------------
// CONF_Rpm - configure rotation speed
//------------------------------------------------------------------------------
TBool CONF_Rpm(void)
{
  //static __flash char title[]="RPM";
  static __flash char format[]="%4d";
  TWord value;
    
  value=EEPROM1.Speed;
  if (EDIT_Integer(CONF_MENU_TAR[CONFMENU_TAR_RPM],format,&value,TAR_SPEED_MIN,TAR_SPEED_MAX,NULL))
  {  
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&value, sizeof(EEPROM1.Speed));
    SND_Play(ssOk);
  } else
  {
    SND_Play(ssCancel);
  }  
  return true;
}

//------------------------------------------------------------------------------
// CONF_Emptying - configure emptying mode
//------------------------------------------------------------------------------
TBool CONF_Emptying(void)
{
  signed char value;
  TList list;
  TWord val;
    
  list.numItems=EMPTY_MODE_LAST;
  list.items=CONF_LIST_EMPTY;
  list.current=EEPROM1.EmptyMode;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_EMPTYING];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.EmptyMode, (TByte *)&value, sizeof(EEPROM1.EmptyMode));
  val=EEPROM1.NumRevRevolutions;
  if (EEPROM1.EmptyMode==EMPTY_MODE_AUTO)
  {  
    if (EDIT_Integer(CONF_LIST_EMPTY[EMPTY_MODE_AUTO], revEnterFmt, &val, TAR_REVERSE_MIN, TAR_REVERSE_MAX, NULL))
    {  
      EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.NumRevRevolutions, (TByte *)&val, sizeof(EEPROM1.NumRevRevolutions));
      SND_Play(ssOk);
    } else
    {
      SND_Play(ssCancel);
    }
  }  

  return true;
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
  list.current=EEPROM1.Sound;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_SOUND];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&value, sizeof(EEPROM1.Sound));
  if (value)
    SND_On();
  else
    SND_Off();
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
  list.current=EEPROM1.Print;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_PRINT];
  value=MENU_RunList(&list);
  if ((value<0)||(value>=PRINTER_MODE_LAST))
    //return false;
    return true;
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
  list.current=EEPROM1.PrinterType;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_PTYPE];
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
  TWord value1=0; 											// v2.20, Gavin, 2019-08-15 星期四 15:11:12

  list.numItems=DTIME_MODE_LAST;
  list.items=CONF_LIST_DATE;
  list.current=0;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_DATE];
  
  // 密码检查，v2.20，Gavin，2019-08-08 星期四 14:18:24
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
// CONF_Brightness - set LED's brightness
//------------------------------------------------------------------------------
void OnBrightness(TWord val)
{
  LED_SetBrightness(val);
}
  
TBool CONF_Brightness(void)
{
  static __flash char title[]="LED ";
  static __flash char format[]="%4d";
  TWord value;
    
  value=EEPROM1.LED;
  if (EDIT_Integer(title, format, &value, LED_BRIGHTNESS_0, LED_BRIGHTNESS_NUM-1, OnBrightness))
  {  
    //eepromLED=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&value, sizeof(EEPROM1.LED));
    SND_Play(ssOk);
  } else
  {  
    LED_SetBrightness(EEPROM1.LED);
    SND_Play(ssCancel);
  }  
  return true;
}

//------------------------------------------------------------------------------
// CONF_Version - show DT firmware version
//------------------------------------------------------------------------------
TBool CONF_Version(void)
{
  char str[10];
  char str2[5];
    
  strcpy_P(str, VERSION_Revision);
  strcpy_P(str2, CONF_MENU_TAR[CONFMENU_TAR_VERSION]);
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
// CONF_PWMG - pwmg menu，增加PWMG菜单，v2.20，Gavin，2019-08-07 星期三 15:53:11
//------------------------------------------------------------------------------
TBool CONF_Pwmg(void)
{
	char title[]="PWMG";
	signed char value;
  TList list;
	TWord value1=0; 											// v2.20, Gavin, 2019-08-15 星期四 15:11:12

  list.numItems=PWMG_MODE_LAST;
  list.items=CONF_LIST_PWMG;
  list.current=0;
  list.title=CONF_MENU_TAR[CONFMENU_TAR_PWMG];
  
  // 密码检查，v2.20，Gavin，2019-08-08 星期四 14:18:24
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
  ptrTItemHandler handler[CONFMENU_TAR_LAST];

  menu.title=CONF_MENU_TITLE;
  menu.handler=handler;

    
  if (DEVICE_TYPE_TAR==EEPROM1.DevType)
  {
    menu.numItems=CONFMENU_TAR_LAST;
    menu.items=CONF_MENU_TAR;
    handler[CONFMENU_TAR_UNITS]=CONF_Units;
    handler[CONFMENU_TAR_RPM]=CONF_Rpm;
    handler[CONFMENU_TAR_EMPTYING]=CONF_Emptying;
    handler[CONFMENU_TAR_SOUND]=CONF_Sound;
    handler[CONFMENU_TAR_PRINT]=CONF_Print;
    handler[CONFMENU_TAR_PTYPE]=CONF_PrintType;
    handler[CONFMENU_TAR_DATE]=CONF_DateTime;
    handler[CONFMENU_TAR_BRIGHTNESS]=CONF_Brightness;
    handler[CONFMENU_TAR_VERSION]=CONF_Version;
    handler[CONFMENU_TAR_EKEY]=CONF_Ekey;
    handler[CONFMENU_TAR_PWMG]=CONF_Pwmg; 				// 增加PWMG菜单，v2.20，Gavin，2019-08-07 星期三 15:52:10
    handler[CONFMENU_TAR_EXIT]=CONF_Exit;
  } 
  else
  if (DEVICE_TYPE_TA==EEPROM1.DevType)
  {
    menu.numItems=CONFMENU_TA_LAST;
    menu.items=CONF_MENU_TA;
    handler[CONFMENU_TA_UNITS]=CONF_Units;
    handler[CONFMENU_TA_SOUND]=CONF_Sound;
    handler[CONFMENU_TA_PRINT]=CONF_Print;
    handler[CONFMENU_TA_PTYPE]=CONF_PrintType;
    handler[CONFMENU_TA_DATE]=CONF_DateTime;
    handler[CONFMENU_TA_BRIGHTNESS]=CONF_Brightness;
    handler[CONFMENU_TA_VERSION]=CONF_Version;
    handler[CONFMENU_TA_EKEY]=CONF_Ekey;
    handler[CONFMENU_TA_EXIT]=CONF_Exit;
  } 
  else
  {
    menu.numItems=CONFMENU_FT_LAST;
    menu.items=CONF_MENU_FT;
    handler[CONFMENU_FT_UNITS]=CONF_Units;
    handler[CONFMENU_FT_EMPTYING]=CONF_Emptying;
    handler[CONFMENU_FT_SOUND]=CONF_Sound;
    handler[CONFMENU_FT_PRINT]=CONF_Print;
    handler[CONFMENU_FT_PTYPE]=CONF_PrintType;
    handler[CONFMENU_FT_DATE]=CONF_DateTime;
    handler[CONFMENU_FT_BRIGHTNESS]=CONF_Brightness;
    handler[CONFMENU_FT_VERSION]=CONF_Version;
    handler[CONFMENU_FT_EKEY]=CONF_Ekey;
    handler[CONFMENU_FT_EXIT]=CONF_Exit;
  }
    
  MENU_Run(&menu);
}
