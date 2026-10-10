#ifndef __DMA_H
#define	__DMA_H	   

#include "stm32f4xx_conf.h"
#include "defines.h"

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,uint16_t ndtr);//配置DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,uint16_t ndtr);	//使能一次DMA传输		   

#endif






























