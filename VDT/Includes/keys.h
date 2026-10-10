/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    keys.h
 *      Purpose: Key driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef _KEYS_H
#define _KEYS_H

#include "defines.h"
#include "pins.h"

#define KEY_TOPLEFT		      KB1	//input
#define KEY_TOPMID		      KB2	//input
#define KEY_TOPRIGHT		    KB3	//input
#define KEY_BOTLEFT		      KB4	//input
#define KEY_BOTMID		      KB5	//input
#define KEY_BOTRIGHT			  KB6	//input
#define KEYS_GPIO           PORT_B
#define KEYS_PERIPH         RCC_APB2Periph_GPIOB

//enum KEYS
//  {
//    KEY_TOPLEFT,
//    KEY_TOPMID,
//    KEY_TOPRIGHT,
//    KEY_BOTLEFT,
//    KEY_BOTMID,
//    KEY_BOTRIGHT,
//    KEY_NUM
//  };

#endif
