/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    vinculum.h
 *      Purpose: Vinculum driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef _VINCULUM_H
#define _VINCULUM_H

#include "pins.h"

#define PAGESIZE	128

//PORT_D
// #define VINC_PROG  		VNC_PROG
// #define VINC_RESET 		VNC_RESET

#define VINC_RXD			(GPIO_Pin_8)
#define VINC_TXD			(GPIO_Pin_9)
#define VINC_DTR			(GPIO_Pin_10)
#define VINC_RTS			(GPIO_Pin_11)
#define VINC_CTS			(GPIO_Pin_12)
#define VINC_DSR			(GPIO_Pin_13)
#define VINC_PROG		(GPIO_Pin_14)
#define VINC_RESET		(GPIO_Pin_15)
//#define VINC_GPIOC		(PORT_C)

//PORT_B
//#define VINC_DTR		DTR3 // (DTR) Data acknowledge (data terminal ready control) VNC output
//#define VINC_DSR		DSR3 // (DSR) Data request (data set ready control) VNC input
#define VINC_GPIOD	(PORT_D)

#define VINC_RESET_BAUD	9600
#define VINC_BAUD				115200
#define VINC_UART				USART3

void VINC_Init(void);
void VINC_Reset(void);
void VINC_Reset_Old(void);
void Vinc_Extended(void);
void Vinc_Send(char *bef);
void Vinc_GetBuf(char *buff) ;
void Vinc_Vers(char *erg);
unsigned int Vinc_Query(unsigned int port);
void Vinc_Dir(char *erg);
void Vinc_QD(unsigned int num,char *erg);
void Vinc_SC_Old(unsigned int num);
void Vinc_QD_Old(unsigned int num,char *erg);
int StartPrinter(void);
void SendPrinter(char *str);
int VincErase(void);
unsigned int Vinc_StartProg(void);
int Vinc_Prog_Block(int block,unsigned char *buf,unsigned int size);

int CheckPrompt(void);
int  Vinc_GetPrompt(void);
int Vinc_GetPrompt_Old(void);

void PrintBold(void);
void PrintNorm(void);

int StartKalibrator(void);
char SendKalibrator(char *str,char *answer);
float GetAutocalVal(float *weight);
int GetLength(void);
void Vinc_SF(unsigned int num);

void SendPrinterCC(const char *str);
void StopPrinter(void);

void PrinterInit(void);
void PrinterExit(void);

//2022/1/5 星期三 16:27:06 增加U盘导出识别相关函数
char DetectionUdisk(void);
int Vinc_OpenFile(char *nam);
int Vinc_CloseFile(char *nam);
int Vinc_WriteFile(int num,char *buf);
int Vinc_DeleteFile(char *fname);
void Vinc_CDFile(char *fname);
void Vinc_MKDFile(char *fname);
void Vinc_CD(void);
#endif
