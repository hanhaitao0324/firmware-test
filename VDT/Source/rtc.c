/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    rtc.c
 *      Purpose: rtc
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"
#include "timedate.h"
#include <time.h>
#include "rtc.h"

const char *month_names[]   = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
//const char *weekday_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

/**
  * @brief  set the RTC
  * @param  None
  * @retval None
  */
 void RTC_WriteDateTime(struct tm *s_newTime){				// set clock to new values.

	 //time_t time_value = mktime(s_newTime); 	// mktime validates (and updates) the tm struct 
   //if (time_value == (time_t) -1) return;		// error
	 //RTC_SetCounter(time_value);
  TTimeDate dt;
  uint32_t utime;
 
  dt.Second = s_newTime->tm_sec;
  dt.Minute = s_newTime->tm_min;
  dt.Hour = s_newTime->tm_hour;
  dt.Day = s_newTime->tm_mday;
  dt.Month = s_newTime->tm_mon;
  dt.Year = s_newTime->tm_year;
  utime = TIMEDATE_SecondsFromNull(&dt);
  RTC_SetCounter(utime);
}

/**
  * @brief  read the RTC
  * @param  None
  * @retval None
  */
void RTC_ReadDateTime(struct tm *s_newTime){
  TTimeDate dt;
	uint32_t utime = RTC_GetCounter();
  
  TIMEDATE_CalcDateFromSeconds(utime, &dt);
  s_newTime->tm_sec = dt.Second;
  s_newTime->tm_min = dt.Minute;
  s_newTime->tm_hour = dt.Hour;
  s_newTime->tm_mday = dt.Day;
  s_newTime->tm_mon = dt.Month;
  s_newTime->tm_year = dt.Year;
}

/*******************************************************************************
* Function Name  : RTC_GetDateTime
* Description    : Wrapper for AKC Software
* Input          : struct of type TTimeDate
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_GetDateTime(TTimeDate *dt)
{
  struct tm now;
  
  RTC_ReadDateTime(&now);
  dt->Second = now.tm_sec;
  dt->Minute = now.tm_min; 
  dt->Hour = now.tm_hour;
  dt->Day = now.tm_mday;
  dt->Month = now.tm_mon;
  dt->Year = now.tm_year;
}

/*******************************************************************************
* Function Name  : RTC_SetDateTime
* Description    : Wrapper for AKC Software
* Input          : struct of type TTimeDate
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SetDateTime(TTimeDate *dt)
{
  struct tm now;
  
  now.tm_sec = dt->Second;
  now.tm_min = dt->Minute; 
  now.tm_hour = dt->Hour;
  now.tm_mday = dt->Day;
  now.tm_mon = dt->Month;
  now.tm_year = dt->Year;
  RTC_WriteDateTime(&now);
}

/*******************************************************************************
* Function Name  : RTC_Set_DefaultTimeDate
* Description    : set default values, 01.01.2015, 01:00:00:00
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Set_DefaultTimeDate(DTIM *dtim)
{	
	dtim->Day		= 1;
	dtim->Month	= 1;
	dtim->Year	= TIMEDATE_DEF_YEAR;
	dtim->Hour	= 1;
	dtim->Minute= 0;
	dtim->Second= 0;
  RTC_SetDateTime(dtim);
}

/**
  * @brief  Configures the RTC
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  // Enable PWR and BKP clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  // Allow access to BKP Domain
  PWR_BackupAccessCmd(ENABLE);

  // Reset Backup Domain
  //BKP_DeInit();

  // Enable LSE
  RCC_LSEConfig(RCC_LSE_ON);
  // Wait till LSE is ready
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  // Select LSE as RTC Clock Source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  // Enable RTC Clock
  RCC_RTCCLKCmd(ENABLE);

  // Wait for RTC registers synchronization
  RTC_WaitForSynchro();

  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();

  // Enable the RTC Second
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();

  // Set RTC prescaler: set RTC period to 1sec
  RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)

  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();
}

/**
  * @brief  Initialize RTC module
  * @param  None
  * @retval None
  */
void RTC_Init(void)
{
	RTC_Configuration();
  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    // Backup data register value is not correct or not yet programmed (when the first time the program is executed)
		DTIM dtim;			
		RTC_Set_DefaultTimeDate(&dtim);
		RTC_Configuration();														// RTC Configuration
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);				// Adjust time by values entered by the user on the hyperterminal

  } else
  {
    RTC_WaitForSynchro();														// Wait for RTC registers synchronization
		RTC_WaitForLastTask();													// Wait until last write operation on RTC registers has finished
		RCC_ClearFlag();																// Clear reset flags
	}
}
