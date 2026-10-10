#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f4xx_conf.h"

#define XTAL (1750000)

extern volatile uint16_t MOTOR_Tick_Rev; 					// 计算速度的脉冲数
extern volatile uint16_t MOTOR_Revolution; 				// 电机实际运转的圈数
extern volatile uint32_t MOTOR_Times; 						// 电机运转的时间

// Gavin，修改为全局变量 --------------------------
extern volatile uint16_t MotorSetSpeed; 					// 马达设定转速
extern volatile uint16_t MOTOR_CurrSpeed; 				// 当前获取计算的马达转速
// 2022-10-20 周四 16:15:01

typedef struct
{
  uint8_t gotPackage:1;
  //uint8_t kbdHit:1;
  //uint8_t timeTick:1;
  uint8_t enableRxTx:1;
  uint8_t installedPrinter:1;
  uint8_t printerFailure:1;
  uint8_t motorFailure:1;
  uint8_t usbFailure:1;
  uint8_t testStart:1;
  uint8_t valueUpdate:1;
  uint8_t ekeyStatus;
} TDeviceStatus;

extern volatile uint8_t Bit_MOTOR; 						// working bit 				// 工作位
extern volatile uint8_t Bit_Fast; 							// starting bit 			// 启动位
extern volatile uint8_t Bit_Slow; 							// stopping bit 			// 停止位
extern volatile uint8_t Bit_OneRev; 						// one rotation bit 	// 一个站
extern volatile uint8_t Bit_Stab; 							// stabilization bit 	// 转动
extern volatile uint8_t Bit_Pulse; 						// got pulse bit 			// 有脉冲
extern volatile uint8_t Bit_1sec; 							// 1 sec bit 					// 1秒
extern volatile uint8_t Bit_SnsrStop; 					// sensor stop bit 		// 编码器停止

// PA8，电机方向继电器控制，Motor Reverse ------------------
#define MOT_DIR0 		GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define MOT_DIR1 		GPIO_SetBits(GPIOA,GPIO_Pin_8)

// PC8，电机电流超限报错 -----------------------------------
#define MOT_FAULT 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
//#define MOTOR_PWM_MAX	      (0x03F0)
//#define MOTOR_PWM_MIN	      (0x0000)
//#define MOTOR_PWM_START       (0x0020)
#define MOTOR_PWM_MAX	      (0x03E7)
#define MOTOR_PWM_MIN	      (0x0000)
#define MOTOR_PWM_START       (0x0020)

#define MOTOR_IMPULSE_SEC     (MOTOR_IMPULSE_REV/60)

#define DEVICE_SVM
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


extern void TIM9_CH1_Cap_Init(u16 arr,u16 psc);

#endif
