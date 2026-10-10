#include "INP.h"
#include "delay.h"



//按键初始化函数
void INP_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE(); 				// 开启GPIOE时钟
	
	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4; 				// PE0,1,3,4
	GPIO_Initure.Mode=GPIO_MODE_INPUT; 		// 输入
	GPIO_Initure.Pull=GPIO_PULLUP; 				//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 	// 高速
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 INP_Scan(u8 mode)
{
	static u8 inp_up=1; 									// 开关松开标志
	
	if(mode==1)
		inp_up=1; 													// 支持连按
	if(inp_up&&(INP1==0||INP2==0||INP3==0||INP4==0))
	{
		delay_ms(10);
		inp_up=0;
		if(INP1==0)       return INP1_PRES;
		else if(INP2==0)  return INP2_PRES;
		else if(INP3==0)  return INP3_PRES;
		else if(INP4==0) 	return INP4_PRES;
	}
	else
	if(INP1==1&&INP2==1&&INP3==1&&INP4==1) 
		inp_up=1;
	
	return 0; 														// 无按键按下
}
