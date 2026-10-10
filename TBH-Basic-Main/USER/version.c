/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    version.c
 *      Purpose: Versions history and numeration
 *----------------------------------------------------------------------------
 *      Copyright (c) 2016 Erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
 
/**
 @file   version.c
 @brief  Versions history and numeration
 @date  05.07.2016
 @addtogroup SourceCode
 @{
 @defgroup Version Version
 @brief Versions history and numeration
 @{
*/
 
#include "version.h"


/* nur noch im History file updaten ! "History FW V2.xx.x.txt"
  2.00.0  - new PCB (STM32F103) version;
*/

const char DEVICE_VERSION[] = "2090";
const char DEVICE_BETAVERSION[] = "a";
const char DEVICE_SONDERVERSION[] = "000";
const char DEVICE_FWDATE[] = "06.03.18";

char TOUCH_HARDWARE_VERSION[HVER_MAX_LEN+1] = "1.00.0";
char TOUCH_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "00.11";				// 

char MAIN_HARDWARE_VERSION[HVER_MAX_LEN+1] = "1.00.0";
const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.02";		// HHT 2026-09-11

/// @}
/// @}
