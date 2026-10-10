#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"



extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

// 加热PWM驱动，TIM2_CH2, 32位计数器，输出口PB3 --------------------------------
void TIM2CH2_PWM_Init(u32 arr,u16 psc);
void TIM2CH2_SetCompare(u32 compare);
void TIM2_SetFreq(u32 frequency);

// 蜂鸣器PWM驱动，TIM3_CH4，16位计数器，输出口PC9 ------------------------------
void TIM3CH4_PWM_Init(u16 arr,u16 psc);
void TIM3CH4_SetCompare(u16 compare);
void TIM3_SetFreq(u16 frequency);

// 电机PWM驱动，TIM8_CH2, 16位计数器，输出口PC7 --------------------------------
void TIM8CH2_PWM_Init(u16 arr,u16 psc);
void TIM8CH2_SetCompare(u16 compare);
void TIM8_SetFreq(u16 frequency);
void TIM8CH2_PwmOnf(u8 onf);

// 电机编码器捕捉, TIM9_CH1, 16位计数器，输入口：PE5 ---------------------------
void TIM9CH1_Cap_Init(u16 arr,u16 psc);

// 定时器中断 ------------------------------------------------------------------
void TIM4_Init(u16 arr,u16 psc);

#endif

