#include "pwm.h"
#include "led.h"
#include "usart.h"

// 加热PWM驱动，TIM2_CH2, 32位计数器，输出口PB3 ---------------------------------
// arr：自动重装值
// psc：时钟预分频数
void TIM2CH2_PWM_Init(u32 arr, u32 psc)
{
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 				TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 			// TIM2时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 		// 使能PORTB时钟	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2); 		// GPIOB3复用为定时器2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 								// GPIOB3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure); 										// 初始化PB3
	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 								// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; 									// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); 						// 初始化定时器2
	
	//初始化TIM2 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 			// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 	// 输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 									// 根据T指定的参数初始化外设TIM2 OC2

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 				// 使能TIM2在CCR2上的预装载寄存器
	
  TIM_ARRPreloadConfig(TIM2, ENABLE); 											// ARPE使能 
	
	TIM_Cmd(TIM2, ENABLE); 																		// 使能TIM2
}

// 设置TIM2通道2的占空比
// compare:比较值
void TIM2CH2_SetCompare(u32 compare)
{
	TIM2->CCR2 = compare;
}

// 设置TIM2通道2的输出频率
// frequency :频率值
void TIM2_SetFreq(u32 frequency)
{
	TIM2->ARR = frequency;
}

// 蜂鸣器PWM驱动，TIM3_CH4，16位计数器，输出口PC9 ------------------------------
//arr：自动重装值
//psc：时钟预分频数
/*void TIM3CH4_PWM_Init(u16 arr,u16 psc)
{
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef 						GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 					TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 			// TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 		// 使能PORTC时钟	
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3); 		// GPIOC9复用为定时器3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 								// GPIOC9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 										// 初始化PC9
	  
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 								// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; 									// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); 						// 初始化定时器3
	
	//初始化TIM3 Channel4 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 			// 比较输出使能
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 	// 输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	// 输出极性:TIM输出比较极性低
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); 									// 根据T指定的参数初始化外设TIM3OC4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); 				// 使能TIM3在CCR4上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM3, ENABLE); 											// ARPE使能 
	
	TIM_Cmd(TIM3, ENABLE); 																		// 使能TIM3
}  

// 设置TIM3通道4的占空比
// compare:比较值
void TIM3CH4_SetCompare(u16 compare)
{
	float com;
	
	com = TIM3->ARR;
	com = (com*compare)/100;
	compare = (u16)com;
	
	TIM3->CCR4 = compare;
}

// 设置TIM3通道4的输出频率
// frequency :频率值
void TIM3_SetFreq(u16 frequency)
{
	float feq = frequency;
	
	feq = 1000000/feq;
	frequency = (u16)feq;
	
	TIM3->ARR = frequency;
}*/

// 蜂鸣器PWM驱动，TIM5_CH1，32位计数器，输出口PA0 ------------------------------
//arr：自动重装值
//psc：时钟预分频数
void TIM5CH1_PWM_Init(u32 arr,u16 psc)
{
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef 						GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 					TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 			// TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 		// 使能PORTA时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); 		// GPIOA0复用为定时器5
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 								// GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 										// 初始化PA0
	  
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 								// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; 									// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure); 						// 初始化定时器5
	
	//初始化TIM5 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 			// 比较输出使能
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 	// 输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	// 输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM5, &TIM_OCInitStructure); 									// 根据T指定的参数初始化外设TIM5OC1

	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable); 				// 使能TIM5在CCR1上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM5, ENABLE); 											// ARPE使能 
	
	TIM_Cmd(TIM5, ENABLE); 																		// 使能TIM5
}  

// 设置TIM5通道1的占空比
// compare:比较值
void TIM5CH1_SetCompare(u16 compare)
{
	float com;
	
	com = TIM5->ARR;
	com = (com*compare)/100;
	compare = (u16)com;
	
	TIM5->CCR1 = compare;
}

// 设置TIM5通道1的输出频率
// frequency :频率值
void TIM5_SetFreq(u16 frequency)
{
	float feq = frequency;
	
	feq = 1000000/feq;
	frequency = (u16)feq;
	
	TIM5->ARR = frequency;
}

// 电机PWM驱动，TIM8_CH2, 16位计数器，输出口PC7 --------------------------------
// arr：自动重装值
// psc：时钟预分频数
void TIM8CH2_PWM_Init(u16 arr,u16 psc)
{
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef 						GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 		TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 					TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); 			// TIM8时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 		// 使能PORTC时钟	
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8); 		// GPIOC7复用为定时器8
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 								// GPIOC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 										// 初始化PC7
	  
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 								// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; 									// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure); 						// 初始化定时器8
	
	//初始化TIM8 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 			// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	// 输出极性:TIM输出比较极性低
	
	TIM_OC2Init(TIM8, &TIM_OCInitStructure); 									// 根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable); 				// 使能TIM8在CCR2上的预装载寄存器
	
  TIM_ARRPreloadConfig(TIM8,ENABLE); 												// ARPE使能 
	
	TIM_Cmd(TIM8, ENABLE); 																		// 使能TIM8
	
	TIM8CH2_SetCompare(0);																		// 修改比较值，修改占空比
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
}  

// 设置TIM8通道2的占空比
// compare:比较值
void TIM8CH2_SetCompare(u16 compare)
{
	float com;
	
	com = TIM8->ARR;
	com = (com*compare)/100;
	compare = (u16)com;
	
	TIM8->CCR2 = compare;
}

// 设置TIM8通道4的输出频率
// frequency :频率值
void TIM8_SetFreq(u16 frequency)
{
	float feq = frequency;
	
	feq = 1000000/feq;
	frequency = (u16)feq;
	
	TIM8->ARR = frequency;
}











