#include "stm32f4xx_conf.h"
#include "timer.h"
#include "led.h"
#include "defines.h"
#include "beeper.h"
#include "pwm.h"
#include "motor.h"
#include "can.h"
#include "system.h"

volatile uint16_t PowOnDelay;
volatile TTimer timers[TIMER_NUM_TIMERS];

volatile uint16_t clockCounter;

volatile u8 upd_rxtover; 								// ��������ʱ�Ľ��ճ�ʱ�ж�

extern u32 lwip_localtime;

extern short SecCount;
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //������ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		//lwip_localtime +=10; //��10
		
		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}

u8 flash_flag;
u8 flash_cont;

u8 dtFlash;

u16 Dis_Flash;

//ͨ�ö�ʱ��4�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��4!
void TIM4_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef 		TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef 						NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 			// ʹ��TIM4ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 							// �Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; 						// ��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 		// ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure); 				// ��ʼ��TIM4
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 									// ������ʱ��4�����ж�
	TIM_Cmd(TIM4,ENABLE); 																		// ʹ�ܶ�ʱ��4
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 						// ��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // ��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03; 			// �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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

uint16_t msec;
int ForcSec = 0; 		// 步进电机脉冲计数器，多少个脉冲到1秒
extern unsigned int mesenable;

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	uint8_t i;
  //static uint32_t counter1OneSec=0, counter2OneSec= 0, counter_Lic=0;
	//static unsigned int reduce_counter, lastPress;
	//uint16_t capture = 0;
	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) 							// ����ж�
	{
		for (i=0; i<TIMER_NUM_TIMERS; i++)
			{
				if (timers[i].flgRunning)
				{
					if (timers[i].value)
					{
						timers[i].value--;
					}
					else
					{
						timers[i].flgTimeOut = true;
			}
				}
		}
		
		msec++;
		if(msec <= 1000)
		{
			if(msec%500 == 0)
				flash_flag = !flash_flag;
		}
		else 																// 秒计时
		{
			msec = 0;

			if(SecCount != 0)
				SecCount--;			
		}
		
		if(msec%10) 												// 10mS��ʱ��
		{
			lwip_localtime +=10; 							// ��10
		}
		
		if(PowOnDelay != 0)
			PowOnDelay--;
		
		if(upd_rxtover != 0) 								// ��������ʱ�Ľ��ճ�ʱ�ж�
			upd_rxtover--;
		
		if(msec < 500)
			GPIO_SetBits(GPIOB,GPIO_Pin_0); 							// ����
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_0); 							// ����
		
		if(dtFlash != 0)
			dtFlash--;

		if(Dis_Flash != 0)
			Dis_Flash--;

		ForcSec++;
		if(ForcSec>=10)
		{
			ForcSec = 0;
			mesenable = 1;
		}

		// CAN���ͳ�ʱ -------------------------------------------------------------
		if(CanTx_TimeOut != 0)
			CanTx_TimeOut--;
		
		// ѹ������� --------------------------------------------------------------
		//*
		if (_beep_duration) 																												// ���ų���ʱ��ά��
			_beep_duration--;
		else 																																				// ����ʱ�䵽��
		{
			if (_tones_playing) 																											// �ڲ����У�
			{
				_tones++; 																															// Currently playing tones, take next tone
				if (_tones->frequency == 0 && _tones->duration == 0) 
				{
					BEEPER_Disable(); 																										// Last tone in sequence
					_tones_playing = false;
					_tones = NULL;
				} 
				else 																																		// ������һ������
				{
					if (_tones->frequency == 0) 
					{
						//TIM3CH4_SetCompare(0); 																							// ռ�ձ�Ϊ0
						TIM5CH1_SetCompare(0); 																							// ռ�ձ�Ϊ0
						//BEEPER_TIM->ARR = SystemCoreClock / (100 * BEEPER_TIM->PSC) - 1; 		// Silence period
						//BEEPER_TIM->CCR2 = 0; 																							// 0% duty cycle
						//_beep_duration = _tones->duration + 1;
						_beep_duration = _tones->duration * 10;
					} 
					else 
					{
						BEEPER_Enable(_tones->frequency,_tones->duration); 									// Play next tone in sequence
					}
				}
			}
			else 																																			// �������
			{
				BEEPER_Disable();
			}
		}
		//*/
		
		// ������� ----------------------------------------------------------------
		//MOTOR_IntHandler();
  }
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 								// ����жϱ�־λ
}

