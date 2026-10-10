/*----------------------------------------------------------------------------
 *      Erweka - SVM
 *----------------------------------------------------------------------------
 *      Name:    pins.h
 *      Purpose: Controller Pin defines
 *			Controller: STM32F407VET6
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef _PINS_H
#define _PINS_H

//----------------------------------------------------------------------------
// PORT A
//----------------------------------------------------------------------------
#define PORT_A       					(GPIOA)

#define POW_CHK 							(GPIO_Pin_0)				// ADC, 电源电压检测
#define ETH_REF_CLK						(GPIO_Pin_1)				// out, 网口
#define ETH_MDIO	    				(GPIO_Pin_2)				// out, 网口

#define NU_PA3 								(GPIO_Pin_3) 				// IN，备用
#define NU_PA4 								(GPIO_Pin_4) 				// IN，备用
#define NU_PA5 								(GPIO_Pin_5) 				// IN，备用
#define NU_PA6 								(GPIO_Pin_6) 				// IN，备用

#define RMII_CRS_DV						(GPIO_Pin_7)				// out, 网口
#define M1_DIR 								(GPIO_Pin_8) 				// OUT, MOTOR
#define USB_VBC 							(GPIO_Pin_9) 				// OUT, 
#define PT100_CS1 						(GPIO_Pin_10) 			// OUT, 片选
#define USB_DM 								(GPIO_Pin_11) 			// USB接口
#define USB_DP 								(GPIO_Pin_12) 			// USB接口
#define SWD_IO 								(GPIO_Pin_13) 			// 调试接口
#define SWD_CK 								(GPIO_Pin_14) 			// 调试接口

#define NU_PA15 							(GPIO_Pin_15) 			// IN，备用

//----------------------------------------------------------------------------
// Port B
//----------------------------------------------------------------------------
#define PORT_B    						(GPIOB)

//#define LED0		    					(GPIO_Pin_0)				// in，指示灯
//#define LED1		    					(GPIO_Pin_1)				// in，指示灯
#define NOTUSED     					(GPIO_Pin_2)				// in，备用
#define HEATPWM 							(GPIO_Pin_3) 				// OUT，加热固态继电器驱动

#define NU_PB4 								(GPIO_Pin_4) 				// IN, 备用

#define FROM_CS 							(GPIO_Pin_5) 				// Flash Rom 片选
#define TXD1 									(GPIO_Pin_6)				// out, RS232 Port
#define RXD1 									(GPIO_Pin_7)				// in, RS232 Port
#define CAN_RX 								(GPIO_Pin_8) 				// CAN to LCM
#define CAN_TX 								(GPIO_Pin_9) 				// CAN to LCM

#define NU_PB10 							(GPIO_Pin_10) 			// 备用

#define RMII_TX_EN 						(GPIO_Pin_11) 			// ETHERNET
#define RMII_TXD1 						(GPIO_Pin_12) 			// ETHERNET
#define RMII_TXD0 						(GPIO_Pin_13) 			// ETHERNET
#define ETH_RESET 						(GPIO_Pin_14) 			// ETHERNET

#define NU_PB15 							(GPIO_Pin_15) 			// 备用

//----------------------------------------------------------------------------
// Port C
//----------------------------------------------------------------------------
#define PORT_C 								(GPIOC)

#define NU_PC0 								(GPIO_Pin_0)

#define ETH_MDC 							(GPIO_Pin_1) 				// ETHERNET
#define EROM_SCL 							(GPIO_Pin_2) 				// EEPROM
#define EROM_SDA 							(GPIO_Pin_3) 				// EEPROM
#define RMII_RXD0 						(GPIO_Pin_4) 				// ETHERNET
#define RMII_RXD1 						(GPIO_Pin_5) 				// ETHERNET
#define PT100_CS0 						(GPIO_Pin_6) 				// OUT, PT100片选
#define M1_PWM 								(GPIO_Pin_7) 				// OUT，电机PWM驱动
#define M1_FINT 							(GPIO_Pin_8) 				// IN,  电机过流保护输入
#define BEEP 									(GPIO_Pin_9) 				// out, Piezo
#define SPI3_SCK 							(GPIO_Pin_10) 			// FROM, AD7792
#define SPI3_MISO 						(GPIO_Pin_11) 			// FROM, AD7792
#define SPI3_MOSI 						(GPIO_Pin_12) 			// FROM，AD7792

#define NU_PC13 							(GPIO_Pin_13) 			// IN，备用

#define OSC32_IN 							(GPIO_Pin_14) 			// 时钟晶振
#define OSC32_OUT 						(GPIO_Pin_15) 			// 时钟晶振

//----------------------------------------------------------------------------
// Port D
//----------------------------------------------------------------------------
#define PORT_D 								(GPIOD)

#define NU_PD0 								(GPIO_Pin_0) 				// IN,  备用
#define NU_PD1 								(GPIO_Pin_1) 				// IN,  备用
#define NU_PD2 								(GPIO_Pin_2) 				// IN,  备用
#define NU_PD3 								(GPIO_Pin_3) 				// IN,  备用

#define RS485_TR 							(GPIO_Pin_4) 				// OUT, RS485收发转换
#define TXD2_RS485 						(GPIO_Pin_5) 				// OUT, RS485数据
#define RXD2_RS485 						(GPIO_Pin_6) 				// IN,  RS485数据
#define RS485_INT 						(GPIO_Pin_7) 				// IN,  RS485接收中断

#define TXD3 									(GPIO_Pin_8) 				// out, VINC Pin32 in  (RX)
#define RXD3 									(GPIO_Pin_9) 				// in,  VINC Pin31 out (TX)
#define VINC_DATACK 					(GPIO_Pin_10) 			// in,  VINC Pin35 out (DTR) Data acknowledge 
#define CTS3 									(GPIO_Pin_11) 			// in,  VINC Pin33 out (RTS)
#define RTS3 									(GPIO_Pin_12) 			// out, VINC Pin34 in  (CTS)
#define VINC_DATAREQ 					(GPIO_Pin_13) 			// out, VINC Pin36 in  (DSR)
#define VNC_PROG 							(GPIO_Pin_14) 			// out, VINC
#define VNC_RESET 						(GPIO_Pin_15) 			// out, VINC

//#define VINC_CTS			(RTS3)
//#define VINC_RTS			(CTS3)

//#define VENTIL			(GPIO_Pin_5) //out, Ventil

//#define VNC_RESET 	(GPIO_Pin_8) //out, VINC
//#define VNC_PROG 		(GPIO_Pin_9) //out, VINC

//----------------------------------------------------------------------------
// Port D
//----------------------------------------------------------------------------
#define PORT_E 								(GPIOE)

#define INP_B4 								(GPIO_Pin_0) 				// IN, 接近开关4
#define INP_B3 								(GPIO_Pin_1) 				// IN, 接近开关3

#define UN_PE2 								(GPIO_Pin_2) 				// IN, 备用

#define INP_B2 								(GPIO_Pin_3) 				// IN, 接近开关2
#define INP_B1 								(GPIO_Pin_4) 				// IN, 接近开关1
#define M1_PULIN 							(GPIO_Pin_5) 				// IN, 电机编码器信号捕捉

#define UN_PE6 								(GPIO_Pin_6) 				// IN, 备用
#define UN_PE7 								(GPIO_Pin_7) 				// IN, 备用

#define UN_PE8 								(GPIO_Pin_8) 				// IN, 备用
#define UN_PE9 								(GPIO_Pin_9) 				// IN, 备用
#define UN_PE10 							(GPIO_Pin_10) 			// IN, 备用
#define UN_PE11 							(GPIO_Pin_11) 			// IN, 备用
#define UN_PE12 							(GPIO_Pin_12) 			// IN, 备用
#define UN_PE13 							(GPIO_Pin_13) 			// IN, 备用
#define UN_PE14 							(GPIO_Pin_14) 			// IN, 备用
#define UN_PE15 							(GPIO_Pin_15) 			// IN, 备用

#endif
