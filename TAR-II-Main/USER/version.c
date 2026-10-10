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
//char TOUCH_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "00.10";				// 2022-03-21 周一 16:42:31
char TOUCH_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.00";				// 2022-10-26 周三 15:11:50

char MAIN_HARDWARE_VERSION[HVER_MAX_LEN+1] = "1.00.0";
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "00.10";		// 2022-03-21 周一 16:42:36
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.00";		// 2022-10-26 周三 15:12:12
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.01";		// 2023-03-11 周六 14:09:28
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.02";		// 2023-08-04 周五 10:02:53
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.03";		// 2023-12-28 周四 18:43:54
//const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.04";		// 2024-03-20 周三 16:58:09，无电子秤版本首页显示问题
const char MAIN_FIRMWARE_VERSION[FVER_MAX_LEN+1] = "01.05";     // 2024-11-21 周四 修复IP地址不能如预期输入的问题

/// @}
/// @}
