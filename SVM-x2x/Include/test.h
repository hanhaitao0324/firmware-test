#ifndef _TEST_H
  #define _TEST_H

#include "defines.h"

//show speed
extern void TEST_ShowSpeed(TBool Led, TReal32 speed, TBool blink);

//左右显示标志
#define LEDNB_LEFT 		false
#define LEDNB_RIGHT 	true
//显示体积
extern void TEST_ShowVolt(TWord rev,TBool blink,TBool rl);
//显示质量
extern void TEST_ShowWeih(TWord rev,TBool blink,TBool rl);
//show strokes
extern void TEST_ShowRev(TWord rev,TBool blink);
//show time
extern void TEST_ShowTime(TWord time,TBool blink);
//run test
extern TBool TEST_Run(void);

#endif
