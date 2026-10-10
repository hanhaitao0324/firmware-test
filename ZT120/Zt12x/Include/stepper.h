#ifndef _STEPPER_H
  #define _STEPPER_H

#include "platform.h"
#include "defines.h"
#include "const.h"
#include "timer.h"

#define STEPPER_STEPS_PER_MM     (200)
#define STEPPER_STEPS_DIVIDER    (3)
#define STEPPER_MAX_DISTANCE_MM  (90L)
//#define STEPPER_MAX_DISTANCE     (STEPPER_MAX_DISTANCE_MM*STEPPER_STEPS_PER_MM*4/STEPPER_STEPS_DIVIDER)

extern __flash char STATUS_PROBE[PROBE_STATUS_LAST][3];

extern __flash TWord PROBE_VOLUME[DEPTH_MODE_LAST];

extern __flash TWord METHOD_MAX_OFFS[METH_MODE_LAST];

extern void STEPPER_Init(void);
extern void STEPPER_SetDir(TBool dir);
extern void STEPPER_Start(TBool dir, TBool absolute);
extern void STEPPER_Stop(void);
extern void STEPPER_GetPulses(TDWord *Num);
extern TByte STEPPER_GetStatus(void);
extern void STEPPER_IntHandler(void);
extern void STEPPER_SetDistance(TReal32 *Dist);
extern void STEPPER_GetPosition(TReal32 *Position);
extern void STEPPER_CalcDistance(TByte VolIndex);
extern void STEPPER_CalcDistanceNew(TWord *Volume);

#endif
