/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : common.h
* Author             : MCD Application Team
* Date First Issued  : 09/15/2006 : IAP Version 1.0
* Description        : This file provides all the headers of the common 
*                      functions.
********************************************************************************
* History:
* 09/15/2006 : IAP Version 1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#define ApplicationAddress 0x80000
#include "ymodem.h" 

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE     128

#define SILENT_MODE 2
#define VERBOSE_MODE 1
#define NORMAL_EXECUTION 0

/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)	((c >= 'A') && (c <= 'F'))
#define IS_af(c)	((c >= 'a') && (c <= 'f'))
#define IS_09(c)	((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)	IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)	IS_09(c)
#define CONVERTDEC(c)	(c - '0')

#define CONVERTHEX_alpha(c)	(IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

/* Exported functions ------------------------------------------------------- */
void Int2Str(u8* str,s32 intnum);
u32 Str2Int(u8 *inputstr,s32 *intnum);
u32 SerialKeyPressed(u8 *key);
void SerialPutChar(u8 c);

#endif  /* _COMMON_H */

/*******************(C)COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
