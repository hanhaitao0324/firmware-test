#include "eeprom.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "printrtc.h"
#include "report.h"
#include "rep_strings_svm.h"
#include "rs485.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "version.h"

TByte REPORT_PrintLine;

TByte REPORT_PrintStat;

TBool REPORT_PrintInit=false;

//TByte REPORT_NumStatRecords=0;

// Q2，Gavin，文件格式不保存汉字，2019-05-09 15:50:58
TByte DevMt = 0;

extern TBool acalOver1,acalOver2; 			// 测试完成标志, Gavin, 2019-10-12 星期六 09:20:12
extern TBool acalNa1,acalNa2; 					// 工作站未使用标志

//------------------------------------------------------------------------------
// REPORT_Print - print report
//------------------------------------------------------------------------------
TBool REPORT_Print(TByte ReportType)
{
  char tempBuffer[80];
  char tempBuffer2[40];
  char tempBuffer3[20];
  char tempBuffer4[20];
  TByte cntHourGlass;
  TPrinterStatus prnStatus;
  TByte key;
  TReport __flash *prnReport;
  TByte StartPos;
  TByte DevMode;
  TByte AcalTimes; 											// v1.30, Gavin, 2019-08-23 星期五 10:37:51
  int tHour, tMin, tSec; 								// Gavin, 2019-10-12 星期六 09:38:07
  TBool testCF; 												// v1.30, 测试完成标志，Gavin, 2020-05-29 星期五 09:40:02

  if (ReportType>REPORT_TYPE_LAST)
    return true;
  
  // 打印格式传递-----------------------
  switch (ReportType)
  {
    case REPORT_TYPE_TEST: 							// 工作测试，2019-08-22 星期四 13:49:05
    {
			if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
			{
				if(EEPROM1.TimeUnits != TIME_UNIT_ACAL)
				{
					prnReport=TestReport;
				}
				else
				{
					prnReport = TestReportAcal; 						// 增加普打格式，v1.30，Gavin，2019-08-22 星期四 13:52:55
				}
			}
			else
			{
				if(EEPROM1.TimeUnits != TIME_UNIT_ACAL)
				{
					prnReport=TestReportSm;
				}
				else
				{
					prnReport = TestReportAcalSm; 					// 增加微打格式，v1.30，Gavin, 2019-08-22 星期四 13:53:23
				}
			}
		}break;
    case REPORT_TYPE_PRINTER: 					// 打印机测试
    {
      if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
        prnReport=PrinterReport;
      else
        prnReport=PrinterReportSm;
    }break;
    case REPORT_TYPE_OQ: 								// 其它
    {
      if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
        prnReport=QReport;
      else
        prnReport=QReportSm;
    }break;
    default:
      return true;
  }  

  DevMode=EEPROM1.DevMode;
  //if (EEPROM1.DevType==DEVICE_TYPE_TDT) !!!!!
  //  DevMode=DevMode*2+1;
  if (DevMode>DEVICE_MODE_223)
    DevMode=DEVICE_MODE_223;
  
  LED_Print_P(LED_DISPLAY_RIGHT, false, blankStr);                       

	// v1.30，Gavin，增加打印机复位，2019-08-15 星期四 16:56:33
	PRN_Reset();
  // printer isn't init
  PRN_Init();          
  REPORT_PrintInit=true;
  REPORT_PrintLine=0;
  REPORT_PrintStat=0;
  cntHourGlass=0;
   
   if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
     StartPos=SPACE_SYMBOLS;
   else
     StartPos=0;
	
	DevMt = 0; 														// Q2£¬Á½¸ö¹¤×÷Õ¾µÄ¼ÆÊýÆ÷£¬Gavin£¬2019-05-09 ÐÇÆÚËÄ 15:52:30
	
  for (;;)
  {
    if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
      memset(tempBuffer, 0x20, SPACE_SYMBOLS);

    // draw hour-glass
    if (cntHourGlass==0)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr1);
    if (cntHourGlass==1)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr2);
    if (cntHourGlass==2)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr3);
    cntHourGlass++;  
    if (cntHourGlass>=3) cntHourGlass=0;
    
    // check status of printer
    PRN_Status(&prnStatus);
    // if printer error, set flag Failure

    if (prnStatus.error)                 
      globalStatus.printerFailure=true;

    if (globalStatus.printerFailure)
    {
      LED_Print_P(LED_DISPLAY_LEFT, true, printStr0); 
      LED_Print_P(LED_DISPLAY_RIGHT, true, errorStr);
  
      for(;;)
      {
        while(!KBD_KeyPressed());        
        key=KBD_GetKey();
        if (key==KBD_CODE_STOP)
        {
          globalStatus.printerFailure=false;
          SND_Play(ssStop);     
          return false;
        }       
        SND_Play(ssError);     
      }
    }
              
    if (EEPROM1.PrinterType==PRINTER_TYPE_PORT)
    {  
      TIMER_Start(TIMER_GENERAL, TO_200MSEC);
      while (!TIMER_TimeOut(TIMER_GENERAL));
      TIMER_Stop(TIMER_GENERAL);
    }

    if (prnStatus.busy)
      continue;

    if (!prnStatus.connected)
      break;

    // print report
    switch(prnReport[REPORT_PrintLine].type)
    {
      case REP_STRING: 									// 字符串
      {
        strcpy_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt); 
      }break;
      case REP_DEVICE_NAME: 						// 设备名称
      {
        //EEPROM_ReadString(tempBuffer2, EEPROM1.FirmLogo, LOGO_MAX_LEN+1);
        strcpy_P(tempBuffer2, DEVICE_LOGO[EEPROM1.FirmLogo]);
        sprintf_P(tempBuffer3, DEVICE_TYPE[EEPROM1.DevType]);
        sprintf_P(tempBuffer4, DEVICE_MODE[EEPROM1.DevType][EEPROM1.DevMode]);
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3, tempBuffer4);
      }break;
      case REP_SERIAL: 									// 序列号
      {
        EEPROM_ReadString(tempBuffer2, EEPROM1.Serial, SERIAL_NUMBER_LEN+1);
        tempBuffer2[SERIAL_NUMBER_LEN]=0;
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
      }break;
      case REP_DEVICE_ID: 							// 设备ID
      {
        //print PCBID
        EEPROM_ReadString(tempBuffer2, PCBID, PCB_ID_LEN+1);
        tempBuffer2[PCB_ID_LEN]=0;
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, &tempBuffer2[4]);
      }break;
      case REP_VERSION: 								// 版本号
      {
        strcpy_P(tempBuffer2, VERSION_Revision);
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);              
      }break;
      case REP_CUR_DATETIME: 						// 当前日期时间
      {
        if (RTC_GetDateTime(&dt))
        {
          TByte hour;
          
          if (!TIMEDATE_IsValidDate(dt))
          {  
						strcpy_P(tempBuffer2, reportNoDate);
						strcpy_P(tempBuffer3, blankStr);
          }
          else
          {
            //if (eepromDFormat==DFORMAT_MODE_EUR)
            if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
              sprintf_P(tempBuffer2,paserDateFormatEUR,dt.Day,dt.Month,dt.Year);
            else
              sprintf_P(tempBuffer2,paserDateFormatUSA,dt.Month,dt.Day,dt.Year);

            //if (eepromDFormat==DFORMAT_MODE_EUR)
            if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
              sprintf_P(tempBuffer3,paserTimeFormatEUR,dt.Hour,dt.Minute,dt.Second);
            else
            {
              hour = dt.Hour;
              if (!hour) hour=12;
                else
                  if (hour>12) hour-=12;
              if (dt.Hour>12)
                sprintf_P(tempBuffer3,paserTimeFormatUSA_PM,hour,dt.Minute,dt.Second);
              else
                sprintf_P(tempBuffer3,paserTimeFormatUSA_AM,hour,dt.Minute,dt.Second);
            }
          }
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
        }
      }break;
      case REP_METHOD:
      {
        //switch (eepromDevType)
        switch (DevMode)
        {
          case DEVICE_MODE_121:
          case DEVICE_MODE_221:
            sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, 1);
            break;
          case DEVICE_MODE_122:
          case DEVICE_MODE_222:
            sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, 2);
            break;
          case DEVICE_MODE_223:
          default:
            tempBuffer[0]=0;
            break;
        }
      }break;
      case REP_PRESET_RUNTIME: 					// 设置运行时间
      {
        //if test unlimited - print UNLIMITED, otherwise print set time
        if (EEPROM1.TimeUnits==TIME_UNIT_TIME)
        {
          if (globalTestStatus.Unlimited)
            strcpy_P(tempBuffer2, unlimitedStr);
          else
            sprintf_P(tempBuffer2, testTimeFmt, (int)(EEPROM1.TestTime)/3600, (int)((EEPROM1.TestTime)%3600)/60, (int)(EEPROM1.TestTime)%60);
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
        } else tempBuffer[0]=0;
      }break;
      case REP_TOTAL_RUNTIME: 					// 全部运行时间
      {
        if (globalTestStatus.Ticker<SVM_TIME_MAX)
        {
        	tHour = globalTestStatus.Ticker/3600;
        	tMin  = globalTestStatus.Ticker%3600/60;
        	tSec  = globalTestStatus.Ticker%60;
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tHour, tMin, tSec);
        }
        else
          tempBuffer[0]=0;
      }break;
      case REP_STROKES_NUM:
      {
        if (EEPROM1.TimeUnits==TIME_UNIT_REVOLUTIONS)
        {
          if (globalTestStatus.Unlimited)
            strcpy_P(&tempBuffer[StartPos], unlimitedStr);
          else
            sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, EEPROM1.TestRevolutions);
        } else tempBuffer[0]=0;
      }break;
      case REP_STAT_HEADER:
      {
        //if no stat, don't print stat header
        if ((globalTestStatus.NumStatRecords)&&(DevMode!=DEVICE_MODE_223))
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt);
        else
          tempBuffer[0]=0;
      }break;
      case REP_STAT:
      {
        //if no stat, skip print stat
        if ((REPORT_PrintStat<globalTestStatus.NumStatRecords)&&(DevMode!=DEVICE_MODE_223))
        //print stat
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, REPORT_PrintStat+1, (TWord)(Speed_Buf[REPORT_PrintStat]+0.5));
        else
          tempBuffer[0]=0;
      }break;
      case REP_MINMAX_SPEED: 						// 运行最小和最大速度
      {
      	// Q1£¬Gavin£¬ÐÞ¸Ä¼°Ôö¼ÓÒÔÏÂÄÚÈÝ£»2019-5-4 ÐÇÆÚÁù 14:18:11----------------------------------
      	if (globalTestStatus.MinSpeed!=globalTestStatus.MaxSpeed)
        {
          sprintf_P(tempBuffer2, mmSpeedFmt, (TReal32)globalTestStatus.MinSpeed/10, (TReal32)globalTestStatus.MaxSpeed/10);
        	sprintf_P(tempBuffer3, mmSpeedFmt2, (TReal32)globalTestStatus.MinSpeed/10, (TReal32)globalTestStatus.MaxSpeed/10);
        }
        else
        {
          sprintf_P(tempBuffer2, minSpeedFmt, (TReal32)globalTestStatus.MinSpeed/10);
          sprintf_P(tempBuffer3, minSpeedFmt2, (TReal32)globalTestStatus.MinSpeed/10);
        }
        
        // Q2, Gavin, 2019-05-09 ÐÇÆÚËÄ 15:38:39
        if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
        {  
	        switch (DevMode)
	        {
	          case DEVICE_MODE_121:
	          case DEVICE_MODE_122:
	          	strcpy_P(tempBuffer3, reportEmptyLine);
	          	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
	            break;
	          case DEVICE_MODE_221:
	          case DEVICE_MODE_222:
	            sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
	            break;
	          case DEVICE_MODE_223:
	          	if (globalTestStatus.MinSpeed!=globalTestStatus.MaxSpeed)
	          	{
			          sprintf_P(tempBuffer3, mmSpeedFmt2, (TReal32)globalTestStatus.MinSpeed*0.83333/10, (TReal32)globalTestStatus.MaxSpeed*0.83333/10);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer3, minSpeedFmt2, (TReal32)globalTestStatus.MinSpeed*0.83333/10);
			        }
			        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
			        break;
	          default:
	            break;
	        }
	      }
	      else 																								// 微打
	      {
	      	switch (DevMode)
	        {
	          case DEVICE_MODE_121:
	          case DEVICE_MODE_122:
	          	strcpy_P(tempBuffer3, reportEmptyLine);
	          	if(DevMt == 0)
	          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
	          	
	          	DevMt = 2;
	          	REPORT_PrintLine+=2; 								// Ö»´òÓ¡Ò»¸öÕ¾µÄÐÅÏ¢£¬Ìø¹ýºóÃæÁ½ÐÐ£¬Gavin£¬2019-05-09 ÐÇÆÚËÄ 15:49:13
	            break;
	          case DEVICE_MODE_221:
	          case DEVICE_MODE_222:
	            if(DevMt == 0)
	            {
	          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
	          		
	          	}
	          	else
	          	{
	          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
	          		
	          	}
	          	
	          	DevMt++;
	            break;
	          case DEVICE_MODE_223:
	          	if (globalTestStatus.MinSpeed!=globalTestStatus.MaxSpeed)
	          	{
			          sprintf_P(tempBuffer3, mmSpeedFmt, (TReal32)globalTestStatus.MinSpeed*0.83333/10, (TReal32)globalTestStatus.MaxSpeed*0.83333/10);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer3, minSpeedFmt, (TReal32)globalTestStatus.MinSpeed*0.83333/10);
			        }
			        //sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
			        if(DevMt == 0)
			        {
	          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
	          		
	          	}
	          	else
	          	{
	          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3);
	          		
	          	}
	          	
	          	DevMt++;
			        break;
	          default:
	            break;
	        }
	      }
      }break;
      case REP_STATION: 								// 工作站
      {
        if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
        {  
          sprintf_P(tempBuffer2, reportFirstStation);
          if (DevMode<DEVICE_MODE_221)
            strcpy_P(tempBuffer3, reportEmptyLine); 				// 字符串结束
          else
            strcpy_P(tempBuffer3, reportSecondStation); 		// 
          
          //Q2, Gavin, 2019-05-09 ÐÇÆÚËÄ 15:34:20
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
        }
        else 																								// 微打
        {
          /*
          sprintf_P(tempBuffer2, reportFirstStationSm);
          if (DevMode<DEVICE_MODE_221)
            strcpy_P(tempBuffer3, reportEmptyLine);
          else
            strcpy_P(tempBuffer3, reportSecondStationSm);
          */
          
          switch(DevMt)
         	{
         		case 0:
          		sprintf_P(tempBuffer2, reportFirstStation);
          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
          		break;
          	case 1:
          		sprintf_P(tempBuffer2, reportSecondStation);
          		sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
          		break;
          	case 2:
          		sprintf_P(tempBuffer2, reportFirstStationSm);
		          if (DevMode<DEVICE_MODE_221)
		            strcpy_P(tempBuffer3, reportEmptyLine);
		          else
		            strcpy_P(tempBuffer3, reportSecondStationSm);
		          
		          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
		          break;
		        default: 
		        	break;
		      }
				}
        //sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
			}break;
      case REP_RESULTS: 								// 结果
      {
        if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
        {  
          sprintf_P(tempBuffer2, reportFirstColumn);
          
          if (DevMode<DEVICE_MODE_221) 										// 一个站
            sprintf_P(tempBuffer3, reportEmptyLine);
          else 																						// 两个站
            sprintf_P(tempBuffer3, reportSecondColumn);
        } 
        else 																								// 微打
        {
          sprintf_P(tempBuffer2, reportFirstColumnSm);
          if (DevMode<DEVICE_MODE_221)
            sprintf_P(tempBuffer3, reportEmptyLine);
          else
            sprintf_P(tempBuffer3, reportSecondColumnSm);
        }
        
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3);
      }break;
      case REP_ACAL_RESULTS0: 					// ACAL测试初始体积
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if(EEPROM1.TestWeih01 != 0) 										// 1号站有输入质量
	        	{
		        	sprintf_P(tempBuffer3, reportVolt, EEPROM1.TestVolt01);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0) 									// 2号站有输入质量
		        	{
			        	sprintf_P(tempBuffer4, reportVolt, EEPROM1.TestVolt02);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer4, reportVoltNa);
			        }
			      }
			    }
		      else 																								// 微打
	        {
	          if(EEPROM1.TestWeih01 != 0) 										// 1号站有输入质量
	        	{
		        	sprintf_P(tempBuffer3, reportVoltSm, EEPROM1.TestVolt01);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0) 									// 2号站有输入质量
		        	{
			        	sprintf_P(tempBuffer4, reportVoltSm, EEPROM1.TestVolt02);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer4, reportVoltNaSm);
			        }
			      }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		      AcalTimes = 0;
		    }
      }break;
      case REP_ACAL_RESULTS1: 					// ACAL测试体积
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if(EEPROM1.TestWeih01 != 0) 											// 1号站有输入质量
	        	{
		        	if(AcalTimes < acalStep1)
		        		sprintf_P(tempBuffer3, reportVolt, testVolt[AcalTimes][0]);
		        	else
		        	if(AcalTimes == acalStep1)
		        	{
		        		//if (globalTestStatus.Value)
		        		if(!acalOver1)
		        			sprintf_P(tempBuffer3, reportVoltNa);
		        		else
		        			sprintf_P(tempBuffer3, reportVoltFinal, testVolt[AcalTimes][0]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer3, reportVoltNa);
		        	}
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if(EEPROM1.TestWeih02 != 0) 									// 2号站有输入质量
	        		{
			        	if(AcalTimes < acalStep2)
		            	sprintf_P(tempBuffer4, reportVolt, testVolt[AcalTimes][1]);
		            else
		            if(AcalTimes == acalStep2)
		            {
		            	//if (globalTestStatus.Value)
		            	if(!acalOver2)
		            		sprintf_P(tempBuffer4, reportVoltNa);
		            	else
		            		sprintf_P(tempBuffer4, reportVoltFinal, testVolt[AcalTimes][1]);
		            }
		            else
		            	sprintf_P(tempBuffer4, reportVoltNa);
		          }
		          else
		          {
		          	sprintf_P(tempBuffer4, reportVoltNa);
		          }
	          }
	          
	          if(AcalTimes < 2)
			      	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, AcalTimes+1, tempBuffer3, tempBuffer4, acalRev[AcalTimes]);
			      else
			      	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, AcalTimes+1, tempBuffer3, tempBuffer4, acalRev[2]);
	          
	          AcalTimes++;
	          if(AcalTimes <= acalStep)
	          	REPORT_PrintLine--;
	        }
	        else 																								// 微打
	        {
	          if(EEPROM1.TestWeih01 != 0)
	        	{
		        	if(AcalTimes < acalStep1)
		        		sprintf_P(tempBuffer3, reportVoltSm, testVolt[AcalTimes][0]);
		        	else
		        	if(AcalTimes == acalStep1)
		        	{
		        		//if (globalTestStatus.Value)
		        		if(!acalOver1)
		        			sprintf_P(tempBuffer3, reportVoltNaSm);
		        		else
		        			sprintf_P(tempBuffer3, reportVoltFinalSm, testVolt[AcalTimes][0]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer3, reportVoltNaSm);
		        	}
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if(EEPROM1.TestWeih02 != 0)
	        		{
			        	if(AcalTimes < acalStep2)
		            	sprintf_P(tempBuffer4, reportVoltSm, testVolt[AcalTimes][1]);
		            else
		            if(AcalTimes == acalStep2)
		            {
		            	//if (globalTestStatus.Value)
		            	if(!acalOver2)
		            		sprintf_P(tempBuffer4, reportVoltNaSm);
		            	else
		            		sprintf_P(tempBuffer4, reportVoltFinalSm, testVolt[AcalTimes][1]);
		            }
		            else
		            	sprintf_P(tempBuffer4, reportVoltNaSm);
		          }
		          else
		          {
		          	sprintf_P(tempBuffer4, reportVoltNaSm);
		          }
	          }
	          
	          if(AcalTimes < 2)
			      	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, AcalTimes+1, tempBuffer3, tempBuffer4, acalRev[AcalTimes]);
			      else
			      	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, AcalTimes+1, tempBuffer3, tempBuffer4, acalRev[2]);
	          
	          AcalTimes++;
	          if(AcalTimes <= acalStep)
	          	REPORT_PrintLine--;
		      }
		      
		    }
      }break;
      case REP_ACAL_RESULTS2: 					// ACAL测试质量
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if(EEPROM1.TestWeih01 != 0)
        		{
		        	sprintf_P(tempBuffer3, reportWeih, (TReal32)EEPROM1.TestWeih01/10);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
        			{
	            	sprintf_P(tempBuffer4, reportWeih, (TReal32)EEPROM1.TestWeih02/10);
	            }
	            else
	            {
	            	sprintf_P(tempBuffer4, reportVoltNa);
	            }
	          }
	        }
	        else 																								// 微打
	        {
	          if(EEPROM1.TestWeih01 != 0)
        		{
		        	sprintf_P(tempBuffer3, reportWeihSm, (TReal32)EEPROM1.TestWeih01/10);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
        			{
	            	sprintf_P(tempBuffer4, reportWeihSm, (TReal32)EEPROM1.TestWeih02/10);
	            }
	            else
	            {
	            	sprintf_P(tempBuffer4, reportVoltNaSm);
	            }
	          }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS3: 					// ACAL测试总次数
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if(EEPROM1.TestWeih01 != 0)
        		{
		        	//sprintf_P(tempBuffer3, reportTTap, ACAL_REV_ADD[acalStep1]);
		        	sprintf_P(tempBuffer3, reportTTap, acalRevTotal_1);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
	          	{
	          		//sprintf_P(tempBuffer4, reportTTap, ACAL_REV_ADD[acalStep2]);
	          		sprintf_P(tempBuffer4, reportTTap, acalRevTotal_2);
	          	}
	          	else
	          	{
	            	sprintf_P(tempBuffer4, reportVoltNa);
	            }
	          }
	        }
	        else 																								// 微打
	        {
	          if(EEPROM1.TestWeih01 != 0)
        		{
		        	//sprintf_P(tempBuffer3, reportTTap, ACAL_REV_ADD[acalStep1]);
		        	sprintf_P(tempBuffer3, reportTTapSm, acalRevTotal_1);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
	          	{
	          		//sprintf_P(tempBuffer4, reportTTap, ACAL_REV_ADD[acalStep2]);
	          		sprintf_P(tempBuffer4, reportTTapSm, acalRevTotal_2);
	          	}
	          	else
	          	{
	            	sprintf_P(tempBuffer4, reportVoltNaSm);
	            }
	          }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS4: 					// ACAL测试初始密度
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if(EEPROM1.TestWeih01 != 0)
        		{
		        	sprintf_P(tempBuffer3, reportDensity, (TReal32)EEPROM1.TestWeih01/10/EEPROM1.TestVolt01);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
	          	{
	            	sprintf_P(tempBuffer4, reportDensity, (TReal32)EEPROM1.TestWeih02/10/EEPROM1.TestVolt02);
	            }
	            else
	            {
	            	sprintf_P(tempBuffer4, reportVoltNa);
	            }
	          }
	        }
	        else 																								// 微打
	        {
	          if(EEPROM1.TestWeih01 != 0)
        		{
		        	sprintf_P(tempBuffer3, reportDensitySm, (TReal32)EEPROM1.TestWeih01/10/EEPROM1.TestVolt01);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if(EEPROM1.TestWeih02 != 0)
	          	{
	            	sprintf_P(tempBuffer4, reportDensitySm, (TReal32)EEPROM1.TestWeih02/10/EEPROM1.TestVolt02);
	            }
	            else
	            {
	            	sprintf_P(tempBuffer4, reportVoltNaSm);
	            }
	          }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS5: 					// ACAL测试结束密度
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if((EEPROM1.TestWeih01 != 0)&&
	        		 (testCF)) 																		// 测试完成标志
        		{
		        	sprintf_P(tempBuffer3, reportDensity, (TReal32)EEPROM1.TestWeih01/10/testVolt[acalStep1][0]);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNa);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if((EEPROM1.TestWeih02 != 0)&&
	            	 (testCF)) 																	// 测试完成标志
	        		{
			        	sprintf_P(tempBuffer4, reportDensity, (TReal32)EEPROM1.TestWeih02/10/testVolt[acalStep2][1]);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer4, reportVoltNa);
			        }
	          }
	        }
	        else 																								// 微打
	        {
	          if((EEPROM1.TestWeih01 != 0)&&
	          	 (testCF)) 																		// 测试完成标志
        		{
		        	sprintf_P(tempBuffer3, reportDensitySm, (TReal32)EEPROM1.TestWeih01/10/testVolt[acalStep1][0]);
		        }
		        else
		        {
		        	sprintf_P(tempBuffer3, reportVoltNaSm);
		        }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if((EEPROM1.TestWeih02 != 0)&&
	            	 (testCF)) 																	// 测试完成标志
	        		{
			        	sprintf_P(tempBuffer4, reportDensitySm, (TReal32)EEPROM1.TestWeih02/10/testVolt[acalStep2][1]);
			        }
			        else
			        {
			        	sprintf_P(tempBuffer4, reportVoltNaSm);
			        }
	          }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS6: 					// ACAL测试体积比
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if((EEPROM1.TestWeih01 != 0)&&
	        		 (testCF)) 																		// 测试完成标志
	        	{
	        		sprintf_P(tempBuffer3, reportHRatio, (TReal32)EEPROM1.TestVolt01/testVolt[acalStep1][0]);
	        	}
	        	else
	        	{
	        		sprintf_P(tempBuffer3, reportVoltNa);
	        	}
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if((EEPROM1.TestWeih02 != 0)&&
	          		 (testCF)) 																	// 测试完成标志
		        	{
		        		sprintf_P(tempBuffer4, reportHRatio, (TReal32)EEPROM1.TestVolt02/testVolt[acalStep2][1]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer4, reportVoltNa);
		        	}
	          }
	        }
	        else 																								// 微打
	        {
	          if((EEPROM1.TestWeih01 != 0)&&
	          	 (testCF)) 																		// 测试完成标志
	        	{
	        		sprintf_P(tempBuffer3, reportHRatioSm, (TReal32)EEPROM1.TestVolt01/testVolt[acalStep1][0]);
	        	}
	        	else
	        	{
	        		sprintf_P(tempBuffer3, reportVoltNaSm);
	        	}
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	          	if((EEPROM1.TestWeih02 != 0)&&
	          		 (testCF)) 																	// 测试完成标志
		        	{
		        		sprintf_P(tempBuffer4, reportHRatioSm, (TReal32)EEPROM1.TestVolt02/testVolt[acalStep2][1]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer4, reportVoltNaSm);
		        	}
	          }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS7: 					// ACAL测试压缩率
      {
      	if(EEPROM1.TimeUnits == TIME_UNIT_ACAL)
				{
	      	if (EEPROM1.PrinterType==PRINTER_TYPE_NORM) 				// 普打
	        {
	        	if((EEPROM1.TestWeih01 != 0)&&
	        		 (testCF)) 																		// 测试完成标志
		       	{
		       		if(EEPROM1.IVol == 0) 												// V0
		        		sprintf_P(tempBuffer3, reportCIndex, ((TReal32)EEPROM1.TestVolt01-testVolt[acalStep1][0])*100/(TReal32)EEPROM1.TestVolt01);
		        	else 																					// V10
		        		sprintf_P(tempBuffer3, reportCIndex, ((TReal32)testVolt[0][0]-testVolt[acalStep1][0])*100/(TReal32)testVolt[0][0]);
	          }
	          else
	          {
	          	sprintf_P(tempBuffer3, reportVoltNa);
	          }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if((EEPROM1.TestWeih02 != 0)&&
	            	 (testCF)) 																	// 测试完成标志
		       		{
		        		if(EEPROM1.IVol == 0) 											// V0
		        			sprintf_P(tempBuffer4, reportCIndex, ((TReal32)EEPROM1.TestVolt02-testVolt[acalStep2][1])*100/(TReal32)EEPROM1.TestVolt02);
		        		else 																				// V10
		        			sprintf_P(tempBuffer4, reportCIndex, ((TReal32)testVolt[0][1]-testVolt[acalStep2][1])*100/(TReal32)testVolt[0][1]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer4, reportVoltNa);
		        	}
		        }
		      }
	        else 																							// 微打
	        {
	          if((EEPROM1.TestWeih01 != 0)&&
	          	 (testCF)) 																		// 测试完成标志
		       	{
		        	if(EEPROM1.IVol == 0) 												// V0
		        		sprintf_P(tempBuffer3, reportCIndexSm, ((TReal32)EEPROM1.TestVolt01-testVolt[acalStep1][0])*100/(TReal32)EEPROM1.TestVolt01);
		        	else 																					// V10
		        		sprintf_P(tempBuffer3, reportCIndexSm, ((TReal32)testVolt[0][0]-testVolt[acalStep1][0])*100/(TReal32)testVolt[0][0]);
	          }
	          else
	          {
	          	sprintf_P(tempBuffer3, reportVoltNaSm);
	          }
	          
	          if (DevMode<DEVICE_MODE_221) 										// 一个站
	            sprintf_P(tempBuffer4, reportEmptyLine);
	          else 																						// 两个站
	          {
	            if((EEPROM1.TestWeih02 != 0)&&
	            	 (testCF)) 																	// 测试完成标志
		       		{
		        		if(EEPROM1.IVol == 0) 											// V0
		        			sprintf_P(tempBuffer4, reportCIndexSm, ((TReal32)EEPROM1.TestVolt02-testVolt[acalStep2][1])*100/(TReal32)EEPROM1.TestVolt02);
		        		else
		        			sprintf_P(tempBuffer4, reportCIndexSm, ((TReal32)testVolt[0][1]-testVolt[acalStep2][1])*100/(TReal32)testVolt[0][1]);
		        	}
		        	else
		        	{
		        		sprintf_P(tempBuffer4, reportVoltNaSm);
		        	}
		        }
		      }
		      
		      sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer3, tempBuffer4);
		    }
      }break;
      case REP_ACAL_RESULTS8: 					// 打印压缩指数选择
      {
      	if(EEPROM1.IVol == 0) 					// V0
      	{
      		strcpy_P(tempBuffer2, testIVol_V0);
      	}
      	else
      	if(EEPROM1.IVol == 1) 					// V10
      	{
      		strcpy_P(tempBuffer2, testIVol_V10); 
      	}
      	
      	sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2); 
      }break;
      case REP_TEST_RESULT: 						// 测试结果
      {
        //test aborted or completed
        testCF = 0; 															// 测试完成标志
        if (globalTestStatus.Unlimited)
          strcpy_P(tempBuffer2, testFinished);
        else if (globalTestStatus.Value)
          strcpy_P(tempBuffer2, testAborted); 
        else
        {
          strcpy_P(tempBuffer2, testCompleted); 
          testCF = 1; 														// 测试完成标志
        }
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2); 
      }break; 
      case REP_OQ_HEIGHT:
      {
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, (TReal32)OQReport.Height[0]/10, (TReal32)OQReport.RHeight[0]/10);
      }break;
      case REP_OQ_HEIGHT2:
      {
        if (DevMode==DEVICE_MODE_223)
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, (TReal32)OQReport.Height[1]/10, (TReal32)OQReport.RHeight[1]/10);
        else
          tempBuffer[0]=0;
      }break;
      case REP_OQ_STROKES:
      {
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, OQReport.Strokes, OQReport.RStrokes);
      }break;
      case REP_OQ_RUNTIME:
      {
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, (int)(OQReport.Time)/60, (int)(OQReport.Time)%60, (int)(OQReport.RTime)/60, (int)(OQReport.RTime)%60);
      }break;
      case REP_END:
      {
        REPORT_PrintInit=false;
        PRN_Done();
        return true;
			}break;
      default:
        strcpy_P(&tempBuffer[StartPos], printError);
        break;
    }
		
    if (PRN_Print(tempBuffer))
    {
      // if stat is printed don't increment REPORT_PrintLine
      if (prnReport[REPORT_PrintLine].type==REP_STAT)
      {
        // increment REPORT_PrintStat
        REPORT_PrintStat++;
        // if all stat records are printed, goto next line
        if (REPORT_PrintStat<globalTestStatus.NumStatRecords) continue;
      }
                     
      REPORT_PrintLine++;
      continue;
    }
  }
  
  if (globalStatus.printerFailure)
  {
    PRN_Done();            
    REPORT_PrintInit=false;
  }     
      
  return true;
}
