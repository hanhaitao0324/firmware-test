#include "stm32f4xx_conf.h"
#include "timer.h"
#include "led.h"
#include "defines.h"
#include "beeper.h"
#include "pwm.h"
#include "motor.h"
#include "can.h"
#include "system.h"

volatile uint16_t PowOnDelay;
volatile TTimer timers[TIMER_NUM_TIMERS];

volatile uint16_t clockCounter;

volatile u8 upd_rxtover; 								// 升级程序时的接收超时判断

extern u32 lwip_localtime;
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime +=10; //加10
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}

u8 flash_flag;
u8 flash_cont;

u8 dtFlash;

//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!
void TIM4_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef 		TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef 						NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 			// 使能TIM4时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 							// 自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; 						// 定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 		// 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure); 				// 初始化TIM4
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 									// 允许定时器4更新中断
	TIM_Cmd(TIM4,ENABLE); 																		// 使能定时器4
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 						// 定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03; 			// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//------------------------------------------------------------------------------
// TIMER_Start - start a timer
//------------------------------------------------------------------------------
void TIMER_Start(uint8_t index, uint16_t timeout)
{
  timers[index].flgTimeOut = false;
  timers[index].value = timeout;
  timers[index].flgRunning = true;
}

//------------------------------------------------------------------------------
// TIMER_Stop - stop a timer
//------------------------------------------------------------------------------
void TIMER_Stop(uint8_t index)
{
  timers[index].flgTimeOut = false;
  timers[index].flgRunning = false;
}
  
//------------------------------------------------------------------------------
// TIMER_TimeOut - check if timeout
//------------------------------------------------------------------------------
uint8_t TIMER_TimeOut(uint8_t index)
{
  return timers[index].flgTimeOut;
}

uint16_t msec;

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	uint8_t i;
  //static uint32_t counter1OneSec=0, counter2OneSec= 0, counter_Lic=0;
	//static unsigned int reduce_counter, lastPress;
	//uint16_t capture = 0;
	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) 							// 溢出中断
	{
    for (i=0; i<TIMER_NUM_TIMERS; i++)
		{
			if (timers[i].flgRunning)
			{
				if (timers[i].value)
				{
					timers[i].value--;
				}
				else
				{
					timers[i].flgTimeOut = true;
        }
			}
    }
		
		msec++;
		if(msec < 1000)
		{
			if(msec < 500)
			{
				flash_flag = 0; 									// 闪烁标志
				
			}
			else
			{
				if(!flash_flag)
				{
					flash_flag = 1;
					flash_cont++; 									// 闪烁计数器
				}
			}
		}
		else 																// 秒定时
		{
			msec = 0;
			
		}
		
		if(msec%10) 												// 10mS定时器
		{
			lwip_localtime +=10; 							// 加10
		}
		
		if(PowOnDelay != 0)
			PowOnDelay--;
		
		if(upd_rxtover != 0) 								// 升级程序时的接收超时判断
			upd_rxtover--;
		
		if(msec < 500)
			GPIO_SetBits(GPIOB,GPIO_Pin_0); 							// 灯灭
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_0); 							// 灯亮
		
		if(dtFlash != 0)
			dtFlash--;
		
		// CAN发送超时 -------------------------------------------------------------
		if(CanTx_TimeOut != 0)
			CanTx_TimeOut--;
		
		// 压电蜂鸣器 --------------------------------------------------------------
		//*
		if (_beep_duration) 																												// 播放持续时间维护
			_beep_duration--;
		else 																																				// 播放时间到？
		{
			if (_tones_playing) 																											// 在播放中？
			{
				_tones++; 																															// Currently playing tones, take next tone
				if (_tones->frequency == 0 && _tones->duration == 0) 
				{
					BEEPER_Disable(); 																										// Last tone in sequence
					_tones_playing = false;
					_tones = NULL;
				} 
				else 																																		// 播放下一个音调
				{
					if (_tones->frequency == 0) 
					{
						TIM3CH4_SetCompare(0); 																							// 占空比为0
						//BEEPER_TIM->ARR = SystemCoreClock / (100 * BEEPER_TIM->PSC) - 1; 		// Silence period
						//BEEPER_TIM->CCR2 = 0; 																							// 0% duty cycle
						//_beep_duration = _tones->duration + 1;
						_beep_duration = _tones->duration * 10;
					} 
					else 
					{
						BEEPER_Enable(_tones->frequency,_tones->duration); 									// Play next tone in sequence
					}
				}
			}
			else 																																			// 播放完毕
			{
				BEEPER_Disable();
			}
		}
		//*/
		
		// 电机控制 ----------------------------------------------------------------
		MOTOR_IntHandler();
  }
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 								// 清除中断标志位
}

