/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    timedate.c
 *      Purpose: time keeper
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "timedate.h"
#include "const.h"

const char TIMEDATE_DateFormatEUR[]="%02d.%02d.20%02d"; 
const char TIMEDATE_DateFormatUSA[]="%02d/%02d/20%02d";
const char TIMEDATE_DateFormatCHN[]="20%02d.%02d.%02d"; 
const char TIMEDATE_TimeFormatEUR[]="%02d:%02d"; 
const char TIMEDATE_TimeFormatUSA_AM[]="%2d:%02dAM";
const char TIMEDATE_TimeFormatUSA_PM[]="%2d:%02dPM";
const char TIMEDATE_TimeFormatCHN[]="%02d:%02d"; 

//------------------------------------------------------------------------------
// Number of days in a month
//------------------------------------------------------------------------------
const uint8_t TIMEDATE_MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

//------------------------------------------------------------------------------
// Names of the days of the week
//------------------------------------------------------------------------------
/*const char TIMEDATE_DayOfWeek[8][4]=
{
  "  ",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
  "Sun"
};

//------------------------------------------------------------------------------
// Names of the monthes
//------------------------------------------------------------------------------
const char TIMEDATE_DayWeek[][10]=
{
  "  ",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};

const char TIMEDATE_MonthName[13][10]=
{
  "    ",
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
};*/

//------------------------------------------------------------------------------
// TIMEDATE_IsLeapYear - check if this year is a leap one
//------------------------------------------------------------------------------
uint8_t TIMEDATE_IsLeapYear(uint8_t Year)
{
  Year=Year&0x03;
  return (Year==0x00);
}

//------------------------------------------------------------------------------
// TIMEDATE_GetDayOfWeek - get date of week
//------------------------------------------------------------------------------
uint8_t TIMEDATE_GetDayOfWeek(TTimeDate *dt)//uint8_t year, uint8_t month, uint8_t day)
{
  int a = (14 - dt->Month)/12; 
  int y = dt->Year + 2000 + 4800 - a; 
  int m = dt->Month + 12*a - 3;
  int Date = dt->Day + (153*m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
  int day_of_week = Date % 7 + 1;

  if (day_of_week==7)
    day_of_week = 0;

  return (uint8_t)day_of_week;
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareDate - compare date only
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareDate(TTimeDate *dt0, TTimeDate *dt1)
{
  if (dt0->Year<dt1->Year)
    return TIMEDATE_LESS;
  if (dt0->Year>dt1->Year)
    return TIMEDATE_MORE;
  if (dt0->Month<dt1->Month)
    return TIMEDATE_LESS;
  if (dt0->Month>dt1->Month)
    return TIMEDATE_MORE;
  if (dt0->Day<dt1->Day)
    return TIMEDATE_LESS;
  if (dt0->Day>dt1->Day)
    return TIMEDATE_MORE;
  return TIMEDATE_EQUAL;
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareTime - compare time only
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareTime(TTimeDate *dt0, TTimeDate *dt1)
{
  if (dt0->Hour<dt1->Hour)
    return TIMEDATE_LESS;
  if (dt0->Hour>dt1->Hour)
    return TIMEDATE_MORE;
  if (dt0->Minute<dt1->Minute)
    return TIMEDATE_LESS;
  if (dt0->Minute>dt1->Minute)
    return TIMEDATE_MORE;
  if (dt0->Second<dt1->Second)
    return TIMEDATE_LESS;
  if (dt0->Second>dt1->Second)
    return TIMEDATE_MORE;
  return TIMEDATE_EQUAL;
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareDateTime - compare date and time
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareDateTime(TTimeDate *dt0, TTimeDate *dt1)
{
  if (TIMEDATE_CompareDate(dt0, dt1)==TIMEDATE_LESS)
    return TIMEDATE_LESS;
  if (TIMEDATE_CompareDate(dt0, dt1)==TIMEDATE_MORE)
    return TIMEDATE_MORE;
  //dates are equal
  if (TIMEDATE_CompareTime(dt0, dt1)==TIMEDATE_LESS)
    return TIMEDATE_LESS;
  if (TIMEDATE_CompareTime(dt0, dt1)==TIMEDATE_MORE)
    return TIMEDATE_MORE;
  //times are equal
  return TIMEDATE_EQUAL;
}


//------------------------------------------------------------------------------
// TIMEDATE_IsValidDate - check if date is valid
//------------------------------------------------------------------------------
bool TIMEDATE_IsValidDate(TTimeDate *dt)
{
  uint8_t maxDay = TIMEDATE_MaxDay[dt->Month];

  if ((dt->Month==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
    maxDay++;
  
  if ((dt->Month==0)||(dt->Month>12))
    return false;
  if ((dt->Day==0)||(dt->Day>maxDay))
    return false;
  return true;
} 

//------------------------------------------------------------------------------
// TIMEDATE_SetValidDate - set default valid date
//------------------------------------------------------------------------------
void TIMEDATE_SetValidDate(TTimeDate *dt)
{
  dt->Day = 1;
  dt->Month = 1;
  dt->Year = TIMEDATE_DEF_YEAR;
}  

//------------------------------------------------------------------------------
// TIMEDATE_IsValidTime - check if time is valid
//------------------------------------------------------------------------------
bool TIMEDATE_IsValidTime(TTimeDate *dt)
{
  if (dt->Hour>23)
    return false;
  if (dt->Minute>59)
    return false;
  if (dt->Second>59)
    return false;
  return true;
}

//------------------------------------------------------------------------------
// TIMEDATE_IsValid - check if date and time are valid
//------------------------------------------------------------------------------
bool TIMEDATE_IsValid(TTimeDate *dt)
{
  return (TIMEDATE_IsValidDate(dt)&&TIMEDATE_IsValidTime(dt));
}

//------------------------------------------------------------------------------
// TIMEDATE_Validate - validate date/time
//------------------------------------------------------------------------------
void TIMEDATE_Validate(TTimeDate *dt)
{
  if ((dt->Month>12)||(dt->Month==0))
    dt->Month=1;
  if ((dt->Day>TIMEDATE_MaxDay[dt->Month])||(dt->Day==0))
    dt->Day=1;
  if (dt->Hour>23)
    dt->Hour=0;
  if (dt->Minute>59)
    dt->Minute=0;
  if (dt->Second>59)
    dt->Second=0;
}

//------------------------------------------------------------------------------
// TIMEDATE_IncDate - increment date
//------------------------------------------------------------------------------
void TIMEDATE_IncDate(TTimeDate *dt)
{
  uint8_t MaxDay;

  MaxDay=TIMEDATE_MaxDay[dt->Month];
  if (dt->Month==2)
    if (TIMEDATE_IsLeapYear(dt->Year))
  MaxDay++;
  dt->Day++;
  if (dt->Day>MaxDay)
  {
		dt->Day=1;
		dt->Month++;
		if (dt->Month>12)
    {
			dt->Month=1;
			dt->Year++;
		}
  }
}

//------------------------------------------------------------------------------
// TIMEDATE_Encode - pack date and time into 5-byte structure
//------------------------------------------------------------------------------
void TIMEDATE_Encode(TTimeDate *dt, uint8_t *Buffer)
{
  uint32_t *ptrDWord;

  ptrDWord=(uint32_t *)Buffer;
  //Day
  *ptrDWord = dt->Day;
  //Month
  *ptrDWord <<= 4;
  *ptrDWord |= dt->Month;
  //Hour
  *ptrDWord <<= 6;
  *ptrDWord |= dt->Hour;
  //Minute
  *ptrDWord <<= 6;
  *ptrDWord |= dt->Minute;
  //Second
  *ptrDWord <<= 6;
  *ptrDWord |= dt->Second;
  //HSec
  *ptrDWord <<= 4;
  //store year as unpacked value
  Buffer[4] = dt->Year;
}

//------------------------------------------------------------------------------
// TIMEDATE_Decode - unpack date and time from 5-byte structure
//------------------------------------------------------------------------------
TTimeDate TIMEDATE_Decode(uint8_t *Buffer)
{
  uint32_t *ptrDWord;
  TTimeDate dt;

  ptrDWord=(uint32_t *)Buffer;
  //store year as unpacked value
  dt.Year=Buffer[4];
  //Second
  dt.Second=(uint8_t)*ptrDWord&0x3FL;
  *ptrDWord>>=6;
  //Minute
  dt.Minute=(uint8_t)*ptrDWord&0x3FL;
  *ptrDWord>>=6;
  //Hour
  dt.Hour=(uint8_t)*ptrDWord&0x3FL;
  *ptrDWord>>=6;
  //Month
  dt.Month=(uint8_t)*ptrDWord&0x0FL;
  *ptrDWord>>=4;
  //Day
  dt.Day=(uint8_t)*ptrDWord&0x3FL;
  return dt;
}

//------------------------------------------------------------------------------
// TIMEDATE_DaysOfYear - calc number of days from the 1st of January 2000
//------------------------------------------------------------------------------
uint16_t TIMEDATE_DaysOfYear(TTimeDate *dt)
{
  uint8_t i;
  uint16_t days=0;
  
  for (i=0; i<dt->Year; i++)
  {
    days+=365;
    if (TIMEDATE_IsLeapYear(i))
      days++;
  }
  for (i=1; i<dt->Month; i++)
  {  
    days+=TIMEDATE_MaxDay[i];
    if ((i==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
      days++;
  }
  
  days+=dt->Day;
  days--;
  
  return days;
}

//------------------------------------------------------------------------------
// TIMEDATE_TimeFromNull - calc number of minutes from the 1st of January 2000
//------------------------------------------------------------------------------
uint32_t TIMEDATE_TimeFromNull(TTimeDate *dt)
{
  uint8_t i;
  uint32_t time=0;
  
  for (i=0; i<dt->Year; i++)
  {
    time+=365;
    if (TIMEDATE_IsLeapYear(i))
      time++;
  }
  for (i=1; i<dt->Month; i++)
  {  
    time+=TIMEDATE_MaxDay[i];
    if ((i==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
      time++;
  }
  
  time += dt->Day;
  time--;

  time *= (24*60);

  time += dt->Hour*60;
  time += dt->Minute;
  
  return time;
}

//------------------------------------------------------------------------------
// TIMEDATE_CalcDateFromDays - calc date from the number of days (1.1.2000)
//------------------------------------------------------------------------------
void TIMEDATE_CalcDateFromDays(uint16_t days, TTimeDate *dt)
{
  uint8_t i;
  uint16_t value;
  uint16_t temp=days;
  
  dt->Year=0;
  dt->Month=1;
  dt->Day=1;
  
  value=366;
  while (temp>=value)
  {
    temp-=value;
    dt->Year++;
    value=365;
    if (TIMEDATE_IsLeapYear(dt->Year))
      value++;
  }
  i=1;
  value=TIMEDATE_MaxDay[i];
  while (temp>=value)
  {
    temp-=value;
    i++;
    dt->Month++;
    value=TIMEDATE_MaxDay[i];
    if ((i==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
      value++;
  }
  dt->Day+=temp;
}

//------------------------------------------------------------------------------
// TIMEDATE_SecondsFromNull - calc number of seconds from the 1st of January 2000
//------------------------------------------------------------------------------
uint32_t TIMEDATE_SecondsFromNull(TTimeDate *dt)
{
  uint8_t i;
  uint32_t time = 0;
  
  for (i=0; i<dt->Year; i++)
  {
    time += 365;
    if (TIMEDATE_IsLeapYear(i))
      time++;
  }
  for (i=1; i<dt->Month; i++)
  {  
    time += TIMEDATE_MaxDay[i];
    if ((i==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
      time++;
  }
  
  time += dt->Day;
  time--;

  time *= 24;
  time += dt->Hour;
  time *= 60;
  time += dt->Minute;
  time *= 60;
  
  return time;
}

//------------------------------------------------------------------------------
// TIMEDATE_CalcDateFromSeconds - calc date from the number of seconds (1.1.2000)
//------------------------------------------------------------------------------
void TIMEDATE_CalcDateFromSeconds(uint32_t seconds, TTimeDate *dt)
{
  uint16_t day;
  uint8_t year;
  uint16_t dayofyear;
  uint8_t month;
  uint8_t dayofmonth;

  dt->Second = seconds % 60;
  seconds /= 60;
  dt->Minute = seconds % 60;
  seconds /= 60;
  dt->Hour = seconds % 24;
  day = (uint16_t)(seconds / 24);

  //dt->DayOfWeek = (day + TIMEDATE_FIRSTDAY_2000) % 7;		// weekday

  year = 0;

  for(;;)
  {
    dayofyear = 365;
    if (TIMEDATE_IsLeapYear(year))
    {
      dayofyear = 366;					// leap year
      /*if( year == 0 || year == 100 || year == 200 )	// 100 year exception
        if( --leap400 )					// 400 year exception
          dayofyear = 365;*/
    }
    if (day < dayofyear)
      break;
    day -= dayofyear;
    year++;
  }
  dt->Year = year;

  month = 1;
  dayofmonth = TIMEDATE_MaxDay[month];
  //for (month = 1; day >= dayofmonth; month++)
  while (day >= dayofmonth)
  {
    day -= dayofmonth;
    month++;
    dayofmonth = TIMEDATE_MaxDay[month];
    if ((month==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
      dayofmonth++;
  }

  dt->Month = month;			// 1..12
  dt->Day = day + 1;			// 1..31
}

//------------------------------------------------------------------------------
// TIMEDATE_DaysInterval - calc interval in days between two dates
//------------------------------------------------------------------------------
int TIMEDATE_DaysInterval(TTimeDate *dt0, TTimeDate *dt1)
{
  int days;
  
  days = TIMEDATE_DaysOfYear(dt1) - TIMEDATE_DaysOfYear(dt0);
  
  return days;
}

//------------------------------------------------------------------------------
// TIMEDATE_AddDays - add days to date
//------------------------------------------------------------------------------
void TIMEDATE_AddDays(TTimeDate *dt, uint16_t days)
{
  uint16_t temp;
  
  temp = TIMEDATE_DaysOfYear(dt);
  temp += days;
  TIMEDATE_CalcDateFromDays(temp, dt);
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareDateInterval - compare two dates with interval
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareDateInterval(TTimeDate *dt0, TTimeDate *dt1, int Interval)
{
  int days;
  
  days = TIMEDATE_DaysInterval(dt1, dt0);
  if (days<Interval)
    return TIMEDATE_LESS;
  if (days>Interval)
    return TIMEDATE_MORE;
  return TIMEDATE_EQUAL;
}

//------------------------------------------------------------------------------
// TIMEDATE_Date2Str - format date to string with EUR/USA/CHN format
//------------------------------------------------------------------------------
void TIMEDATE_Date2Str(TTimeDate *dt, char *str, int Frmt)
{
  switch (Frmt)
  {
		case DFORMAT_MODE_EUR:
			sprintf(str, TIMEDATE_DateFormatEUR, (int)dt->Day, (int)dt->Month, (int)dt->Year);
			break;
		case DFORMAT_MODE_USA:
			sprintf(str, TIMEDATE_DateFormatUSA, (int)dt->Month, (int)dt->Day, (int)dt->Year);
			break;
		case DFORMAT_MODE_CHN:
			sprintf(str, TIMEDATE_DateFormatCHN, (int)dt->Year, (int)dt->Month, (int)dt->Day);
			break;
  }
}

//------------------------------------------------------------------------------
// TIMEDATE_Time2Str - format time to string
//------------------------------------------------------------------------------
void TIMEDATE_Time2Str(TTimeDate *dt, char *str, int Frmt)
{
  sprintf(str, "%02d:%02d:%02d", (int)dt->Hour, (int)dt->Minute, (int)dt->Second);
}

//------------------------------------------------------------------------------
// TIMEDATE_RealTime2Str - format real time to string with EUR/USA/CHN format
//------------------------------------------------------------------------------
void TIMEDATE_RealTime2Str(TTimeDate *dt, char *str, int Frmt)
{
  int hour;
  
  switch (Frmt)
  {
		case DFORMAT_MODE_EUR:
			sprintf(str, TIMEDATE_TimeFormatEUR, (int)dt->Hour, (int)dt->Minute);
			break;
		case DFORMAT_MODE_USA:
      hour = (int)dt->Hour;
      if (!hour) hour=12;
        else
      if (hour>12) hour-=12;
      if ((int)dt->Hour>12)
        sprintf(str, TIMEDATE_TimeFormatUSA_PM, hour, (int)dt->Minute);
      else									    
        sprintf(str, TIMEDATE_TimeFormatUSA_AM, hour, (int)dt->Minute);
			break;
		case DFORMAT_MODE_CHN:
      sprintf(str, TIMEDATE_TimeFormatCHN, (int)dt->Hour, (int)dt->Minute);
      break;
  }
}

//------------------------------------------------------------------------------
// TIMEDATE_SimpleTime2Str - format integer time to string
//------------------------------------------------------------------------------
void TIMEDATE_SimpleTime2Str(int dt, char *str)
{
  sprintf(str, "%02d:%02d:%02d", dt/3600, (dt%3600)/60, dt%60);
}

//------------------------------------------------------------------------------
// TIMEDATE_GetFatTime - convert time/date to a FAT time/date record
//------------------------------------------------------------------------------
uint32_t TIMEDATE_GetFatTime(TTimeDate *dt)
{
  uint32_t Result;

  Result =  ((uint32_t)(dt->Year + 2000 - 1980) << 25)
			| ((uint32_t)dt->Month << 21)
			| ((uint32_t)dt->Day << 16)
			| ((uint32_t)dt->Hour << 11)
			| ((uint32_t)dt->Minute << 5)
			| ((uint32_t)dt->Second >> 1);

	return Result;
}
