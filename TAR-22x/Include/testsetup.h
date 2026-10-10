#ifndef _TESTSETUP_H
  #define _TESTSETUP_H

#include "defines.h"

//parameter increment
extern TBool TESTSETUP_IncInt(signed char step,TWord *value,TWord lo,TWord hi);

//setup test
extern void TESTSETUP_Run(void);

#endif
