#ifndef _MOTOR_H
#define _MOTOR_H

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

//extern signed int MOTOR_Power;

extern float Speed_Buf[SPEED_BUF_LEN+1];

extern void MOTOR_Init(void);
extern void MOTOR_IntHandler(void);
extern void MOTOR_Start(void);
extern void MOTOR_Stop(void);
extern void MOTOR_Fast(void);
extern void MOTOR_Slow(void);
extern void MOTOR_SetSpeed(float speed);
extern uint16_t MOTOR_GetSpeed(void);  
extern uint16_t MOTOR_GetCurrSpeed(void);
extern uint16_t MOTOR_GetRevolutions(void);
extern void MOTOR_ResetRevolutions(void);
extern uint8_t MOTOR_SensorReady(void);
extern void MOTOR_SetReverse(uint8_t dir);

#endif
