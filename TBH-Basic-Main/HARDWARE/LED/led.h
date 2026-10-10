#ifndef __LED_H
#define __LED_H


// LED端口定义 -------------------------
#define LED0_ON 		GPIO_ResetBits(GPIOE,GPIO_Pin_1)				// PB0置0，LED0点亮
#define LED0_OFF 		GPIO_SetBits(GPIOE,GPIO_Pin_1)					// PB0置1，LED0熄灭
#define LED1_ON 		GPIO_ResetBits(GPIOE,GPIO_Pin_0)				// PB1置0，LED1点亮
#define LED1_OFF 		GPIO_SetBits(GPIOE,GPIO_Pin_0)					// PB1置1，LED1熄灭

#define LED0 PBout(0)
#define LED1 PBout(1)

void LED_Init(void); 										// 初始化

#endif
