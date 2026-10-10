#ifndef _TEST_H
  #define _TEST_H

#include "defines.h"

//show speed
extern void TEST_ShowSpeed(TBool Led, TReal32 speed, TBool blink);
//show strokes
extern void TEST_ShowRev(TWord rev,TBool blink);
//show time
extern void TEST_ShowTime(TWord time,TBool blink);
//run test
extern TBool TEST_Run(void);

#endif
