#ifndef _MOTOR_H
#define _MOTOR_H

#include "defines.h"
#include "const.h"

// биты Регистар MOTOR_CON
#define Bit_MOTOR     0     // working bit
#define Bit_Fast      1     // starting bit
#define Bit_Slow      2     // stopping bit
#define Bit_OneRev    3     // one rotation bit
#define Bit_Stab      4	    // stabilization bit
#define Bit_Pulse     5     // got pulse bit
#define Bit_1sec      6     // 1 sec bit
#define Bit_SnsrStop  7     // sensor stop bit

#define MOTOR_PWM_MAX	      (0x03F0)
#define MOTOR_PWM_MIN	      (0x0000)
#define MOTOR_PWM_START       (0x0020)

#define MOTOR_IMPULSE_SEC     (MOTOR_IMPULSE_REV/60)

#ifdef DEVICE_SVM
#define MOTOR_PWM_STEP        (0x20)
#define MOTOR_MAX_STEP        (64)
#define MOTOR_PRESCALER       (64)
#endif

#ifdef DEVICE_TAR
#define MOTOR_PWM_STEP        (0x10)
#define MOTOR_MAX_STEP        (8)
#define MOTOR_PRESCALER       (8)
#endif

#define MOTOR_MAX_POWER       (1023)
#define MOTOR_MIN_POWER       (0)

#define INT_SPEED_BUF_LEN     (1)
#define INT_SPEED_BUF_PAUSE   (1)     // wait before accepting speed value

#define SECONDS_COUNTER       (1)

#define SPEED_BUF_LEN         (10)

extern signed int MOTOR_Power;

extern volatile TByte MOTOR_CON;

extern TReal32 Speed_Buf[SPEED_BUF_LEN+1];

extern void MOTOR_Init(void);
extern void MOTOR_IntHandler(void);
extern void MOTOR_Start(void);
extern void MOTOR_Stop(void);
extern void MOTOR_Fast(void);
extern void MOTOR_Slow(void);
extern void MOTOR_SetSpeed(TReal32 speed);
extern TWord MOTOR_GetSpeed(void);  
extern TWord MOTOR_GetCurrSpeed(void);
extern TWord MOTOR_GetRevolutions(void);
extern void MOTOR_ResetRevolutions(void);
extern TBool MOTOR_SensorReady(void);
extern void MOTOR_SetReverse(TBool On);

#endif
