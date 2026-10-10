#include "INP.h"

extern uint16_t Inp1_Cnt; 							// 位置传感器获取的运转圈数
extern u8 Inp1_Rise; 										// 位置传感器上升沿标志

// 金属接近开关初始化函数
void INP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure; 					// Gavin, 2023-03-20 周一 11:16:46
	EXTI_InitTypeDef  EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 		// 使能GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4; 		// PE0,1,3,4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 							// 普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
  
  GPIO_Init(GPIOE, &GPIO_InitStructure); 										// 初始化GPIOE0,1,3,4
	
	
	// 外部中断初始化，Gavin，2023-03-20 周一 11:18:12
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 		// 使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0); // PE0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1); // PE1 连接到中断线1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3); // PE3 连接到中断线3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4); // PE4 连接到中断线4
	
	// 配置EXTI_Line0，1，3，4
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line3 | EXTI_Line4; // LINE0,1,3,4
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			// 中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 		// 上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 								// 使能LINE0
  EXTI_Init(&EXTI_InitStructure); 													// 配置
  
	//NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn | EXTI1_IRQn | EXTI3_IRQn | EXTI4_IRQn; // 外部中断0
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 				// 抢占优先级0
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
	//NVIC_Init(&NVIC_InitStructure); 																		// 配置
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // 外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 				// 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 																		// 配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; // 外部中断1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 				// 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 																		// 配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; // 外部中断3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 				// 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 																		// 配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; // 外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 				// 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 																		// 配置
	
}

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	if(INP4)
	{
		;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}

//外部中断1服务程序
void EXTI1_IRQHandler(void)
{
	if(INP3)
	{
		;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE1上的中断标志位 
}

//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	if(INP2)
	{
		;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	//delay_ms(10);	//消抖
	if(INP1)
	{
		Inp1_Cnt++;
		Inp1_Rise = 1;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line4);//清除LINE4上的中断标志位  
}

// 接近开关处理函数
// 返回按键值
// mode:0,不支持连续按;1,支持连续按;
// 0，没有任何按键按下
// 1，WKUP按下 WK_UP
// 注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 INP_Scan(u8 mode)
{
	u8 ucdat;
	static u8 inp_up = 1; 																		// 开关松开标志
	
	if(mode == 1)
		inp_up = 1; 																						// 支持连按
	
	if(inp_up&&(INP1==0||INP2==0||INP3==0||INP4==0))
	{
		inp_up = 0;
		//delay_ms(10);
		ucdat = 0;
		
		if(INP1==0) ucdat |= 1<<INP1_PRES;
		if(INP2==0) ucdat |= 1<<INP2_PRES;
		if(INP3==0) ucdat |= 1<<INP3_PRES;
		if(INP4==0) ucdat |= 1<<INP4_PRES;
		
		return ucdat;
	}
	else
	if(INP1==1&&INP2==1&&INP3==1&&INP4==1) 
		inp_up=1;
	
	return 0; 																								// 无按键按下
}
