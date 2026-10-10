#ifndef _SAMPLER_H
#define _SAMPLER_H

#include "defines.h"
#include "const.h"
#include "commands.h"

//extern TReal32 SAMPLER_Temp[MAX_SENSORS_NUM];
extern TsWord SAMPLER_Temp[MAX_SENSORS_NUM];

extern void SAMPLER_SetActive(TBool active);
extern void SAMPLER_Init(void);
extern void SAMPLER_IntHandler(void);
extern TBool SAMPLER_Connect(void);
//extern TBool SAMPLER_GetTemp(TByte sens, TReal32 *temp);
extern TBool SAMPLER_GetTemp(void);
extern TBool SAMPLER_Calibrate(TByte point, TReal32 *temp, TByte channel);
extern TBool SAMPLER_SetMinTemp(TReal32 *temp);
extern TBool SAMPLER_SetMaxTemp(TReal32 *temp);
extern TBool SAMPLER_GetSerial(TByte *buffer);

#endif
