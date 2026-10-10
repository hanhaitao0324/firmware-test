#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "defines.h"

#define RS232_BUFFER_SIZE					(128)
#define SER_BUF_SIZE 2048
#define RBUF_SIZE    2048 

#define HARDWARE_FLOW_CONTROL	1
#define NO_FLOW_CONTROL				0

//RS232
#define UART1_BAUD								9600
#define UART1_BAUD2 							115200
#define UART1_BAUD3 							4800
#define USART1_GPIO              	GPIOB
#define USART1_RxPin             	RXD1
#define USART1_TxPin             	TXD1

#define USART1_Tx_DMA_Channel			DMA2_Stream7 //DMA1_Channel4
//#define USART1_Tx_DMA_FLAG				DMA1_FLAG_TC4
#define USART1_DR_Base           	(u32)&USART1->DR

extern void USART1_Init(void);
int com_getchar1 (void);
void com_bufclr1(void);

extern void RS232_Init(void);										// RS232 init
extern void RS232_SendData(char *buf, int len);// send serial data from buffer
extern void RS232_SendString(char *buffer);		 //	send string
extern void RS232_SendChar(char data);				 // put char





#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

typedef struct buf_st 
{
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  char buf [SER_BUF_SIZE];     /* Buffer */
}SBUF;

//Vinculum
#define UART3_BAUD								115200

extern void com_baudrate(USART_TypeDef* USARTx, uint32_t baudrate, bool HfC);

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;   						//接收数据状态

void USART3_Init(void);				//串口3初始化 
void u3_printf(char* fmt, ...);

int com_getchar3 (int *ret);
uint16_t com_putchar3(uint16_t data);

void com_bufclr3(void);
void com_put3ww(int c);

#endif


