#include "timer.h"
#include "led.h"
#include "GUI.h"

extern __IO int32_t OS_TimeMS;

TIM_HandleTypeDef TIM3_Handler; 				// 定时器句柄 
TIM_HandleTypeDef TIM4_Handler; 				// 定时器句柄 

TIM_HandleTypeDef TIM5_Handler; 				// 定时器句柄 

u16 msec; 															// 毫秒计数器
u16 BeepTime; 													// 蜂鸣器鸣响时间，ms
u16 tp_ktime; 													// 触摸持续时间，ms
u16 dtFlash; 														// 日期时间刷新间隔，ms
u8 tp_onf; 															// 有触摸标志

u16 CanRx_TimeOut; 											// CAN接收超时

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{
	TIM3_Handler.Instance=TIM3;                          //通用定时器3
	TIM3_Handler.Init.Prescaler=psc;                     //分频
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
	TIM3_Handler.Init.Period=arr;                        //自动装载值
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM3_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3中断   
}

//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!(定时器4挂在APB1上，时钟为HCLK/2)
void TIM4_Init(u16 arr,u16 psc)
{
	TIM4_Handler.Instance=TIM4;                          //通用定时器4
	TIM4_Handler.Init.Prescaler=psc;                     //分频
	TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
	TIM4_Handler.Init.Period=arr;                        //自动装载值
	TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM4_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器4和定时器4中断   
}

//通用定时器5中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM5_Init(u16 arr,u16 psc)
{  
	TIM5_Handler.Instance=TIM5;                          //通用定时器5
	TIM5_Handler.Init.Prescaler=psc;                     //分频系数
	TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
	TIM5_Handler.Init.Period=arr;                        //自动装载值
	TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
	HAL_TIM_Base_Init(&TIM5_Handler);

	HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器5和定时器5更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE(); 									// 使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3); 					// 设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn); 								// 开启ITM3中断  
	}
	else
	if(htim->Instance == TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE(); 									// 使能TIM4时钟
		HAL_NVIC_SetPriority(TIM4_IRQn,2,3); 					// 设置中断优先级，抢占优先级2，子优先级0
		HAL_NVIC_EnableIRQ(TIM4_IRQn); 								// 开启ITM4中断  
	}
	else
	if(htim->Instance == TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE(); 									// 使能TIM3时钟
		HAL_NVIC_SetPriority(TIM5_IRQn,1,3); 					// 设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM5_IRQn); 								// 开启ITM5中断   
	}
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);
}

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
		OS_TimeMS++;
	}
	else
	if(htim==(&TIM4_Handler))
	{
		GUI_TOUCH_Exec();
	}
	else
	if(htim==(&TIM5_Handler))
	{
		if(msec < 1000)
		{
			msec++;
		}
		else
		{
			msec = 0;
			
			LED1=!LED1;        								// LED1反转
		}
		
		if(tp_onf) 													// 有触摸标志
		{
			if(tp_ktime < 60000) 							// 上限约束
				tp_ktime++; 										// 触摸计时
		}
		
		if(dtFlash != 0) 										// 日期时间刷新间隔
			dtFlash--;
		
		if(CanRx_TimeOut != 0) 							// can接收超时
			CanRx_TimeOut--;
		
		if(BeepTime != 0)
		{
			BeepTime--;
			BEEP = 0; 												// 鸣响
		}
		else
		{
			BEEP = 1; 												// 停止
		}
	}
}





