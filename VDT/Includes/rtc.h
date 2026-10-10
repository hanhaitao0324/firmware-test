#ifndef _RTC_H
#define _RTC_H

#include "defines.h"
#include <time.h>
#include "timedate.h"

typedef struct tm THETIME;
 
#define LEAPYEAR(y)   ( ((y + 1900) % 4 == 0) && (((y + 1900) % 100 != 0) || ((y + 1900) % 400 == 0)) )

//extern const U8 DayOfMonth[];
//extern const U16 mon_day[];
extern const char *month_names[];
extern const char *weekday_names[];

extern void RTC_GetDateTime(TTimeDate *dt);
extern void RTC_SetDateTime(TTimeDate *dt);
extern void RTC_Set_DefaultTimeDate(DTIM *dtim);
extern void RTC_Init(void);

#endif

