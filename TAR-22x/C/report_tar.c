#include "eeprom.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "printrtc.h"
#include "report.h"
#include "rep_strings_tar.h"
#include "rs485.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "version.h"

TByte REPORT_PrintLine;

TByte REPORT_PrintStat;

TBool REPORT_PrintInit=false;

TByte REPORT_DrumQuantity, REPORT_CurrentDrum;

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
  TByte i, j;  
  TReport __flash *prnReport;
  TByte StartPos;

  if (ReportType>REPORT_TYPE_LAST)
    return true;
  
  switch (ReportType) 									// 报告类型
  {
    case REPORT_TYPE_TEST: 							// 工作测试报告
      if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
        prnReport=TestReport; 					// 普通打印机格式
      else
        prnReport=TestReportSm; 				// 微型打印机格式
      break;
    case REPORT_TYPE_PRINTER: 					// 打印机测试报告
      if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
        prnReport=PrinterReport; 				// 普通打印机格式
      else
        prnReport=PrinterReportSm; 			// 微型打印机格式
      break;
    default:
      return true;
  }  

  LED_Print_P(LED_DISPLAY_RIGHT, false, blankStr);                       
	
	// v2.20，Gavin 2019-08-14 星期三 17:58:29
	PRN_Reset();
  // printer isn't init
  PRN_Init();          
  REPORT_PrintInit=true;
  REPORT_PrintLine=0;
  REPORT_PrintStat=0;
  cntHourGlass=0;
   
  key=EEPROM1.DevMode;
  switch (key)
  {
    case DEVICE_MODE_220:
      REPORT_DrumQuantity=2;
      break;
    case DEVICE_MODE_420:
      REPORT_DrumQuantity=4;
      break;
    case DEVICE_MODE_120:
    default:
      REPORT_DrumQuantity=1;
      break;
  }
  REPORT_CurrentDrum=1;
    
  if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
    StartPos=SPACE_SYMBOLS;
  else
    StartPos=0;

  for(;;)
  {
    if (EEPROM1.PrinterType==PRINTER_TYPE_NORM)
    {  
      memset(tempBuffer, 0x20, SPACE_SYMBOLS);
      tempBuffer[SPACE_SYMBOLS] = 0;
    }
    
    // draw hour-glass
    if (cntHourGlass==0)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr1);
    if (cntHourGlass==1)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr2);
    if (cntHourGlass==2)
      LED_Print_P(LED_DISPLAY_LEFT, false, printStr3);
    cntHourGlass++;  
    if (cntHourGlass>=3) cntHourGlass=0;
    
    TIMER_Start(TIMER_GENERAL, TO_100MSEC);
    while (!TIMER_TimeOut(TIMER_GENERAL))
    {
      if (KBD_KeyPressed())
      {
        key=KBD_GetKey();
        if (key==KBD_CODE_STOP)
        {
          SND_Play(ssStop);     
          return false;
        }       
        SND_Play(ssError);
      }
    }
    TIMER_Stop(TIMER_GENERAL);

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
        /*Gavin，打印机出错后，按任意键返回
        if (key==KBD_CODE_STOP)
        {
          globalStatus.printerFailure=false;
          SND_Play(ssStop);     
          return false;
        }       
        SND_Play(ssError);
        */
        globalStatus.printerFailure=false;
        SND_Play(ssStop);     
        return false;
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
      case REP_STRING:
        strcpy_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt); 
        break;
        
      case REP_DEVICE_NAME:
        //EEPROM_ReadString(tempBuffer2, EEPROM1.FirmLogo, LOGO_MAX_LEN+1);
        strcpy_P(tempBuffer2, DEVICE_LOGO[EEPROM1.FirmLogo]);
        strcpy_P(tempBuffer3, DEVICE_TYPE[EEPROM1.DevType]);
        strcpy_P(tempBuffer4, DEVICE_MODE[EEPROM1.DevType][EEPROM1.DevMode]);
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2, tempBuffer3, tempBuffer4);
        break;
        
      case REP_SERIAL:
        EEPROM_ReadString(tempBuffer2, EEPROM1.Serial, SERIAL_NUMBER_LEN+1);
        tempBuffer2[SERIAL_NUMBER_LEN]=0;
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);              
        break;

      case REP_DEVICE_ID:
        //print PCBID
        EEPROM_ReadString(tempBuffer2, PCBID, PCB_ID_LEN+1);
        tempBuffer2[PCB_ID_LEN]=0;
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, &tempBuffer2[4]);
        break;

      case REP_VERSION:
        strcpy_P(tempBuffer2, VERSION_Revision);
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);              
        break;

      case REP_CUR_DATETIME:
        if (RTC_GetDateTime(&dt))
        {
          TByte hour;
          
          if (!TIMEDATE_IsValidDate(dt))
          {  
              strcpy_P(tempBuffer2, reportNoDate);
              strcpy_P(tempBuffer3, blankStr);
          } else
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
        break;
      case REP_PRESET_RUNTIME:
        //if test unlimited - print UNLIMITED, otherwise print set time
        if (EEPROM1.TimeUnits==TIME_UNIT_TIME)
        {
          if (globalTestStatus.Unlimited)
            strcpy_P(tempBuffer2, unlimitedStr);
          else
            sprintf_P(tempBuffer2, paserTimeFormatEUR, (TWord)(EEPROM1.TestTime)/3600, (TWord)((EEPROM1.TestTime)%3600)/60, (TWord)(EEPROM1.TestTime)%60);
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
        } else tempBuffer[0]=0;
        break;
        
      case REP_TOTAL_RUNTIME:
        if (globalTestStatus.Ticker<TAR_TIME_MAX)
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, (TWord)(globalTestStatus.Ticker/3600), (TWord)((globalTestStatus.Ticker%3600)/60), (TWord)(globalTestStatus.Ticker%60));
        else
          tempBuffer[0]=0;
        break;
              
      case REP_REV_NUM:
        if (EEPROM1.TimeUnits==TIME_UNIT_REVOLUTIONS)
        {
          if (globalTestStatus.Unlimited)
            strcpy_P(&tempBuffer[StartPos], unlimitedStr);
          else
            sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt,EEPROM1.TestRevolutions);
        } else tempBuffer[0]=0;
        break;         
                  
      case REP_NOM_SPEED:
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt,EEPROM1.Speed);
        break;      
        
      case REP_MINMAX_SPEED:   
        if (globalTestStatus.MinSpeed!=globalTestStatus.MaxSpeed)
          sprintf_P(tempBuffer2, mmSpeedFmt, (TReal32)globalTestStatus.MinSpeed/10, (TReal32)globalTestStatus.MaxSpeed/10);
        else  
          sprintf_P(tempBuffer2, minSpeedFmt, (TReal32)globalTestStatus.MinSpeed/10);
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2);
        break;
        
      case REP_STAT_HEADER:
        //if no stat, don't print stat header
        if (globalTestStatus.NumStatRecords)
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt);
        else
          tempBuffer[0]=0;
        break;

      case REP_STAT:
        //if no stat, skip print stat
        if (REPORT_PrintStat<globalTestStatus.NumStatRecords)
        //print stat
          sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, REPORT_PrintStat+1, Speed_Buf[REPORT_PrintStat]);
        else
          tempBuffer[0]=0;
         break;
      
      case REP_DRUM_BIGNUM:
        if (REPORT_DrumQuantity>1)
          j = 2;
        else
          j = 1;
        for (i=0; i<j; i++)
        {  
          sprintf_P(tempBuffer2, prnReport[REPORT_PrintLine].name_fmt, REPORT_CurrentDrum);
          strcat(tempBuffer, tempBuffer2);
          REPORT_CurrentDrum++;
        }
        strcat_P(tempBuffer, reportNewDoubleLine);
        break;
        
      case REP_DRUM_NUM:
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, REPORT_CurrentDrum);
        REPORT_CurrentDrum++; // for small printer report
        break;
        
      case REP_DRUM:
        if (REPORT_DrumQuantity>1)
          j = 2;
        else
          j = 1;
        for (i=0; i<j; i++)
          strcat_P(tempBuffer, prnReport[REPORT_PrintLine].name_fmt);
        strcat_P(tempBuffer, reportNewDoubleLine);
        break;
      
      case REP_END_DRUM:
        strcpy_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt);
        //REPORT_CurrentDrum++;
        break;

      case REP_TEST_RESULT:
        //test aborted or completed or finished
        if (globalTestStatus.Unlimited)
          strcpy_P(tempBuffer2, testFinished);
        else if (!globalTestStatus.Ready)
          strcpy_P(tempBuffer2, testAborted); 
        else
          strcpy_P(tempBuffer2, testCompleted); 
        sprintf_P(&tempBuffer[StartPos], prnReport[REPORT_PrintLine].name_fmt, tempBuffer2); 
        break;
        
      case REP_END:
        REPORT_PrintInit=false;
        PRN_Done();
        return true;

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
                     
      if (prnReport[REPORT_PrintLine].type==REP_END_DRUM)
      {
        if (REPORT_CurrentDrum>REPORT_DrumQuantity)
          REPORT_PrintLine++;
        else
          REPORT_PrintLine-=8;
      } else REPORT_PrintLine++;
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
