#ifndef _TEMP_H
#define _TEMP_H

#include "defines.h"

#define coefA   (3.9083E-3)
#define coefB   (5.775E-7)
#define Rt0     (100.0)

#define Z1      (-coefA)
#define Z2      (coefA*coefA - 4*coefB)
#define Z3      (4*coefB/Rt0)
#define Z4      (2*coefB)

#define TEMP_MAX_PRESETS    (2)

#define MIN_TEMP            (-10.0)
#define MAX_TEMP            (90.0)

#define ADC_MAX_TEMP_DIFF   (5.0)

extern void TEMP_Init(void);
extern TReal32 TEMP_GetValue(TByte channel);
extern TBool TEMP_SetCalPoint(TByte channel, TByte pointNum, TReal32 *realTemp);
extern TBool TEMP_Calibrate(void);

#endif
