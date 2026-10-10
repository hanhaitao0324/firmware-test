/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    main.c
 *      Purpose: VDT main program
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"
#include <string.h>
#include <stdio.h>
#include "display.h"
#include "defines.h"
#include "beeper.h"
#include "spi.h"
#include "uart.h"
#include "vinculum.h"
#include "led.h"
#include "kbd.h"
#include "eeprom.h"
#include "m24xx.h"
#include "mcp3201.h"
#include "rtc.h"
#include "timer.h"
#include "system.h"
#include "io.h"
#include "time.h"

void Display_thread(void const *args);
void LEDRefresh_thread(void const *args);
void Communicate_thread(void const *args);
void LicManager_thread(void const *args);
void Key_thread(void const *args);
void ADC_thread(void const *args);
void VDT_System_init(void);

extern void Init_Timers (void);
extern void Communicate_Init(void);
extern void Communicate_Handler(void);
extern void LicManager_Handler(void);

osThreadId DispTask; 			/* assigned task id of task:  DispTask */
osThreadId LEDTask; 			/* assigned task id of task:  LEDTask */
osThreadId CommTask; 			/* assigned task id of task:  CommTask */
osThreadId LicTask; 			/* assigned task id of task:  LicTask */
//osThreadId ADCTask; 				/* assigned task id of task:  ADCTask */

osThreadDef(Display_thread, 	osPriorityHigh, 	1, 2048 );                       
osThreadDef(LEDRefresh_thread, 	osPriorityRealtime, 1, 0 );                       
osThreadDef(Communicate_thread, osPriorityNormal, 	1, 2048 );                       
osThreadDef(LicManager_thread,	osPriorityNormal, 	1, 0 );                        
//osThreadDef(ADC_thread, 				osPriorityLow, 1, 0 );                        

u8 globalPCBType = 0;

void GPIO_Initialize(void)
{
	//enable bus clocks
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, 	ENABLE); /* Enable GPIO bank clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, 	ENABLE); /* Enable GPIO bank clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, 	ENABLE); /* Enable GPIO bank clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, 	ENABLE); /* Enable UART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, 	ENABLE); /* Enable UART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, 	ENABLE); //need for Jtag disable
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, 		ENABLE); /* Enable DMA clock */
	
	IO_Init();
	
	// ¼ì²éPCBÀàÐÍ
	if(GPIO_ReadInputDataBit(PORT_C,PCBSW0)) 		// TLC5927
	{
		globalPCBType = PCB_TLC5927;
	}
	else 																				// DM13H, Gavin, 2022-06-08 16:15
	{
		globalPCBType = PCB_DM13H;
	}
	
//	#ifndef DEBUG
//todo für release Version hier jtag und swd ausschalten wegen KEY_BOTRIGHT (PB3) der sonnst durch TRACESWO angesteuert wird
// TRACESWO (PB3) wird von J-Link verwendet ! nicht belegen !
//	Requires AFIO clock enabled, Map JTAG Off pins PB3,4,5, SWD still viable on PA13,14
//  SWO 	PB3 	Data pin from MCU for trace view (Serial Wire Viewer), optional and recommended
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // Disable JTAG, Enable SWJ
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//	#endif
}

void Display_thread(void const *args)
{
	logo();
	while (1)
	{
		Display_Handler();
		osDelay(100);
	}
}

void Communicate_thread(void const *args)
{
	while(1)
	{
		osSignalWait(0x1, osWaitForever); 		// Signal flags that are reported as event are automatically cleared.
		Communicate_Handler();
		osDelay(1);
	}
}

void LicManager_thread(void const *args)
{
	while(1)
	{
		osSignalWait(0x1, osWaitForever); 		// Signal flags that are reported as event are automatically cleared.
		LicManager_Handler();
		osDelay(12);
	}
}

/*
 * main: initialize and start the system
 */
int main(void) 
{
	osKernelInitialize (); 					// initialize CMSIS-RTOS
	
	// initialize internal peripherals
	GPIO_Initialize();
	TIMER_Init();
	SPI1_Init(); 							// SPI1 for LED 
	USART1_Init(); 							// Usart2 for RS232 Port
	USART3_Init(); 							// Usart3 for Vinculum-II
	M24XX_Init(); 							// M24256 EEPROM init
	MCP3201_Init(); 						// ADC 
	RTC_Init(); 							// RTC Configuration
	
	// initialize peripherals
	BEEPER_Init(); 							// Piezo Beeper
	BEEPER_PlayTones(tones_startup);
	VINC_Init(); 							// Vinculum
	LED_Init(); 							// LED Display
	KBD_Init(); 							// Keys
	
	//Communicate_Init(); 						//RS232 Communication für Vinculum Debuging
	VDT_System_init();
	
	// initialize threads
	DispTask = osThreadCreate(osThread(Display_thread), NULL);
	LEDTask  = osThreadCreate(osThread(LEDRefresh_thread), NULL);
	CommTask = osThreadCreate(osThread(Communicate_thread), NULL);
	LicTask  = osThreadCreate(osThread(LicManager_thread), NULL);
//	 ADCTask  = osThreadCreate(osThread(ADC_thread), NULL);
	
	osKernelStart (); 						// start thread execution 
}

//void ADC_thread(void const *args){
//	while(1) {
//		ADC_Handler();
//		osDelay(ADC_HANDLER_UPD_RATE);
//	}
//}
