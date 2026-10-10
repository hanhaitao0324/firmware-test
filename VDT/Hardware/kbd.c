/*----------------------------------------------------------------------------
 *      Erweka - Keys
 *----------------------------------------------------------------------------
 *      Name:    keys.c
 *      Purpose: Key Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"    							// Device header
#include "kbd.h"
#include "pins.h"
#include "beeper.h"

unsigned char gettasto(void);

uint8_t lastPressedKey;
//current key combination
uint8_t kbdCurrentKeys;
extern bool RemoteFlg;

//kbd buffer
//uint8_t kbdBuffer[KBD_BUFFER_SIZE];
//
uint8_t curBut;
uint8_t kbdHead; 
uint8_t kbdTail;
//uint8_t kbdTail2;
uint8_t oldBut;
uint32_t repeatCounter;
bool repeatDelay;
bool repeatEnabled;
uint16_t repeatReload;
uint8_t repeatButton;
uint8_t repeatedKeys;
uint16_t buttonDelay=0;
bool kbdRepeatMode;

char BUTTON_NAME[KBD_CODE_LAST][7]=
{
  {"NO_KEY\x00"},
  {"START\x00"},
  {"STOP\x00"},
  {"S_UP\x00"},
  {"S_DOWN\x00"},
  {"F_UP\x00"},
  {"F_DOWN\x00"}
};

/*******************************************************************************
* Function Name  : Keys_Init
* Description    : Initialize input Keys
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KBD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(KEYS_PERIPH, 	ENABLE);					/* Enable GPIO bank clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, 	ENABLE);	/* Enable GPIO bank clock */

	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin   = (KEY_TOPLEFT | KEY_TOPRIGHT | KEY_BOTLEFT | KEY_BOTMID | KEY_BOTRIGHT | NOTUSED);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEYS_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin   = (KEY_TOPMID);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PORT_C, &GPIO_InitStructure);							//Pin f黵 SB3 musste auf Port PC.4 umgelegt werden
	
//  KBD_Reset();				//reset
}

unsigned char gettast(void)
{
	uint8_t ch;

	while(gettasto()!=KBD_CODE_NONE);//查询是否有按键按下
	while((ch=gettasto())==KBD_CODE_NONE);//再次查询是否有按键按下
	BEEPER_PlayTones(tones_1khz);//按键音
	return(ch);
}

unsigned char gettasto(void)
{
	if ( (GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTLEFT) && \
			GPIO_ReadInputDataBit(KEYS_GPIO,KEY_TOPLEFT) 	&& \
			GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTMID)		&& \
			GPIO_ReadInputDataBit(PORT_C,KEY_TOPMID)		&& \
			GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTRIGHT)	&& \
			GPIO_ReadInputDataBit(KEYS_GPIO,KEY_TOPRIGHT)	&& \
			GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTLEFT)) || RemoteFlg)
  {
    osDelay(10);
    return(0);
  }
	osDelay(20);
	
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTLEFT) && !GPIO_ReadInputDataBit(KEYS_GPIO,KEY_TOPRIGHT)) 
    return(KBD_CODE_MENU_DOWN_KBD_CODE_VALUE_UP);
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_TOPLEFT)) 
		return(KBD_CODE_MENU_UP);
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTMID)) 
		return(KBD_CODE_START);
	if(!GPIO_ReadInputDataBit(PORT_C,KEY_TOPMID)) 
		return(KBD_CODE_STOP);
#ifndef DEBUG
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTRIGHT)) 
		return(KBD_CODE_VALUE_DOWN);
#endif
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_TOPRIGHT))
		return(KBD_CODE_VALUE_UP);
	if(!GPIO_ReadInputDataBit(KEYS_GPIO,KEY_BOTLEFT))
		return(KBD_CODE_MENU_DOWN);

	return(0);
} 
