#ifndef _KBD_H
#define _KBD_H

#include "const.h"
#include "defines.h"
#include "platform.h"
#include "timer.h"

#define KBD_MASK_SLOW_DOWN	(0x01)
#define KBD_MASK_START		(0x02)
#define KBD_MASK_SLOW_UP	(0x04)
#define KBD_MASK_FAST_DOWN	(0x08)
#define KBD_MASK_STOP		(0x10)
#define KBD_MASK_FAST_UP	(0x20)

//buffer size
#define KBD_BUFFER_SIZE	        (16)

//a hidden combination for "set range"
#define KBD_MASK_CONFIGURATION	(KBD_MASK_START|KBD_MASK_STOP)
//a hidden combination for "engineering"
#define KBD_MASK_ENGINEERING	(KBD_MASK_START|KBD_MASK_STOP|KBD_MASK_SLOW_UP)

//poll rate, Hz
#define KBD_POLL_RATE		(TMR0_KBD_RATE)
#define KBD_POLL_RATE_MS	(1000/KBD_POLL_RATE)

//slow and fast autorepeat, msec
#define KBD_SLOW_MS	        (200)
#define KBD_FAST_MS	        (200)
//a delay before the autorepeat starts, msec
#define KBD_AUTOREPEAT_MS	(1000)

#define KBD_NUM_AUTO_PRESSES    (200)
#define KBD_WAIT_NEXT           (200)

//#define KBD_SLOW_REPEAT         (KBD_SLOW_MS/KBD_POLL_RATE_MS)
//#define KBD_FAST_REPEAT         (KBD_FAST_MS/KBD_POLL_RATE_MS)
//#define KBD_AUTOREPEAT          (KBD_AUTOREPEAT_MS/KBD_POLL_RATE_MS)

#define KBD_BUTTON_DELAY_MS     (100)

#define KBD_SLOW_REPEAT         (KBD_SLOW_MS*TMR0_INT_RATE/1000)
#define KBD_FAST_REPEAT         (KBD_FAST_MS*TMR0_INT_RATE/1000)
#define KBD_AUTOREPEAT          (KBD_AUTOREPEAT_MS*TMR0_INT_RATE/1000)
#define KBD_BUTTON_DELAY        (KBD_BUTTON_DELAY_MS*TMR0_INT_RATE/1000)

//current key combination
extern TByte kbdCurrentKeys;

extern TByte curBut;

extern __flash char BUTTON_NAME[KBD_CODE_LAST][7];

// KBD_Init - init module
extern void KBD_Init(void);
// KBD_KeyPressed - check if key pressed
extern TBool KBD_KeyPressed(void);
// KBD_IsRepeated - check if key is repeated or not
extern TBool KBD_IsRepeated(void);
// KBD_GetKey - get key
extern TByte KBD_GetKey(void);
// KBD_Reset - reset module
extern void KBD_Reset(void);
// KBD_UngetKey - unget a key
extern __monitor void KBD_UngetKey(TByte key);
// KBD_IntHandler - keypad interrupt handler
extern void KBD_IntHandler(void);

#endif
