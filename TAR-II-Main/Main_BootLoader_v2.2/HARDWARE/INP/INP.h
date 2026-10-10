#ifndef _INP_H
#define _INP_H

#include "sys.h"



//下面的方式是通过位带操作方式读取IO
//#define KEY0        PHin(3) //KEY0按键PH3
//#define KEY1        PHin(2) //KEY1按键PH2
//#define KEY2        PCin(13)//KEY2按键PC13
//#define WK_UP       PAin(0) //WKUP按键PA0


//下面的方式是通过直接操作HAL库函数方式读取IO
//#define KEY0        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)  //KEY0按键PE4
//#define KEY1        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)  //KEY1按键PE3
//#define KEY2        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2) 	//KEY2按键PE2
//#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define INP1 				HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) 			// InP1按键PE4
#define INP2 				HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) 			// InP2按键PE3
#define INP3 				HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1) 			// InP3按键PE1
#define INP4 				HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) 			// InP4按键PE0

#define INP1_PRES 						1
#define INP2_PRES 						2
#define INP3_PRES 						3
#define INP4_PRES 						4

void INP_Init(void);
u8 INP_Scan(u8 mode);

#endif
