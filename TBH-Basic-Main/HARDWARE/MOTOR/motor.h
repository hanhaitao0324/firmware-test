#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f4xx_conf.h"
#include "rtc.h"

#define SPI_SPEED 4

#define ST_ENA1_H() 			GPIO_SetBits(GPIOE,GPIO_Pin_8) 		// ST_ENA1置高，MOSFET输出关闭
#define ST_ENA1_L() 			GPIO_ResetBits(GPIOE,GPIO_Pin_8)

#define SPI_ST_SS1_H() 		GPIO_SetBits(GPIOE,GPIO_Pin_13) 	// SPI_ST_SS1置高，SPI片选禁止
#define SPI_ST_SS1_L() 		GPIO_ResetBits(GPIOE,GPIO_Pin_13)

#define SPI_ST_SCK_H() 		GPIO_SetBits(GPIOE,GPIO_Pin_12)
#define SPI_ST_SCK_L() 		GPIO_ResetBits(GPIOE,GPIO_Pin_12) // SPI_ST_SCK置低，SPI时钟常态

#define SPI_ST_MOSI_H() 	GPIO_SetBits(GPIOE,GPIO_Pin_11)
#define SPI_ST_MOSI_L() 	GPIO_ResetBits(GPIOE,GPIO_Pin_11) // SPI_ST_MOSI置低

#define SPI_ST_MISO() 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)

// ===== TMC5160 register set =====

#define TMC5160_GCONF          0x00 		// 全局配置标志
#define TMC5160_GSTAT          0x01 		// 全局状态标志
#define TMC5160_IFCNT          0x02 		// 接口传输计数器
#define TMC5160_SLAVECONF      0x03
#define TMC5160_INP_OUT        0x04
#define TMC5160_X_COMPARE      0x05
#define TMC5160_OTP_PROG       0x06
#define TMC5160_OTP_READ       0x07
#define TMC5160_FACTORY_CONF   0x08         // 0：位置模式  1：速度模式到+VMAX 2：速度模式到-VMAX 3：保持模式
#define TMC5160_SHORT_CONF     0x09
#define TMC5160_DRV_CONF       0x0A
#define TMC5160_GLOBAL_SCALER  0x0B
#define TMC5160_OFFSET_READ    0x0C
#define TMC5160_IHOLD_IRUN     0x10
#define TMC5160_TPOWERDOWN     0x11
#define TMC5160_TSTEP          0x12
#define TMC5160_TPWMTHRS       0x13
#define TMC5160_TCOOLTHRS      0x14
#define TMC5160_THIGH          0x15

#define TMC5160_RAMPMODE       0x20
#define TMC5160_XACTUAL        0x21     // 实际电机位置，在位置模式下，修改内容将启动一个运动。该值通常只应在归零时修改
#define TMC5160_VACTUAL        0x22
#define TMC5160_VSTART         0x23
#define TMC5160_A1             0x24
#define TMC5160_V1             0x25
#define TMC5160_AMAX           0x26
#define TMC5160_VMAX           0x27
#define TMC5160_DMAX           0x28
#define TMC5160_D1             0x2A
#define TMC5160_VSTOP          0x2B
#define TMC5160_TZEROWAIT      0x2C
#define TMC5160_XTARGET        0x2D

#define TMC5160_VDCMIN         0x33
#define TMC5160_SWMODE         0x34
#define TMC5160_RAMPSTAT       0x35
#define TMC5160_XLATCH         0x36
#define TMC5160_ENCMODE        0x38
#define TMC5160_XENC           0x39
#define TMC5160_ENC_CONST      0x3A
#define TMC5160_ENC_STATUS     0x3B
#define TMC5160_ENC_LATCH      0x3C
#define TMC5160_ENC_DEVIATION  0x3D

#define TMC5160_MSLUT0         0x60
#define TMC5160_MSLUT1         0x61
#define TMC5160_MSLUT2         0x62
#define TMC5160_MSLUT3         0x63
#define TMC5160_MSLUT4         0x64
#define TMC5160_MSLUT5         0x65
#define TMC5160_MSLUT6         0x66
#define TMC5160_MSLUT7         0x67
#define TMC5160_MSLUTSEL       0x68
#define TMC5160_MSLUTSTART     0x69
#define TMC5160_MSCNT          0x6A
#define TMC5160_MSCURACT       0x6B
#define TMC5160_CHOPCONF       0x6C
#define TMC5160_COOLCONF       0x6D
#define TMC5160_DCCTRL         0x6E
#define TMC5160_DRVSTATUS      0x6F
#define TMC5160_PWMCONF        0x70
#define TMC5160_PWMSCALE       0x71
#define TMC5160_PWM_AUTO       0x72
#define TMC5160_LOST_STEPS     0x73


extern u8 MotSpiStatus;
extern int nullwert;
extern float factor ;
extern int step;
extern int blops;
extern float NomiLength;
extern float DiamOffset;
extern float mmfactor;
extern int ContactForce;
extern int Sensitivity;
extern float FastMoveSP;
extern float DiaMeasSpeed;

extern long int ad7190Result;

extern void MOTOR_Init(void);

extern void motor_set_vmax(u32 velocity);
extern void motor_stop(void);
extern void motor_moveTo(int target, u32 speed);
extern void motor_rotate(int velocity);

void backreset(void);
void tbhnull(void);
unsigned char dmmess(void);
float GetNomiLength(void);
void mekabef(void);

extern void sendMotDat(u8 r, u32 dat);
extern u32 readMotDat(u8 r, u8 *ss);

u8 Encoder_deviation_detected(void);
void Encoder_clear_deviation_warning(void);

#define NwtFak                (107374.1824f/factor)     // 一牛顿对应的ad变化值

#endif
