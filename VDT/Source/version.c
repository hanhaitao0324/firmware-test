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

//const char DEVICE_VERSION[] = "2090";
//const char DEVICE_VERSION[] = "2100";
const char DEVICE_BETAVERSION[] = "a";
const char DEVICE_SONDERVERSION[] = "000";
const char DEVICE_FWDATE[] = "06.03.18";

//const char DEVICE_VERSION[] = "2110"; 			// Gavin，2022-11-25 16:45, 增加v2.09关于打印机复位的内容
//const char DEVICE_VERSION[] = "2120"; 				// 2023-05-17 11：08，对设置菜单中内部时钟不更新的漏洞进行了修复
const char DEVICE_VERSION[] = "2130"; 				// 2023/11/3 周五 11:20:25，初始上电尺寸300时压力值被强制修改成700

/// @}
/// @}
