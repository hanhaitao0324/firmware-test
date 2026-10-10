/*----------------------------------------------------------------------------
 *      Erweka - Keys
 *----------------------------------------------------------------------------
 *      Name:    keys.c
 *      Purpose: Key Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "stm32f10x_conf.h"   							// Device header
#include "keys.h"

/*******************************************************************************
* Function Name  : Keys_Init
* Description    : Initialize input Keys
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Keys_Init(void) {

  GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin   = (KEY_TOPLEFT | KEY_TOPMID | KEY_TOPRIGHT | KEY_BOTLEFT | KEY_BOTMID | KEY_BOTRIGHT);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEYS_GPIO, &GPIO_InitStructure);
	
}



