/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    uart.h
 *      Purpose: uart driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
// Define to prevent recursive inclusion -------------------------------------
#ifndef __UART_H
#define __UART_H

#include "stm32f10x_conf.h"
#include "defines.h"

#define UART_EMPTY_BYTE  (0xff)

/* Constants used by Serial Command Line Mode */
//#define CMD_STRING_SIZE     128

#define RS232_BUFFER_SIZE					(128)
#define SER_BUF_SIZE 2048
#define RBUF_SIZE    2048 

#define HARDWARE_FLOW_CONTROL	1
#define NO_FLOW_CONTROL				0

//RS232
#define UART1_BAUD								9600
#define USART1_GPIO              	GPIOA
#define USART1_RxPin             	RXD1
#define USART1_TxPin             	TXD1

#define USART1_Tx_DMA_Channel			DMA1_Channel4
#define USART1_Tx_DMA_FLAG				DMA1_FLAG_TC4
#define USART1_DR_Base           	0x40013804

//Vinculum
#define UART3_BAUD								115200
#define USART3_GPIO              	GPIOB

typedef struct buf_st {
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  char buf [SER_BUF_SIZE];     /* Buffer */
}SBUF;


extern void USART1_Init(void);
extern void USART3_Init(void);

extern uint16_t com_putchar2(uint16_t data);
//extern int com_getchar2 (void);
int com_getchar2 (int *ret);
int com_getchar1 (void);
void com_bufclr1(void);
	
extern void com_baudrate(USART_TypeDef* USARTx, uint32_t baudrate, bool HfC);
extern void RS232_Init(void);										// RS232 init
extern void RS232_SendData(char *buf, int len);// send serial data from buffer
extern void RS232_SendString(char *buffer);		 //	send string
extern void RS232_SendChar(char data);				 // put char

#endif
