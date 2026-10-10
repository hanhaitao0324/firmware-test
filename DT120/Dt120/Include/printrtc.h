#ifndef _PRINTRTC_H
#define _PRINTRTC_H

#include "defines.h"
#include "commands.h"
#include "timedate.h"

extern TBool PRN_Busy;

extern void PRN_Check(void);

extern TBool PRN_Init(void);

extern TByte PRN_Reset(void); 					// 2016-10-11 ÐÇÆÚ¶þ 10:33:54

extern TBool PRN_Done(void);

extern TByte PRN_Print(char *buffer);

extern TByte PRN_Status(TPrinterStatus *status);

extern TBool RTC_GetDateTime(TTimeDate *dt);

extern TBool RTC_SetDateTime(TTimeDate dt);

extern TBool PRN_Connect(void);

#endif
