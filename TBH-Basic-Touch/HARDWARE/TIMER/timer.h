#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

void TIM3_Init(u16 arr,u16 psc);
void TIM4_Init(u16 arr,u16 psc);

extern TIM_HandleTypeDef TIM5_Handler;      //定时器句柄 

extern u16 BeepTime; 										// 蜂鸣器鸣响时间，ms
extern u16 tp_ktime; 										// 触摸持续时间，ms
extern u16 dtFlash; 										// 日期时间刷新间隔，ms
extern u8 tp_onf; 											// 有触摸标志

extern u16 CanRx_TimeOut; 							// CAN接收超时

void TIM5_Init(u16 arr,u16 psc);
#endif

