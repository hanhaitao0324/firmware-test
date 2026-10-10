#ifndef _INP_H
#define _INP_H

#include "stm32f4xx_conf.h"

/*下面的方式是通过直接操作库函数方式读取IO*/
#define INP1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) 		// InP1按键PE4
#define INP2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)			// InP2按键PE3
#define INP3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) 		// InP3按键PE1
#define INP4 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)			// InP4按键PE0

#define INP1_PRES 						0
#define INP2_PRES 						1
#define INP3_PRES 						2
#define INP4_PRES 						3

void INP_Init(void); 										// IO初始化
u8 INP_Scan(u8 mode); 									// 输入开关扫描函数

#endif
