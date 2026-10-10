#include "confmenu.h"
#include "const.h"
#include "control.h"
#include "dhc.h"
#include "dropper.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "engineering.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "printrtc.h"
#include "report.h"
#include "rs232.h"
#include "sampler.h"
#include "sound.h"
#include "stepper.h"
#include "strings.h"
#include "temp.h"
#include "version.h"

enum ENGMENU_DT
{
  ENGMENU_TEST,
  ENGMENU_CAPT,
  ENGMENU_CAHE,
  //ENGMENU_CAAS, 						// Gavin 20131224
  ENGMENU_SVCA,
  ENGMENU_RSCA,
  //ENGMENU_APOS, 						// Gavin 20131224
  ENGMENU_COUNTER,
  ENGMENU_VES_NUM,
  ENGMENU_PORT,
  //ENGMENU_MERR, 						// Gavin,同步带检测，2014/4/18 16:24:37；Gavin 20131224, 2013/12/28 12:12:28
  ENGMENU_DEV_SNUM,
  ENGMENU_DH_SN, 							// Gavin,增加加热器序列号，2016-10-22 星期六 18:29:56
  ENGMENU_LOGO,
  ENGMENU_DEV_TYPE,
  //ENGMENU_EKEY,
  
  // 增加RSPW菜单，v2.10，Gavin，2020-09-03 星期四 11:35:53
  ENGMENU_DEV_RSPW,
  
  ENGMENU_EXIT,
  ENGMENU_LAST
};

// 增加RSPW菜单，v2.10，Gavin，2020-09-03 星期四 14:12:50
enum RSPW_MODE
{
  RSPW_MODE_YES,
  RSPW_MODE_NO,
  RSPW_MODE_LAST
};

TMenuItemStr ENG_MENU_TITLE="SERV";

TMenuItemStr ENG_MENU_DT[ENGMENU_LAST]=
{
  "TEST",
  "AJ.PT",
  "AJ.HE",
  //"AJ.AS", 				// Gavin 20131224
  "SV.CA",
  "RS.CA",
  //"A.POS", 				// Gavin 20131224
  "USE",
  "N.STR",
  "PORT",
  //"M.ERR", 				// Gavin,同步带检测，2014/4/18 16:25:16；Gavin 20131224, 2013/12/28 12:12:44
  "S.NUM",
  "DH.SN", 					// Gavin,加热器序列号，2016-10-22 星期六 18:29:47
  "LOGO",
  "TYPE",
  //"EKEY",
  "RSPW", 					//增加密码复位菜单，v2.10，Gavin，2020-09-03 星期四 11:37:35
  "EXIT"
};

TMenuItemStr ENG_LIST_TEST[TEST_MODE_LAST]=
{
  "LED",
  "TIME",
  "MOTR",
  "DHC",
  "T.EXT",
  "PRNT",
  //"DROP", 				// Gavin 20131224
  //"A.SMP", 				// Gavin 20131224
  //"SMP.T" 				// Gavin 20131224
};

TMenuItemStr ENG_LIST_LOGO[DEVICE_LOGO_LAST]=
{
  "ERW."
  //"CAL.", 				// V200-Q10.4, 2019-2-25 星期一 13:45:51
  //"TTST",
  //"QTST"
};

TMenuItemStr ENG_LIST_PORT[PORT_MODE_LAST]=
{
  "USB",
  "COM"
};

TMenuItemStr ENG_LIST_MERR[MOTORERROR_MODE_LAST]=
{
  "ON",
  "OFF"
};

TMenuItemStr ENG_LIST_TYPE[DEVICE_LAST]=
{
	/*
	" 620",
  " 720",
  "1420"
  */
  
  //" 520", 										// Gavin,131223
  " 120" 										// Gavin, 2015-06-17
  // V200-Q1.4, 2019-2-24 星期日 20:20:52
  //"   6", 										// 2014/3/13 22:15:07
  //"  6S" 											// 2014/3/13 22:15:15
};

__flash TByte PRESET_SPEED[NUM_PRESET_SPEEDS+1]=
{
  0,
  20,
  50,
  100,
  150,
  200
};

// 增加RSPW菜单显示，v2.10，Gavin，2020-09-03 星期四 11:38:40
TMenuItemStr ENG_LIST_RSPW[RSPW_MODE_LAST]=
{
  "YES ",
  "NO  "
};

//------------------------------------------------------------------------------
// ENGINEERING_Test - test mode
//------------------------------------------------------------------------------
TBool ENGINEERING_Test(void)
{
  //signed char value, value2;
  //TList list, list2;
  // Gavin 20131224---------------------
  signed char value;
  TList list;
  
  TReal32 temp;
  //TDWord dist;
  TByte Val;
  TByte key;
  TBool stop;
  char str[10];
  
  list.numItems=TEST_MODE_LAST;
  list.items=ENG_LIST_TEST;
  list.current=0;
  list.title=ENG_MENU_DT[ENGMENU_TEST];
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case TEST_MODE_LED:   // test LED
        Val=0;
        for (;;)
        {  
          LED_Check(Val);
          while(!KBD_KeyPressed());

          if (KBD_KeyPressed())
          {
            key=KBD_GetKey();
            if (key==KBD_CODE_STOP)
            {
            	Val = 0;
            	LED_Check(Val);
              SND_Play(ssCancel);
              break;
            }
            else
            {
              Val++;
              if (Val>LED_NUM_DIGITS)
                Val=0;
              SND_Play(ssOk);
            }
          }  
        }  
        break;
      case TEST_MODE_TIMER: // test timer
        globalDeviceStatus.timeTick=false;
        Val=60;
        CTL_ShowTemp(0, false);
        CTL_ShowSpeed(0, false, false);
        CTL_ShowTime(Val, false);
        stop=false;
        while ((Val)&&(!stop))
        {  
          while (!globalDeviceStatus.timeTick)
          {
            if (KBD_KeyPressed())
            {
              KBD_GetKey();
              SND_Play(ssOk);
              stop=true;
              break;
            }
          }
          globalDeviceStatus.timeTick=false;
          Val--;
          CTL_ShowTime(Val, false);
        }
        SND_Play(ssOk);
        break;
      case TEST_MODE_MOTOR: // test motor
        //set speed
        Val=1;
        //CTL_ShowTemp(0, false);
        //CTL_ShowTime(0, false);
        LED_Print_P(LED_DISPLAY_TEMP, false, ENG_LIST_TEST[TEST_MODE_MOTOR]);
        for (;;)
        {  
          stop=false;
          for (;;)
          {
            CTL_ShowSpeed((TReal32)PRESET_SPEED[Val], false, false);
            while (!KBD_KeyPressed());
          
            key=KBD_GetKey();
            switch(key)
            {
              case KBD_CODE_START:
                SND_Play(ssOk);
                stop=true;
                break;
              case KBD_CODE_STOP:
                SND_Play(ssCancel);
                Val=0;
                stop=true;
                break;
              case KBD_CODE_SPEED_UP:
                Val++;
                if (Val>NUM_PRESET_SPEEDS)
                  Val=1;
                SND_Play(ssSlowUp);
                break;
              case KBD_CODE_SPEED_DOWN:
                Val--;
                if (!Val) Val=NUM_PRESET_SPEEDS;
                SND_Play(ssSlowDown);
                break;
            }
            if (stop)
              break;
          }
          if (!Val) break;
          MOTOR_SetSpeed(PRESET_SPEED[Val]);
          MOTOR_Start();
          globalDeviceStatus.timeTick=true;
          while(!KBD_KeyPressed())
          {
            //show speed
            if (globalDeviceStatus.timeTick)
            {
              CTL_ShowSpeed((TReal32)MOTOR_GetSpeed()/10, false, true);
              globalDeviceStatus.timeTick=false;
            }
          }
          KBD_GetKey();        
          MOTOR_Stop();
          SND_Play(ssOk);
        }  
        break;
      case TEST_MODE_DHC:   // test DHC
        LED_Print_P(LED_DISPLAY_TEMP, false, ENG_LIST_TEST[TEST_MODE_DHC]);
        while(!KBD_KeyPressed())
        {
          //show temperature from DHC
          DHC_GetTemp(&temp);
          sprintf_P(str, tempFmt, temp);
          LED_Print(LED_DISPLAY_SPEED, false, str);
        }
        KBD_GetKey();        
        SND_Play(ssOk);
        break;
      case TEST_MODE_TEXT:   // test external sensor
        LED_Print_P(LED_DISPLAY_TEMP, false, ENG_LIST_TEST[TEST_MODE_TEXT]);
        while(!KBD_KeyPressed())
        {
          //show temperature from external sensor
          sprintf_P(str, tempFmt, TEMP_GetValue(ADC_CHNL_EXTERNAL));
          LED_Print(LED_DISPLAY_SPEED, false, str);
        }
        KBD_GetKey();        
        SND_Play(ssOk);
        break;
      case TEST_MODE_PRINT:
			{
				//REPORT_Print(REPORT_Printer); 					// Gavin 2015/12/2 10:02:57
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
	  			REPORT_Print(REPORT_Printer_NORM);
	  		else
	  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
	  			REPORT_Print(REPORT_Printer_PORT);
	  		
				SND_Play(ssOk);
			}break;
      // 屏蔽以下case；Gavin 20131224-------------
      /*case TEST_MODE_DROP: // test dropper
        LED_Print_P(LED_DISPLAY_SPEED, false, ENG_LIST_TEST[TEST_MODE_DROP]);
        LED_Print_P(LED_DISPLAY_TEMP, true, messWait);
        DROPPER_Open();
        //wait for some seconds
        TIMER_Start(TIMER_GENERAL,TO_2SEC);
        while (!TIMER_TimeOut(TIMER_GENERAL));
        TIMER_Stop(TIMER_GENERAL);
        DROPPER_Close();
        SND_Play(ssOk);
        break;*/
      // 屏蔽以下case；Gavin 20131224-------------
      /*case TEST_MODE_SAMPLE:
        if (!globalDeviceStatus.haveStepper)
        {  
          SND_Play(ssError);
          break;
        } else SND_Play(ssOk);

        //choose volume
        list2.numItems=DEPTH_MODE_LAST;
        list2.items=CONF_LIST_DEPTH;
        list2.current=0;
        list2.title=ENG_LIST_TEST[TEST_MODE_SAMPLE];
        for (;;)
        {
          value2=MENU_RunList(&list2);
          if (value2==-1) break;
          LED_Print_P(LED_DISPLAY_TEMP, false, CONF_LIST_DEPTH[value2]);
          temp=(TReal32)EEPROM1.VolumeDepth[value2]/10;
          STEPPER_SetDistance(&temp);
          STEPPER_Start(false, false);
          while (!KBD_KeyPressed())
          {  
            STEPPER_GetPosition(&temp);
            sprintf_P(str, speedFmt, temp);
            LED_Print(LED_DISPLAY_SPEED, false, str);
          }
          
          key=KBD_GetKey();
          SND_Play(ssOk);
        }
        STEPPER_Start(true, true);
        break;*/
      // 屏蔽以下case；Gavin 20131224-------------
      /*case TEST_MODE_SAMPLET:
        if (globalDeviceStatus.samplerFailure)
        {
          SND_Play(ssError);
          break;
        }
        SND_Play(ssOk);
        Val=0;
        stop=false;
        while (!stop)
        {
          sprintf_P(str, sensFmt, Val+1);
          LED_Print(LED_DISPLAY_TEMP, false, str);
          for (;;)
          {  
            if (SAMPLER_Temp[Val]==TEMP_ERROR)
            {
              LED_Print_P(LED_DISPLAY_SPEED, false, errorStr);
            } else
            {  
              temp=(TReal32)SAMPLER_Temp[Val]/100;
              sprintf_P(str, tempFmt, temp);
              LED_Print(LED_DISPLAY_SPEED, false, str);
            }
            TIMER_Start(TIMER_GENERAL,TO_1SEC);
            while ((!TIMER_TimeOut(TIMER_GENERAL))&&(!KBD_KeyPressed()))
              continue;
            TIMER_Stop(TIMER_GENERAL);
            if (KBD_KeyPressed())
            {
              key=KBD_GetKey();
              if (key==KBD_CODE_STOP)
              {
                SND_Play(ssCancel);
                stop=true;
                break;
              } else
              {
                SND_Play(ssOk);
                break;
              }
            }
          }
          
          Val++;
          if (Val>=EEPROM1.NumSensors)
            Val=0;
        }
        break;*/
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// ENGINEERING_CalPt - two-points calibration for Pt100
//------------------------------------------------------------------------------
TBool ENGINEERING_CalPt(void)
{
  if ((globalDeviceStatus.installedExtADC)&&(!globalDeviceStatus.sensorExtError))
  {
    SND_Play(ssOk);            
    CTL_Calibration(2);                  
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_CalHt - two-points calibration for the external heater
//------------------------------------------------------------------------------
TBool ENGINEERING_CalHt(void)
{
  if (!globalDeviceStatus.dhcFailure)
  {
    SND_Play(ssOk);            
    CTL_CalibrationDHC(2);                  
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_CalAs - two-points calibration for the autosampler
//------------------------------------------------------------------------------
TBool ENGINEERING_CalAs(void)
{
  if (!globalDeviceStatus.samplerFailure)
  {
    SND_Play(ssOk);            
    CTL_CalibrationAutosampler(2);                  
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_SaveCal - save all calibration values
//------------------------------------------------------------------------------
TBool ENGINEERING_SaveCal(void)
{
  TByte key;
  
  LED_Print_P(LED_DISPLAY_TEMP, false, ENG_MENU_DT[ENGMENU_SVCA]);
  LED_Print_P(LED_DISPLAY_SPEED, false, saveStr);
        
  while(!KBD_KeyPressed());

  key=KBD_GetKey();        

  if (key==KBD_CODE_START)
  {
    // save calibration values
    ECalibr_Write();
    SND_Play(ssOk);            
  } else
  {
    SND_Play(ssCancel);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_RestoreCal - restore all calibration values
//------------------------------------------------------------------------------
TBool ENGINEERING_RestoreCal(void)
{
  TByte key;
  char str[20];
  
  LED_Print_P(LED_DISPLAY_TEMP, false, ENG_MENU_DT[ENGMENU_RSCA]);
  LED_Print_P(LED_DISPLAY_SPEED, false, restoreStr);
        
  while(!KBD_KeyPressed());

  key=KBD_GetKey();        

  if (key==KBD_CODE_START)
  {
    // restore calibration values
    EEPROM_ReadString(str, (char __eeprom *)&ECalibr1, sizeof(_ECalibr)-2);
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempK[0], (TByte *)str, sizeof(_ECalibr)-2);
    TEMP_Init();
    SND_Play(ssOk);            
  } else
  {
    SND_Play(ssCancel);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_AutosamplerPos - calibrate positions of autosampler
//------------------------------------------------------------------------------
TBool ENGINEERING_AutosamplerPos(void)
{
  if (globalDeviceStatus.haveStepper)
  {
    SND_Play(ssOk);            
    CTL_CalibrationStepper();
  } else SND_Play(ssError);
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_Counter - re-zero hours of operation counter
//------------------------------------------------------------------------------
TBool ENGINEERING_Counter(void)
{
  TByte key;
  TDWord value=0;
  
  LED_Print_P(LED_DISPLAY_TEMP, false, ENG_MENU_DT[ENGMENU_COUNTER]);
  LED_Print_P(LED_DISPLAY_SPEED, false, rstStr);
        
  while(!KBD_KeyPressed());

  key=KBD_GetKey();        

  if (key==KBD_CODE_START)
  {
    //eepromUsageCounter=0;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.UsageCounter, (TByte *)&value, sizeof(EEPROM1.UsageCounter));
    SND_Play(ssOk);            
  } else
  {
    SND_Play(ssCancel);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DevSNum - set device serial number
//------------------------------------------------------------------------------
TBool ENGINEERING_DevSNum(void)
{
  char tStr[SERIAL_NUM_MAX_LEN];
  TByte i;
  char title[10]="N-";

  //sprintf_P(title, ENG_MENU_DT[ENGMENU_DEV_SNUM]);
  for (i=0;i<SERIAL_NUM_MAX_LEN;i++)
  {
    tStr[i]=EEPROM1.SerialNumber[i]; 
  }
        
  if (EDIT_Advanced(title, tStr, SERIAL_NUM_MAX_LEN, true))
  {
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.SerialNumber, (TByte *)tStr, SERIAL_NUM_MAX_LEN);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DH-SN - set DH serial number, 设置加热器序列号，2016-10-23 星期日 9:52:58
//------------------------------------------------------------------------------
TBool ENGINEERING_DhSNum(void)
{
  char tStr[SERIAL_NUM_MAX_LEN];
  TByte i;
  char title[10]="N-";

  //sprintf_P(title, ENG_MENU_DT[ENGMENU_DEV_SNUM]);
  for (i=0;i<SERIAL_NUM_MAX_LEN;i++)
  {
    //tStr[i]=EEPROM1.SerialNumber[i]; 
    tStr[i]=EEPROM1.Empty2[i+1];
  }
        
  if (EDIT_Advanced(title, tStr, SERIAL_NUM_MAX_LEN, true))
  {
    //EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.SerialNumber, (TByte *)tStr, SERIAL_NUM_MAX_LEN);
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Empty2+1, (TByte *)tStr, SERIAL_NUM_MAX_LEN);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DevType - configure device type
//------------------------------------------------------------------------------
TBool ENGINEERING_DevType(void)
{
  signed char value;
  TList list;
    
  list.numItems=DEVICE_LAST;
  list.items=ENG_LIST_TYPE;
  list.current=EEPROM1.DeviceType;
  list.title=ENG_MENU_DT[ENGMENU_DEV_TYPE];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DeviceType, (TByte *)&value, sizeof(EEPROM1.DeviceType));
  globalDeviceType=EEPROM1.DeviceType;
  
  //if (globalDeviceType==DEVICE_DT620)
  // Gavin 20131224-----------------------------------------
  if (globalDeviceType==DEVICE_DT520)
  {
    value=HEAD_MODE_HIGH;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Head, (TByte *)&value, sizeof(EEPROM1.Head));
    globalDeviceStatus.Head=EEPROM1.Head;
    //globalDeviceStatus.sensorExtError=true;
    //globalDeviceStatus.installedExtADC=false;
  }
  else
  {
    globalDeviceStatus.installedExtADC=true;
  }  
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_VesNum - configure number of vessels
//------------------------------------------------------------------------------
TBool ENGINEERING_VesNum(void)
{
  TWord value;
  static __flash char format[]="  %2d";
  
  value=EEPROM1.NumVessels;
  if (EDIT_Integer(ENG_MENU_DT[ENGMENU_VES_NUM], format, &value, MIN_VESSELS_NUMBER, MAX_VESSELS_NUMBER, NULL))
    //eepromNumVessels=value;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.NumVessels, (TByte *)&value, sizeof(EEPROM1.NumVessels));
  return true;
}
  
//------------------------------------------------------------------------------
// ENGINEERING_Port - configure communication port
//------------------------------------------------------------------------------
TBool ENGINEERING_Port(void)
{
  signed char value;
  TList list;
    
  list.numItems=PORT_MODE_LAST;
  list.items=ENG_LIST_PORT;
  list.current=EEPROM1.Port;
  list.title=ENG_MENU_DT[ENGMENU_PORT];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Port, (TByte *)&value, sizeof(EEPROM1.Port));
  globalDeviceStatus.portMode=(TByte)value;
  RS232_Init(globalDeviceStatus.portMode);
  globalDeviceStatus.dhcFailure=true;
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_MotorError - configure stirrer motor error mode
//------------------------------------------------------------------------------
TBool ENGINEERING_MotorError(void)
{
  signed char value;
  TList list;
    
  list.numItems=MOTORERROR_MODE_LAST;
  list.items=ENG_LIST_MERR;
  list.current=EEPROM1.MotorError;
  //list.title=ENG_MENU_DT[ENGMENU_MERR]; 				// Gavin，同步带检测，2014/4/18 16:28:33；Gavin 20131224, 2013/12/28 12:13:02
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MotorError, (TByte *)&value, sizeof(EEPROM1.MotorError));
  if (!value)
    globalDeviceStatus.motorErrorSwitch=true;
  else
    globalDeviceStatus.motorErrorSwitch=false;
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_Logo - configure logo
//------------------------------------------------------------------------------
TBool ENGINEERING_Logo(void)
{
  signed char value;
  TList list;
    
  list.numItems=DEVICE_LOGO_LAST;
  list.items=ENG_LIST_LOGO;
  list.current=EEPROM1.FirmLogo;
  list.title=ENG_MENU_DT[ENGMENU_LOGO];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FirmLogo, (TByte *)&value, sizeof(EEPROM1.FirmLogo));
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_SensNum - configure number of sensors of the autosampler
//------------------------------------------------------------------------------
/*TBool ENGINEERING_SensNum(void)
{
  TWord value;
  static __flash char format[]="  %2d";
  
  value=EEPROM1.NumSensors;
  if (EDIT_Integer(ENG_MENU_DT[ENGMENU_SENS_NUM],format,&value,MIN_SENSORS_NUMBER,MAX_SENSORS_NUMBER,NULL))
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.NumSensors, (TByte *)&value, sizeof(EEPROM1.NumSensors));
  return true;
}*/

//------------------------------------------------------------------------------
/*TBool ENGINEERING_Ekey(void)
{
  EKEY_Settings();

  return false;
}*/

//------------------------------------------------------------------------------
// ENGINEERING_DevRspw - configure device reset password, v2.10, Gavin, 2020-09-03 星期四 11:41:25
//------------------------------------------------------------------------------
TBool ENGINEERING_DevRspw(void)
{
	char title[]="RSPW";
	signed char value;
	TList list;
	TWord pd = 0;
	
	list.numItems=RSPW_MODE_LAST;
	
	list.items=ENG_LIST_RSPW;
	list.current=RSPW_MODE_YES;
	list.title=ENG_MENU_DT[ENGMENU_DEV_RSPW];
	
	value=MENU_RunList(&list);
	if (value == RSPW_MODE_YES)
	{
		pd = 807;
		if(CTL_CheckPW(title, pd) == 0)
		{
			pd = 9999;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MGPassword, (TByte *)&pd, sizeof(EEPROM1.MGPassword));
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DTPassword, (TByte *)&pd, sizeof(EEPROM1.DTPassword));
		}
	}
	
	return true;
}


//------------------------------------------------------------------------------
TBool ENGINEERING_Exit(void)
{
  return false;
}

//------------------------------------------------------------------------------
// ENGINEERING_Pass - run engineering menu password check
//------------------------------------------------------------------------------
TBool ENGINEERING_Pass(void)
{
  TWord value;
  TByte ix;
  char title[]="PASS";
  char tStr[LED_NUM_DIGITS];
  
  for (ix=0; ix<LED_NUM_DIGITS; ix++)
    tStr[ix] = '*';
  value=0;
  if (EDIT_Advanced(title, tStr, LED_NUM_DIGITS, true))
  {  
    for (ix=0;ix<LED_NUM_DIGITS;ix++)
    {
      value*=10;
      value+=tStr[ix]-0x30; 
    }
    //value = atoi(tStr);
    if (EEPROM1.Password==value) return true;
      else return false;
  }
  return false;
}

//------------------------------------------------------------------------------
// ENGINEERING_Menu - run engineering menu
//------------------------------------------------------------------------------
void ENGINEERING_Menu(void)
{
  TMenu menu;
  ptrTItemHandler handler[ENGMENU_LAST];
    
  if (!ENGINEERING_Pass()) return;
  
  menu.title=ENG_MENU_TITLE;
  menu.numItems=ENGMENU_LAST;
  menu.items=ENG_MENU_DT;
  menu.handler=handler;
  handler[ENGMENU_TEST]=ENGINEERING_Test;
  handler[ENGMENU_CAPT]=ENGINEERING_CalPt;
  handler[ENGMENU_CAHE]=ENGINEERING_CalHt;
  //handler[ENGMENU_CAAS]=ENGINEERING_CalAs; 			// Gavin 20131224
  handler[ENGMENU_SVCA]=ENGINEERING_SaveCal;
  handler[ENGMENU_RSCA]=ENGINEERING_RestoreCal;
  //handler[ENGMENU_APOS]=ENGINEERING_AutosamplerPos; 			// Gavin 20131224
  handler[ENGMENU_COUNTER]=ENGINEERING_Counter;
  handler[ENGMENU_VES_NUM]=ENGINEERING_VesNum;
  handler[ENGMENU_PORT]=ENGINEERING_Port;
  //handler[ENGMENU_MERR]=ENGINEERING_MotorError; 	// Gavin，同步带检测，2014/4/18 16:27:08；Gavin 20131224, 2013/12/28 12:13:19
  handler[ENGMENU_DEV_SNUM]=ENGINEERING_DevSNum;
  handler[ENGMENU_DH_SN]=ENGINEERING_DhSNum; 		// Gavin, 加热器序列号
  //handler[ENGMENU_EKEY]=ENGINEERING_Ekey;
  handler[ENGMENU_LOGO]=ENGINEERING_Logo;
  handler[ENGMENU_DEV_TYPE]=ENGINEERING_DevType;
  
  // 增加DEV菜单中的RSPW菜单，v2.10，Gavin，2020-09-03 星期四 11:42:50
  handler[ENGMENU_DEV_RSPW]=ENGINEERING_DevRspw;
  
  handler[ENGMENU_EXIT]=ENGINEERING_Exit;
    
  globalDeviceStatus.enableUSB=false;
  MENU_Run(&menu);

  if (globalDeviceStatus.installedPrinter)
  {
    // save service date/time
    if (RTC_GetDateTime(&curDTime))
      EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeService, (TByte *)&curDTime, sizeof(EEPROM1.timeService));
  }
  globalDeviceStatus.enableUSB=true;
}
