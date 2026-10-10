#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
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

