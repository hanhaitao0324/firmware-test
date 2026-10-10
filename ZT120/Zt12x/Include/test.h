#ifndef _TEST_H
  #define _TEST_H

#include "defines.h"

//show speed
extern void TEST_ShowSpeed(TReal32 speed, TBool blink);
//show speed
extern void TEST_ShowTemp(TReal32 temp, TBool blink);
//show time
extern void TEST_ShowTime(TDWord time, TBool blink);

//run test
extern TBool TEST_Run(void);

#endif
