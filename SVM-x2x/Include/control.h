#ifndef _CONTROL_H
#define _CONTROL_H

#include "defines.h"

extern void CTL_ShowTime(TWord time, TBool blink);
extern void CTL_ShowSpeed(TReal32 speed, TBool blink, TBool run);
//extern void CTL_SetTimeUnits(void);
extern void CTL_SetTime(void);
extern void CTL_SetDate(void);
extern void CTL_SetYear(void);
extern void CTL_SetDFormat(void);

// 增加外部函数声明，v1.30，Gavin，2019-08-08 星期四 11:27:44
extern void CTL_SetPwst(void);
extern void CTL_SetPwdt(void);
extern signed char CTL_CheckPW(char *title, TWord code);

#endif
