#ifndef _INP_H
#define _INP_H

#include "stm32f4xx_conf.h"
#include "led.h" 




/*下面的方式是通过直接操作库函数方式读取IO*/
#define INP1 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) 		// InP1按键PB0,读到0开关闭合，读到1开关断开
#define INP2 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)			// InP2按键PB1

#define INP1_PRES 						0
#define INP2_PRES 						1

void INP_Init(void); 										// IO初始化
u8 INP_Scan(u8 mode); 									// 输入开关扫描函数

#endif
