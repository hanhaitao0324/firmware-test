#ifndef _LIFTER_H
  #define _LIFTER_H

#include "platform.h"
#include "defines.h"
#include "const.h"

extern __flash char STATUS_LIFTER[LIFTER_STATUS_LAST][3];

extern TByte LIFTER_GetStatus(void);
extern void LIFTER_Up(TWord distance);
extern void LIFTER_Down(TWord distance);

#endif
