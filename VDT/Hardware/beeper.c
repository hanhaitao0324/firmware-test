/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    beeper.c
 *      Purpose: Piezo beeper program
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "beeper.h"
#include "stm32f10x_conf.h"   							// Device header

volatile uint32_t         		_beep_duration;
volatile bool             	  _tones_playing;
volatile static const Tone_TypeDef *_tones;

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : BEEPER TIM4 IRQ handler
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void) {
  if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);

		if (_beep_duration)
			_beep_duration--;
		if (_beep_duration == 0) {
			if (_tones_playing) {
				_tones++;// Currently playing tones, take next tone
				if (_tones->frequency == 0 && _tones->duration == 0) {
					BEEPER_Disable();// Last tone in sequence
					_tones_playing = false;
					_tones = NULL;
				} else {
					if (_tones->frequency == 0) {
						BEEPER_TIM->ARR = SystemCoreClock / (100 * BEEPER_TIM->PSC) - 1;// Silence period
						BEEPER_TIM->CCR2 = 0; // 0% duty cycle
						_beep_duration = _tones->duration + 1;
					} else {
						BEEPER_Enable(_tones->frequency,_tones->duration);// Play next tone in sequence
					}
				}
			} else {
				BEEPER_Disable();
			}
		}
	}
}

/*******************************************************************************
* Function Name  : BEEPER_Init
* Description    : Initialize Timer4 , Initialize buzzer output
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Init(void) {
	NVIC_InitTypeDef NVICInit;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin   = BEEPER_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  /*Clock setup for timer4 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	/*Timer Base Initialization for timer4 */
	TIM_TimeBaseInitStructure.TIM_Prescaler			= (SystemCoreClock / 720000) -1;	//99
	TIM_TimeBaseInitStructure.TIM_CounterMode 	= TIM_CounterMode_Up ;
	TIM_TimeBaseInitStructure.TIM_Period 				= PERIOD_1KHZ; 										//3599 Auto-Reload Value
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	/*Auto-reload always */
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	/*Channels initialization for timer4 */
	TIM_OCInitStructure.TIM_OCMode 					= TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable ;
	TIM_OCInitStructure.TIM_Pulse 					= 499; // 50% duty cycle
	TIM_OCInitStructure.TIM_OCPolarity 			= TIM_OCPolarity_High;
        
	BEEPER_TIM->CCER  |= TIM_CCER_CC3P; 				// Channel Capture/Compare 3 output polarity
	BEEPER_TIM->CCER  |= TIM_CCER_CC3E; 				// BEEPER_TIM_CH3 output Capture/Compare 3 output enable
	BEEPER_TIM->EGR    = TIM_EGR_UG; 						// Generate an update event to reload the prescaler value immediately
				
	//TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	//TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 							// init Channel 3
	//TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	/*CCRs auto-reload always */
	//TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
	//TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);			// important to make pulse change in NEXT period
	//TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	/* TIM IT enable */
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
	/*Start counter for timer4 */
	//TIM_Cmd(TIM4, ENABLE);
				
	// BEEPER_TIM IRQ
	NVICInit.NVIC_IRQChannel 		= BEEPER_TIM_IRQN;
	NVICInit.NVIC_IRQChannelCmd = ENABLE;
	NVICInit.NVIC_IRQChannelPreemptionPriority = 11;
	NVIC_Init(&NVICInit);
}

/*******************************************************************************
* Function Name  : BEEPER_Enable
* Description    : Turn on buzzer with specified frequency
* Input          : freq - PWM frequency for buzzer (Hz)
* Input          : duration - duration of buzzer work (tens ms: 1 -> 10ms sound duration)
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Enable(uint16_t freq, uint32_t duration) {
//	GPIO_InitTypeDef PORT;
	if (freq < 100 || freq > 8000 || duration == 0) {
		BEEPER_Disable();
	} else {
		_beep_duration = (freq / 100) * duration + 1;
	
		/*PORT.GPIO_Mode  = GPIO_Mode_AF_PP;				//GPIO_Mode_Out_OD;				// Configure buzzer pin
//		PORT.GPIO_PuPd  = GPIO_Mode_IPU;				//GPIO_PuPd_UP;
		GPIO_Init(BEEPER_GPIO, &PORT);*/
		
		TIM_Cmd(TIM4, ENABLE);

		// Configure and enable PWM timer
		RCC->APB1ENR |= BEEPER_TIM_PERIPH; 				// Enable TIMx peripheral
		BEEPER_TIM->ARR  = SystemCoreClock / (freq * BEEPER_TIM->PSC) - 1;
		BEEPER_TIM->CCR3 = BEEPER_TIM->ARR >> 1;  // 50% duty cycle, Capture/Compare value
		BEEPER_TIM->CR1 |= TIM_CR1_CEN; 					// Counter enable
	}
}

/*******************************************************************************
* Function Name  : BEEPER_Disable
* Description    : Turn off buzzer
* Input          : none
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_Disable(void) {
//  GPIO_InitTypeDef PORT;
	//BEEPER_TIM->CR1 &= ~TIM_CR1_CEN;					// Counter disable
	//RCC->APB1ENR    &= ~BEEPER_TIM_PERIPH;				// Disable TIMx peripheral to conserve power
	TIM_Cmd(TIM4, DISABLE);
	//PORT.GPIO_Mode   = GPIO_Mode_IN_FLOATING;		// Configure buzzer pin as analog input without pullup to conserve power
	//GPIO_Init(BEEPER_GPIO, &PORT);
}

/*******************************************************************************
* Function Name  : BEEPER_PlayTones
* Description    : Start playing tones sequence
* Input          : tones - pointer to tones array
* Output         : None
* Return         : None
*******************************************************************************/
void BEEPER_PlayTones(const Tone_TypeDef * tones) {
#ifdef BEEP_OFF
  return;
#endif
	_tones = tones;
	_tones_playing = true;
	BEEPER_Enable(_tones->frequency,_tones->duration);
}

