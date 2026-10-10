/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    beeper.c
 *      Purpose: Piezo beeper program
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
//#include "osObjects.h"                      // RTOS object definitions
#include "beeper.h"
//#include "stm32f10x_conf.h"   							// Device header
#include "stm32f4xx_conf.h"   							// Device header
#include "pwm.h"

volatile uint32_t         		_beep_duration;
volatile _Bool             	  _tones_playing;
//volatile static const Tone_TypeDef *_tones;
volatile const Tone_TypeDef *_tones;

/*******************************************************************************
* Function Name  : BEEPER_Init
* Description    : Initialize Timer4 , Initialize buzzer output
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Init(void) 
{
	//定时器时钟84M，分频系数84，所以84M/84=1Mhz的计数频率，计数500次为2ms
	TIM3CH4_PWM_Init(500-1,84-1); 	// 
}

/*******************************************************************************
* Function Name  : BEEPER_Enable
* Description    : Turn on buzzer with specified frequency
* Input          : freq - PWM frequency for buzzer (Hz)
* Input          : duration - duration of buzzer work (tens ms: 1 -> 10ms sound duration)
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Enable(uint16_t freq, uint32_t duration) 
{
	if (freq < 100 || freq > 8000 || duration == 0) 
	{
		BEEPER_Disable();
	} 
	else 
	{
		TIM3_SetFreq(freq);
		TIM3CH4_SetCompare(50);
		
		_beep_duration = duration*10;
	}
}

/*******************************************************************************
* Function Name  : BEEPER_Disable
* Description    : Turn off buzzer
* Input          : none
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Disable(void) 
{
	TIM3CH4_SetCompare(0);
}

/*******************************************************************************
* Function Name  : BEEPER_PlayTones
* Description    : Start playing tones sequence
* Input          : tones - pointer to tones array
* Output         : None
* Return         : None
*******************************************************************************/
extern u8 SoundSw; 												// 键盘音开关
void BEEPER_PlayTones(const Tone_TypeDef * tones) 
{
//#ifdef BEEP_OFF
//	return;
//#end if
	if(SoundSw == 0) 											// 声音开关
		return;
	
	_tones = tones;
	_tones_playing = true;
	BEEPER_Enable(_tones->frequency,_tones->duration);
}

