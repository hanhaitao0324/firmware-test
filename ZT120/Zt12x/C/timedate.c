#include "timedate.h"

//=================================================================
// Number of days in a month
//=================================================================
__flash TByte TIMEDATE_MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
__flash TByte TIMEDATE_MaxDay_leap[13]={0,31,29,31,30,31,30,31,31,30,31,30,31}; // HHT  240612-1105    增加闰年的最大天数限制

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
// TIMEDATE_CompareDateTime - compare date and time
//------------------------------------------------------------------------------
signed char TIMEDATE_CompareDateTime(TTimeDate dt0, TTimeDate dt1)
  {
    if (TIMEDATE_CompareDate(dt0,dt1)==TIMEDATE_LESS)
      return TIMEDATE_LESS;
    if (TIMEDATE_CompareDate(dt0,dt1)==TIMEDATE_MORE)
      return TIMEDATE_MORE;
    //dates are equal
    if (TIMEDATE_CompareTime(dt0,dt1)==TIMEDATE_LESS)
      return TIMEDATE_LESS;
    if (TIMEDATE_CompareTime(dt0,dt1)==TIMEDATE_MORE)
      return TIMEDATE_MORE;
    //times are equal
    return TIMEDATE_EQUAL;
  }


//------------------------------------------------------------------------------
// TIMEDATE_IsValidDate - check if date is valid
//------------------------------------------------------------------------------
TBool TIMEDATE_IsValidDate(TTimeDate dt)
  {
    if ((dt.Month==0)||(dt.Month>12))
      return false;
    if ((!TIMEDATE_IsLeapYear(dt.Year))&&((dt.Day==0)||(dt.Day>TIMEDATE_MaxDay[dt.Month])))     // HHT 240612-1130  增加平年闰年月份的区别
      return false;
    if ((TIMEDATE_IsLeapYear(dt.Year))&&((dt.Day==0)||(dt.Day>TIMEDATE_MaxDay_leap[dt.Month]))) // HHT 240612-1130
      return false;    
//    if ((dt.Month==2)&&(!TIMEDATE_IsLeapYear(dt.Year))&&      // HHT 240612-1130
//      (dt.Day>28))
//      return false;
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
    if ((dt->Month>12)||(dt->Month==0))
      dt->Month=1;
    if ((!TIMEDATE_IsLeapYear(dt->Year))&&((dt->Day>TIMEDATE_MaxDay[dt->Month])||(dt->Day==0)))  // HHT 240612-1130  增加平年闰年月份的区别
      dt->Day=1;
    if ((TIMEDATE_IsLeapYear(dt->Year))&&((dt->Day>TIMEDATE_MaxDay_leap[dt->Month])||(dt->Day==0)))  // HHT 240612-1130    
      dt->Day=1; 
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

    if(!TIMEDATE_IsLeapYear(DT->Year))   // HHT 240612-1130  增加平年闰年月份的区别
      MaxDay=TIMEDATE_MaxDay[DT->Month];
    else
      MaxDay=TIMEDATE_MaxDay_leap[DT->Month];
    
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

//------------------------------------------------------------------------------
// TIMEDATE_Encode - pack date and time into 5-byte structure
//------------------------------------------------------------------------------
void TIMEDATE_Encode(TTimeDate dt,TByte *Buffer)
  {
    TDWord *ptrDWord;

    ptrDWord=(TDWord *)Buffer;
    //Day
    *ptrDWord=dt.Day;
    //Month
    *ptrDWord<<=4;
    *ptrDWord|=dt.Month;
    //Hour
    *ptrDWord<<=6;
    *ptrDWord|=dt.Hour;
    //Minute
    *ptrDWord<<=6;
    *ptrDWord|=dt.Minute;
    //Second
    *ptrDWord<<=6;
    *ptrDWord|=dt.Second;
    //HSec
    *ptrDWord<<=4;
    //store year as unpacked value
    Buffer[4]=dt.Year;
  }

//------------------------------------------------------------------------------
// TIMEDATE_Decode - unpack date and time from 5-byte structure
//------------------------------------------------------------------------------
TTimeDate TIMEDATE_Decode(TByte *Buffer)
  {
    TDWord *ptrDWord;
    TTimeDate dt;

    ptrDWord=(TDWord *)Buffer;
    //store year as unpacked value
    dt.Year=Buffer[4];
    //Second
    dt.Second=(TByte)*ptrDWord&0x3FL;
    *ptrDWord>>=6;
    //Minute
    dt.Minute=(TByte)*ptrDWord&0x3FL;
    *ptrDWord>>=6;
    //Hour
    dt.Hour=(TByte)*ptrDWord&0x3FL;
    *ptrDWord>>=6;
    //Month
    dt.Month=(TByte)*ptrDWord&0x0FL;
    *ptrDWord>>=4;
    //Day
    dt.Day=(TByte)*ptrDWord&0x3FL;
    return dt;
  }
