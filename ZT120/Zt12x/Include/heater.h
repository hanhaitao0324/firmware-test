#ifndef _HEATER_H
#define _HEATER_H

#include "defines.h"

#define P_COEF      (0)
#define I_COEF      (0)

extern void HEATER_Init(void);
extern void HEATER_TimeTick(void);
extern void HEATER_Regulate(void);
extern void HEATER_SetOff(void);
extern void HEATER_SetTemp(TReal32 *temp);
extern TBool HEATER_On(void);
extern TByte HEATER_GetPower(void);
extern TBool HEATER_Flag(void);

#endif

