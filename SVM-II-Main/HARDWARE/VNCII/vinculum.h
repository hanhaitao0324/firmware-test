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
#define VINC_PROG  		VNC_PROG
#define VINC_RESET 		VNC_RESET
//#define VINC_GPIOC		(PORT_C)

//PORT_B
#define VINC_DTR		DTR3 // (DTR) Data acknowledge (data terminal ready control) VNC output
#define VINC_DSR		DSR3 // (DSR) Data request (data set ready control) VNC input
#define VINC_GPIOD	(PORT_D)

#define VINC_RESET_BAUD	9600
#define VINC_BAUD				115200
#define VINC_UART				USART3

extern void VINC_Init(void);
extern void VINC_Reset(void);
extern void Vinc_Extended(void);
extern void Vinc_Send(char *bef);
extern void Vinc_GetBuf(char *buff) ;
extern void Vinc_Vers(char *erg);
extern unsigned int Vinc_Query(unsigned int port);
extern void Vinc_Dir(char *erg);
extern void Vinc_QD(unsigned int num,char *erg);
extern int StartPrinter(void);
extern void SendPrinter(char *str);
extern int VincErase(void);
extern unsigned int Vinc_StartProg(void);
extern int Vinc_Prog_Block(int block,unsigned char *buf,unsigned int size);

void SendPrinterCC(const char *str);
void StopPrinter(void);

extern void PrinterInit(void);

//2022/1/5 星期三 16:27:06 增加U盘导出识别相关函数
extern void DetectionUdisk(void);
extern int Vinc_OpenFile(char *nam);
extern int Vinc_CloseFile(char *nam);
extern int Vinc_WriteFile(int num,char *buf);
extern int Vinc_DeleteFile(char *fname);
extern void Vinc_CDFile(char *fname);
extern void Vinc_MKDFile(char *fname);
extern void Vinc_CD(void);
#endif
