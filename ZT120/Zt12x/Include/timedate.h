#ifndef _TIMEDATE_H
  #define _TIMEDATE_H

#include "defines.h"
#pragma pack(1)
typedef struct
  {
    TByte Day;
    TByte Month;
    TByte Year;
    TByte Hour;
    TByte Minute;
    TByte Second;
  } TTimeDate;
#pragma pack()

enum TIMEDATE_COMPARE_RESULTS
  {
    TIMEDATE_LESS=-1,
    TIMEDATE_EQUAL=0,
    TIMEDATE_MORE=1
  };

#define SECOND_WEIGHT	(1.0/(24*60*60L))
#define MINUTE_WEIGHT	(1.0/(24*60))
#define HOUR_WEIGHT	(1.0/24)

// Number of days in a month
extern __flash TByte TIMEDATE_MaxDay[13];
extern __flash TByte TIMEDATE_MaxDay_leap[13];  // HHT  240612-1105     ╘Ў╝╙╚Є─ъ╡─╫ю┤є╠ь╩¤╧▐╓╞
extern __flash char TIMEDATE_DayOfWeek[8][4];
extern __flash char TIMEDATE_MonthName[13][4];

// compare date only
extern signed char TIMEDATE_CompareDate(TTimeDate dt0, TTimeDate dt1);
// compare time only;
extern signed char TIMEDATE_CompareTime(TTimeDate dt0, TTimeDate dt1);
//compare date and time
extern signed char TIMEDATE_CompareDateTime(TTimeDate dt0, TTimeDate dt1);
// check if date is valid
extern TBool TIMEDATE_IsValidDate(TTimeDate dt);
// check if time is valid
extern TBool TIMEDATE_IsValidTime(TTimeDate dt);
// check if date and time are valid
extern TBool TIMEDATE_IsValid(TTimeDate dt);
//validate date/time
extern void TIMEDATE_Validate(TTimeDate *dt);

//check if this year is a leap one
extern TByte TIMEDATE_IsLeapYear(TByte Year);
//определить день недели указанной даты
extern TByte TIMEDATE_GetDayOfWeek(TTimeDate Date);
//увеличить на 1 день дату
extern void TIMEDATE_IncDate(TTimeDate *DT);

// TIMEDATE_Encode - pack date and time into 5-byte structure
extern void TIMEDATE_Encode(TTimeDate dt,TByte *Buffer);
// TIMEDATE_Decode - unpack date and time from 5-byte structure
extern TTimeDate TIMEDATE_Decode(TByte *Buffer);

#endif
