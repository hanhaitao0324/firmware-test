/*------------------------------------------------------------------------------
MAIN.C:  Interrupt Driven SIO Using printf for ST STR91x.

Copyright 2004-2006 KEIL Software, Inc.
------------------------------------------------------------------------------*/
#include <string.h>
#include <limits.h>
#include "tbh125.h"

extern OS_TID USART3Task;

extern BOOL BusyFlg;
extern int YModemAktiv;



struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((unsigned short)(tbuf.in - tbuf.out))

struct buf_st tbuf2 = { 0, 0, };
#define SIO_TBUFLEN2 ((unsigned short)(tbuf2.in - tbuf2.out))

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

static struct buf_st rbuf2 = { 0, 0, };
#define SIO_RBUFLEN2 ((unsigned short)(rbuf2.in - rbuf2.out))


extern OS_TID CommTask; 
extern OS_TID BusyTask; 
static unsigned int tx_restart = 1;   /* NZ if TX restart is required */
static unsigned int tx_restart2 = 1;   

unsigned int rspoint=0,tcppoint=0;
unsigned char VincHand=0;

//extern void putusbstr(char *);
BOOL USB_Comm=__FALSE;
BOOL TCP_Comm=__FALSE;
BOOL Connected=__FALSE;
BOOL Connected1=__FALSE;
BOOL RTS_TimeOut = __FALSE;

int delay_sio = 20000;
int FLAG_TC = 0;
extern BOOL BusyFlg;

/*------------------------------------------------------------------------------
串口波特率设置函数
------------------------------------------------------------------------------*/
void SET_Baudrate(USART_TypeDef* USARTx, uint32_t baudrate)
{
	USART_InitTypeDef	USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // Enable Tx and Rx
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx, &USART_InitStructure);												//Write USARTx parameters
  USART_Cmd(USARTx, ENABLE);																			//Enable USARTx

}
/*------------------------------------------------------------------------------
串口2中断函数
------------------------------------------------------------------------------*/
void USART2_IRQHandler(void)																								
{
	unsigned char ch;
	struct buf_st *p;

	
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)																//串口接收中断
	{
		USART_ClearFlag(USART2,USART_FLAG_RXNE);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		
		ch = USART_ReceiveData(USART2);
		p = &rbuf;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) 
		{
			p->buf [p->in & (RBUF_SIZE-1)] = ch;
      p->in++;
		}
		if(!YModemAktiv && ch==0x0D) 
		{
			if(!BusyFlg)
				isr_evt_set(0x0001,CommTask);
			else
				isr_evt_set(0x0001,BusyTask);
			
		}
		
	}
	
	if(USART_GetITStatus(USART2,USART_IT_TC)!=RESET)																//串口2发送中断部分
	{

		USART_ClearITPendingBit(USART2,USART_IT_TC);

		p = &tbuf;
		if (p->in != p->out) 
		{
			
			USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
			USART_SendData(USART2,(p->buf [p->out & (TBUF_SIZE-1)]));
			p->out++;
			tx_restart = 0;
		}
		else 
		{
			tx_restart = 1;
		}
			
	}
}
/*------------------------------------------------------------------------------
串口3中断函数
------------------------------------------------------------------------------*/
void USART3_IRQHandler(void)	
{
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)																//????
	{
		
		unsigned char ch; 
		struct buf_st *p;
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		
		ch = USART_ReceiveData(USART3);
		p = &rbuf2;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) 
		{
			p->buf [p->in & (RBUF_SIZE-1)] = ch;
			p->in++;
		}
	}
	
	if(USART_GetITStatus(USART3,USART_IT_TC))																//????
	{
		USART_ClearFlag(USART3,USART_FLAG_TC);
//		USART_ClearITPendingBit(USART3,USART_IT_TC);
		isr_evt_set(0x0001,USART3Task);
		FLAG_TC = 1 ;
	}
}

__task void UART3Send(void)
{
	
  struct buf_st *p;
	int i;
	while(1) 
	{
		os_evt_wait_or(0x0001,0xFFFF);
   	p = &tbuf2;
   	if (p->in != p->out) 
		{
			if(VincHand)
			{
				i=0;
				while(GPIO_ReadInputDataBit(GPIOD,VNC_RTS)==1 && i++<1000)
					os_dly_wait(10);
				
				if(i>=1000) RTS_TimeOut=__TRUE;
			}
			USART_SendData(USART3,(p->buf [p->out & (TBUF_SIZE-1)]));
			p->out++;
			tx_restart2 = 0;
    }
    else 
		{
			tx_restart2 = 1;
    }
	}
}
/*------------------------------------------------------------------------------
串口初始化
------------------------------------------------------------------------------*/
void com_initialize (void) 														//???
{
  volatile char dummy;
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
  /*------------------------------------------------
  Clear com buffer indexes.
  ------------------------------------------------*/
  tbuf.in = 0;
  tbuf.out = 0;
  tx_restart = 1;

  tbuf2.in = 0;
  tbuf2.out = 0;
  tx_restart2 = 1;

   /*------------------------------------------------
  Setup serial port registers.
  ------------------------------------------------*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3,ENABLE);						//串口2，3时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);																//D5,D6	复用为串口2												
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);																//D8,D9	复用为串口3													
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);


	GPIO_InitStructure.GPIO_Pin = USART2_TX|USART2_RX|VNC_RXD|VNC_TXD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;																					//复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;																					//设置波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;													//设置模式，TX和RX都设置上	
	USART_InitStructure.USART_Parity = USART_Parity_No;																		//奇偶校验不开
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																//停止位1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;														//8字节
	USART_Init(USART2,&USART_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;																						//设置波特率
	USART_Init(USART3,&USART_InitStructure);
	/*使能串口2、串口3*/
	USART_Cmd(USART2,ENABLE);
	USART_Cmd(USART3,ENABLE);
	/*串口2中断配置*/
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);															//使能串口接收和发送中断
	USART_ITConfig(USART2,USART_IT_TC,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	USART_ClearITPendingBit(USART2,USART_IT_TC);
	USART_ClearFlag(USART2,USART_FLAG_TC);
	/*串口3中断配置*/
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);															//使能串口接收和发送中断
	USART_ITConfig(USART3,USART_IT_TC,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
//	USART_ClearITPendingBit(USART3,USART_IT_TC);
	USART_ClearFlag(USART3,USART_FLAG_TC);
}


/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int com_putchar2 (int c) 
{
 	struct buf_st *p = &tbuf;

	if (SIO_TBUFLEN >= TBUF_SIZE)
    	return (-1);

  	if (tx_restart) 
		{ 
    	tx_restart = 0;
			USART_SendData(USART2,c);
  	}
  	else {
    	p->buf [p->in & (TBUF_SIZE - 1)] = c;
    	p->in++;
  	}
  	return (0);
}

int com_getchar2 (void) {
	struct buf_st *p = &rbuf;

  	if (SIO_RBUFLEN == 0)
    	return (-1);
  	return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}


int com_put3ww(int c)
{
	int i;

	i=0;
	while(com_putchar3(c)==-1 && i++<100)
		os_dly_wait(100);


	if(i>=100) {
		return(1);
	}
	return(0);
} 


int com_putchar3 (unsigned char c) 
{
  	struct buf_st *p = &tbuf2;
		FLAG_TC = 0 ;
  	if (SIO_TBUFLEN2 >= TBUF_SIZE)
    	return (-1);

  	if (tx_restart2) 
		{ 
    	tx_restart2 = 0;
			USART_SendData(USART3,c);
			while(FLAG_TC == 0);
			FLAG_TC = 0;
  	}
  	else 
		{
    	p->buf [p->in & (TBUF_SIZE - 1)] = c;
    	p->in++;
  	}
  	return (0);
}

int com_getchar3 (void) 
{
  struct buf_st *p = &rbuf2;

  if (SIO_RBUFLEN2 == 0)
    return (-1);
  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}


void com_bufclr3(void)
{
	struct buf_st *p = &rbuf2;
	p->in=p->out;
}
	
void com_bufclr2(void)
{
	struct buf_st *p = &rbuf;
	p->in=p->out;
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

void putstr(char *str)
{
	if(USB_Comm==__TRUE) 
	{
		os_dly_wait(3);
    putusbstr(str);
		return;
	}
	
	while(*str!=0) 
	{
		delay_sio = 20000;
		while(com_putchar2(*str)==-1);
		
		while(delay_sio--);
		str++;
	}
}

void SendIntPrinter(char *str)
{
  while(*str!=0) 
	{
		com_putchar2(*str);
		str++;
		os_dly_wait(20);
	}

}

void SendRS232(char *str)
{
  while(*str!=0) 
	{
		delay_sio = 20000;
		while(com_putchar2(*str)==-1);
//		com_putchar2(*str);
		while(delay_sio--);
		str++;
	}

}


BOOL buf_Put (SBUF *buf, char c) 
{
  int next;

  next = (buf->in + 1) % SER_BUF_SIZE;        /* check if space is available */
  if (next == buf->out) {                                     /* buffer full */
	return (__FALSE);
  }

  buf->buf[buf->in] = c;
  buf->in = next;

  return (__TRUE);
}


/*----------------------------------------------------------------------------
  get a character from the buffer
 *---------------------------------------------------------------------------*/
BOOL buf_Get (SBUF *buf, char *c) 
{
  int next;

  if (buf->in == buf->out) 								/* check if data is available */
	{                   
    return (__FALSE);
  }

  next = (buf->out + 1) % SER_BUF_SIZE;

  *c = buf->buf[buf->out];
  buf->out = next;

  return (__TRUE);
}

