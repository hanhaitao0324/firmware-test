#include "timer.h"
#include "led.h"


// 加热PWM驱动，TIM2_CH2, 32位计数器，输出口PB3 --------------------------------
TIM_HandleTypeDef 	TIM2_Handler;      	// 定时器句柄 
TIM_OC_InitTypeDef 	TIM2_CH2Handler; 		// 定时器2通道2句柄

//TIM2 PWM部分初始化 
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void TIM2CH2_PWM_Init(u32 arr,u16 psc)
{
	TIM2_Handler.Instance=TIM2; 															// 定时器2
	TIM2_Handler.Init.Prescaler=psc; 													// 定时器分频
	TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP; 				// 向上计数模式
	TIM2_Handler.Init.Period=arr; 														// 自动重装载值
	TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM2_Handler); 													// 初始化PWM
	
	TIM2_CH2Handler.OCMode=TIM_OCMODE_PWM1; 									// 模式选择PWM1
	TIM2_CH2Handler.Pulse=arr/2; 															// 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
	TIM2_CH2Handler.OCPolarity=TIM_OCPOLARITY_LOW; 						// 输出比较极性为低 
	HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH2Handler,TIM_CHANNEL_2); 			// 配置TIM2通道2
	
	HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_2); 					// 开启PWM通道2
}

//设置TIM2通道2的占空比
//compare:比较值
void TIM2CH2_SetCompare(u32 compare)
{
	TIM2->CCR2 = compare;
}

//设置TIM2通道2的输出频率
//frequency :频率值
void TIM2_SetFreq(u32 frequency)
{
	TIM2->ARR = frequency;
}

// 蜂鸣器PWM驱动，TIM3_CH4，16位计数器，输出口PC9 ------------------------------
TIM_HandleTypeDef 	TIM3_Handler;      	// 定时器句柄 
TIM_OC_InitTypeDef 	TIM3_CH4Handler; 		// 定时器3通道4句柄

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3CH4_PWM_Init(u16 arr,u16 psc)
{
	TIM3_Handler.Instance=TIM3; 															// 通用定时器3
	TIM3_Handler.Init.Prescaler=psc; 													// 分频系数
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP; 				// 向上计数器
	TIM3_Handler.Init.Period=arr; 														// 自动装载值
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 	// 时钟分频因子
	HAL_TIM_PWM_Init(&TIM3_Handler); 													// 初始化PWM
	
	TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; 									// 模式选择PWM1
	TIM3_CH4Handler.Pulse=arr/2; 															// 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
	TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; 						// 输出比较极性为低 
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_4); 			// 配置TIM3通道4
	
	HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_4); 					// 开启PWM通道4
}

//设置TIM3通道4的占空比
//compare:比较值
void TIM3CH4_SetCompare(u16 compare)
{
	TIM3->CCR4 = compare;
}

//设置TIMx通道x的输出频率
//frequency :频率值
void TIM3_SetFreq(u16 frequency)
{
	TIM3->ARR = frequency;
}

// 电机PWM驱动，TIM8_CH2, 16位计数器，输出口PC7 --------------------------------
TIM_HandleTypeDef 	TIM8_Handler;      	// 定时器句柄 
TIM_OC_InitTypeDef 	TIM8_CH2Handler; 		// 定时器8通道2句柄

//TIM8 PWM部分初始化 
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void TIM8CH2_PWM_Init(u16 arr,u16 psc)
{
	TIM8_Handler.Instance=TIM8; 															// 定时器8
	TIM8_Handler.Init.Prescaler=psc; 													// 定时器分频
	TIM8_Handler.Init.CounterMode=TIM_COUNTERMODE_UP; 				// 向上计数模式
	TIM8_Handler.Init.Period=arr; 														// 自动重装载值
	TIM8_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM8_Handler); 													// 初始化PWM
	
	TIM8_CH2Handler.OCMode=TIM_OCMODE_PWM1; 									// 模式选择PWM1
	TIM8_CH2Handler.Pulse=arr/2; 															// 设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
	TIM8_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; 						// 输出比较极性为低 
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH2Handler,TIM_CHANNEL_2); 			// 配置TIM8通道2
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_2); 					// 开启PWM通道2
}

void TIM8CH2_PwmOnf(u8 onf)
{
	if(onf)
		HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_2); 					// 开启PWM通道2
	else
		HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_2);
}

//设置TIM8通道2的占空比
//compare:比较值
void TIM8CH2_SetCompare(u16 compare)
{
	TIM8->CCR2=compare; 
}

//设置TIMx通道x的输出频率
//frequency :频率值
void TIM8_SetFreq(u16 frequency)
{
	TIM8->ARR = frequency;
}

//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if(htim->Instance==TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE(); 									// 使能定时器2
		__HAL_RCC_GPIOB_CLK_ENABLE(); 								// 开启GPIOB时钟
		
		GPIO_Initure.Pin=GPIO_PIN_3; 									// PB3
		GPIO_Initure.Mode=GPIO_MODE_AF_PP; 						// 复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP; 								// 上拉
		GPIO_Initure.Speed=GPIO_SPEED_HIGH; 					// 高速
		GPIO_Initure.Alternate= GPIO_AF1_TIM2; 				// PB2复用为TIM2_CH2
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
	else
	if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE(); 									// 使能定时器3
		__HAL_RCC_GPIOC_CLK_ENABLE(); 								// 开启GPIOC时钟
		
		GPIO_Initure.Pin=GPIO_PIN_9; 									// PC9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP; 						// 复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP; 								// 上拉
		GPIO_Initure.Speed=GPIO_SPEED_HIGH; 					// 高速
		GPIO_Initure.Alternate= GPIO_AF2_TIM3; 				// PC9复用为TIM3_CH4
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	}
	else
	if(htim->Instance==TIM8)
	{
		__HAL_RCC_TIM8_CLK_ENABLE(); 									// 使能定时器3
		__HAL_RCC_GPIOC_CLK_ENABLE(); 								// 开启GPIOC时钟
		
		GPIO_Initure.Pin=GPIO_PIN_7; 									// PC7
		GPIO_Initure.Mode=GPIO_MODE_AF_PP; 						// 复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP; 								// 上拉
		GPIO_Initure.Speed=GPIO_SPEED_HIGH; 					// 高速
		GPIO_Initure.Alternate= GPIO_AF3_TIM8; 				// PC7复用
		HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	}
}

/***************************************************************************
****************************************************************************
  下面是输入捕获相关源码实验相关函数源码
****************************************************************************
****************************************************************************/
// 电机编码器捕捉, TIM9_CH1, 16位计数器，输入口：PE5 -----------------------------------------------
TIM_HandleTypeDef TIM9_Handler;         //定时器9句柄

//定时器9通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)(TIM9到TIM14是16位的)
//psc：时钟预分频数
void TIM9CH1_Cap_Init(u16 arr,u16 psc)
{
	TIM_IC_InitTypeDef TIM9_CH1Config;
	
	TIM9_Handler.Instance=TIM9; 															// 通用定时器9
	TIM9_Handler.Init.Prescaler=psc; 													// 分频系数
	TIM9_Handler.Init.CounterMode=TIM_COUNTERMODE_UP; 				// 向上计数器
	TIM9_Handler.Init.Period=arr; 														// 自动装载值
	TIM9_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 	// 时钟分频银子
	HAL_TIM_IC_Init(&TIM9_Handler); 													// 初始化输入捕获时基参数
	
	TIM9_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING; 					// 上升沿捕获
	TIM9_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI; 			// 映射到TI1上
	TIM9_CH1Config.ICPrescaler=TIM_ICPSC_DIV1; 								// 配置输入分频，不分频
	TIM9_CH1Config.ICFilter=0; 																// 配置输入滤波器，不滤波
	HAL_TIM_IC_ConfigChannel(&TIM9_Handler,&TIM9_CH1Config,TIM_CHANNEL_1); 				// 配置TIM9通道1
	
	HAL_TIM_IC_Start_IT(&TIM9_Handler,TIM_CHANNEL_1); 				// 开启TIM9的捕获通道1，并且开启捕获中断
	__HAL_TIM_ENABLE_IT(&TIM9_Handler,TIM_IT_UPDATE); 				// 使能更新中断
}

//定时器9底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器9句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM9_CLK_ENABLE(); 															// 使能TIM9时钟
	__HAL_RCC_GPIOE_CLK_ENABLE(); 														// 开启GPIOE时钟
	
	GPIO_Initure.Pin=GPIO_PIN_5; 															// PE5
	GPIO_Initure.Mode=GPIO_MODE_AF_OD; 												// 复用开漏模式
	GPIO_Initure.Pull=GPIO_NOPULL; 														// 无拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 											// 高速
	GPIO_Initure.Alternate=GPIO_AF3_TIM9; 										// PE5复用为TIM9通道1
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn,2,0); 						// 设置中断优先级，抢占优先级2，子优先级0
	HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn); 									// 开启ITM9中断通道  
}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM9CH1_CAPTURE_STA=0; 							// 输入捕获状态		    				
u16	TIM9CH1_CAPTURE_VAL; 								// 输入捕获值(TIM2/TIM5是32位)(TIM9到TIM14是16位)

//定时器9中断服务函数
void TIM1_BRK_TIM9_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM9_Handler); 		// 定时器共用处理函数
}

// 定时器中断 ------------------------------------------------------------------
TIM_HandleTypeDef TIM4_Handler;      //定时器句柄 

//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器4挂在APB1上，时钟为HCLK/2)
void TIM4_Init(u16 arr,u16 psc)
{
	TIM4_Handler.Instance=TIM4; 															// 通用定时器4
	TIM4_Handler.Init.Prescaler=psc; 													// 分频系数
	TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP; 				// 向上计数器
	TIM4_Handler.Init.Period=arr; 														// 自动装载值
	TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 	// 时钟分频因子
	HAL_TIM_Base_Init(&TIM4_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM4_Handler); 										// 使能定时器3和定时器4更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE(); 														// 使能TIM4时钟
		HAL_NVIC_SetPriority(TIM4_IRQn,1,3); 										// 设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM4_IRQn); 													// 开启ITM4中断
	}
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);
}

// 中断函数 --------------------------------------------------------------------

//u16 MotExpCapVal = 35000;
u16 MotExpCapVal = 3000;
u16 MotorPwmVal = 450;
u8 MotSw = 1;

//定时器更新中断（计数溢出）中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 更新中断（溢出）发生时执行
{
	if(htim == (&TIM4_Handler))
	{
		LED1=!LED1; 																						// LED1反转
	}
	else
	if(htim == (&TIM9_Handler))
	{
		//MotorPwmVal++;
		if((TIM9CH1_CAPTURE_STA&0X80)==0) 											// 还未成功捕获
		{
			if(TIM9CH1_CAPTURE_STA&0X40) 													// 已经捕获到高电平了
			{
				/*
				if((TIM9CH1_CAPTURE_STA&0X3F)==0X3F) 								// 高电平太长了
				{
					TIM9CH1_CAPTURE_STA|=0X80; 												// 标记成功捕获了一次
					TIM9CH1_CAPTURE_VAL=0XFFFF;
				}
				else
					TIM9CH1_CAPTURE_STA++;
				*/
				
				TIM9CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM9_Handler,TIM_CHANNEL_1); 				// 获取当前的捕获值.
				if(TIM9CH1_CAPTURE_VAL > MotExpCapVal)
				{
					//MotorPwmVal--;
					if(MotSw)
					{
						if(MotorPwmVal < 499)
							MotorPwmVal++;
					}
				}
				
				if(TIM9CH1_CAPTURE_VAL < MotExpCapVal)
				{
					//MotorPwmVal++;
					if(MotSw)
					{
						if(MotorPwmVal > 1)
							MotorPwmVal--;
					}
				}
				
			}
		}
		
	}
}

//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) 		//捕获中断发生时执行
{
	if(!(TIM9CH1_CAPTURE_STA&0X80)) 			// 还未成功捕获
	{
		if(TIM9CH1_CAPTURE_STA&0X40) 				// 捕获到大于一次的上升沿（一个下降沿） 	
		{
			//TIM9CH1_CAPTURE_STA|=0X80; 				// 标记成功捕获到一次高电平脉宽
			TIM9CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM9_Handler,TIM_CHANNEL_1); 				// 获取当前的捕获值.
			if(TIM9CH1_CAPTURE_VAL > MotExpCapVal)
			{
				//MotorPwmVal--;
				if(MotSw)
				{
					if(MotorPwmVal < 499)
						MotorPwmVal++;
				}
			}
			
			if(TIM9CH1_CAPTURE_VAL < MotExpCapVal)
			{
				//MotorPwmVal++;
				if(MotSw)
				{
					if(MotorPwmVal > 1)
						MotorPwmVal--;
				}
			}
			
			__HAL_TIM_DISABLE(&TIM9_Handler); // 关闭定时器9
			__HAL_TIM_SET_COUNTER(&TIM9_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1); 				// 一定要先清除原来的设置！！
			//TIM_SET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING); 				// 定时器9通道1设置为下降沿捕获
			TIM_SET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING); 				// 配置TIM5通道1上升沿捕获
			__HAL_TIM_ENABLE(&TIM9_Handler); 	// 使能定时器9
		}
		else 																// 还未开始,第一次捕获上升沿
		{
			TIM9CH1_CAPTURE_STA=0; 						// 清空
			TIM9CH1_CAPTURE_VAL=0;
			TIM9CH1_CAPTURE_STA|=0X40; 				// 标记捕获到了上升沿
			__HAL_TIM_DISABLE(&TIM9_Handler); // 关闭定时器9
			__HAL_TIM_SET_COUNTER(&TIM9_Handler,0);
			TIM_RESET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1); 				// 一定要先清除原来的设置！！
			//TIM_SET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING); 				// 定时器9通道1设置为下降沿捕获
			TIM_SET_CAPTUREPOLARITY(&TIM9_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING); 				// 配置TIM5通道1上升沿捕获
			__HAL_TIM_ENABLE(&TIM9_Handler); 	// 使能定时器9
		}
	}
}
