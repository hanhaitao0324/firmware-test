 /*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    uart.c
 *      Purpose: uart driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"
#include "stm32f10x_usart.h"
#include "defines.h"
#include "pins.h"
#include "uart.h"
#include "system.h"
#include "timer.h"
#include <string.h>

extern osThreadId CommTask;													/* assigned task id of task:  CommTask */

void USART3_IRQHandler(void);
	
// Receive Buffer	an RS232
static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))
		
// MC uart an Vinculum-II
struct buf_st rbuf2 = { 0, 0, };
#define SIO_RBUFLEN2 ((unsigned short)(rbuf2.in - rbuf2.out))
	
char RS232_RXBuffer[RS232_BUFFER_SIZE];
char RS232_TxBuffer[RS232_BUFFER_SIZE];

uint8_t RS232_IHead;

volatile bool RS232_SendBusy = false;
	
/*******************************************************************************
* Function Name  : USART1_Init
* Description    : Initialize USART1, 115200,8,N,1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_Init(void) 
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//Set USART1 Tx (PA.9) as AF push-pull
	GPIO_InitStructure.GPIO_Pin 	= TXD1;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
	
	//Set USART1 Rx (PA.10) as input 
	GPIO_InitStructure.GPIO_Pin 	= RXD1;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
	GPIO_PinLockConfig(USART1_GPIO, TXD1 | RXD1);

	com_baudrate(USART1, UART1_BAUD, NO_FLOW_CONTROL);	
	
	// Enable the USART1 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;								//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;		//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;						//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//enable IRQ channel
	NVIC_Init(&NVIC_InitStructure);																	//update NVIC registers
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									// Enable USARTy Receive interrupt
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);									// Enable USART1 DMA Tx request
}

/*******************************************************************************
* Function Name  : USART3_Init
* Description    : Initialize UART3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_Init(void) {	

	GPIO_InitTypeDef 				GPIO_InitStructure;
//	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef 				NVIC_InitStructure;														//configure NVIC

//start: wenn vinculum mit rs232 parallel zu STM32 programmiert werden soll, wieder rein
	//Set USART3 Tx (PB.10) as AF push-pull
//	GPIO_InitStructure.GPIO_Pin = TXD3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = RTS3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

//	
//	//Set USART3 Rx (PB.11) as input Pullup
//	GPIO_InitStructure.GPIO_Pin = CTS3 | RXD3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING
//  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
//	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);
//end: wenn vinculum mit rs232 parallel zu STM32 programmiert werden soll, wieder rein
	
	//Set USART3 Tx (PB.10) as AF push-pull
	GPIO_InitStructure.GPIO_Pin = TXD3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RTS3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

	//Set USART3 Rx (PB.11) as input Pullup
	GPIO_InitStructure.GPIO_Pin = CTS3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

	//Set USART3 Rx (PB.11) as input Pullup
	GPIO_InitStructure.GPIO_Pin = RXD3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(USART3_GPIO, &GPIO_InitStructure);

	// Enable the USART3 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;								//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;		//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;						//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//enable IRQ channel
	NVIC_Init(&NVIC_InitStructure);	
	
	com_baudrate(USART3, UART3_BAUD, HARDWARE_FLOW_CONTROL);				//Vinculum-II Communication
	
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);										//enable Transmit Data Register empty interrupt
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//enable Receive Data register not empty interrupt
}

/*******************************************************************************
* Function Name  : com_baudrate
* Description    : change baudrate uart 8,N,1
* Input          : uint32_t baudrate, BOOL Hardware flow Controll
* Output         : None
* Return         : None
*******************************************************************************/
void com_baudrate(USART_TypeDef* USARTx, uint32_t baudrate, bool HfC)
{
	USART_InitTypeDef 	USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // Enable Tx and Rx
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	if(HfC)	
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
	USART_Init(USARTx, &USART_InitStructure);												//Write USARTx parameters
  USART_Cmd(USARTx, ENABLE);																			//Enable USARTx
}

/*******************************************************************************
* Function Name  : com_putchar2
* Description    : send byte via uart3 to Vinculum-II
* Input          : uint16_t data
* Output         : uint16_t data
* Return         : uint16_t
*******************************************************************************/
uint16_t com_putchar2(uint16_t data)
{ 
	TIMER_Start(TIMER_Vinculum, TO_500MSEC);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) { if(TIMER_TimeOut(TIMER_Vinculum))return 1; };
	USART_SendData(USART3, data);
  return 0;
}

/*******************************************************************************
* Function Name  : com_getchar2
* Description    : receive byte via uart3 from Vinculum-II
* Input          : uint16_t data
* Output         : uint16_t data
* Return         : int 
*******************************************************************************/
int com_getchar2 (int *ret){
  struct buf_st *p = &rbuf2;

  if (SIO_RBUFLEN2 == 0)
    return (-1);
	*ret = p->buf [(p->out++) & (RBUF_SIZE - 1)];
	return 0;
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : receive from Vinculum-II and fill rbuf2
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void USART3_IRQHandler(void)
{
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // Received characters modify string
  {
		unsigned char ch; 
		struct buf_st *p;

		ch = USART_ReceiveData(USART3);
		//RS232_SendData((char *)&ch,1);//rein für vinculum communication über terminal und dem rs232 port
		p = &rbuf2;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
				p->buf [p->in & (RBUF_SIZE-1)] = ch;
				p->in++;
		}
	}
}


/*******************************************************************************
* Function Name  : RS232_SendData
* Description    : send byte via uart1 to RS232 Port
* Input          : char pointer to buffer, int lenght of data to send
* Output         : None
* Return         : None
*******************************************************************************/
void RS232_SendData(char *buf, int len)
{
	DMA_InitTypeDef DMA_InitStructure;
	TIMER_Start(TIMER_RS232, TO_200MSEC);
	
	while (USART1_Tx_DMA_Channel->CNDTR)
  { 
    if (TIMER_TimeOut(TIMER_RS232))
      break; 
 //   osDelay(1);
  }
	
  if (len)
  {
		memcpy(RS232_TxBuffer, buf, len);
	  RS232_SendBusy = true;
		
		// USART1 TX DMA1 Channel (triggered by USART1 Tx event) Config
		DMA_DeInit(USART1_Tx_DMA_Channel);  
		DMA_StructInit(&DMA_InitStructure);
		DMA_InitStructure.DMA_PeripheralBaseAddr 	= USART1_DR_Base;
		DMA_InitStructure.DMA_MemoryBaseAddr 		 	= (uint32_t)RS232_TxBuffer;
		DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize 					= len;
		DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority 						= DMA_Priority_High;
		DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;
		DMA_Init(USART1_Tx_DMA_Channel, &DMA_InitStructure);
		//DMA_ITConfig (DMA1_Channel4, DMA_IT_TC, ENABLE);
	  DMA_Cmd(USART1_Tx_DMA_Channel, ENABLE);
  }
}

//------------------------------------------------------------------------------
// RS232_SendChar - send a char to RS232
//------------------------------------------------------------------------------
void RS232_SendChar(char data)
{        
	TIMER_Start(TIMER_RS232, TO_200MSEC);	

	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) { if(TIMER_TimeOut(TIMER_RS232))break; };	// wait until transmit buffer is empty
	USART_SendData(USART1, data);	
}

void RS232_SendString(char *buffer)
{
  int len = strlen(buffer);
  RS232_SendData(buffer, len);
}

/*******************************************************************************
* Function Name  : com_getchar1
* Description    : receive byte via uart1 from RS232 Port
* Input          : uint16_t data
* Output         : uint16_t data
* Return         : int 
*******************************************************************************/
int com_getchar1 (void){
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);
  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}
 
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : receive line and fill rbuf1, RS232
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void USART1_IRQHandler(void) 
{
	char c;
	struct buf_st *p;
	
  if (USART_GetITStatus(USART1, USART_IT_RXNE)) 
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		c = USART_ReceiveData(USART1);
	//com_putchar2(c);//rein für vinculum communication über terminal und dem rs232 port
//-----------------------------
// Vinculum rxbuffer
		p = &rbuf;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
				p->buf [p->in & (RBUF_SIZE-1)] = c;
				p->in++;
		}
//-----------------------------
    if (c==CHAR_NL) return;
		
    RS232_RXBuffer[RS232_IHead++] = c;
		
    if (c==CHAR_ESC)
    {
      RS232_IHead = 0;
      return;
    }
		
    if (c==CHAR_CR)
    {
//-----------------------------
// Vinculum rxbuffer
//			com_bufclr1();
//-----------------------------

      RS232_RXBuffer[RS232_IHead-1] = 0;											//clear CHAR_CR from RS232_RXBuffer
			RS232_IHead = 0;
	    osSignalSet(CommTask, 0x1);//raus für vinculum communication über terminal und dem rs232 port
		}
  }

}


void com_bufclr1(void){
	struct buf_st *p = &rbuf;
	p->in=p->out;
}

void com_bufclr2(void)
{
	struct buf_st *p = &rbuf2;
	p->in=p->out;
}

void com_put2ww(int c)
{
	while(com_putchar2(c) > 0)
		osThreadYield();
}
