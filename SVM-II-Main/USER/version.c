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
char TOUCH_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.03";				//2022/1/5 星期三 17:25:26

char MAIN_HARDWARE_VERSION[HVER_MAX_LEN+1] = "1.00.0";
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.14";		//2022/1/5 星期三 17:25:22
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.15";		// 2023-01-29 周日 9:45:44
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.16";		// Gavin，2023-09-10 周日 16:45:33
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.17";		// HHT，2024-09-05
const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.18";		// HHT，2024-11-21	// 修复IP输入问题

/// @}
/// @}
