#ifndef _VERSION_H
#define _VERSION_H

#include "defines.h"
#include "const.h"

#define HVER_MAX_LEN 6
#define FVER_MAX_LEN 5

// 10: SVM_II
// 20: TAR_II
// 30: TBH_II
#define DEV_NAME 30

extern const char DEVICE_VERSION[];
extern const char DEVICE_SONDERVERSION[];
extern const char DEVICE_FWDATE[];

//extern const char FIRMWARE_VERSION[];
extern char TOUCH_HARDWARE_VERSION[];
extern char TOUCH_FIRMWARE_VERSION[];
extern char MAIN_HARDWARE_VERSION[];
extern const char MAIN_FIRMWARE_VERSION[];

#endif
