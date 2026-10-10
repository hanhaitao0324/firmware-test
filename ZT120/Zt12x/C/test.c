#include "eeprom.h"
#include "global.h"
#include "pins.h"
#include "fram.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "external.h"
#include "control.h"
#include "sound.h"
#include "strings.h"
#include "temp.h"
#include "dhc.h"
#include "syserror.h"
#include "printrtc.h"
#include "dropper.h"
#include "stepper.h"
#include "sampler.h"
#include "test.h"
#include "heater.h"
#include <math.h>

TBool M2_cur_f=false;
TBool M2_old_f=false;
TByte M2_flag = 0;
	
//------------------------------------------------------------------------------
//process key press during the test
//------------------------------------------------------------------------------
TBool TEST_ProcessKey(TBool *paused)
{
  TByte key;
  
  if(M2_flag != 0)
  {
  	if(M2_flag == 1)
  	{
  		if(!*paused)
	  	{
	  		MOTOR_Stop();
	    	SND_Play(ssOk);
	    	
	    	*paused = true;
	  	}
  	}
  	else
  	if(M2_flag == 2)
  	{
  		if (*paused)
	    {
	      *paused=false;
	      SND_Play(ssOk);
	      MOTOR_SetSpeed(EEPROM1.Speed);
	      MOTOR_Start();
	      TIMER_TickReset();
	    }
  	}
  	
  	M2_flag = 0;
  	return false;
  }
  
  key=KBD_GetKey();
  
  //if (!key) return false;
  if (KBD_CODE_START==key)
  {
    if (*paused)
    {
      *paused=false;
      SND_Play(ssOk);
      MOTOR_SetSpeed(EEPROM1.Speed);
      MOTOR_Start();
      TIMER_TickReset();
      //globalDeviceStatus.timeTick=false;
      //while (!globalDeviceStatus.timeTick);
      //globalDeviceStatus.timeTick=false;
    } 
    else 
      SND_Play(ssError);
  }
  else
  if (KBD_CODE_STOP==key) // 2017-11-9 ÐÇÆÚËÄ 15:42:54
  {
    MOTOR_Stop();
    SND_Play(ssOk);
    if ((*paused)||(globalTestStatus.goMode)||(globalTestStatus.unlimMode))
      return true;
    *paused=true;
  }
  else
    SND_Play(ssError);
  return false;
}

//------------------------------------------------------------------------------
//run test
//------------------------------------------------------------------------------
TBool TEST_Run(void)
{
	TBool paused, ledsUpdated, tempAlarm, speedAlarm, makeReport, timeOut;
	TBool tempflag; 										// ·ûºÏÎÂ¶ÈÌõ¼þ±êÖ¾,2014/7/10 9:50:37
	TWord SoundCount; 									// ÌáÊ¾Òô¼ÆÊýÆ÷,2014/7/10 14:02:05
	TWord SoundCount1; 									// 2014/7/15 16:09:51
	
	TReal32 speed, temp, prevTemp, checkTemp;
	TReal32 testMinSpeed, testMaxSpeed;
	TWord testMinTemp, testMaxTemp;
	TWord wVal;
	TByte key;
	//TWord ReportNum;
	TDWord usageCounter=0;
	//TWord ReportInterval=0; 						// 2017-11-9 ÐÇÆÚËÄ 18:50:23
	//TByte speedDelay=0;
	globalTestStatus.goMode=false;
	globalTestStatus.unlimMode=false;
	globalDeviceStatus.droppedMode=false;
	
	//autosampler parking
	//speed=0.0;
	//STEPPER_SetDistance(&speed);
	if (!globalDeviceStatus.remoteMode)//&&(globalDeviceStatus.Head==HEAD_MODE_LOW))
		STEPPER_Start(true, true);
	
	tempAlarm=false;
	speedAlarm=false;
	
	// wait for temperature
	temp=EEPROM1.Temp + (TReal32)EEPROM1.TempOffset[ADC_CHNL_INTERNAL]/10;      // Éè¶¨ÎÂ¶ÈÖµ
	//globalDeviceStatus.dhcFailure=false;
	//DHC_SetTemp(&temp);
	if(EEPROM1.HeatFlag == HEAT_MODE_ON) 		// 2017-12-17 ÐÇÆÚÈÕ 14:51:04
		HEATER_SetTemp(&temp); 								// 2017-11-7 ÐÇÆÚ¶þ 22:21:59
	checkTemp=0.0;
	
	if (globalTestStatus.State!=TEST_RUNNING)
	{
		//LED_Print_P(LED_DISPLAY_SPEED, false, messWait);
		LED_Print_P(LED_DISPLAY_TIME, false, messWait); 				// 2017-11-8 ÐÇÆÚÈý 23:13:20
		prevTemp=0.0;
		
		//2014/3/16 13:59:30
		//if ((globalDeviceStatus.installedExtADC)&&
		//		(globalDeviceType != DEVICE_QDT6))
        if (globalDeviceStatus.installedExtADC)
        {
            temp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
			
            if (globalDeviceStatus.sensorExtError)
            {
                //DHC_GetTemp(&temp);
                temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 	// 2017-11-7 ÐÇÆÚ¶þ 22:02:40
            }
        }
		else
        {
            //DHC_GetTemp(&temp);
            temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 		// 2017-11-7 ÐÇÆÚ¶þ 22:02:46
        }
		
		CTL_ShowTemp(temp, false);  // ÏÔÊ¾ÊµÊ±ÎÂ¶È
		TIMER_Start(TIMER_GENERAL, TO_HEATER);
		
		tempflag = 0; 										// 2014/7/10 9:51:11
		SoundCount = 0; 									// 2014/7/10 14:02:38
		SoundCount1 = 0; 									// 2014/7/15 16:10:31
		
		for (;;)
		{
			globalTestStatus.State=TEST_WAITING;
			if (TIMER_TimeOut(TIMER_GENERAL))
			{
				TIMER_Start(TIMER_GENERAL,TO_HEATER);
				
				//2014/3/17 16:21:21
				//if ((globalDeviceStatus.installedExtADC)&&
				//		(globalDeviceType != DEVICE_QDT6))
				if (globalDeviceStatus.installedExtADC)
				{
					temp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
					if (globalDeviceStatus.sensorExtError)
					{
						//DHC_GetTemp(&temp);
						temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 				// 2017-11-7 ÐÇÆÚ¶þ 22:03:31
					}
				}
				else
				{
					//DHC_GetTemp(&temp);
					temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 					// 2017-11-7 ÐÇÆÚ¶þ 22:03:56
				}
				
				checkTemp=0.97*checkTemp+0.03*temp;
				
				//check temperature, exit if ok£» MAX_TEMP_DIFF = 0.2
				if ((fabs(temp-(TReal32)EEPROM1.TempOffset[ADC_CHNL_INTERNAL]/10-EEPROM1.Temp)<=MAX_TEMP_DIFF)&&(fabs(temp-checkTemp)<0.1))
				{
					//TIMER_Stop(TIMER_GENERAL); 					// 2014/7/9 14:01:55
					//break; 															// 2014/7/9 14:02:06
					
					if(tempflag == 0) 										// 2014/7/10 9:52:22
					{
						tempflag = 1;
						//SND_Play(ssAlarm); 										// 2014/7/8 11:27:07
						if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
						{
							SND_Play(ssAlarm);
						}
						
						//LED_Print_P(LED_DISPLAY_SPEED, true, messOK);
						LED_Print_P(LED_DISPLAY_TIME, true, messOK); 		// 2017-11-8 ÐÇÆÚÈý 23:12:58
					}
				}
				
				/*prevTemp=temp; 
				  if (globalDeviceStatus.installedExtADC)
				  {
				  	temp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
				  	if (globalDeviceStatus.sensorExtError)
				  		temp=prevTemp;
				  }*/
				
				CTL_ShowTemp(temp, false);
			}
			
			// 2014/7/10 14:11:50
			if(tempflag == 1)
			{
				SoundCount++;
				if(SoundCount >= 0x8000) 			//2014/7/12 9:35:07
				{
					SoundCount = 0;
					
					SoundCount1++;
					if(SoundCount1 >= 3)
					{
						SoundCount1 = 0;
						//SND_Play(ssOk);
						//SND_Play(ssAlarm);
						if (EEPROM1.Alarm==ALARM_MODE_ON) 	// 2014/8/18 19:21:39
						{
							SND_Play(ssAlarm);
						}
					}
				}
			}
			
			if (KBD_KeyPressed())
			{
				//any key is pressed
				key=KBD_GetKey();
				if (key==KBD_CODE_START)
				{
					if(!M2_cur_f)
					{
						SND_Play(ssOk);
						TIMER_Stop(TIMER_GENERAL);
						break;
					}
					else
						SND_Play(ssError);
				}
				else
				if (key==KBD_CODE_STOP)
				{
					SND_Play(ssCancel);
					TIMER_Stop(TIMER_GENERAL);
					globalTestStatus.State=TEST_STOPPED;
					//DHC_Standby();
					return false;
				}
				else
					SND_Play(ssError);
			}
			
			// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
			if(macBitTest(PIN_DROP_IN, pinM2_Close))
			{
				M2_cur_f = true;
				macBitOff(PORT_LED0, pinLED); 				// µãÁÁPCBÌùÆ¬LEDÖ¸Ê¾µÆ
			}
			else
			{
				M2_cur_f = false;
				macBitOn(PORT_LED0, pinLED);
			}
			
			if((!M2_cur_f)&&(M2_old_f))
			{
				SND_Play(ssOk);
				TIMER_Stop(TIMER_GENERAL);
				break;
			}
			M2_old_f = M2_cur_f;
			
			EXTERNAL_Process();
			ERROR_Check();
			if (globalTestStatus.State==TEST_STOPPED)
				return false;
		}
		
		if ((EEPROM1.Head==HEAD_MODE_LOW)&&(EEPROM1.Method==METH_MODE_PADL))
		{
			//dropping mode - manual or auto
			//if (EEPROM1.DeviceType<DEVICE_DT722)
			// Gavin 20131226---------------
			/*
			if (!globalDeviceStatus.haveDropper)
			{  
			  //wait for dropping
			  if (globalTestStatus.State!=TEST_RUNNING)
			  {
			    LED_Print_P(LED_DISPLAY_SPEED, true, messDrop);
			    for (;;)
			    {
			      if (KBD_KeyPressed())
			      {
			        //any key is pressed
			        key=KBD_GetKey();
			        if (key==KBD_CODE_START)
			        {
			          SND_Play(ssOk);
			          TIMER_Stop(TIMER_GENERAL);
			          break;
			        } else
			        if (key==KBD_CODE_STOP)
			        {
			          SND_Play(ssCancel);
			          TIMER_Stop(TIMER_GENERAL);
			          globalTestStatus.State=TEST_STOPPED;
			          //DHC_Standby();
			          return false;
			        } else SND_Play(ssError);
			      }
			      EXTERNAL_Process();
			      ERROR_Check();
			      if (globalTestStatus.State==TEST_STOPPED)
			        return false;
			    }
			  }
			}
			else
			{
			  // auto dropping
			  LED_Print_P(LED_DISPLAY_SPEED, false, messDrop);
			  if (!DROPPER_Open())
			    ERROR_Check();
			  if (globalTestStatus.State==TEST_STOPPED)
			    return false;
			  //wait for some seconds
			  TIMER_Start(TIMER_GENERAL,TO_2SEC);
			  //while ((!KBD_KeyPressed())&&(!TIMER_TimeOut(TIMER_GENERAL)))
			  while (!TIMER_TimeOut(TIMER_GENERAL))
			  {
			    EXTERNAL_Process();
			    if (KBD_KeyPressed())
			    {
			      key=KBD_GetKey();
			      //if (key==KBD_CODE_STOP)
			      //{
			      //  SND_Play(ssOk);
			      //  return false;
			      //} else 
			        SND_Play(ssError);
			    }
			  }
			  TIMER_Stop(TIMER_GENERAL);
			  if (!DROPPER_Close())
			    ERROR_Check();
			  if (globalTestStatus.State==TEST_STOPPED)
			    return false;
			}
			*/
			;
		}
	}
	
	globalTestStatus.State=TEST_RUNNING;
	
	//2014/3/17 16:23:12
	//if(globalDeviceType != DEVICE_QDT6)
	//{
    if (globalDeviceStatus.installedExtADC)
    {
        temp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
        //DHC_GetTemp(&prevTemp);
        prevTemp = TEMP_GetValue(ADC_CHNL_INTERNAL); 					// 2017-11-7 ÐÇÆÚ¶þ 22:06:39
        if (globalDeviceStatus.sensorExtError)
            temp=prevTemp;
    }
    else
    {
        //DHC_GetTemp(&temp);
        temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 							// 2017-11-7 ÐÇÆÚ¶þ 22:07:29
    }
	//}
	//else
	//{
		//DHC_GetTemp(&temp); // 2017-11-9 ÐÇÆÚËÄ 15:28:40
		//temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 								// 2017-11-7 ÐÇÆÚ¶þ 22:07:56
	//}
	
	CTL_ShowTemp(temp, false);
	
	globalTestStatus.Value=0;//DT_MAX_TIME-600;
	if (!globalDeviceStatus.remoteMode)
	{
		globalTestStatus.Unlimited=false;    
		if (0==EEPROM1.Time)
			globalTestStatus.Unlimited=true;
		else
		if (EEPROM1.TestCount==TESTCNT_MODE_DOWN)
			globalTestStatus.Value=EEPROM1.Time;//600;
	}
	else
	{
		globalTestStatus.Unlimited=true;
	}
	
	if (!globalDeviceStatus.remoteMode)
	{
		// initialize reports
		// set interval, write report header
		if (globalDeviceStatus.installedPrinter)
			if (RTC_GetDateTime(&curDTime))
				FHeader.StartTime=curDTime;
		
		FHeader.Time=EEPROM1.Time;
		FHeader.Speed=EEPROM1.Speed;
		FHeader.Temp=EEPROM1.Temp;
		FHeader.Interval=(TWord)EEPROM1.ReportInterval*60;
		
		if (FHeader.Time)
		{
			while (FHeader.Time/FHeader.Interval>MAX_REPORT_NUMBER)
			{
				FHeader.Interval+=60;
			}
		}
		
		FHeader.Number=0;
		FHeader.Method=EEPROM1.Method;
		FHeader.Volume=EEPROM1.Volume;
		FHeader.TempOffset=EEPROM1.TempOffset[ADC_CHNL_INTERNAL];
		FHeader.Head=EEPROM1.Head;
		FHeader.RunTime=0;
		FHeader.Status=RSTS_RUNNING;
		//FHeader.Multiplex=(FHeader.Time/(60*MAX_REPORT_NUMBER))+1;
		FHeader.MinTemp=TEMP_ERROR;
		FHeader.MaxTemp=0;
		
		if ((temp>MAX_CURTEMP_VALUE)||(temp<MIN_CURTEMP_VALUE))
		{
			FReport.MinTemp=TEMP_ERROR;
			FReport.MaxTemp=TEMP_ERROR;
			globalTestStatus.MinTemp=TEMP_ERROR;
			globalTestStatus.MaxTemp=0;
			testMinTemp=TEMP_ERROR;
			testMaxTemp=0;
		}
		else
		{  
			wVal=(TWord)((temp+0.05)*10);
			FReport.MinTemp=wVal;
			FReport.MaxTemp=wVal;
			FHeader.MinTemp=wVal;
			FHeader.MaxTemp=wVal;
			globalTestStatus.MinTemp=globalTestStatus.MaxTemp=testMinTemp=testMaxTemp=wVal;
		}
		
		FReport.MinSpeed=0;
		FReport.MaxSpeed=0;
		//memcpy(&FReport.SampT[0],&SAMPLER_Temp[0],sizeof(TWord)*MAX_SENSORS_NUM);
		
		// save one record to FRAM
		FRAM_Do(MEM_WRITE, FHeader.Number);
		FHeader.Number++;
		FRAM_Do(MEM_WRITE_HEADER, 0);
		makeReport=true;
	}
	else
	{
		makeReport=false;
	}
	
	timeOut=false;
	
	//in pause mode update LED's only one time - cont update prevents from blinking
	//MOTOR_Stop();
	
	MOTOR_SetSpeed(EEPROM1.Speed);
	MOTOR_ResetRevolutions();
	MOTOR_Start();
	paused=false;
	//testValue=0;
	//sync
	if (!globalDeviceStatus.remoteMode)
	{  
		//globalDeviceStatus.timeTick=false;
		TIMER_TickReset();
		//while (!globalDeviceStatus.timeTick);
		while (!TIMER_Ticked());
	}
	
	//globalDeviceStatus.timeTick=false;
	TIMER_TickReset();
	KBD_Reset();
	globalTestStatus.Ticker=0;//DT_MAX_TIME-600;
	speed=globalTestStatus.MinSpeed=globalTestStatus.MaxSpeed=testMinSpeed=testMaxSpeed=EEPROM1.Speed;
	    
	//CTL_ShowSpeed((TReal32)MOTOR_GetSpeed()/10, false, true);//CTL_ShowSpeed((TReal32)EEPROM1.Speed, false, true);
	//CTL_ShowSpeed((TReal32)MOTOR_GetSpeed(), false, true); 							// ZT120²»ÏÔÊ¾×ªËÙ£¬2017-11-8 ÐÇÆÚÈý 21:16:46
	if (!globalDeviceStatus.remoteMode)
		CTL_ShowTime(globalTestStatus.Value, false);
	else
		LED_Empty_Time();
	
	TIMER_Start(TIMER_GENERAL, EEPROM1.TempUpdateInterval*TO_1SEC);
	ledsUpdated=false;
	
	//run test
	for (;;)
	{
		M2_cur_f = M2_old_f = false;
		//wait for key press
		while(!KBD_KeyPressed())
		{
			EXTERNAL_Process();
			ERROR_Check();
			
			if (globalTestStatus.State==TEST_STOPPED)
				goto TestDone;
			
			if (globalTestStatus.State==TEST_MOTORSTOPPED)
			{
				//motor error, be able to print test report
				MOTOR_Stop();
				FHeader.Status=RSTS_MOTORERR;
				FHeader.RunTime=globalTestStatus.Ticker;
				FRAM_Do(MEM_WRITE_HEADER, 0);
				usageCounter += EEPROM1.UsageCounter;
				EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.UsageCounter, (TByte *)&usageCounter, sizeof(EEPROM1.UsageCounter));
				globalTestStatus.State=TEST_STOPPED;
				return true;
			}
			
			//time tick?
			//if (globalDeviceStatus.timeTick)
			if (TIMER_Ticked())     // ¼ÆÊ±µ½´ï1Ãë
			{
				TIMER_TickReset();
				
				if (((tempAlarm)||(speedAlarm))&&(EEPROM1.Alarm==ALARM_MODE_ON)&&(!globalDeviceStatus.remoteMode))
				{
					SND_Play(ssAlarm);
				}
				
				if (!paused)
				{
					usageCounter++;
					//if (speedDelay<SPEED_DELAY) speedDelay++;
					//if (globalTestStatus.Ticker<DT_MAX_TIME) 
						globalTestStatus.Ticker++;
					
					if (globalTestStatus.Unlimited)
					{
						if (!globalDeviceStatus.remoteMode)//||(globalDeviceStatus.droppedMode))
							if (globalTestStatus.Value<=DT_MAX_TIME)
								globalTestStatus.Value++;
					}
					else
					if (EEPROM1.TestCount==TESTCNT_MODE_UP)
					{
						if (globalTestStatus.Value<EEPROM1.Time)
							globalTestStatus.Value++;
						
						if (((EEPROM1.Time-globalTestStatus.Value)<EEPROM1.StartBeep)&&(EEPROM1.Alarm==ALARM_MODE_ON)&&(!tempAlarm)&&(!speedAlarm))
							SND_Play(ssWarning);
					}
					else
					{
						if (globalTestStatus.Value)
							globalTestStatus.Value--;
						
						if ((globalTestStatus.Value<EEPROM1.StartBeep)&&(EEPROM1.Alarm==ALARM_MODE_ON)&&(!tempAlarm)&&(!speedAlarm))
							SND_Play(ssWarning);
					}
				}
				
				//2014/3/17 16:25:39
				//show temperature - always
				//if(globalDeviceType != DEVICE_QDT6)
				//{
					if (globalDeviceStatus.installedExtADC)
					{
						temp=TEMP_GetValue(ADC_CHNL_EXTERNAL);
						//DHC_GetTemp(&prevTemp);
						prevTemp = TEMP_GetValue(ADC_CHNL_INTERNAL); 		// 2017-11-7 ÐÇÆÚ¶þ 22:08:45
						if (globalDeviceStatus.sensorExtError)
							temp=prevTemp;
					}
					else 
					{
						//DHC_GetTemp(&temp);
						temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 				// 2017-11-7 ÐÇÆÚ¶þ 22:09:12
						prevTemp=temp;
					}
				//}
				//else
				//{
					//DHC_GetTemp(&temp);
				//	temp = TEMP_GetValue(ADC_CHNL_INTERNAL); 					// 2017-11-7 ÐÇÆÚ¶þ 22:09:24
				//	prevTemp=temp;
				//}
				
				if ((temp<=MAX_CURTEMP_VALUE)&&(temp>=MIN_CURTEMP_VALUE))
				{
					wVal=(TWord)((temp+0.05)*10);   // ËÄÉáÎåÈëÀ©´óÊ®±¶
					if (wVal>globalTestStatus.MaxTemp)
						globalTestStatus.MaxTemp=wVal;
					
					if (wVal<globalTestStatus.MinTemp)
						globalTestStatus.MinTemp=wVal;
					
					if (wVal>testMaxTemp)
						testMaxTemp=wVal;
					
					if (wVal<testMinTemp)
						testMinTemp=wVal;
				}
				
				if (TIMER_TimeOut(TIMER_GENERAL))
				{
					tempAlarm=false;
					//if ((fabs(temp-EEPROM1.Temp)>MAX_TEMP_ERROR)||(fabs(prevTemp+(TReal32)EEPROM1.TempOffset[ADC_CHNL_INTERNAL]/10-EEPROM1.Temp)>MAX_TEMP_ERROR))
					//if ((fabs(temp-EEPROM1.Temp)>MAX_TEMP_ERROR)||(fabs(prevTemp-(TReal32)EEPROM1.TempOffset[ADC_CHNL_INTERNAL]/10-EEPROM1.Temp)>MAX_TEMP_ERROR))     // HHT  v1.04  240705-1540
					if ((fabs(temp-EEPROM1.Temp)>MAX_TEMP_ERROR)||(fabs(prevTemp-EEPROM1.Temp)>MAX_TEMP_ERROR))     // HHT  v1.04  240722-1620
                    {
						CTL_ShowTemp(temp, true);
						tempAlarm=true;
					}
					else
						CTL_ShowTemp(temp, false);
					
					TIMER_Start(TIMER_GENERAL, EEPROM1.TempUpdateInterval*TO_1SEC);
				}
				
				//show test parameters
				if (!ledsUpdated)
				{
					//speed=(TReal32)MOTOR_GetSpeed()/10;
					speed=(TReal32)MOTOR_GetSpeed(); 				// 2017-11-8 ÐÇÆÚÈý 21:17:41
					if ((!globalTestStatus.goMode)&&(MOTOR_IsStable()))//(speedDelay>=SPEED_DELAY))
					{
						if ((speed<globalTestStatus.MinSpeed)&&(speed>0))
							globalTestStatus.MinSpeed=speed;
						
						if ((speed>globalTestStatus.MaxSpeed)&&(speed<100)) //2018-1-26 ÐÇÆÚÎå 11:24:35
							globalTestStatus.MaxSpeed=speed;
						
						if ((speed<testMinSpeed)&&(speed>0))
							testMinSpeed=speed;
						
						if ((speed>testMaxSpeed)&&(speed<100)) 			// 2018-1-26 ÐÇÆÚÎå 11:25:06
							testMaxSpeed=speed;
					}
					
					if (timeOut)
					{
						if (!globalDeviceStatus.remoteMode)
							CTL_ShowTime(EEPROM1.Time, true);
					}
					else
					{
						if (globalTestStatus.Value>DT_MAX_TIME)
						{
							if (!globalDeviceStatus.remoteMode)
								CTL_ShowTime(globalTestStatus.Value, true);
							
							makeReport=false;
						}
						else
						{
							if (!globalDeviceStatus.remoteMode)
								CTL_ShowTime(globalTestStatus.Value, paused);
						}
					}
					
					if (paused)
					{
						;
						//CTL_ShowSpeed(EEPROM1.Speed, false, false); // ZT120²»ÏÔÊ¾×ªËÙ
					}
					else
					{
						//if ((!globalTestStatus.goMode)&&((fabs(speed-EEPROM1.Speed)*100/EEPROM1.Speed)>MAX_SPEED_ERROR)&&(MOTOR_IsStable()))
						if ((!globalTestStatus.goMode)&&(fabs(speed-EEPROM1.Speed)>MAX_SPEED_ERROR)&&(MOTOR_IsStable()))
						{
							//CTL_ShowSpeed(speed, true, true);   // ZT120²»ÏÔÊ¾×ªËÙ
							speedAlarm=true;
						}
						else
						{
							//CTL_ShowSpeed(speed, false, true);  // ZT120²»ÏÔÊ¾×ªËÙ
							speedAlarm=false;
						}
					}
				}
				
				//SAMPLER_GetTemp();
				
				//globalDeviceStatus.timeTick=false;
				//TIMER_TickReset();
				if (paused)
				{
					ledsUpdated=true;
					continue;
				}
				else
					ledsUpdated=false;
				
				// áûëî òóò!!!!
				
				// test if report period is reached
				if (makeReport)
				{
					// 2017/1/14 12:06:21,¿Í»§ÒªÇó²»ÔÙÐèÒª×ªËÙºÍÎÂ¶ÈµÄ¸üÐÂÆµÂÊ²ÎÊý£¬Í¬Ê±²»´òÓ¡¶ÔÓ¦²ÎÊý
					//ReportInterval++;
					//if (ReportInterval==FHeader.Interval)
					//{
					  //ReportInterval=0;
					FReport.MinTemp=testMinTemp;
					FReport.MaxTemp=testMaxTemp;
					if ((temp>MAX_CURTEMP_VALUE)||(temp<MIN_CURTEMP_VALUE))
					{  
					  testMinTemp=TEMP_ERROR;
					  testMaxTemp=0;
					}
					else
					{  
					  testMinTemp=testMaxTemp=(TWord)((temp+0.05)*10);
					}  
					FReport.MinSpeed=(TWord)(testMinSpeed*10);//(TWord)(speed*10);
					FReport.MaxSpeed=(TWord)(testMaxSpeed*10);
					testMinSpeed=testMaxSpeed=speed;
					//memcpy(&FReport.SampT[0],&SAMPLER_Temp[0],sizeof(TWord)*MAX_SENSORS_NUM);
					FHeader.MinSpeed=(TWord)(globalTestStatus.MinSpeed*10);
					FHeader.MaxSpeed=(TWord)(globalTestStatus.MaxSpeed*10);
					FHeader.MinTemp=globalTestStatus.MinTemp;
					FHeader.MaxTemp=globalTestStatus.MaxTemp;
					// save report to FRAM
					//ReportNum=((FHeader.Number-1)%FHeader.Multiplex)+((FHeader.Number-1)/FHeader.Multiplex+1);
					FRAM_Do(MEM_WRITE, FHeader.Number);
					// save report header
					FHeader.Number++;
					FRAM_Do(MEM_WRITE_HEADER, 0);
					//} 	// 2017/1/14 12:08:06
				}
			}
			
			// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
			if(macBitTest(PIN_DROP_IN, pinM2_Close))
			{
				M2_cur_f = true;
				macBitOff(PORT_LED0, pinLED); 				// µãÁÁPCBÌùÆ¬LEDÖ¸Ê¾µÆ
			}
			else
			{
				M2_cur_f = false;
				macBitOn(PORT_LED0, pinLED);
			}
			
			M2_flag = 0;
			if(paused)
			{
				// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
				if((!M2_cur_f)&&(M2_old_f)) 		// 2017-11-9 ÐÇÆÚËÄ 15:43:39
				{
					M2_flag = 2;
				}
			}
			else
			{
				// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
				if(macBitTest(PIN_DROP_IN, pinM2_Close)) 		// 2017-11-9 ÐÇÆÚËÄ 15:43:39
				{
					M2_flag = 1;
				}
			}
			
			M2_old_f = M2_cur_f;
			if(M2_flag != 0)
				break;
			
			//check if test complete?
			if (globalTestStatus.Unlimited)
			{
				if (globalTestStatus.Ticker>=DT_MAX_TIME) 					// 99:59:59
					globalTestStatus.unlimMode=true;
				
				continue;
			}
			
			if (((globalTestStatus.Value==0)&&(EEPROM1.TestCount==TESTCNT_MODE_DOWN))||((globalTestStatus.Value==EEPROM1.Time)&&(EEPROM1.TestCount==TESTCNT_MODE_UP)))
			{
				//makeReport=false;
				timeOut=true;
				if (EEPROM1.MotorStop==MOTOR_MODE_STOP)
				{
					makeReport=false;
					goto TestDone;
				}
				else
				{
					/*if (EEPROM1.TestCount==TESTCNT_MODE_DOWN)
						globalTestStatus.Value=0;
					else
						globalTestStatus.Value=EEPROM1.Time;*/
					
					if (!globalTestStatus.goMode)
						//MOTOR_SetSpeed(MOTOR_GO_SPEED);
						MOTOR_SetSpeed(EEPROM1.Speed);
					
					globalTestStatus.goMode=true;
				}
			}
			
			/*
			// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
			if(macBitTest(PIN_DROP_IN, pinM2_Close))
			{
				M2_cur_f = true;
				macBitOff(PORT_LED0, pinLED); 				// µãÁÁPCBÌùÆ¬LEDÖ¸Ê¾µÆ
			}
			else
			{
				M2_cur_f = false;
				macBitOn(PORT_LED0, pinLED);
			}
			
			M2_flag = 0;
			if(paused)
			{
				// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
				if((!M2_cur_f)&&(M2_old_f)) 		// 2017-11-9 ÐÇÆÚËÄ 15:43:39
				{
					M2_flag = 2;
				}
			}
			else
			{
				// Î»ÖÃ´«¸ÐÆ÷¼ì²â-----------------
				if(macBitTest(PIN_DROP_IN, pinM2_Close)) 		// 2017-11-9 ÐÇÆÚËÄ 15:43:39
				{
					M2_flag = 1;
				}
			}
			
			M2_old_f = M2_cur_f;
			if(M2_flag != 0)
				break;
			*/
		}
		
		if (!globalDeviceStatus.remoteMode)
		{
			if (TEST_ProcessKey(&paused))
			{
				MOTOR_ResetRevolutions();
				globalTestStatus.State=TEST_STOPPED;
				usageCounter += EEPROM1.UsageCounter;
				EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.UsageCounter, (TByte *)&usageCounter, sizeof(EEPROM1.UsageCounter));
				
				if (globalTestStatus.goMode)
					FHeader.Status=RSTS_FINISHED;
				else
					FHeader.Status=RSTS_CANCEL;
				
				FHeader.RunTime=globalTestStatus.Ticker;
				FRAM_Do(MEM_WRITE_HEADER, 0);
				//wait a second
				CTL_ShowTime(EEPROM1.Time, false);
				//CTL_ShowSpeed(EEPROM1.Speed, false, false);   // ZT120²»ÏÔÊ¾×ªËÙ
				CTL_ShowTemp(EEPROM1.Temp, false);
				__delay_cycles(XTAL/4);
				if (globalTestStatus.Unlimited)
					return true;
				//return false;
				return true;
	    }
	    else
	    {
	    	//CTL_ShowSpeed(0.0, false, true);//CTL_ShowSpeed((TReal32)EEPROM1.Speed, false, true);
				CTL_ShowTime(globalTestStatus.Value, paused);
			}
		}
		else
		{
			KBD_GetKey();
			SND_Play(ssError);
		}
	}
	
TestDone:
	MOTOR_Stop();
	globalTestStatus.State=TEST_STOPPED;
	usageCounter += EEPROM1.UsageCounter;
	//eepromUsageCounter = usageCounter;
	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.UsageCounter, (TByte *)&usageCounter, sizeof(EEPROM1.UsageCounter));
	FHeader.RunTime=globalTestStatus.Ticker;
	FHeader.Status=RSTS_READY;
	FRAM_Do(MEM_WRITE_HEADER, 0);
	//update screen
	CTL_ShowTime(globalTestStatus.Value, false);
	//CTL_ShowSpeed(EEPROM1.Speed, false, false);  // ZT120²»ÏÔÊ¾×ªËÙ
	CTL_ShowTemp(EEPROM1.Temp, false);
	SND_Play(ssOn);
	
	if ((EEPROM1.Sams==SAMPLER_MODE_AUTO)&&
			(!globalDeviceStatus.remoteMode)&&
			(globalDeviceStatus.Head==HEAD_MODE_LOW)) 		//&&, 2018-1-9 ÐÇÆÚ¶þ 09:57:01
			//(EEPROM1.Volume>=VOLUME_MODE_1000))
	{
		//speed=50.0;
		//STEPPER_SetDistance(&speed);
		STEPPER_CalcDistance(EEPROM1.Volume-VOLUME_MODE_1000);
		STEPPER_Start(false, false);
	}
	
	if (!globalDeviceStatus.remoteMode)
		return true;
	else
		return false;
}                          
