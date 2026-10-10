#ifndef _KBD_H
  #define _KBD_H

#include "platform.h"
#include "defines.h"
#include "const.h"
#include "timer.h"

// 按键编码-----------------------------
#define KBD_MASK_START        (0x0001)
#define KBD_MASK_STOP         (0x0002)
#define KBD_MASK_DOWN         (0x0004)
#define KBD_MASK_QDOWN        (0x0008)
#define KBD_MASK_UP           (0x0010)
#define KBD_MASK_QUP          (0x0020)

//buffer size（缓存空间）---------------
#define KBD_BUFFER_SIZE       (8)

//a hidden combination for "set range"（“设置范围”的隐藏组合键码
#define KBD_MASK_CONFIGURATION          (KBD_MASK_START|KBD_MASK_STOP)

//a hidden combination for "engineering"（“工程”的隐藏组合键码
#define KBD_MASK_ENGINEERING            (KBD_MASK_START|KBD_MASK_STOP|KBD_MASK_UP)

//poll rate, Hz（轮询速率）
#define KBD_POLL_RATE                   (TMR0_KBD_RATE)
#define KBD_POLL_RATE_MS                (1000/KBD_POLL_RATE)

//slow and fast autorepeat, msec（低速和高速自动重复定时，毫秒）
#define KBD_SLOW_MS           (200)
#define KBD_FAST_MS           (200)

//a delay before the autorepeat starts, msec（自动重复开始前的延时，毫秒）
#define KBD_AUTOREPEAT_MS     (1000)

#define KBD_NUM_AUTO_PRESSES  (200)
#define KBD_WAIT_NEXT         (200)

#define KBD_BUTTON_DELAY_MS   (100)

#define KBD_SLOW_REPEAT       (KBD_SLOW_MS*TMR0_INT_RATE/1000)
#define KBD_FAST_REPEAT       (KBD_FAST_MS*TMR0_INT_RATE/1000)
#define KBD_AUTOREPEAT        (KBD_AUTOREPEAT_MS*TMR0_INT_RATE/1000)
#define KBD_BUTTON_DELAY      (KBD_BUTTON_DELAY_MS*TMR0_INT_RATE/1000)

//current key combination（当前的组合键码）
extern TWord kbdCurrentKeys;

extern TWord curBut;

// KBD_Init - init module（模块初始化）
extern void KBD_Init(void);

// KBD_KeyPressed - check if key pressed（检测按键按下）
extern TBool KBD_KeyPressed(void);

// KBD_IsRepeated - check if key is repeated or not（检测是否有重复按键）
extern TBool KBD_IsRepeated(void);

// KBD_GetKey - get key（获取按键码）
extern TByte KBD_GetKey(void);

// KBD_Reset - reset module（模块复位）
extern void KBD_Reset(void);

// KBD_IntHandler - keypad interrupt handler（键盘中断处理）
extern void KBD_IntHandler(void);

#endif
