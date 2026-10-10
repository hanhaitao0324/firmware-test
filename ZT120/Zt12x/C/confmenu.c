#include "calibrate.h"
#include "confmenu.h"
#include "const.h"
#include "control.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "engineering.h"
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
#include "temp.h"
#include "timedate.h"
#include "version.h"

enum CONFMENU_TEST
{
  CONFMENU_VOL,
  CONFMENU_METH,
  // 屏蔽以下两行; Gavin 20131224-------
  //CONFMENU_SAMS,
  //CONFMENU_FOFF,
  CONFMENU_PRINT,
  CONFMENU_TEST_EXIT,
  CONFMENU_TEST_LAST
};

enum CONFMENU_CONF
{
  CONFMENU_SOUND,
  CONFMENU_ALARM,
  CONFMENU_STOP,
  CONFMENU_LED,
  CONFMENU_DATE,
  
  CONFMENU_PWMG, 						// 增加密码管理菜单，v1.03，HHT，2024-06-14  
  
  CONFMENU_PRN,
  CONFMENU_TOFF,
  CONFMENU_HEAT,
  CONFMENU_ADJU, 												// 2018-7-12 星期四 21:18:55
  CONFMENU_CHKT, 												// Gavin, 2019-03-29 星期五 14:26:41, A6, 代替CALI
  CONFMENU_VER,
  CONFMENU_CONF_EXIT,
  CONFMENU_CONF_LAST
};

enum USERMENU_LIST
{
  USERMENU_TEST,
  USERMENU_CONF,
  USERMENU_LAST
};

TMenuItemStr CONFMENU_TITLE="CONF";

TMenuItemStr USERMENU_TITLE="USER";

TMenuItemStr USER_MENU[USERMENU_LAST]=
{
  "TEST",
  "CONF"
};

TMenuItemStr CONF_MENU_TEST[CONFMENU_TEST_LAST]=
{
  "VOL",
  "METH",
  // 屏蔽以下两行; Gavin 20131224-------
  //"SAMS",
  //"F.OFF",
  "PRNT",
  "EXIT"
};

TMenuItemStr CONF_MENU_CONF[CONFMENU_CONF_LAST]=
{
  "SND ",
  "ALRM",
  "STOP",
  "LED ",
  "DATE",
  
  "PWMG", 						// 增加密码管理菜单，v1.03，HHT，2024-06-14
  
  "PRN ",
  "T.OFF",
  "HEAT",
  "ADJU", 															// 2018-7-12 星期四 21:19:25
  "CALI", 															// Gavin, 2019-03-29 星期五 14:27:55, A6
  "VER",
  "EXIT"
};
/*
  "HEAD",
  "STIR",
  "COUN",
  "AJ.PT",
  "AJ.HE",
  //"CA.AS", 						// Gavin 20131224---
  "CAL.T",
  //"A.POS", 						// Gavin 20131224---
  //"M.OFF", 						// Gavin 20131224---
  "DFLT",
  "USE ",
  "REP ",
  //"EKEY",
  "PORT",
  
};
*/

TMenuItemStr CONF_LIST_PRN[PRN_MODE_LAST]=
{
	"P.MOD",
	"P.TYP",
	"LANG"
};

// 2018-7-12 星期四 21:20:25
TMenuItemStr CONF_LIST_ADJU[ADJU_MODE_LAST]=
{
	"INT ",
	"EXT "
};

TMenuItemStr CONF_LIST_SOUND[SOUND_MODE_LAST]=
{
  "OFF ",
  "ON  "
};

TMenuItemStr CONF_LIST_ALARM[ALARM_MODE_LAST]=
{
  "OFF ",
  "ON  "
};

TMenuItemStr CONF_LIST_STOP[STOP_MODE_LAST]= 
{
	"ON  ", 							// 2017-12-12 星期二 20:59:22
	"OFF "
};

TMenuItemStr CONF_LIST_DATE[DTIME_MODE_LAST]=
{
  "TIME",
  "DAY",
  "YEAR",
  "FRMT"
};

TMenuItemStr CONF_LIST_DFORMAT[DFORMAT_MODE_LAST]=
{
  "EUR ",
  "USA "
};

TMenuItemStr CONF_LIST_PRINT[PRINTER_MODE_LAST]=
{
  "OFF ",
  "AUTO",
  "ASK "
};

TMenuItemStr CONF_LIST_HEAT[HEAT_MODE_LAST]=
{
	"OFF ",
	"ON  "
};

TMenuItemStr CONF_LIST_HEAD[HEAD_MODE_LAST]=
{
  "LH",
  "HH"
};

TMenuItemStr CONF_LIST_STIR[MOTOR_MODE_LAST]=
{
  "STOP",
  "GO  "
};

TMenuItemStr CONF_LIST_COUNT[TESTCNT_MODE_LAST]=
{
  "DOWN",
  "UP  "
};

TMenuItemStr CONF_LIST_METH[METH_MODE_LAST]=
{
  "BSKT",
  "PADL",
  "POVD",
  "CYLN"
};

TMenuItemStr CONF_LIST_VOLUME[VOLUME_MODE_LAST]=
{
  //"4000",
  //"2000",
  //"1900",
  //"1750",
  //"1500",
  // Gavin 20131224---------------------
  "1000",
  "900",
  "750",
  "500"
};

TMenuItemStr CONF_LIST_DEPTH[DEPTH_MODE_LAST]=
{
  "1000",
  "900",
  "750",
  "500"
};

TMenuItemStr CONF_LIST_SAMS[SAMPLER_MODE_LAST]=
{
  "OFF",
  "AUTO"
};

TMenuItemStr CONF_LIST_REPORT[REPORT_MODE_LAST]=
{
  "LAST",
  //"INTR", 				// 2017/1/14 11:09:17,客户要求不再需要转速和温度的更新频率参数
  //"PAGE",
  "LANG",
  "P.TYP" 					// 2015/11/26 17:22:13
};

TMenuItemStr CONF_LIST_LANGUAGE[LANGUAGE_MODE_LAST]=
{
  "ENGL",
  "GERM",
  "FREN", 				// 2015/11/26 17:26:22
  "ITAL",
  "SPAN"
};

// 2015/11/26 17:27:23
TMenuItemStr CONF_LIST_PTYPE[PTYPE_MODE_LAST]=
{
	"NORM",
	"PORT"
};
// 2015/11/26 17:27:29 end

TMenuItemStr CONF_LIST_CHECK[CHECK_MODE_LAST]=
{
  "INT", 																// Gavin, 2019-03-29 星期五 14:59:28, A6, "PT",
  "EXT" 																// Gavin, 2019-03-29 星期五 14:59:43, A6, "HEAT."
  // Gavin 2013/12/30 20:06:51----------
  //"AS"
};

// 增加PWMG的子菜单，v1.03，HHT，2024-06-14
TMenuItemStr CONF_LIST_PWMG[PWMG_MODE_LAST]=
{
	"PWST",
	"DATE"
};

//------------------------------------------------------------------------------
// CONF_Head - configure head option
//------------------------------------------------------------------------------
/*TBool CONF_Head(void)
{
  signed char value;
  TList list;
    
  //if (globalDeviceType!=DEVICE_DT620)
  //Gavin 20131224----------------------------------------
  if((globalDeviceType!=DEVICE_DT520)&&
  	 (globalDeviceType!=DEVICE_QDT6)&& 						// Gavin,2014/4/18 15:04:22
  	 (globalDeviceType!=DEVICE_QDT6S)) 						// Gavin,2014/4/18 15:04:37
  {
    list.numItems=HEAD_MODE_LAST;
    list.items=CONF_LIST_HEAD;
    list.current=EEPROM1.Head;
    list.title=CONF_MENU_CONF[CONFMENU_HEAD];
    value=MENU_RunList(&list);
    if (value<0)
      return true;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Head, (TByte *)&value, sizeof(EEPROM1.Head));
    globalDeviceStatus.Head=EEPROM1.Head;
  }
  else
  	SND_Play(ssError);
  
  return true;
}*/

//------------------------------------------------------------------------------
// CONF_Methode - configure stirrer methode
//------------------------------------------------------------------------------
TBool CONF_Methode(void)
{
  signed char value;
  TList list;
    
  list.numItems=METH_MODE_LAST;
  list.items=CONF_LIST_METH;
  list.current=EEPROM1.Method;
  //list.title=CONF_MENU_DT[CONFMENU_METH];
  list.title=CONF_MENU_TEST[CONFMENU_METH];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromAlarm=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Method, (TByte *)&value, sizeof(EEPROM1.Method));
  return true;
}

//------------------------------------------------------------------------------
// CONF_Volume - configure volume value for autosampler
//------------------------------------------------------------------------------
TBool CONF_Volume(void)
{
  signed char value;
  TList list;
    
  list.numItems=VOLUME_MODE_LAST;
  list.items=CONF_LIST_VOLUME;
  list.current=EEPROM1.Volume;
  //list.title=CONF_MENU_DT[CONFMENU_VOL];
  list.title=CONF_MENU_TEST[CONFMENU_VOL];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Volume, (TByte *)&value, sizeof(EEPROM1.Volume));
  return true;
}

//------------------------------------------------------------------------------
// CONF_Filter - configure sampler filter
//------------------------------------------------------------------------------
TBool CONF_Filter(void)
{
  //if (globalDeviceType!=DEVICE_DT620)
  // Gavin 20131224-----------------------------------------
  //if (globalDeviceType!=DEVICE_DT520)
  //{
  	;
  	// 屏蔽以下两行；Gavin 20131224-----
    //LED_Print_P(LED_DISPLAY_TEMP, false, CONF_MENU_TEST[CONFMENU_FOFF]);
    //CTL_SetFilter();
  //}
  //else
  	SND_Play(ssError);
  
  return true;
}
  
//------------------------------------------------------------------------------
// CONF_SamplerMode - configure sampler mode
//------------------------------------------------------------------------------
TBool CONF_SamplerMode(void)
{
  //signed char value;
  //TList list;
    
  //if (globalDeviceType!=DEVICE_DT620)
  
  /*
  if (globalDeviceType!=DEVICE_DT520)
  {
    list.numItems=SAMPLER_MODE_LAST;
    list.items=CONF_LIST_SAMS;
    list.current=EEPROM1.Sams;
    // 屏蔽以下一行；Gavin 20131224-----
    //list.title=CONF_MENU_TEST[CONFMENU_SAMS];
    value=MENU_RunList(&list);
    if (value<0)
      //return false;
      return true;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sams, (TByte *)&value, sizeof(EEPROM1.Sams));
  }
  else*/
  	SND_Play(ssError);
  
  return true;
}

//------------------------------------------------------------------------------
// CONF_Stir - configure stirrer on completion of the test
//------------------------------------------------------------------------------
/*TBool CONF_Stir(void)
{
  signed char value;
  TList list;
    
  list.numItems=MOTOR_MODE_LAST;
  list.items=CONF_LIST_STIR;
  list.current=EEPROM1.MotorStop;
  list.title=CONF_MENU_CONF[CONFMENU_STIR];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromMotorStop=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MotorStop, (TByte *)&value, sizeof(EEPROM1.MotorStop));
  return true;
}*/

//------------------------------------------------------------------------------
// CONF_Count - configure direction of time counting
//------------------------------------------------------------------------------
/*TBool CONF_Count(void)
{
  signed char value;
  TList list;
    
  list.numItems=TESTCNT_MODE_LAST;
  list.items=CONF_LIST_COUNT;
  list.current=EEPROM1.TestCount;
  //list.title=CONF_MENU_DT[CONFMENU_COUNT];
  list.title=CONF_MENU_CONF[CONFMENU_COUNT];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromTestCount=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestCount, (TByte *)&value, sizeof(EEPROM1.TestCount));
  return true;
}*/

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
  //list.title=CONF_MENU_DT[CONFMENU_SOUND];
  list.title=CONF_MENU_CONF[CONFMENU_SOUND];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromSound=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&value, sizeof(EEPROM1.Sound));
  if (value)
    SND_On();
  else
    SND_Off();
  return true;
}

//------------------------------------------------------------------------------
// CONF_Alarm - configure alarm before test end
//------------------------------------------------------------------------------
TBool CONF_Alarm(void)
{
  signed char value;
  TList list;
    
  list.numItems=ALARM_MODE_LAST;
  list.items=CONF_LIST_ALARM;
  list.current=EEPROM1.Alarm;
  //list.title=CONF_MENU_DT[CONFMENU_ALARM];
  list.title=CONF_MENU_CONF[CONFMENU_ALARM];
  value=MENU_RunList(&list);
  if (value<0)
    //return false;
    return true;
  //eepromAlarm=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Alarm, (TByte *)&value, sizeof(EEPROM1.Alarm));
  return true;
}

//------------------------------------------------------------------------------
// CONF_Stop - configure Stop before test end
//------------------------------------------------------------------------------
TBool CONF_Stop(void)
{
  signed char value;
  TList list;
    
  list.numItems=STOP_MODE_LAST;
  list.items=CONF_LIST_STOP;
  list.current=EEPROM1.MotorStop;
  list.title=CONF_MENU_CONF[CONFMENU_STOP];
  
  value=MENU_RunList(&list);
  
  if (value<0)
    //return false;
    return true;
  //eepromAlarm=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MotorStop, (TByte *)&value, sizeof(EEPROM1.MotorStop));
  return true;
}

//------------------------------------------------------------------------------
// CONF_Brightness - set LED's brightness 数码管亮度
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
  if (EDIT_Integer(title,format,&value,LED_BRIGHTNESS_0,LED_BRIGHTNESS_NUM-1,OnBrightness))
    //eepromLED=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&value, sizeof(EEPROM1.LED));
  return true;
}

//------------------------------------------------------------------------------
// CONF_TempOffs - configure temperature offset 配置温度偏移
//------------------------------------------------------------------------------
TBool CONF_TempOffs(void)
{
  //LED_Print_P(LED_DISPLAY_TEMP, false, CONF_MENU_DT[CONFMENU_TOFF]);
  
  //LED_Print_P(LED_DISPLAY_TEMP, false, CONF_MENU_CONF[CONFMENU_TOFF]);
  LED_Print_P(LED_DISPLAY_TIME, false, CONF_MENU_CONF[CONFMENU_TOFF]);
  
  //CTL_SetOffset(ADC_CHNL_EXTERNAL);
  CTL_SetOffset(ADC_CHNL_INTERNAL);
  return true;
}

//------------------------------------------------------------------------------
// CONF_Heat - configure Heat at test
//------------------------------------------------------------------------------
TBool CONF_Heat(void)
{
  signed char value;
  TList list;
    
  list.numItems=HEAT_MODE_LAST;
  list.items=CONF_LIST_HEAT;
  list.current=EEPROM1.HeatFlag;
  list.title=CONF_MENU_CONF[CONFMENU_HEAT];
  
  value=MENU_RunList(&list);
  
  if (value<0)
    //return false;
    return true;
  //eepromAlarm=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.HeatFlag, (TByte *)&value, sizeof(EEPROM1.HeatFlag));
  return true;
}

//------------------------------------------------------------------------------
// CONF_CalPt - one-point Adjustment for Pt100 PT100单点校正
//------------------------------------------------------------------------------
TBool CONF_CalPt(void)
{
  if ((globalDeviceStatus.installedExtADC)&&(!globalDeviceStatus.sensorExtError))
  {
    SND_Play(ssOk);            
    CTL_Adjustment(1);
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// CONF_CalHt - one-point Adjustment for the external heater 对外部加热器单点校准
//------------------------------------------------------------------------------
TBool CONF_CalHt(void)
{
  //if (!globalDeviceStatus.dhcFailure)
  if ((globalDeviceStatus.installedInsADC)&&(!globalDeviceStatus.sensorInsError))
  {
    SND_Play(ssOk);            
    CTL_AdjustmentDHC(1);
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// CONF_CalAs - one-point Adjustment for the autosampler
//------------------------------------------------------------------------------
TBool CONF_CalAs(void)
{
  if (!globalDeviceStatus.samplerFailure)
  {
    SND_Play(ssOk);            
    CTL_AdjustmentAutosampler(1);
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// CONF_CheckTemp - check temperature with 3 points
//------------------------------------------------------------------------------
// Gavin, 2019-03-29 星期五 14:35:07, A6
TBool CONF_CheckTemp(void)
{
  signed char value;
  TList list;
    
  list.numItems=CHECK_MODE_LAST;
  list.items=CONF_LIST_CHECK;
  //list.current=CHECK_MODE_PT;
  list.current=CHECK_MODE_INT; 										// Gavin, 2019-03-29 星期五 15:04:16, A6
  list.title=CONF_MENU_CONF[CONFMENU_CHKT];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  switch (value)
  {
    //case CHECK_MODE_PT: 			// 外部PT100已经连接且没有报错
    case CHECK_MODE_EXT: 								// Gavin, 2019-03-29 星期五 15:08:32, A6
      //if ((globalDeviceStatus.installedExtADC)&&(!globalDeviceStatus.sensorExtError))
      if ((globalDeviceStatus.installedExtADC)&&(!globalDeviceStatus.sensorExtError))
        CTL_CheckTemperature(CALIBRATE_ADC);
      else
        SND_Play(ssError);
      break;
    //case CHECK_MODE_HEATER: 	// 加热器
    case CHECK_MODE_INT: 								// Gavin, 2019-03-29 星期五 15:09:12, A6
      //if (!globalDeviceStatus.dhcFailure)
      if ((globalDeviceStatus.installedInsADC)&&(!globalDeviceStatus.sensorInsError))
        CTL_CheckTemperature(CALIBRATE_DHC);
      else
        SND_Play(ssError);
      break;
    default:
      SND_Play(ssError);
      break;
  }  
  return true;  
}

//------------------------------------------------------------------------------
// CONF_AutosamplerPos - calibrate positions of autosampler
//------------------------------------------------------------------------------
TBool CONF_AutosamplerPos(void)
{
  if ((globalDeviceStatus.haveStepper)&&(globalDeviceStatus.Head==HEAD_MODE_LOW))
  {
    SND_Play(ssOk);            
    CTL_AdjustmentStepper();
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// CONF_MethodOffset - configure method offset
//------------------------------------------------------------------------------
TBool CONF_MethodOffset(void)
{
  signed char value;
  TList list;

  if ((globalDeviceStatus.haveStepper)&&(globalDeviceStatus.Head==HEAD_MODE_LOW))
  {
    SND_Play(ssOk);            
    
    list.numItems=METH_MODE_LAST;
    list.items=CONF_LIST_METH;
    list.current=0;
    //list.title=CONF_MENU_CONF[CONFMENU_MOFF]; 						// Gavin 20131224
    while (1)
    {  
      value=MENU_RunList(&list);
      if (value<0)
        //return false;
        return true;
      CTL_MethodOffset(value);
    }  
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// CONF_Default - reset settings to default
//------------------------------------------------------------------------------
/*TBool CONF_Default(void)
{
  TByte key;
 
  LED_Print_P(LED_DISPLAY_TEMP, false, CONF_MENU_CONF[CONFMENU_DFLT]);
  LED_Print_P(LED_DISPLAY_SPEED, false, rstStr);
        
  while(!KBD_KeyPressed())
  {
    EXTERNAL_Process();
  }

  key=KBD_GetKey();        

  if (key==KBD_CODE_START)
  {
    EEPROM_LoadDefault();
    TEMP_Init();
    SND_Play(ssOk);            
  } else
  {
    SND_Play(ssCancel);
  }
  return true;
}*/

//------------------------------------------------------------------------------
// CONF_Usage - display hour of operation counter
//------------------------------------------------------------------------------
TBool CONF_Usage(void)
{
  char str[10];

  sprintf_P(str, usageDayFmt, (TWord)(EEPROM1.UsageCounter/(3600L*24)));
  LED_Print(LED_DISPLAY_TEMP, false, str);
  sprintf_P(str, usageHourFmt, (TByte)((EEPROM1.UsageCounter/3600)%24), (TByte)((EEPROM1.UsageCounter%3600)/60));
  LED_Print(LED_DISPLAY_SPEED, false, str);
        
  while(!KBD_KeyPressed())
  {
    EXTERNAL_Process();
  }
  
  KBD_GetKey();
  SND_Play(ssOk);
  
  return true;
}

//------------------------------------------------------------------------------
//2017/1/14 11:11:17,客户要求不再需要转速和温度的更新频率参数
/*void CTL_SetInterval(void)
{
  static __flash char format[]="%4d";
  TWord value;
    
  value=EEPROM1.ReportInterval;
  if (EDIT_Integer(CONF_LIST_REPORT[REPORT_MODE_INTR],format,&value,MIN_REPORT_INTERVAL,MAX_REPORT_INTERVAL,NULL))
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.ReportInterval, (TByte *)&value, sizeof(EEPROM1.ReportInterval));
  return;
}*/

/*void CTL_SetPage(void)
{
  static __flash char format[]="%4d";
  TWord value;
    
  value=EEPROM1.ReportLines;
  if (EDIT_Integer(CONF_LIST_REPORT[REPORT_MODE_PAGE],format,&value,MIN_REPORT_LINES,MAX_REPORT_LINES,NULL))
    //eepromReportLines=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.ReportLines, (TByte *)&value, sizeof(EEPROM1.ReportLines));
  return;
}*/

//------------------------------------------------------------------------------
// CONF_SetDFormat - set date/time format (USA/Europe)
//------------------------------------------------------------------------------
void CONF_SetDFormat(void)
{
  signed char value;
  TList list;
    
  list.numItems=DFORMAT_MODE_LAST;
  list.items=CONF_LIST_DFORMAT;
  list.current=EEPROM1.DFormat;
  list.title=CONF_LIST_DATE[DTIME_MODE_FORMAT];//CONF_DFORMAT_TITLE;

  value=MENU_RunList(&list);
  if ((value>=0)&&(value<DFORMAT_MODE_LAST))
    //eepromDFormat=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DFormat, (TByte *)&value, sizeof(EEPROM1.DFormat));
}

//------------------------------------------------------------------------------
// CONF_DateTime - configure date and time
//------------------------------------------------------------------------------
TBool CONF_DateTime(void)
{
    char title[]="DATE";
    
    signed char value;
    TList list;
    
    TWord value1 = 0; 										// v1.03，HHT，2024-06-14

    list.numItems=DTIME_MODE_LAST;
    list.items=CONF_LIST_DATE;
    list.current=0;
    //list.title=CONF_MENU_DT[CONFMENU_DATE];
    list.title=CONF_MENU_CONF[CONFMENU_DATE];

    // 密码检查，v1.03，HHT，2024-06-14
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
            CONF_SetDFormat();
            break;
          case -1:
            return true;
        }
    }
}

void CTL_SetLanguage(void)
{
  signed char value;
  TList list;
    
  list.numItems=LANGUAGE_MODE_LAST - 3; // 只保留英语和德语，后面的3种语言屏蔽；Gavin 2015/11/27 11:28:15
  list.items=CONF_LIST_LANGUAGE;
  list.current=EEPROM1.ReportLanguage;
  list.title=CONF_LIST_REPORT[REPORT_MODE_LANG];
  value=MENU_RunList(&list);
  if (value<0)
    return;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.ReportLanguage, (TByte *)&value, sizeof(EEPROM1.ReportLanguage));
  return;
}

void CTL_SetPType(void)
{
	signed char value;
	TList list;
	
	list.numItems = PTYPE_MODE_LAST;
	list.items = CONF_LIST_PTYPE;
	
	list.current = EEPROM1.ReportPType;
	
	list.title = CONF_LIST_REPORT[REPORT_MODE_PTYP];
	
	value = MENU_RunList(&list);
	if(value < 0)
		return;
	
	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.ReportPType, (TByte *)&value, sizeof(EEPROM1.ReportPType));
	return;
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
  //list.title=CONF_MENU_DT[CONFMENU_PRINT];
  list.title=CONF_LIST_PRN[PRN_MODE_PMOD];
  value=MENU_RunList(&list);
  if ((value<0)||(value>2))
    //return false;
    return true;
  //eepromPrint=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&value, sizeof(EEPROM1.Print));
  return true;
}

//------------------------------------------------------------------------------
// CONF_PRN - configure prn
//------------------------------------------------------------------------------
TBool CONF_Prn(void)
{
  signed char value;
  TList list;
  
  list.numItems=PRN_MODE_LAST;
  list.items=CONF_LIST_PRN;
  list.current=0;
  //list.title=CONF_MENU_DT[CONFMENU_PRINT];
  list.title=CONF_MENU_CONF[CONFMENU_PRN];
  
  /*
  value=MENU_RunList(&list);
  if ((value<0)||(value>2))
    //return false;
    return true;
  //eepromPrint=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&value, sizeof(EEPROM1.Print));
  return true;
  */
  
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case PRN_MODE_PMOD: // 
      	CONF_Print();
        break;
      case PRN_MODE_PTYP: // 
        CTL_SetPType();
        break;
      case PRN_MODE_LANG: // 
      	CTL_SetLanguage();
      	break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// CONF_ADJU - configure ADJu 2018-7-12 星期四 21:21:11
//------------------------------------------------------------------------------
TBool CONF_ADJu(void)
{
  signed char value;
  TList list;
  
  list.numItems=ADJU_MODE_LAST;
  list.items=CONF_LIST_ADJU;
  list.current=0;
  //list.title=CONF_MENU_DT[CONFMENU_PRINT];
  list.title=CONF_MENU_CONF[CONFMENU_ADJU];
  
  /*
  value=MENU_RunList(&list);
  if ((value<0)||(value>2))
    //return false;
    return true;
  //eepromPrint=value;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&value, sizeof(EEPROM1.Print));
  return true;
  */
  
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case ADJU_MODE_INT: // configure INT
        CONF_CalHt();
        break;
      case ADJU_MODE_EXT: // configure EXT
      	CONF_CalPt();
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// CONF_Blocked - blocked menu
//------------------------------------------------------------------------------
TBool CONF_Blocked(void)
{
  SND_Play(ssError);
  return true;
}

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
// CONF_Pwmg - configure Pwmg, v1.03，HHT，2024-06-14
//------------------------------------------------------------------------------
TBool CONF_Pwmg(void)
{
  char title[]="PWMG";
  signed char value;
  TList list;
  TWord value1 = 0; 										// v1.03，HHT，2024-06-14
    
  list.numItems=PWMG_MODE_LAST;
  list.items=CONF_LIST_PWMG;
  list.current=0;
  list.title=CONF_MENU_CONF[CONFMENU_PWMG];
  
  // 密码检查，v1.03，HHT，2024-06-14
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
// CONF_Version - show DT firmware version
//------------------------------------------------------------------------------
TBool CONF_Version(void)
{
  char str[10];
  char str2[5];
    
  strcpy_P(str, DEVICE_VERSION);
  strcpy_P(str2, CONF_MENU_CONF[CONFMENU_VER]);
  EDIT_Show(str2, str, false);
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
  //ptrTItemHandler handler[CONFMENU_LAST];
  ptrTItemHandler handler[CONFMENU_CONF_LAST];
  //TList list;
  //signed char value;
  
  globalDeviceStatus.enableUSB=false;
  
	menu.title=USER_MENU[USERMENU_CONF];
  menu.handler=handler;
  menu.numItems=CONFMENU_CONF_LAST;
  menu.items=CONF_MENU_CONF;
  
  handler[CONFMENU_SOUND]=CONF_Sound;
  handler[CONFMENU_ALARM]=CONF_Alarm;
  handler[CONFMENU_STOP]=CONF_Stop;
  handler[CONFMENU_LED]=CONF_Brightness;
  handler[CONFMENU_DATE]=CONF_DateTime;
  
  handler[CONFMENU_PWMG]=CONF_Pwmg; 	// 增加密码管理菜单，v1.03，HHT，2024-06-14
  
  handler[CONFMENU_PRN]=CONF_Prn;
  handler[CONFMENU_TOFF]=CONF_TempOffs;
  handler[CONFMENU_HEAT]=CONF_Heat;
  handler[CONFMENU_ADJU]=CONF_ADJu;
  handler[CONFMENU_CHKT]=CONF_CheckTemp; 	// Gavin, 2019-03-29 星期五 14:31:38, A6，代替CALI
  handler[CONFMENU_VER]=CONF_Version;
  handler[CONFMENU_CONF_EXIT]=CONF_Exit;
  
  MENU_Run(&menu);

  /*menu.title=CONFMENU_TITLE;
  menu.handler=handler;
  menu.numItems=CONFMENU_LAST;
  menu.items=CONF_MENU_DT;
  handler[CONFMENU_HEAD]=CONF_Head;
  handler[CONFMENU_STIR]=CONF_Stir;
  handler[CONFMENU_COUNT]=CONF_Count;
  handler[CONFMENU_ALARM]=CONF_Alarm;
  handler[CONFMENU_SOUND]=CONF_Sound;
  handler[CONFMENU_LED]=CONF_Brightness;
  handler[CONFMENU_TOFF]=CONF_TempOffs;
  handler[CONFMENU_CAPT]=CONF_CalPt;
  handler[CONFMENU_CAHE]=CONF_CalHt;
  handler[CONFMENU_CAAS]=CONF_CalAs;
  handler[CONFMENU_METH]=CONF_Methode;
  handler[CONFMENU_VOL]=CONF_Volume;
  handler[CONFMENU_FOFF]=CONF_Filter;
  handler[CONFMENU_SAMS]=CONF_SamplerMode;
  handler[CONFMENU_DFLT]=CONF_Default;
  handler[CONFMENU_USAGE]=CONF_Usage;
  handler[CONFMENU_REPORT]=CONF_Report;
  handler[CONFMENU_DATE]=CONF_DateTime;
  handler[CONFMENU_PRINT]=CONF_Print;
  handler[CONFMENU_EXIT]=CONF_Exit;
  MENU_Run(&menu);*/
}
