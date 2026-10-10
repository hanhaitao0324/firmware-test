#ifndef _TESTSETUP_H
  #define _TESTSETUP_H

#include "defines.h"

extern volatile TBool TT_s; 				// 时分秒设置标志，true(0)，设置小时/分钟；false(1)，设置秒/温度；

//parameter increment
//extern TBool TESTSETUP_IncInt(signed char step,TWord *value,TWord lo,TWord hi);
extern TBool TESTSETUP_IncInt(signed int step,TWord *value,TWord lo,TWord hi);
//setup starting temperature
extern void TESTSETUP_SetTemp(void);
//setup test
extern void TESTSETUP_Run(void);

#endif
