#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"
#include "timer.h"
#include "system.h"

volatile TTimer timers[TIMER_NUM_TIMERS];
uint16_t CCR1_Val;
extern bool CountFree, MessFlg, FirstReached;
extern char PresReduced;
extern int tMode, ablZeitL, ablZeitH, intervall, aktPress, tmpVacuum;
extern osThreadId LicTask;													/* assigned task id of task:  LicTask */
extern int Vacuum;

//------------------------------------------------------------------------------
// TIMER_Init - init module
//------------------------------------------------------------------------------
void TIMER_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint8_t i;
  NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue;
  
  // TIM2 clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  // Enable the TIM2 global Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

	// Compute the prescaler value
  PrescalerValue = (uint16_t)(SystemCoreClock / TIMER_FREQ) - 1;

  // Time base configuration
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  // Prescaler configuration
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	CCR1_Val = TIMER_VALUE;

  // Output Compare Timing Mode configuration: Channel1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
	
  // TIM IT enable
  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

  // TIM2 enable counter
  TIM_Cmd(TIM2, ENABLE);

	for (i=0; i<TIMER_NUM_TIMERS; i++)
    timers[i].flgRunning = false;
}

//------------------------------------------------------------------------------
// TIMER_Start - start a timer
//------------------------------------------------------------------------------
void TIMER_Start(uint8_t index, uint16_t timeout)
{
  timers[index].flgTimeOut = false;
  timers[index].value = timeout;
  timers[index].flgRunning = true;
}
  
//------------------------------------------------------------------------------
// TIMER_Stop - stop a timer
//------------------------------------------------------------------------------
void TIMER_Stop(uint8_t index)
{
  timers[index].flgTimeOut = false;
  timers[index].flgRunning = false;
}
  
//------------------------------------------------------------------------------
// TIMER_TimeOut - check if timeout
//------------------------------------------------------------------------------
uint8_t TIMER_TimeOut(uint8_t index)
{
  return timers[index].flgTimeOut;
}
  
//------------------------------------------------------------------------------
// TIMER_IntHandler - interrupt handler
// for counterOneSec Timer (1sec.)
// for Timeouts (TTimer struct)
//------------------------------------------------------------------------------
void TIM2_IRQHandler(void)
{
  uint8_t i;
  static uint32_t counter1OneSec=0, counter2OneSec= 0, counter_Lic=0;
	static unsigned int reduce_counter, lastPress,reduce_counter2;
	uint16_t capture = 0;

  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

    capture = TIM_GetCapture1(TIM2);
    TIM_SetCompare1(TIM2, capture + CCR1_Val);
		for (i=0; i<TIMER_NUM_TIMERS; i++){
			if (timers[i].flgRunning){
				if (timers[i].value){
					timers[i].value--;
				}else{
					timers[i].flgTimeOut = true;
        }
			}
    }
		counter1OneSec++;
		counter2OneSec++;
				
		//------- timeout für Druckanstieg, falls Druck nicht steigt -------
		if (counter1OneSec>= TO_100MSEC)
		{
				if(MessFlg && PresReduced == 0 && !CountFree) 
				{ // wenn Messung läuft, wenn noch kein timeout, wenn zeit nicht läuft
					if(reduce_counter++>PRESSURE_RISE) 
					{
						if(aktPress >= lastPress+3) 
						{						// war > 3 Druck muss steigen
							reduce_counter = 0;										// zähler zurücksetzen
						}	
						else 
						{
							PresReduced = 1;										// timeout true ???????60?
						}
						lastPress = aktPress;
					}

					if(reduce_counter2++>1200) 
					{
						if(aktPress <= Vacuum+3)
						{
							PresReduced = 2;
						}
					}
				}
				else 
				{
					reduce_counter = 0;
					reduce_counter2 = 0;
					lastPress = 0;
				}
			counter1OneSec = 0;
		}		
		//------- timeout für Druckanstieg, falls Druck nicht steigt -------

		if (counter2OneSec>= ((tMode)? TIMER_ONE_SEC_COUNTER*60:TIMER_ONE_SEC_COUNTER))
		{
			if(CountFree) 
			{
				//------- Sekunden counter (Messung) -------
				if(ablZeitL>0) 
				{
					ablZeitL--;
				}	
				else 
				{
					if(ablZeitH>0) 
					{
						ablZeitH--;
						ablZeitL=59;
					}
				}
				if(intervall>0) intervall--;
				//------- Sekunden counter (Messung) -------
			}
			counter2OneSec = 0;
		}

		if (counter_Lic >= TO_10SEC){							//LicManager_Handler //todo Zeit ? stimmt ??	
	//		if(prescale++>5998) {							
//			osSignalSet(LicTask, 0x1);
			counter_Lic = 0;
		}	
  }
}


