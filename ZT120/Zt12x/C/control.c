#include <math.h>
#include "adc.h"
#include "calibrate.h"
#include "confmenu.h"
#include "control.h"
#include "dhc.h"
#include "edit.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"                                                        
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "platform.h"
#include "printrtc.h"
#include "report.h"
#include "rs485.h"
#include "sampler.h"
#include "sound.h"
#include "stepper.h"
#include "strings.h"
#include "syserror.h"
#include "timer.h"
#include "utils.h"
#include "version.h"
#include "heater.h"

TBool CTL_Temp_Blink;

//------------------------------------------------------------------------------
//show test time
//------------------------------------------------------------------------------
void CTL_ShowTime(TDWord time, TBool blink)
{
  //TDWord temp=0;
  TDWord temp = DT_MAX_TIME;
  
  if (time>DT_MAX_TIME)
    LED_Print_Time(blink, &temp);
  else
    LED_Print_Time(blink, &time);
}

//------------------------------------------------------------------------------
//show temperature
//------------------------------------------------------------------------------
void CTL_ShowTemp(TReal32 temp, TBool blink)
{
	char str[10];
	
	if ((temp>MAX_CURTEMP_VALUE)||(temp<MIN_CURTEMP_VALUE))
		strcpy_P(str, errorTemp);
	else
		sprintf_P(str, tempFmt, temp);
	
	LED_Print(LED_DISPLAY_TEMP, blink, str);
	CTL_Temp_Blink=blink;  
}

//------------------------------------------------------------------------------
//show speed
//------------------------------------------------------------------------------
void CTL_ShowSpeed(TReal32 speed, TBool blink, TBool run)
{
	char str[10];
	
	if (run)
	{
		if (speed>=100.0)
			sprintf_P(str, speedFmt, speed);
		else
			sprintf_P(str, speedFmt2, speed);
	}
	else
		sprintf_P(str, ospeedFmt, (TWord)speed);
	
  LED_Print(LED_DISPLAY_TEMP, blink, str);
}

void CTL_SetOffset(TByte ch)
{
  char str[10];
  TByte key;
  TsWord value;
  
  value=EEPROM1.TempOffset[ch];

  for(;;)
  {
    sprintf_P(str, otempFmt, (TReal32)value/10);
    //LED_Print(LED_DISPLAY_SPEED, false, str);
    LED_Print(LED_DISPLAY_TEMP, false, str);
    
    while(!KBD_KeyPressed());
  
    key=KBD_GetKey();
      
    switch(key)
    {
      case KBD_CODE_UP:
        SND_Play(ssSlowUp);
        value++;
        break;
      case KBD_CODE_DOWN:              
        SND_Play(ssSlowDown);
        //if (value)
      	  value--;
      	break;
      case KBD_CODE_START:
				SND_Play(ssOk);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempOffset[ch], (TByte *)&value, sizeof(EEPROM1.TempOffset[ch]));
				TEMP_Init();
				return;
      case KBD_CODE_STOP:
				SND_Play(ssCancel);
				return;
      default:
				SND_Play(ssError);
    }
    if ((TReal32)value/10<MIN_OFFSET_VALUE)
      value=(TsWord)(MIN_OFFSET_VALUE*10);
    if ((TReal32)value/10>MAX_OFFSET_VALUE)
      value=(TsWord)(MAX_OFFSET_VALUE*10);
  }
}

void CTL_SetFilter(void)
{
  char str[10];
  TByte key;
  signed char value;
  //TReal32 value;
  
  /*value=EEPROM1.FilterOffset;

  for(;;)
  {
    sprintf_P(str, filterFmt, value);
    LED_Print(LED_DISPLAY_SPEED, false, str);
    
    while(!KBD_KeyPressed());
  
    key=KBD_GetKey();
      
    switch(key)
    {
      case KBD_CODE_SPEED_UP:
        SND_Play(ssSlowUp);                
        value+=0.1;                                
        break;
      case KBD_CODE_SPEED_DOWN:              
        SND_Play(ssSlowDown);
        value-=0.1;
	break;
      case KBD_CODE_START:
	SND_Play(ssOk);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FilterOffset, (TByte *)&value, sizeof(EEPROM1.FilterOffset));
	return;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
	return;
      default:
	SND_Play(ssError);
    }
    if ((value>-0.1)&&(value<0.1)) value=0;
    if (value<MIN_FILTER_VALUE)
      value=MIN_FILTER_VALUE;
    if (value>MAX_FILTER_VALUE)
      value=MAX_FILTER_VALUE;
  }*/
  
  value=EEPROM1.FilterOffset;

  for(;;)
  {
    sprintf_P(str, filterFmt, (TReal32)value/10);
    LED_Print(LED_DISPLAY_SPEED, false, str);
    
    while(!KBD_KeyPressed());
  
    key=KBD_GetKey();
      
    switch(key)
    {
      case KBD_CODE_UP:
        SND_Play(ssSlowUp);                
        value++;                                
        break;
      case KBD_CODE_DOWN:              
        SND_Play(ssSlowDown);
        value--;
	break;
      case KBD_CODE_START:
	SND_Play(ssOk);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FilterOffset, (TByte *)&value, sizeof(EEPROM1.FilterOffset));
	return;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
	return;
      default:
	SND_Play(ssError);
    }
    if (value<(signed char)(MIN_FILTER_VALUE*10))
      value=(signed char)(MIN_FILTER_VALUE*10);
    if (value>(signed char)(MAX_FILTER_VALUE*10))
      value=(signed char)(MAX_FILTER_VALUE*10);
  }
  
}

// 内部PT100温度校正----------------------------------------
void CTL_AdjustmentDHC(TByte NumPoints)
{
  TByte point, key, i;
  TBool tempflag = 0; 									// Gavin, 2019-3-27 15:35:15, A4.2
  TReal32 targetTemp, actTemp;
  TReal32 checkTemp = 0.0; 							// Gavin, 2019-3-27 15:35:15, A4.2
  char str[10];
	
  if ((!NumPoints)||(NumPoints>NUM_CAL_POINTS))
    return;

  // if internal sensor is installed
  if (!globalDeviceStatus.sensorInsError) 				// 内部PT100没有报错
  {
    for(point=0;point<NumPoints;point++)
    {
      // show setting temperature string
      LED_Print_P(LED_DISPLAY_TIME, false, setTempStr); 		// 标题，设置加热温度
      targetTemp = TEMP_GetValue(ADC_CHNL_INTERNAL);    		// Gavin,171207 获取加热温度，内部PT100
      
      // 设置加热器工作温度---------------------------------
      for(;;)
      {
        //set DHC temperature
        sprintf_P(str, tempFmt, targetTemp);
    	  LED_Print(LED_DISPLAY_TEMP, true, str); 						// 显示刚才获得的加热器温度
        
        while(!KBD_KeyPressed())   													// 判断是否有按键操作
        {
          EXTERNAL_Process(); 															// 扫描键码
        }
        key=KBD_GetKey(); 																	// 读取键码
            
        if (key==KBD_CODE_START)
        {
          //set DHC temperature
          HEATER_SetTemp(&targetTemp);    									// Gavin, 171207，设置加热器温度
          break;
        } else
        switch(key)
        {
          case KBD_CODE_STOP:
            SND_Play(ssCancel);
            return;
          case KBD_CODE_DOWN:
            SND_Play(ssSlowDown);
            targetTemp-=0.1;
            break;
          case KBD_CODE_UP:
            SND_Play(ssSlowDown);
            targetTemp+=0.1;
            break;
                
          default:
            SND_Play(ssError);
        }
        
        if (targetTemp>MAX_TEMP_VALUE)
          targetTemp=MAX_TEMP_VALUE;
        
        if (targetTemp<MIN_TEMP_VALUE)
          targetTemp=MIN_TEMP_VALUE;
      }        
      SND_Play(ssOk);

      // show calibr str
      LED_Print(LED_DISPLAY_TIME, false, "WAIT");      // 2017-12-12 星期二 21:04:42，显示校正标题
      
      // 获取内部PT100检测温度------------------------------
      TIMER_Start(TIMER_GENERAL, TO_HEATER); 								// Gavin, 2019-3-27 16:35:26, A4.1，启动通用定时器
      for(;;)
      {
      	// Gavin, 2019-3-27 15:29:53, A4.1
	      //*
      	if (TIMER_TimeOut(TIMER_GENERAL)) 									// 通用定时器中断
				{
					TIMER_Start(TIMER_GENERAL,TO_HEATER); 						// 启动动用定时器
					
	        //show temperature from ADC and wait for start key
	        actTemp=TEMP_GetValue(ADC_CHNL_INTERNAL); 					// 获取内部温度
	        sprintf_P(str, tempFmt, actTemp);        
	        LED_Print(LED_DISPLAY_TEMP, false, str); 						// 显示内部温度
	        
					checkTemp=0.97*checkTemp+0.03*actTemp;
					
					//check temperature, exit if ok£» MAX_TEMP_DIFF = 0.2
					if ((fabs(actTemp-targetTemp)<=MAX_TEMP_DIFF)&&(fabs(actTemp-checkTemp)<0.1))
					{
						if(tempflag == 0) 										// 2014/7/10 9:52:22
						{
							tempflag = 1;
							
							if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
							{
								SND_Play(ssAlarm);
							}
							
							LED_Print_P(LED_DISPLAY_TIME, true, messOK); 		// 2017-11-8 ÐÇÆÚÈý 23:12:58
						}
					}
					
					if(tempflag)
					{
						if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
						{
							SND_Play(ssAlarm);
						}
					}
				}
				
        if (KBD_KeyPressed())
        {
          key=KBD_GetKey();
          if (key==KBD_CODE_STOP) 
          {
            SND_Play(ssCancel);
            return;
          } else
          if (key==KBD_CODE_START)
          {
            break;
          } else SND_Play(ssError);
        }
      }
      SND_Play(ssOk);
        
      LED_Print_P(LED_DISPLAY_TIME, false, realStr);      	// Gavin, 171207，显示输入实际温度标题
      targetTemp = actTemp;
			
			// 设置实际检测温度-----------------------------------
      for(;;)
      {
        sprintf_P(str, tempFmt, targetTemp);
        LED_Print(LED_DISPLAY_TEMP, true, str);    					// Gavin, 171207，显示设置实际温度
        
        while(!KBD_KeyPressed())   
        {
          EXTERNAL_Process();
          //ERROR_Check();
        }
        key=KBD_GetKey();
            
        if (key==KBD_CODE_START)
        {
          break;
        } else
        switch(key)
        {
          case KBD_CODE_STOP:
            SND_Play(ssCancel);
            return;
          case KBD_CODE_DOWN:
            SND_Play(ssSlowDown);
            targetTemp-=0.1;
            break;
          case KBD_CODE_UP:
            SND_Play(ssSlowDown);
            targetTemp+=0.1;
            break;
                
          default:
            SND_Play(ssError);
        }
        
        if (targetTemp>MAX_TEMP_VALUE)
          targetTemp=MAX_TEMP_VALUE;
        
        if (targetTemp<MIN_TEMP_VALUE)
          targetTemp=MIN_TEMP_VALUE;
      }        
      SND_Play(ssOk);

      i=true;
      
      if (!TEMP_SetCalPoint(ADC_CHNL_INTERNAL, point, &targetTemp))
      {
        i=false;
          break;
      }
    }

    if (i)      
      if (!TEMP_Calibrate())
        i=false;

    LED_Print_P(LED_DISPLAY_TIME, false, adjustStr);    // Gavin, 171207
    
    // 显示校正结果-----------------------------------------
    if (i)
    {
      LED_Print_P(LED_DISPLAY_TEMP, false, okStr);    // Gavin, 171207
      //SND_Play(ssOk);
      CALIBRATE_Ok=true;
    }
    else
    {
      LED_Print_P(LED_DISPLAY_TEMP, false, errorStr);   // Gavin, 171207
      //SND_Play(ssError);
      CALIBRATE_Ok=false;
    }

    for(;;)
    {
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
        //ERROR_Check();
      }
      key=KBD_GetKey();
      SND_Play(ssOk);
      break;
    }
		
		// 判断是否连接打印板，以获取时间-----------------------
    if ((CALIBRATE_Ok)&&(globalDeviceStatus.installedPrinter))
    {
      // save Adjustment date/time
      if (RTC_GetDateTime(&curDTime))
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeHeaterCal, (TByte *)&curDTime, sizeof(EEPROM1.timeHeaterCal));
    }
		
		// 判断打印板是否连接，以打印报告-----------------------
    if (globalDeviceStatus.installedPrinter)
    {
    	if (EDIT_Bool(printReq))
    	{
    		//REPORT_Print(REPORT_Calibr); 						// Gavin 2015/12/2 10:02:57
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
    			REPORT_Print(REPORT_Calibr_NORM);
    		else
    		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
    			REPORT_Print(REPORT_Calibr_PORT);
    	}
    }
  } // end if ext. sensor installed
}

// 外部PT100温度校正----------------------------------------
void CTL_Adjustment(TByte NumPoints)
{
  TByte point, key, i;
  TBool tempflag = 0; 									// Gavin, 2019-3-28 15:01:54, A4.2
  TReal32 targetTemp, actTemp;
  TReal32 checkTemp = 0.0; 							// Gavin, 2019-3-28 15:01:59, A4.2
  TReal32 IntTemp; 											// Gavin, A4, 2019-04-01 星期一 15:26:00
  char str[10];

  if ((!NumPoints)||(NumPoints>NUM_CAL_POINTS))
    return;

  // if external sensor is installed
  if (!globalDeviceStatus.sensorExtError) 				// 外部PT100没有报错
  {
    for(point=0;point<NumPoints;point++)
    {
      // show setting temperature string
      //LED_Print_P(LED_DISPLAY_TEMP, false, setTempStr);
      LED_Print_P(LED_DISPLAY_TIME, false, setTempStr); 		// 标题，设置加热温度
      //DHC_GetTemp(&targetTemp);
      targetTemp = TEMP_GetValue(ADC_CHNL_INTERNAL);    // Gavin,171207 获取加热温度，内部PT100
      
      // 设置加热器工作温度---------------------------------
      for(;;)
      {
        //set DHC temperature
        sprintf_P(str, tempFmt, targetTemp);
    	  //LED_Print(LED_DISPLAY_SPEED, true, str);
        LED_Print(LED_DISPLAY_TEMP, true, str); 				// 显示刚才获得的加热器温度
        
        while(!KBD_KeyPressed())   			// 判断是否有按键操作
        {
          EXTERNAL_Process(); 					// 扫描键码
        }
        key=KBD_GetKey(); 							// 读取键码
            
        if (key==KBD_CODE_START)
        {
          //set DHC temperature
          /*
          if (!(DHC_SetTemp(&targetTemp)))
          {
            SND_Play(ssError);
            return;
          }
          */
          HEATER_SetTemp(&targetTemp);    // Gavin, 171207，设置加热器温度
          break;
        } else
        switch(key)
        {
          case KBD_CODE_STOP:
            SND_Play(ssCancel);
            return;
          case KBD_CODE_DOWN:
            SND_Play(ssSlowDown);
            targetTemp-=0.1;
            break;
          case KBD_CODE_UP:
            SND_Play(ssSlowDown);
            targetTemp+=0.1;
            break;
                
          default:
            SND_Play(ssError);
        }
        if (targetTemp>MAX_TEMP_VALUE)
          targetTemp=MAX_TEMP_VALUE;
        if (targetTemp<MIN_TEMP_VALUE)
          targetTemp=MIN_TEMP_VALUE;
      }        
      SND_Play(ssOk);

      // show calibr str
      LED_Print(LED_DISPLAY_TIME, false, "WAIT");      // 2017-12-12 星期二 21:04:42，显示校正标题
      
      // 获取外部PT100检测温度------------------------------
      TIMER_Start(TIMER_GENERAL, TO_HEATER); 								// Gavin, 2019-3-28 14:57:20, A4.1
      for(;;)
      {
      	// Gavin, 2019-3-28 14:58:15, A4.1
	      //*
        if (TIMER_TimeOut(TIMER_GENERAL)) 									// 通用定时器中断
				{
					TIMER_Start(TIMER_GENERAL,TO_HEATER); 						// 启动动用定时器
					
	        //show temperature from ADC and wait for start key
        	actTemp=TEMP_GetValue(ADC_CHNL_EXTERNAL); 					// 获取外部温度
        	sprintf_P(str, tempFmt, actTemp);        
        	//LED_Print(LED_DISPLAY_SPEED, true, str);
        	LED_Print(LED_DISPLAY_TEMP, false, str); 						// 显示外部温度
        	
        	IntTemp=TEMP_GetValue(ADC_CHNL_INTERNAL); 				// 获取内部温度，判断温度稳定需要使用内部温度，Gavin, 2019-3-28 15:27:12, A4.3
        	
					checkTemp=0.97*checkTemp+0.03*IntTemp;
					
					//check temperature, exit if ok£» MAX_TEMP_DIFF = 0.2
					if ((fabs(IntTemp-targetTemp)<=MAX_TEMP_DIFF)&&(fabs(IntTemp-checkTemp)<0.1))
					{
						if(tempflag == 0) 										// 2014/7/10 9:52:22
						{
							tempflag = 1;
							
							if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
							{
								SND_Play(ssAlarm);
							}
							
							LED_Print_P(LED_DISPLAY_TIME, true, messOK); 		// 2017-11-8 ÐÇÆÚÈý 23:12:58
						}
					}
					
					if(tempflag)
					{
						if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
						{
							SND_Play(ssAlarm);
						}
					}
				}
				
        if (KBD_KeyPressed())
        {
          key=KBD_GetKey();
          if (key==KBD_CODE_STOP) 
          {
            SND_Play(ssCancel);
            return;
          } else
          if (key==KBD_CODE_START)
          {
            break;
          } else SND_Play(ssError);
        }
      }
      SND_Play(ssOk);
        
      //LED_Print_P(LED_DISPLAY_TEMP, false, realStr);
      LED_Print_P(LED_DISPLAY_TIME, false, realStr);      // Gavin, 171207，显示输入实际温度标题
      targetTemp=actTemp;
			
			// 设置实际检测温度-----------------------------------
      for(;;)
      {
        sprintf_P(str, tempFmt, targetTemp);
        //LED_Print(LED_DISPLAY_SPEED, true, str);
        LED_Print(LED_DISPLAY_TEMP, true, str);    // Gavin, 171207，显示设置实际温度
        
        while(!KBD_KeyPressed())   
        {
          EXTERNAL_Process();
          //ERROR_Check();
        }
        key=KBD_GetKey();
            
        if (key==KBD_CODE_START)
        {
          break;
        } else
        switch(key)
        {
          case KBD_CODE_STOP:
            SND_Play(ssCancel);
            return;
          //case KBD_CODE_SPEED_DOWN:
          case KBD_CODE_DOWN:
            SND_Play(ssSlowDown);
            targetTemp-=0.1;
            break;
          //case KBD_CODE_SPEED_UP:
          case KBD_CODE_UP:
            SND_Play(ssSlowDown);
            targetTemp+=0.1;
            break;
                
          default:
            SND_Play(ssError);
        }
        if (targetTemp>MAX_TEMP_VALUE)
          targetTemp=MAX_TEMP_VALUE;
        if (targetTemp<MIN_TEMP_VALUE)
          targetTemp=MIN_TEMP_VALUE;
      }        
      SND_Play(ssOk);

      i=true;
      
      if (!TEMP_SetCalPoint(ADC_CHNL_EXTERNAL, point, &targetTemp))
      {
        i=false;
          break;
      }
    }

    if (i)      
      if (!TEMP_Calibrate())
        i=false;

    LED_Print_P(LED_DISPLAY_TIME, false, adjustStr);    // Gavin, 171207
    
    // 显示校正结果-----------------------------------------
    if (i)
    {
      //LED_Print_P(LED_DISPLAY_SPEED, false, okStr);
      LED_Print_P(LED_DISPLAY_TEMP, false, okStr);    // Gavin, 171207
      //SND_Play(ssOk);
      CALIBRATE_Ok=true;
    } else
    {
      //LED_Print_P(LED_DISPLAY_SPEED, false, errorStr);  
      LED_Print_P(LED_DISPLAY_TEMP, false, errorStr);   // Gavin, 171207
      //SND_Play(ssError);
      CALIBRATE_Ok=false;
    }

    for(;;)
    {
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
        //ERROR_Check();
      }
      key=KBD_GetKey();
      SND_Play(ssOk);
      break;
    }
		
		// 判断是否连接打印板，以获取时间-----------------------
    if ((CALIBRATE_Ok)&&(globalDeviceStatus.installedPrinter))
    {
      // save Adjustment date/time
      if (RTC_GetDateTime(&curDTime))
      /*{
        curDTime.Year=0;
        curDTime.Month=0;
        curDTime.Day=0;
        curDTime.Hour=0;
        curDTime.Minute=0;
        curDTime.Second=0;
      }*/
      //timeSensorCal=curDTime;
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeSensorCal, (TByte *)&curDTime, sizeof(EEPROM1.timeSensorCal));
    }
		
		// 判断打印板是否连接，以打印报告-----------------------
    if (globalDeviceStatus.installedPrinter)
    {
    	if (EDIT_Bool(printReq))
    	{
    		//REPORT_Print(REPORT_Calibr); 						// Gavin 2015/12/2 10:02:57
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
    			REPORT_Print(REPORT_Calibr_NORM);
    		else
    		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
    			REPORT_Print(REPORT_Calibr_PORT);
    	}
    }
  } // end if ext. sensor installed
}

void CTL_AdjustmentAutosampler(TByte NumPoints)
{
  TByte point, key, i;
  TByte channel;
  TReal32 targetTemp;
  signed char value;
  TList list;
  char str[10];
	
	// Gavin 20131224---------------------
  //if (EEPROM1.Volume<VOLUME_MODE_1000)
  //  return;

  if ((!NumPoints)||(NumPoints>NUM_CAL_POINTS))
    return;
  
  if (!globalDeviceStatus.haveStepper)
    return;

  list.numItems=DEPTH_MODE_LAST;
  list.items=CONF_LIST_DEPTH;
  list.current=EEPROM1.Volume-VOLUME_MODE_1000;
  list.title=calStepStr;
  value=MENU_RunList(&list);
  if (value==-1)
  {
    STEPPER_Start(true, true);
    return;
  }
  LED_Print_P(LED_DISPLAY_TEMP, false, CONF_LIST_VOLUME[value+VOLUME_MODE_1000]);

  //reset Adjustment variables
  CALIBRATE_Init();
    
  //set depth
  STEPPER_CalcDistance(value);
  STEPPER_Start(false, false);
  LED_Print_P(LED_DISPLAY_SPEED, true, messWait);
    
  do {
    key=STEPPER_GetStatus();
  } while (((key==PROBE_STATUS_MD)||(key==PROBE_STATUS_MU))&&(!KBD_KeyPressed()));
    
  if (KBD_KeyPressed())
  {
    key=KBD_GetKey();
    STEPPER_Stop();
  }

  //sampler down
  //STEPPER_CalcDistance(EEPROM1.Volume);
  //STEPPER_Start(false, false);
  
  i=false;
  CALIBRATE_Point[0][0].channel=CALIBRATE_AS;

  for(point=0;point<NumPoints;point++)
  {
    // show setting temperature string
    LED_Print_P(LED_DISPLAY_TEMP, false, setTempStr);
    DHC_GetTemp(&targetTemp);
    for(;;)
    {
      //set DHC temperature
      sprintf_P(str, tempFmt, targetTemp);
      LED_Print(LED_DISPLAY_SPEED, true, str);
            
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
      }
      key=KBD_GetKey();
            
      if (key==KBD_CODE_START)
      {
        //set DHC temperature
        if (!(DHC_SetTemp(&targetTemp)))
        {
          SND_Play(ssError);
          return;
        }
        break;
      } else
      switch(key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        //case KBD_CODE_SPEED_DOWN:
        case KBD_CODE_DOWN:
          SND_Play(ssSlowDown);
          targetTemp-=0.1;
          break;
        //case KBD_CODE_SPEED_UP:
        case KBD_CODE_UP:
          SND_Play(ssSlowDown);
          targetTemp+=0.1;
          break;
        default:
          SND_Play(ssError);
      }
      if (targetTemp>MAX_TEMP_VALUE)
        targetTemp=MAX_TEMP_VALUE;
      if (targetTemp<MIN_TEMP_VALUE)
        targetTemp=MIN_TEMP_VALUE;
    }        
    SND_Play(ssOk);

    // show adjust str
    sprintf_P(str, adjuFmt, point+1);
    LED_Print(LED_DISPLAY_TEMP, false, str);
        
    for (channel=0; channel<EEPROM1.NumSensors; channel++)
    {
      //calibrate all sensors
      if (SAMPLER_Temp[channel]==TEMP_ERROR)
        continue;
      LED_Print_P(LED_DISPLAY_TEMP, false, calASStr);
      sprintf_P(str, sensNameFmt, channel+1);
      LED_Print(LED_DISPLAY_SPEED, false, str);
      while (!KBD_KeyPressed())
        continue;
      key=KBD_GetKey();
      if (key==KBD_CODE_STOP) 
      {
        SND_Play(ssCancel);
        //temp=0.0;
        //STEPPER_SetDistance(&temp);
        STEPPER_Start(true, true);
        return;
      } else SND_Play(ssOk);
      for(;;)
      {
        //show temperature from DHC and wait for start key
        //SAMPLER_GetTemp(1, &targetTemp);
        //SAMPLER_GetTemp();
        targetTemp=(TReal32)SAMPLER_Temp[channel]/100;
        sprintf_P(str, tempFmt, targetTemp);        
        LED_Print(LED_DISPLAY_SPEED, true, str);
        
        if (KBD_KeyPressed())
        {
          key=KBD_GetKey();
          if (key==KBD_CODE_STOP) 
          {
            SND_Play(ssCancel);
            //temp=0.0;
            //STEPPER_SetDistance(&temp);
            STEPPER_Start(true, true);
            return;
          } else
          if (key==KBD_CODE_START)
          {
            break;
          } else SND_Play(ssError);
        }
      }
      SND_Play(ssOk);
        
      LED_Print_P(LED_DISPLAY_TEMP, false, realStr);
	
      for(;;)
      {
        sprintf_P(str, tempFmt, targetTemp);
    	LED_Print(LED_DISPLAY_SPEED, true, str);
            
        while(!KBD_KeyPressed())   
        {
          EXTERNAL_Process();
          //ERROR_Check();
        }
        key=KBD_GetKey();
            
        if (key==KBD_CODE_START)
        {
          break;
        } else
        switch(key)
        {
          case KBD_CODE_STOP:
            SND_Play(ssCancel);
            return;
          //case KBD_CODE_SPEED_DOWN:
          case KBD_CODE_DOWN:
            SND_Play(ssSlowDown);
            targetTemp-=0.1;
            break;
          //case KBD_CODE_SPEED_UP:
          case KBD_CODE_UP:
            SND_Play(ssSlowDown);
            targetTemp+=0.1;
            break;
                
          default:
            SND_Play(ssError);
        }
        if (targetTemp>MAX_TEMP_VALUE)
          targetTemp=MAX_TEMP_VALUE;
        if (targetTemp<MIN_TEMP_VALUE)
          targetTemp=MIN_TEMP_VALUE;
      }        
      SND_Play(ssOk);

      i=true;
            
      if (!point)
      {
        if (!SAMPLER_Calibrate(0, &targetTemp, channel))
        {
          i=false;
          break;
        }
      } else
      {
        if (!SAMPLER_Calibrate(1, &targetTemp, channel))
        {
          i=false;
          break;
        }
      }
    }//end one point for all sensors
  }

  LED_Print_P(LED_DISPLAY_TEMP, false, adjustStr);
    
  if (i)
  {
    LED_Print_P(LED_DISPLAY_SPEED, false, okStr);
    //SND_Play(ssOk);
    CALIBRATE_Ok=true;
  } else
  {
    LED_Print_P(LED_DISPLAY_SPEED, false, errorStr);  
    //SND_Play(ssError);
    CALIBRATE_Ok=false;
  }

  for(;;)
  {
    while(!KBD_KeyPressed())   
    {
      EXTERNAL_Process();
      //ERROR_Check();
    }
    key=KBD_GetKey();
    SND_Play(ssOk);
    break;
  }

  if ((CALIBRATE_Ok)&&(globalDeviceStatus.installedPrinter))
  {
    // save Adjustment date/time
    if (RTC_GetDateTime(&curDTime))
    /*{
      curDTime.Year=0;
      curDTime.Month=0;
      curDTime.Day=0;
      curDTime.Hour=0;
      curDTime.Minute=0;
      curDTime.Second=0;
    }*/
    //timeSensorCal=curDTime;
      EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeSamplerCal, (TByte *)&curDTime, sizeof(EEPROM1.timeSamplerCal));
  }

  //sampler up
  //temp=0.0;
  //STEPPER_SetDistance(&temp);
  STEPPER_Start(true, true);

  if (globalDeviceStatus.installedPrinter)
	{
		if (EDIT_Bool(printReq))
		{
			//REPORT_Print(REPORT_Calibr); 							// Gavin 2015/12/2 10:02:57
			
			if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
  			REPORT_Print(REPORT_Calibr_NORM);
  		else
  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
  			REPORT_Print(REPORT_Calibr_PORT);
		}
	}
}

TBool CTL_Brightness(TByte level)
{
  return LED_SetBrightness(level);
}                                

TBool CTL_GetBrightness(TByte *level)
{
  if (EEPROM1.LED>=LED_BRIGHTNESS_NUM)
    return false;
  *level = EEPROM1.LED;
  return true;
}                                

void CTL_SetTime(void)
{
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

void CTL_SetDate(void)
{
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
                
void CTL_SetYear(void)
{
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
  } else SND_Play(ssError);
}

void CTL_AdjustmentStepper(void)
{
  TByte key;
  TReal32 tempVal;
  TWord Val;
  char str[10];
  signed char value;
  TList list;
  TBool stop;

  //choose volume
  list.numItems=DEPTH_MODE_LAST;
  list.items=CONF_LIST_DEPTH;
  list.current=0;
  list.title=calStepStr;
  for (;;)
  {
    value=MENU_RunList(&list);
    if (value==-1)
    {
      STEPPER_Start(true, true);
      return;
    }
    LED_Print_P(LED_DISPLAY_TEMP, false, CONF_LIST_DEPTH[value]);
    stop=false;
    
    //set depth
    STEPPER_CalcDistance(value);
    STEPPER_Start(false, false);
    LED_Print_P(LED_DISPLAY_SPEED, true, messWait);
    
    do {
      key=STEPPER_GetStatus();
    } while (((key==PROBE_STATUS_MD)||(key==PROBE_STATUS_MU))&&(!KBD_KeyPressed()));
    
    if (KBD_KeyPressed())
    {
      key=KBD_GetKey();
      STEPPER_Stop();
    }

    //edit depth
    tempVal=0.0;
    for (;;)
    {
      TIMER_Start(TIMER_GENERAL,TO_20MSEC);
      while(!TIMER_TimeOut(TIMER_GENERAL))
      //while (!KBD_KeyPressed())
      {  
        STEPPER_GetPosition(&tempVal);
        sprintf_P(str, speedFmt, tempVal);
        LED_Print(LED_DISPLAY_SPEED, false, str);
      }
      TIMER_Stop(TIMER_GENERAL);
    
      tempVal=0.0;
      if (KBD_KeyPressed())
      {  
        key=KBD_GetKey();

        switch(key)
        {
          //case KBD_CODE_SPEED_DOWN:
          case KBD_CODE_DOWN:
            //STEPPER_SetDistance(&tempVal);
            STEPPER_Start(false, true);
            KBD_Reset();
            break;
          //case KBD_CODE_SPEED_UP:
          case KBD_CODE_UP:
            //STEPPER_SetDistance(&tempVal);
            STEPPER_Start(true, true);
            KBD_Reset();
            break;
          case KBD_CODE_STOP:
            STEPPER_Stop();
            stop=true;
            SND_Play(ssCancel);
            break;
          case KBD_CODE_START:
            STEPPER_Stop();
            STEPPER_GetPosition(&tempVal);
            Val=(TWord)(tempVal*10);
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.VolumeDepth[value], (TByte *)&Val, sizeof(TWord));
            stop=true;
            SND_Play(ssOk);
            break;
          default:
            SND_Play(ssError);
            break;
        }
      } else
      {
        STEPPER_Stop();
      }
      if (stop) break;
    }
    //KBD_Reset();
  }
}

void CTL_MethodOffset(TByte Num)
{
  char str[10];
  TWord value;
  TByte key;
  
  if (Num>=METH_MODE_LAST)
    return;
  
  value=EEPROM1.MethodOffset[Num];

  for(;;)
  {
    sprintf_P(str, basketFmt, (TReal32)value/10);
    LED_Print(LED_DISPLAY_SPEED, false, str);
    
    while(!KBD_KeyPressed());
  
    key=KBD_GetKey();
      
    switch(key)
    {
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        SND_Play(ssSlowUp);                
        value++;                                
        break;
      //case KBD_CODE_SPEED_DOWN:              
      case KBD_CODE_DOWN:              
        SND_Play(ssSlowDown);
        if (value)
          value--;
	break;
      case KBD_CODE_START:
	SND_Play(ssOk);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MethodOffset[Num], (TByte *)&value, sizeof(TWord));
	return;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
	return;
      default:
	SND_Play(ssError);
    }
    //if (value>(TWord)(MAX_BASKET_VALUE*10))
    //  value=(TWord)(MAX_BASKET_VALUE*10);
    if (value>METHOD_MAX_OFFS[Num])
      value = METHOD_MAX_OFFS[Num];
  }
}

/*void CTL_CheckTemperature(void)
{
  TByte key, i;
  TReal32 targetTemp;
  TWord temp;
  char str[10];

  for (i=0; i<TEMP_CHECK_POINTS; i++)
    CALIBRATE_CheckPoint[i].hasResult=false;

  for (i=0; i<TEMP_CHECK_POINTS; i++)
  {  
    //show point
    sprintf_P(str, chkPointFmt, i+1);
    LED_Print(LED_DISPLAY_SPEED, false, str);
    //set target temperature
    temp=EEPROM1.CheckTemp[i];
    for (;;)
    {
      sprintf_P(str, chkTempFmt, (TReal32)temp/10);
      LED_Print(LED_DISPLAY_TEMP, true, str);
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
      }  

      key=KBD_GetKey();
        
      if (key==KBD_CODE_START)
      {
        SND_Play(ssOk);
        if (temp!=EEPROM1.CheckTemp[i])
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.CheckTemp[i], (TByte *)&temp, sizeof(TWord));
        break;
      } else
      switch (key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        case KBD_CODE_TEMP_UP:
          temp++;
          SND_Play(ssSlowUp);
          break;
        case KBD_CODE_TEMP_DOWN:
          if (temp) 
            temp--;
          SND_Play(ssSlowDown);
          break;
        default:
          SND_Play(ssError);
      }
      targetTemp=(TReal32)temp/10;
      if (targetTemp>MAX_TEMP_VALUE)
        targetTemp=MAX_TEMP_VALUE;
      if (targetTemp<MIN_TEMP_VALUE)
        targetTemp=MIN_TEMP_VALUE;
      temp=(TWord)(targetTemp*10);
    }  
    
    //set DHC temperature
    targetTemp=(TReal32)temp/10;
    if (!(DHC_SetTemp(&targetTemp)))
    {
      SND_Play(ssError);
      return;
    }
    //show current temperature string
    sprintf_P(str, chkTempFmt, (TReal32)temp/10);
    LED_Print(LED_DISPLAY_TEMP, false, str);
    for(;;)
    {
      //get DHC temperature
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
        if (globalDeviceStatus.installedExtADC)
        {
          targetTemp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
          if (globalDeviceStatus.sensorExtError)
            DHC_GetTemp(&targetTemp);
        } else DHC_GetTemp(&targetTemp);
        //DHC_GetTemp(&targetTemp);
        sprintf_P(str, tempFmt, targetTemp);
        LED_Print(LED_DISPLAY_SPEED, true, str);
      }

      key=KBD_GetKey();
        
      if (key==KBD_CODE_START)
      {
        SND_Play(ssOk);
        break;
      } else
      switch (key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        default:
          SND_Play(ssError);
      }
    }
    //save temperature
    CALIBRATE_CheckPoint[i].Temperature=EEPROM1.CheckTemp[i];
    temp=(TWord)(targetTemp*10);
    //enter real temperature
    for(;;)
    {
      sprintf_P(str, tempFmt, (TReal32)temp/10);
      LED_Print(LED_DISPLAY_SPEED, true, str);
            
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
      }
      key=KBD_GetKey();

      if (key==KBD_CODE_START)
      {
        //set real temperature
        CALIBRATE_CheckPoint[i].actTemperature=temp;
        CALIBRATE_CheckPoint[i].hasResult=true;
        SND_Play(ssOk);
        break;
      } else
      switch(key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        case KBD_CODE_SPEED_DOWN:
          SND_Play(ssSlowDown);
          if (temp) 
            temp--;
          break;
        case KBD_CODE_SPEED_UP:
          SND_Play(ssSlowDown);
          temp++;
          break;
        default:
          SND_Play(ssError);
      }
      targetTemp=(TReal32)temp/10;
      if (targetTemp>MAX_TEMP_VALUE)
        targetTemp=MAX_TEMP_VALUE;
      if (targetTemp<MIN_TEMP_VALUE)
        targetTemp=MIN_TEMP_VALUE;
      temp=(TWord)(targetTemp*10);
    }        
  }
  //print report
  if (globalDeviceStatus.installedPrinter)
    if (EDIT_Bool(printReq))
      REPORT_Print(REPORT_Check);
}*/

void CTL_CheckTemperature(TByte channel)
{
  TByte key, i;
  TByte pointsNum;
  TBool tempflag = 0; 									// Gavin, A6, 2019-04-01 星期一 14:39:11
  TReal32 targetTemp, haveTemp;
  TReal32 checkTemp = 0.0; 							// Gavin, A6, 2019-04-01 星期一 14:39:16
  TReal32 IntTemp; 											// Gavin, A6, 2019-04-01 星期一 15:27:54
  TWord temp;
  char str[10];

  for (i=0; i<TEMP_CHECK_POINTS; i++)
    CALIBRATE_CheckPoint[i].hasResult=false;
  
  if (channel==CALIBRATE_AS)
    pointsNum=TEMP_CHECK_POINTS;
  else  
    pointsNum=1;
  
  
  //show point
  //sprintf_P(str, chkPointFmt, i+1);
  //strcpy_P(str, checkTempStr);
  //LED_Print(LED_DISPLAY_TEMP, false, str); 						// Gavin, 2019-03-29 星期五 16:05:07, A6
  LED_Print_P(LED_DISPLAY_TIME, false, setTempStr); 		// 标题，设置加热温度
  
  temp=EEPROM1.CheckTemp[0];
  for (;;) 																							// 设置检查温度
  {
    sprintf_P(str, chkTempFmt, (TReal32)temp/10);
    //LED_Print(LED_DISPLAY_SPEED, true, str); 					// Gavin, 2019-03-29 星期五 16:06:31, A6
    LED_Print(LED_DISPLAY_TEMP, true, str); 						// 显示刚才获得的加热器温度
    
    while(!KBD_KeyPressed())   
    {
      EXTERNAL_Process();
    }  

    key=KBD_GetKey();
        
    if (key==KBD_CODE_START)
    {
      SND_Play(ssOk);
      if (temp!=EEPROM1.CheckTemp[0])
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.CheckTemp[0], (TByte *)&temp, sizeof(TWord));
      break;
    } else
    switch (key)
    {
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        temp++;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        if (temp) 
          temp--;
        SND_Play(ssSlowDown);
        break;
      default:
        SND_Play(ssError);
    }
    targetTemp=(TReal32)temp/10;
    if (targetTemp>MAX_TEMP_VALUE)
      targetTemp=MAX_TEMP_VALUE;
    if (targetTemp<MIN_TEMP_VALUE)
      targetTemp=MIN_TEMP_VALUE;
    temp=(TWord)(targetTemp*10);
  }  

  //set DHC temperature
  targetTemp=(TReal32)temp/10;
  /*if (!(DHC_SetTemp(&targetTemp))) 							// Gavin, 2019-03-29 星期五 16:09:44, A6
  {
    SND_Play(ssError);
    return;
  }*/
  HEATER_SetTemp(&targetTemp);    								// Gavin, 171207，设置加热器温度
  
  LED_Print(LED_DISPLAY_TIME, false, "WAIT");     // Gavin, 2019-03-29 星期五 16:15:02, A6
  
  for (i=0; i<pointsNum; i++)
  {  
    //show current temperature string
    if (channel==CALIBRATE_AS)
      sprintf_P(str, chkAsFmt, i+1);
    else
      sprintf_P(str, chkTempFmt, targetTemp);
    LED_Print(LED_DISPLAY_TEMP, false, str);
    
    // Gavin, A6, 2019-04-01 星期一 14:30:11
    TIMER_Start(TIMER_GENERAL, TO_HEATER); 								// Gavin, 2019-3-27 16:35:26, A4.1
    for(;;)
    {
      //get DHC temperature
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
        
        if (TIMER_TimeOut(TIMER_GENERAL)) 									// 通用定时器中断
				{
					TIMER_Start(TIMER_GENERAL,TO_HEATER); 						// 启动动用定时器
					
					switch (channel)
	        {
	          case CALIBRATE_ADC:
	            if (globalDeviceStatus.installedExtADC)
	            {
	              haveTemp=TEMP_GetValue(ADC_CHNL_EXTERNAL); 				// 获取外部温度
	              IntTemp = TEMP_GetValue(ADC_CHNL_INTERNAL); 			// 获取内部温度
	            }
	            break;
	          case CALIBRATE_DHC:
	            //DHC_GetTemp(&haveTemp);
	            haveTemp=TEMP_GetValue(ADC_CHNL_INTERNAL); 					// 获取内部温度
	            IntTemp = haveTemp;
	            break;
	          case CALIBRATE_AS:  
	            haveTemp=(TReal32)SAMPLER_Temp[i]/100;
	            break;
	        }  
	        
	        if ((channel==CALIBRATE_AS)&&(SAMPLER_Temp[i]==TEMP_ERROR))
	          strcpy_P(str, errorTemp);
	        else
	          sprintf_P(str, tempFmt, haveTemp);
	        //LED_Print(LED_DISPLAY_SPEED, true, str); 					// Gavin, 2019-03-29 星期五 16:16:42, A6
	        LED_Print(LED_DISPLAY_TEMP, false, str); 						// Gavin, 2019-03-29 星期五 16:17:05, A6
      		
      		checkTemp=0.97*checkTemp+0.03*IntTemp;
					
					//check temperature, exit if ok£» MAX_TEMP_DIFF = 0.2
					if ((fabs(IntTemp-targetTemp)<=MAX_TEMP_DIFF)&&(fabs(IntTemp-checkTemp)<0.1))
					{
						if(tempflag == 0) 										// 2014/7/10 9:52:22
						{
							tempflag = 1;
							
							if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
							{
								SND_Play(ssAlarm);
							}
							
							LED_Print_P(LED_DISPLAY_TIME, true, messOK); 		// 2017-11-8 ÐÇÆÚÈý 23:12:58
						}
					}
					
					if(tempflag)
					{
						if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
						{
							SND_Play(ssAlarm);
						}
					}
				}
      }
	    
      key=KBD_GetKey();
        
      if (key==KBD_CODE_START)
      {
        SND_Play(ssOk);
        break;
      } else
      switch (key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        default:
          SND_Play(ssError);
      }
    }
    //save temperature
    CALIBRATE_CheckPoint[i].Temperature=(TWord)(targetTemp*10);
    CALIBRATE_CheckPoint[i].readTemperature=(TWord)(haveTemp*10);
    CALIBRATE_CheckPoint[i].channel=channel;
    temp=(TWord)(targetTemp*10);
    
    //enter real temperature
    LED_Print_P(LED_DISPLAY_TIME, false, realStr);      		// Gavin, A6, 2019-04-01 星期一 14:16:03，显示输入实际温度标题
    for(;;)
    {
      sprintf_P(str, tempFmt, (TReal32)temp/10);
      LED_Print(LED_DISPLAY_TEMP, true, str);    						// Gavin, A6, 2019-04-01 星期一 14:17:29，显示设置实际温度
      
      while(!KBD_KeyPressed())   
      {
        EXTERNAL_Process();
      }
      key=KBD_GetKey();

      if (key==KBD_CODE_START)
      {
        //set real temperature
        CALIBRATE_CheckPoint[i].actTemperature=temp;
        if ((channel==CALIBRATE_AS)&&(SAMPLER_Temp[i]==TEMP_ERROR))
          CALIBRATE_CheckPoint[i].hasResult=false;
        else  
          CALIBRATE_CheckPoint[i].hasResult=true;
        SND_Play(ssOk);
        break;
      } else
      switch(key)
      {
        case KBD_CODE_STOP:
          SND_Play(ssCancel);
          return;
        //case KBD_CODE_SPEED_DOWN:
        case KBD_CODE_DOWN:
          SND_Play(ssSlowDown);
          if (temp) 
            temp--;
          break;
        //case KBD_CODE_SPEED_UP:
        case KBD_CODE_UP:
          SND_Play(ssSlowDown);
          temp++;
          break;
        default:
          SND_Play(ssError);
      }
      targetTemp=(TReal32)temp/10;
      if (targetTemp>MAX_TEMP_VALUE)
        targetTemp=MAX_TEMP_VALUE;
      if (targetTemp<MIN_TEMP_VALUE)
        targetTemp=MIN_TEMP_VALUE;
      temp=(TWord)(targetTemp*10);
    }        
  }
  
  //print report
  if (globalDeviceStatus.installedPrinter)
	{
		if (EDIT_Bool(printReq))
		{
			if (channel==CALIBRATE_AS)
			{
				//REPORT_Print(REPORT_CheckCalAs); 				// Gavin 2015/12/2 10:02:57
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
	  			REPORT_Print(REPORT_CheckCalAs_NORM);
	  		else
	  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
	  			REPORT_Print(REPORT_CheckCalAs_PORT);
			}
			else  
			{
				//REPORT_Print(REPORT_CheckCal); 					// Gavin 2015/12/2 10:02:57
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
	  			REPORT_Print(REPORT_CheckCal_NORM);
	  		else
	  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
	  			REPORT_Print(REPORT_CheckCal_PORT);
			}
		}
	}
}

//------------------------------------------------------------------------------
// CTL_SetPwst - set pwst, v1.03，HHT，2024-06-14
//------------------------------------------------------------------------------
void CTL_SetPwst(void)
{
	char title[]="PWST";
	TWord value=0;
	TWord value1 = 0; 										// v1.03，HHT，2024-06-14
	char tStr[PASSWORD_LEN];
	//TByte i;
	
	value = EEPROM1.MGPassword;
	tStr[0] = 0x30 + value/1000; 	value %= 1000;
	tStr[1] = 0x30 + value/100; 	value %= 100;
	tStr[2] = 0x30 + value/10;
	tStr[3] = 0x30 + value%10;
	      
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, true))
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
// CTL_SetPwdt - set pwdt, v1.03，HHT，2024-06-14
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
	      
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, true))
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
// CTL_CheckPW - Check password, v1.03，HHT，2024-06-14
//------------------------------------------------------------------------------
signed char CTL_CheckPW(char *title, TWord code)
{
	//char title[]="PWMG";
	TWord value=0;
	char tStr[PASSWORD_LEN];
	//TByte i;
	signed char rt = 0;
	
	//value = EEPROM1.DTPassword;
	tStr[0] = '*';
	tStr[1] = '*';
	tStr[2] = '*';
	tStr[3] = '*';
	
	if (EDIT_AdvancedPW(title, tStr, PASSWORD_LEN, true))
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
