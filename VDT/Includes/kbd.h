#ifndef _KBD_H
#define _KBD_H

#include "defines.h"
#include "platform.h"

//------------------------------------------------------------------------------
// KBD
//------------------------------------------------------------------------------
enum KBD_CODES
{
  KBD_CODE_NONE,
  KBD_CODE_START,
  KBD_CODE_STOP,
  KBD_CODE_MENU,
  KBD_CODE_MENU_UP,
  KBD_CODE_MENU_DOWN,
  KBD_CODE_VALUE_UP,
  KBD_CODE_VALUE_DOWN,
  KBD_CODE_MENU_DOWN_KBD_CODE_VALUE_UP,
  KBD_CODE_LAST
};
	
#define KBD_MASK_MENU_DOWN	(0x01)
#define KBD_MASK_START			(0x02)
#define KBD_MASK_MENU_UP		(0x04)
#define KBD_MASK_VALUE_DOWN	(0x08)
#define KBD_MASK_STOP				(0x10)
#define KBD_MASK_VALUE_UP		(0x20)

#define KEY_TOPLEFT		      SB1	//input
#define KEY_BOTMID		      SB2	//input
#define KEY_TOPMID		      SB3	//input
#define KEY_BOTRIGHT		    SB4	//input
#define KEY_TOPRIGHT		    SB5	//input
#define KEY_BOTLEFT		  	  SB6	//input
#define KEYS_GPIO           PORT_B
#define KEYS_PERIPH         RCC_APB2Periph_GPIOB

//buffer size
//#define KBD_BUFFER_SIZE (8)			// für Tasten (keine Alpha Tastatur)
#define KBD_HANDLER_UPD_RATE    (100)		// Thread update in ms

//a hidden combination for "set range"
#define KBD_MASK_CONFIGURATION	(KBD_MASK_START|KBD_MASK_STOP)
//a hidden combination for "engineering"
#define KBD_MASK_ENGINEERING	(KBD_MASK_START|KBD_MASK_STOP|KBD_MASK_SLOW_UP)

//slow and fast autorepeat, msec
#define KBD_SLOW_MS	        (200)
#define KBD_FAST_MS	        (200)

//a delay before the autorepeat starts, msec
#define KBD_AUTOREPEAT_MS	(1000)

#define KBD_BUTTON_DELAY_MS     (100)

#define KBD_SLOW_REPEAT         (KBD_SLOW_MS * 					KBD_HANDLER_UPD_RATE/1000)
#define KBD_FAST_REPEAT         (KBD_FAST_MS * 					KBD_HANDLER_UPD_RATE/1000)
#define KBD_AUTOREPEAT          (KBD_AUTOREPEAT_MS * 	 	KBD_HANDLER_UPD_RATE/1000)
#define KBD_BUTTON_DELAY        (KBD_BUTTON_DELAY_MS * 	KBD_HANDLER_UPD_RATE/1000)

//current key combination
extern uint8_t kbdCurrentKeys;
extern uint8_t curBut;
extern char BUTTON_NAME[KBD_CODE_LAST][7];

// KBD_Init - init module
extern void KBD_Init(void);
unsigned char gettast(void);
unsigned char gettasto(void);

#endif
