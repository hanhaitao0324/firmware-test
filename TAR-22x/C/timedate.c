#include "timedate.h"

//------------------------------------------------------------------------------
// Number of days in a month
//------------------------------------------------------------------------------
__flash TByte TIMEDATE_MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

__flash char TIMEDATE_DayOfWeek[8][4]=
  {
    "  ",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sub",
    "Sun"
  };

__flash char TIMEDATE_MonthName[13][4]=
  {
    "    ",
    "JANR",
    "FEBR",
    "MARC",
    "APRL",
    "MAY ",
    "JUNY",
    "JULY",
    "AUGS",
    "SEPT",
    "OCTB",
    "NOVM",
    "DECM",
  };

TTimeDate dt;

//------------------------------------------------------------------------------
// TIMEDATE_IsLeapYear - check if this year is a leap one
//------------------------------------------------------------------------------
TByte TIMEDATE_IsLeapYear(TByte Year)
{
  Year=Year&0x03;
  return (Year==0x00);
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareDate - compare date only
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareDate(TTimeDate dt0, TTimeDate dt1)
{
  if (dt0.Year<dt1.Year)
    return TIMEDATE_LESS;
  //Year0>=Year1
  if (dt0.Year>dt1.Year)
    return TIMEDATE_MORE;
  //Year0==Year1
  if (dt0.Month<dt1.Month)
    return TIMEDATE_LESS;
  //Month0>=Month1
  if (dt0.Month>dt1.Month)
    return TIMEDATE_MORE;
  //Month0==Month1
  if (dt0.Day<dt1.Day)
    return TIMEDATE_LESS;
  //Day0>=Day1
  if (dt0.Day>dt1.Day)
    return TIMEDATE_MORE;
  return TIMEDATE_EQUAL;
}

//------------------------------------------------------------------------------
// TIMEDATE_CompareTime - compare time only
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareTime(TTimeDate dt0, TTimeDate dt1)
{
  if (dt0.Hour<dt1.Hour)
    return TIMEDATE_LESS;
  //Hour0>=Hour1
  if (dt0.Hour>dt1.Hour)
    return TIMEDATE_MORE;
  //Hour0==Hour1
  if (dt0.Minute<dt1.Minute)
    return TIMEDATE_LESS;
  //Minute0>=Minute1
  if (dt0.Minute>dt1.Minute)
    return TIMEDATE_MORE;
  //Minute0==Minute1
  if (dt0.Second<dt1.Second)
    return TIMEDATE_LESS;
  //Second0>=Second1
  if (dt0.Second>dt1.Second)
    return TIMEDATE_MORE;
  return TIMEDATE_EQUAL;
}

//------------------------------------------------------------------------------
// TIMEDATE_IsValidDate - check if date is valid
//------------------------------------------------------------------------------
TBool TIMEDATE_IsValidDate(TTimeDate dt)
{
  TByte maxDay = TIMEDATE_MaxDay[dt.Month];

  if ((dt.Month==2)&&(TIMEDATE_IsLeapYear(dt.Year)))
    maxDay++;
  
  if ((dt.Month==0)||(dt.Month>12))
    return false;
  if ((dt.Day==0)||(dt.Day>maxDay))
    return false;
  return true;
}

//------------------------------------------------------------------------------
// TIMEDATE_IsValidTime - check if time is valid
//------------------------------------------------------------------------------
TBool TIMEDATE_IsValidTime(TTimeDate dt)
{
  if (dt.Hour>23)
    return false;
  if (dt.Minute>59)
    return false;
  if (dt.Second>59)
    return false;
  return true;
}

//------------------------------------------------------------------------------
// TIMEDATE_IsValid - check if date and time are valid
//------------------------------------------------------------------------------
TBool TIMEDATE_IsValid(TTimeDate dt)
{
  return (TIMEDATE_IsValidDate(dt)&&TIMEDATE_IsValidTime(dt));
}

//------------------------------------------------------------------------------
// TIMEDATE_Validate - validate date/time
//------------------------------------------------------------------------------
void TIMEDATE_Validate(TTimeDate *dt)
{
  TByte maxDay = TIMEDATE_MaxDay[dt->Month];

  if ((dt->Month==2)&&(TIMEDATE_IsLeapYear(dt->Year)))
    maxDay++;

  if ((dt->Month>12)||(dt->Month==0))
    dt->Month=1;
  if (dt->Day>maxDay)
    dt->Day=1;
  if (dt->Day==0)
    dt->Day=maxDay;
  if (dt->Hour>23)
    dt->Hour=0;
  if (dt->Minute>59)
    dt->Minute=0;
  if (dt->Second>59)
    dt->Second=0;
}

//------------------------------------------------------------------------------
// TIMEDATE_DayOfWeek - get day of week
//------------------------------------------------------------------------------
__flash TByte Table[13]={0,0,3,3,6,1,4,6,2,5,0,3,5};

TByte TIMEDATE_GetDayOfWeek(TTimeDate Date)
{
  TByte tmpYear;
  TByte AccValue;

  tmpYear=0x00;
  AccValue=0x0C;
  //If tmpYear0 is a leap year AccValue +2, else AccValue+1
  while (tmpYear!=Date.Year)
  {
    AccValue++;
    if (TIMEDATE_IsLeapYear(tmpYear))
      AccValue++;
    tmpYear++;
  }
  if(Date.Month>0x02)
    if (TIMEDATE_IsLeapYear(tmpYear))
      AccValue++;
  AccValue+=Table[Date.Month];
  AccValue+=Date.Day;
  AccValue%=7;
  if (AccValue==0)
    AccValue=7;
  return AccValue;
}

//------------------------------------------------------------------------------
// TIMEDATE_IncDate - increment date
//------------------------------------------------------------------------------
void TIMEDATE_IncDate(TTimeDate *DT)
{
  TByte MaxDay;

  MaxDay=TIMEDATE_MaxDay[DT->Month];
  if (DT->Month==2)
    if (TIMEDATE_IsLeapYear(DT->Year))
      MaxDay++;
  DT->Day++;
  if (DT->Day>MaxDay)
  {
    DT->Day=1;
    DT->Month++;
    if (DT->Month>12)
    {
      DT->Month=1;
      DT->Year++;
    }
  }
}
