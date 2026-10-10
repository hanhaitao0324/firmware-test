#include "INP.h"

// 金属接近开关初始化函数
void INP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 		// 使能GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4; 		// PE0,1,3,4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 							// 普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
  
  GPIO_Init(GPIOE, &GPIO_InitStructure); 										// 初始化GPIOE0,1,3,4
	
} 

// 接近开关处理函数
// 返回按键值
// mode:0,不支持连续按;1,支持连续按;
// 0，没有任何按键按下
// 1，WKUP按下 WK_UP
// 注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 INP_Scan(u8 mode)
{
	u8 ucdat;
	static u8 inp_up = 1; 																		// 开关松开标志
	
	if(mode == 1)
		inp_up = 1; 																						// 支持连按
	
	if(inp_up&&(INP1==0||INP2==0||INP3==0||INP4==0))
	{
		inp_up = 0;
		//delay_ms(10);
		ucdat = 0;
		
		if(INP1==0) ucdat |= 1<<INP1_PRES;
		if(INP2==0) ucdat |= 1<<INP2_PRES;
		if(INP3==0) ucdat |= 1<<INP3_PRES;
		if(INP4==0) ucdat |= 1<<INP4_PRES;
		
		return ucdat;
	}
	else
	if(INP1==1&&INP2==1&&INP3==1&&INP4==1) 
		inp_up=1;
	
	return 0; 																								// 无按键按下
}
