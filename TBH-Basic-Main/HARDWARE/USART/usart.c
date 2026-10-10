#include "osObjects.h"                      // RTOS object definitions
#include "usart.h"
#include "timer.h"
#include "pins.h"
#include "dma.h"
#include "vinculum.h"
#include <string.h>

extern osThreadId CommTask;													/* assigned task id of task:  CommTask */

// Receive Buffer	an RS232
static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

char RS232_RxBuffer[RS232_BUFFER_SIZE];
char RS232_TxBuffer[RS232_BUFFER_SIZE];

uint8_t RS232_IHead;

volatile bool RS232_SendBusy = false;

u8 RS232_RxFlag = 0;

// RS485
char RS485_RxBuffer[64];
char RS485_TxBuffer[64];

uint8_t RS485_IHead;
volatile bool RS485_SendBusy = false;

u8 RS485_RxFlag = 0;

// MC uart an Vinculum-II
struct buf_st rbuf3 = { 0, 0, };
#define SIO_RBUFLEN3 ((unsigned short)(rbuf3.in - rbuf3.out))

// 电子秤，串口6
//static struct buf_st rbuf6 = { 0, 0, };
//#define SIO_RBUFLEN6 ((unsigned short)(rbuf6.in - rbuf6.out))

char Balance_RxBuffer[64];
char Balance_TxBuffer[64];

uint8_t Balance_IHead;

volatile bool Balance_SendBusy = false;

u8 Balance_RxFlag = 0;

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

// 初始化IO 串口1 
// 程序升级，串口指令数据交互
void USART1_Init(void)
{
	// GPIO端口设置
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 			// 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 		// 使能USART1时钟
	
	// 串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 	// GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 	// GPIOA10复用为USART1

	// USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 	// GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 										// 初始化PA9与PA10

	// USART1 波特率设置
	com_baudrate(USART1, UART1_BAUD, NO_FLOW_CONTROL);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 						// 开启相关中断
	
	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 				// 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 				// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														// 根据指定的参数初始化VIC寄存器、
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
	//DMA_InitTypeDef DMA_InitStructure;
	
	TIMER_Start(TIMER_RS232, TO_200MSEC);
	while (USART1_Tx_DMA_Channel->NDTR) //CNDTR)
  { 
    if (TIMER_TimeOut(TIMER_RS232))
      break; 
 //   osDelay(1);
  }
	
  if (len)
  {
		memcpy(RS232_TxBuffer, buf, len);
	  RS232_SendBusy = true;
		
	  //DMA2,STEAM7,CH4,外设为串口1，存储器为SendBuff,长度为：SEND_BUF_SIZE.
		MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(uint32_t)&USART1->DR,(uint32_t)RS232_TxBuffer,len);
		USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  // 使能串口1的DMA发送
		MYDMA_Enable(DMA2_Stream7,len);     					// 开始一次DMA传送！
		
		// USART1 TX DMA1 Channel (triggered by USART1 Tx event) Config
		/*DMA_DeInit(USART1_Tx_DMA_Channel);  
		DMA_StructInit(&DMA_InitStructure);
		
		DMA_InitStructure.DMA_Channel 						= DMA_Channel_4; 					// 通道选择
		DMA_InitStructure.DMA_PeripheralBaseAddr 	= USART1_DR_Base;
		DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)RS232_TxBuffer;
		DMA_InitStructure.DMA_DIR 								= DMA_DIR_MemoryToPeripheral;
		DMA_InitStructure.DMA_BufferSize 					= len;
		DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority 						= DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode 						= DMA_FIFOMode_Disable;         
		DMA_InitStructure.DMA_FIFOThreshold 			= DMA_FIFOThreshold_Full;
		DMA_InitStructure.DMA_MemoryBurst 				= DMA_MemoryBurst_Single; 				// 存储器突发单次传输
		DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single; 		// 外设突发单次传输
		
		DMA_Init(USART1_Tx_DMA_Channel, &DMA_InitStructure);
		//DMA_ITConfig (DMA1_Channel4, DMA_IT_TC, ENABLE);
	  DMA_Cmd(USART1_Tx_DMA_Channel, ENABLE);
	  */
  }
}

//------------------------------------------------------------------------------
// RS232_SendChar - send a char to RS232
//------------------------------------------------------------------------------
void RS232_SendChar(char data)
{        
	TIMER_Start(TIMER_RS232, TO_200MSEC);	

	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) 				// wait until transmit buffer is empty
	{
		if(TIMER_TimeOut(TIMER_RS232))
			break;
	};
	
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
int com_getchar1 (void)
{
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);
	
  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

void USART1_IRQHandler(void) 																// 串口1中断服务程序
{
	char c;
	struct buf_st *p;
	
  if (USART_GetITStatus(USART1, USART_IT_RXNE)) 
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		c = USART_ReceiveData(USART1);
		
		//com_putchar2(c);//rein f眉r vinculum communication 眉ber terminal und dem rs232 port
		//-----------------------------
		// Vinculum rxbuffer
		p = &rbuf;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) 
		{
			p->buf [p->in & (RBUF_SIZE-1)] = c;
			p->in++;
		}
		
		//-----------------------------
    if (c==CHAR_NL) 										// 0x0A,
    	return;
		
    RS232_RxBuffer[RS232_IHead++] = c;
		
    if (c==CHAR_ESC) 										// 0x1B,
    {
      RS232_IHead = 0;
      return;
    }
		
    if (c==CHAR_CR) 										// 0x0D,
    {
//-----------------------------
// Vinculum rxbuffer
//			com_bufclr1();
//-----------------------------

      RS232_RxBuffer[RS232_IHead-1] = 0;										// clear CHAR_CR from RS232_RxBuffer
			RS232_IHead = 0;
	    osSignalSet(CommTask, 0x1); 			// raus f眉r vinculum communication 眉ber terminal und dem rs232 port
	    RS232_RxFlag = 1;
		}
  }

} 

void com_bufclr1(void)
{
	struct buf_st *p = &rbuf;
	p->in=p->out;
}


// 初始化IO 串口2 -----------------------------------------------------------------------------------
// RS485通信
void USART2_Init(void)
{
	// GPIO端口设置
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 			// 使能GPIOD时钟
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 		// 使能USART2时钟
	
	// 串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART1); 	// GPIOD5复用为USART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART1); 	// GPIOD6复用为USART2

	// USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; 		// GPIOC6与GPIOC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 										// 初始化PD5与PD6

	// USART1 波特率设置
	com_baudrate(USART2, UART2_BAUD, NO_FLOW_CONTROL);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 						// 开启相关中断
	
	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 				// 串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 				// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														// 根据指定的参数初始化VIC寄存器、
}

void USART2_IRQHandler(void) 																// 串口2中断服务程序
{
	char c;
	
  if (USART_GetITStatus(USART2, USART_IT_RXNE)) 
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		c = USART_ReceiveData(USART2);
		
		if (c==CHAR_NL) 
    	return;
		
    RS485_RxBuffer[RS485_IHead++] = c;
		
    if (c==CHAR_ESC)
    {
      RS485_IHead = 0;
      return;
    }
		
    if (c==CHAR_CR)
    {
    	RS485_RxBuffer[RS485_IHead-1] = 0;											//clear CHAR_CR from RS232_RxBuffer
			RS485_IHead = 0;
	    osSignalSet(CommTask, 0x1);//raus f眉r vinculum communication 眉ber terminal und dem rs232 port
	    RS485_RxFlag = 1;
		}
  }
} 

void RS485_SendData(char *buf, int len)
{
	//DMA_InitTypeDef DMA_InitStructure;
	
	TIMER_Start(TIMER_RS485, TO_200MSEC);
	while (USART2_Tx_DMA_Channel->NDTR) //CNDTR)
  { 
    if (TIMER_TimeOut(TIMER_RS485))
      break; 
 //   osDelay(1);
  }
	
  if (len)
  {
		memcpy(RS485_TxBuffer, buf, len);
	  RS485_SendBusy = true;
		
	  //DMA1,STEAM6,CH4,外设为串口2，存储器为SendBuff,长度为：SEND_BUF_SIZE.
		MYDMA_Config(DMA1_Stream6,DMA_Channel_4,(uint32_t)&USART2->DR,(uint32_t)RS485_TxBuffer,len);
		USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  // 使能串口6的DMA发送
		MYDMA_Enable(DMA1_Stream6,len);     					// 开始一次DMA传送！
		
  }
}

void RS485_SendChar(char data)
{        
	TIMER_Start(TIMER_RS485, TO_200MSEC);	

	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) 				// wait until transmit buffer is empty
	{
		if(TIMER_TimeOut(TIMER_RS485))
			break;
	};
	
	USART_SendData(USART2, data);	
}

void RS485_SendString(char *buffer)
{
  int len = strlen(buffer);
  
  RS485_SendData(buffer, len);
}

// 初始化IO 串口3 ----------------------------------------------------------------------------------
// VNC II接口
void USART3_Init(void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 			// 使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); 		// 使能USART3时钟

	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); 	// GPIOD8复用为USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); 	// GPIOD9复用为USART3	
	
	//Set USART3 Tx (PD.8) as AF push-pull
	GPIO_InitStructure.GPIO_Pin = VINC_RXD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 				// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Set USART3 RTS (PD.12) as output push-pull
	GPIO_InitStructure.GPIO_Pin = RTS3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Set USART3 CTS (PD.11) as input Pullup
	GPIO_InitStructure.GPIO_Pin = CTS3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Set USART3 Rx (PD.9) as input Pullup
	GPIO_InitStructure.GPIO_Pin = VINC_TXD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	com_baudrate(USART3, UART3_BAUD, HARDWARE_FLOW_CONTROL); 	// Vinculum-II Communication
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 						// 开启中断 
	
	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 	// 抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 				// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure); 													// 根据指定的参数初始化VIC寄存器
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
		
	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 						// 开启中断 

}

void USART3_IRQHandler(void) 																// 串口3中断服务程序
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 		// 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		unsigned char ch; 
		struct buf_st *p;
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		ch = USART_ReceiveData(USART3); 												// 读取接收到的数据
		//RS232_SendData((char *)&ch,1);//rein für vinculum communication über terminal und dem rs232 port
		p = &rbuf3;
		if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) 
		{
			p->buf [p->in & (RBUF_SIZE-1)] = ch;
			p->in++;
		}
  } 
}

/*
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF); 										// 此次发送数据的长度
	for(j=0;j<i;j++) 																					// 循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); 					// 等待上次传输完成 
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 			// 发送数据到串口3 
	}
}
*/

/*******************************************************************************
* Function Name  : com_putchar3
* Description    : send byte via uart3 to Vinculum-II
* Input          : uint16_t data
* Output         : uint16_t data
* Return         : uint16_t
*******************************************************************************/
uint16_t com_putchar3(uint16_t data)
{ 
	TIMER_Start(TIMER_Vinculum, TO_500MSEC);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) 
	{
		if(TIMER_TimeOut(TIMER_Vinculum))
			return 1; 
	};
	
	USART_SendData(USART3, data);
  return 0;
}

/*******************************************************************************
* Function Name  : com_getchar3
* Description    : receive byte via uart3 from Vinculum-II
* Input          : uint16_t data
* Output         : uint16_t data
* Return         : int 
*******************************************************************************/
int com_getchar3 (int *ret)
{
	struct buf_st *p = &rbuf3;
	
	if (SIO_RBUFLEN3 == 0)
		return (-1);
	
	*ret = p->buf [(p->out++) & (RBUF_SIZE - 1)];
	return 0;
}

void com_bufclr3(void)
{
	struct buf_st *p = &rbuf3;
	p->in=p->out;
}

void com_put3ww(int c)
{
	while(com_putchar3(c) > 0)
		osThreadYield();
} 

// 初始化IO 串口6 ----------------------------------------------------------------------------------
// 电子秤接口

// 1.初始化串口6
// 电子秤
void USART6_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 			// 使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); 		// 使能USART6时钟
	
	//串口6对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); 	// GPIOC6复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); 	// GPIOC7复用为USART6
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 		// GPIOC6与GPIOC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 				// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 										// 初始化PC6，PC7
	
	/*  配置串口硬件参数 */
	com_baudrate(USART6, UART6_BAUD, NO_FLOW_CONTROL);
	//USART_ClearFlag(USART6, USART_FLAG_RXNE);
	//USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); 						// 使能接收中断
	
	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn; 				// 串口6中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 				// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														// 根据指定的参数初始化VIC寄存器、
}

void Balance_SendData(char *buf, int len)
{
	//DMA_InitTypeDef DMA_InitStructure;
	
	TIMER_Start(TIMER_BALANCE, TO_200MSEC);
	while (USART6_Tx_DMA_Channel->NDTR) //CNDTR)
  { 
    if (TIMER_TimeOut(TIMER_BALANCE))
      break; 
 //   osDelay(1);
  }
	
  if (len)
  {
		memcpy(Balance_TxBuffer, buf, len);
	  Balance_SendBusy = true;
		
	  //DMA2,STEAM6,CH5,外设为串口6，存储器为SendBuff,长度为：SEND_BUF_SIZE.
		MYDMA_Config(DMA2_Stream6,DMA_Channel_5,(uint32_t)&USART6->DR,(uint32_t)Balance_TxBuffer,len);
		USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);  // 使能串口6的DMA发送
		MYDMA_Enable(DMA2_Stream6,len);     					// 开始一次DMA传送！
		
  }
}

void Balance_SendChar(char data)
{        
	TIMER_Start(TIMER_BALANCE, TO_200MSEC);	

	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET) 				// wait until transmit buffer is empty
	{
		if(TIMER_TimeOut(TIMER_BALANCE))
			break;
	};
	
	USART_SendData(USART6, data);	
}

void Balance_SendString(char *buffer)
{
  int len = strlen(buffer);
  
  Balance_SendData(buffer, len);
}

//串口6 中断函数
void USART6_IRQHandler(void)
{
	char c;
	
  if (USART_GetITStatus(USART6, USART_IT_RXNE)) 
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		
		c = USART_ReceiveData(USART6);
		
		//-----------------------------
    if (c==CHAR_NL) 
    	return;
		
    Balance_RxBuffer[Balance_IHead++] = c;
		
    if (c==CHAR_ESC)
    {
      Balance_IHead = 0;
      return;
    }
		
    if (c==CHAR_CR)
    {
    	Balance_RxBuffer[Balance_IHead-1] = 0;
			Balance_IHead = 0;
	    osSignalSet(CommTask, 0x1);
	    Balance_RxFlag = 1;
		}
  }
}



