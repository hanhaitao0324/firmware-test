#ifndef _PRINTRTC_H
#define _PRINTRTC_H

#include "defines.h"
#include "commands.h"
#include "timedate.h"

#define PRN_OLDVER_MAJOR    (1)
#define PRN_OLDVER_MINOR    (2)

extern TBool PRN_Busy;

extern TBool PRN_Connect(void);
extern void PRN_Check(void);
extern TBool PRN_Init(void);
extern TByte PRN_Reset(void); 					// Gavin, 2019-08-14 ÐÇÆÚÈý 17:48:23
extern TBool PRN_Done(void);
extern TByte PRN_Print(char *buffer);
extern TByte PRN_Status(TPrinterStatus *status);
extern TByte PRN_GetVersion(TWord *Version);
extern TBool RTC_GetDateTime(TTimeDate *dt);
extern TBool RTC_SetDateTime(TTimeDate dt);

#endif
