#include "motor.h"



//初始化PB1为输出.并使能时钟	    
//LED IO初始化
void MOTOR_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟

	GPIO_Initure.Pin=GPIO_PIN_8; 						//PA8
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);	//PB0置1，默认初始化后灯灭
	//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1置1，默认初始化后灯灭
}
