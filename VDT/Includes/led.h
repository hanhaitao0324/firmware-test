/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    led.h
 *      Purpose: led driver program
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef _LED_H
#define _LED_H

#include "defines.h"
#include "font.h"

#define LED_NUM_DIGITS	(4)

enum LED_DISPLAYS
{
  LED_DISPLAY_LEFT,
  LED_DISPLAY_RIGHT
};
	
enum LED_BRIGHTNESS
{
  LED_BRIGHTNESS_0,
  LED_BRIGHTNESS_1,
  LED_BRIGHTNESS_2,
  LED_BRIGHTNESS_3,
  LED_BRIGHTNESS_4,
  LED_BRIGHTNESS_5,
  LED_BRIGHTNESS_6,
  LED_BRIGHTNESS_7,
  LED_BRIGHTNESS_8,
  LED_BRIGHTNESS_9,
  LED_BRIGHTNESS_10,
  LED_BRIGHTNESS_NUM
};
	
#define TIMER_BLINK_RELOAD  (200)
#define LED_MASK_DEC_POINT  (SEG_DP)
#define SPEZIAL_MODE				1
#define NORMAL_MODE				  0
	
#define pinLED_SEL               D_SEL
#define pinLED_SCK               D_SCK
#define pinLED_LATCH             D_LATCH
#define pinLED_DATA              D_DATA

#define pinStrobe1							 STROBE1
#define pinStrobe2							 STROBE2
#define pinStrobe3							 STROBE3
#define pinStrobe4							 STROBE4

#define PORT_LED		             PORT_A
#define PORT_STROBE		           PORT_C

//blink position
extern uint8_t ledBlinkPos;
extern bool ledBlinkPosYes;

// LED_Init - init module
extern void LED_Init(void);
//// LED_Print - print a string on the display
extern void LED_Print(uint8_t led, bool blink, char *str);
// LED interrupt handler
extern void LEDRefresh_thread(const void *arg);
// LED_SetBrightness - set LED brightness
extern bool LED_SetBrightness(uint8_t brightness);
// LED_GetBrightness - get LED brightness
extern uint8_t LED_GetBrightness(void);
// LED_Check - check LEDs
extern void LED_Check(uint8_t Position);
// LED_SwitchOff - switch off LEDs
extern void LED_SwitchOff(void);
// LED_Store - store LEDs values
extern void LED_Store(void);
// LED_Restore - restore LEDs values
extern void LED_Restore(void);

#endif
