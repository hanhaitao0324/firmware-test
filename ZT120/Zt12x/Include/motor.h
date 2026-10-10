#ifndef _MOTOR_H
#define _MOTOR_H

#include "defines.h"
#include "const.h"

// 徼螓 绣汨耱囵 MOTOR_CON
#define Bit_MOTOR     0     // working bit，			工作位
#define Bit_Fast      1     // starting bit，			开始位
#define Bit_Slow      2     // stopping bit，			停止位
#define Bit_OneRev    3     // one rotation bit，	一个循环位
#define Bit_Stab      4	    // stabilization bit，稳定位
#define Bit_Pulse     5     // got pulse bit，		有脉冲位
#define Bit_1sec      6     // 1 sec bit，				一秒位
#define Bit_SnsrStop  7     // sensor stop bit，	传感器停止位

#define MOTOR_PWM_STEP        (0x10)
#define MOTOR_PWM_MAX	      (0x03F0)
#define MOTOR_PWM_MIN	      (0x0000)
#define MOTOR_PWM_START       (0x0020)

#define MOTOR_IMPULSE_SEC     (MOTOR_IMPULSE_REV/60)

#define MOTOR_MAX_POWER       (1023)
#define MOTOR_MIN_POWER       (0)

#define INT_SPEED_BUF_LEN     (1)
#define INT_SPEED_BUF_PAUSE   (1)     // wait before accepting speed value

#define SECONDS_COUNTER       (1)

#define MOTOR_STABLE          (3)

#define MOTOR_FAULT_SECS      (0.5)     // shut motor down after fault time, in seconds
#define MOTOR_FAULT_COUNT     ((XTAL>>10)*MOTOR_FAULT_SECS)   // 10bit PWM without prescaler used

//#define MOTOR_DRV 						(10) 									// DT120马达减速比
#define MOTOR_DRV 						(46) 									// ZT120马达

extern signed int MOTOR_Power;

extern volatile TByte MOTOR_CON;

extern void MOTOR_Init(void);
extern void MOTOR_IntHandler(void);
extern void MOTOR_Start(void);
extern void MOTOR_Stop(void);
//extern void MOTOR_Fast(void);
//extern void MOTOR_Slow(void);
extern void MOTOR_SetSpeed(TReal32 speed);
extern TWord MOTOR_GetSpeed(void);  
extern TWord MOTOR_GetCurrSpeed(void);
extern TBool MOTOR_IsStable(void);
extern TWord MOTOR_GetRevolutions(void);
extern void MOTOR_ResetRevolutions(void);

#endif
