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

//PORT_C
#define VINC_PROG  		VNC_PROG
#define VINC_RESET 		VNC_RESET
#define VINC_GPIOC		(PORT_C)

//PORT_B
#define VINC_DTR		DTR3 // (DTR) Data acknowledge (data terminal ready control) VNC output
#define VINC_DSR		DSR3 // (DSR) Data request (data set ready control) VNC input
#define VINC_GPIOB	(PORT_B)

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
extern void SendPrinter(const char *str);
extern int VincErase(void);
extern unsigned int Vinc_StartProg(void);
extern int Vinc_Prog_Block(int block,unsigned char *buf,unsigned int size);

void SendPrinterCC(const char *str);
void StopPrinter(void);

#endif
