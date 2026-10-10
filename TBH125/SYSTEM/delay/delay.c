#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////  

//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 
 
//static u8  fac_us=0;//us延时倍乘数			   
//static u16 fac_ms=0;//ms延时倍乘数,在ucos下,代表每个节拍的ms数

#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
//systick中断服务函数,使用ucos时用到
//void SysTick_Handler(void)
//{				   
////	OSIntEnter();		//进入中断
////    OSTimeTick();       //调用ucos的时钟服务程序               
////    OSIntExit();        //触发任务切换软中断
//}
#endif
			   
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(u8 SYSCLK)
{
	int i,j;
	for(i=0;i<SYSCLK;i++) {
		for(j=0;j<42;j++);
	}
}								    

#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
//延时nus
//nus:要延时的us数.		    								   
void delay_us(u32 nus)
{		
		int i,j;
	for(i=0;i<nus;i++) {
		for(j=0;j<42;j++);
	}				    
}
//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms)
{	
		int i,j;
	for(i=0;i<nms;i++) {
		for(j=0;j<1000;j++);
	}
}
#else  //不用ucos时
//延时nus
//nus为要延时的us数.	
//注意:nus的值,不要大于798915us
void delay_us(u32 nus)
{		
	int i,j;
	for(i=0;i<nus;i++);
	{
		//for(j=0;j<100;j++);
		for(j=0;j<50;j++);
		//j=0;
		//if(j < 1)
		//	j++;
	}
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对168M条件下,nms<=798ms 
void delay_xms(u16 nms)
{	 		  	  
	int i,j;
	for(i=0;i<nms;i++) {
		for(j=0;j<42;j++);
	}
} 
//延时nms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	int i,j;
	for(i=0;i<nms;i++) {
		for(j=0;j<5000;j++);
	}
} 
#endif
			 



































