#ifndef _VERSION_H
  #define _VERSION_H

#include "const.h"

extern __flash char VERSION_Revision[];

extern __flash char VERSION_Str[];

extern __flash char DEVICE_TYPE[DEVICE_TYPE_LAST][5];

extern __flash char DEVICE_MODE[DEVICE_TYPE_LAST][DEVICE_MODE_LAST][5];

extern __flash char DEVICE_LOGO[DEVICE_LOGO_LAST][10];

#endif
