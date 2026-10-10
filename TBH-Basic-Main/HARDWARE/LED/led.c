#include "led.h" 
#include "stm32f4xx_conf.h"

//初始化PE0和PE1为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 		// 使能GPIOE时钟

  // GPIOF9,F10初始化设置 ----------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 						// 普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure); 										// 初始化
	
	GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1); 							// GPIOB0,B1设置高，灯灭
	//LED0_ON; 																					// 亮灯
	//LED1_ON;
}






