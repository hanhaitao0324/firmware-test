#ifndef _CONTROL_H
#define _CONTROL_H

#include "defines.h"
#include "const.h"

extern void CTL_ShowTime(TDWord time, TBool blink);
extern void CTL_ShowTemp(TReal32 temp, TBool blink);
extern void CTL_ShowSpeed(TReal32 speed, TBool blink, TBool run);
extern void CTL_SetPreset(void);
extern void CTL_SetOffset(TByte ch);
extern void CTL_SetFilter(void);
extern void CTL_Adjustment(TByte NumPoints);
extern void CTL_AdjustmentDHC(TByte NumPoints);
extern void CTL_AdjustmentAutosampler(TByte NumPoints);
extern TBool CTL_Brightness(TByte level);
extern void CTL_SetTime(void);
extern void CTL_SetDate(void);
extern void CTL_SetYear(void);
extern void CTL_AdjustmentStepper(void);
extern void CTL_MethodOffset(TByte Num);
//extern void CTL_CheckTemperature(void);
extern void CTL_CheckTemperature(TByte channel);

// 增加外部函数声明，v2.10，Gavin，2020-09-03 星期四 14:24:08
extern void CTL_SetPwst(void);
extern void CTL_SetPwdt(void);
extern signed char CTL_CheckPW(char *title, TWord code);

#endif
