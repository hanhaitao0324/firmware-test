#ifndef _PWM_H
#define _PWM_H
#include "stm32f4xx_conf.h"

extern void TIM3CH4_PWM_Init(u16 arr,u16 psc);
extern void TIM3CH4_SetCompare(u16 compare);
extern void TIM3_SetFreq(u16 frequency);

extern void TIM8CH2_PWM_Init(u16 arr,u16 psc);
extern void TIM8CH2_SetCompare(u16 compare);
extern void TIM8_SetFreq(u16 frequency);

#endif
