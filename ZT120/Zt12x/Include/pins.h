#ifndef _PINS_H
  #define _PINS_H

//===================================================================
// PORT A - extension port
//===================================================================

#define pinUSBD0	0
#define pinUSBD1	1
#define pinUSBD2	2
#define pinUSBD3	3
#define pinUSBD4	4
#define pinUSBD5	5
#define pinUSBD6	6
#define pinUSBD7	7

#define iniDDRA		(0xFF)
#define iniPORTA	(0x00)

//===================================================================
// PORT B
//===================================================================

#define pinFM_CS 		0
#define pinSCK 			1
#define pinMOSI 		2
#define pinMISO 		3
#define pinM2_Out 	4
#define pinM1_Out 	5
//#define pinM3_Step 	6
#define pinRelay 		6 									// 2017-10-24 星期二 21:21:09, 新增加
#define pinSound 		7

#define iniDDRB		((1<<pinFM_CS)|(1<<pinSCK)|(1<<pinMOSI)|(1<<pinM2_Out)|(1<<pinM1_Out)|(1<<pinRelay)|(1<<pinSound))
#define iniPORTB	((1<<pinFM_CS)|(1<<pinMISO))

//===================================================================
// PORT C
//===================================================================
/*
#define pinKBIn3 		0
#define pinKBIn2 		1
#define pinKBIn1 		2
#define pinKBOut4 	3
#define pinKBOut3 	4
#define pinKBOut2 	5
#define pinKBOut1 	6
#define pinUSBWR 		7

#define iniDDRC		((1<<pinKBIn3)|(1<<pinKBIn2)|(1<<pinKBIn1)|(1<<pinUSBWR))
#define iniPORTC	(0x00)
*/

#define pinPCB_Version  0 		// 输入
#define pinKBIn5 		1 				// 输入
#define pinADC_CS1 	2 				// 输出
#define pinADC_CS0 	3 				// 输出
#define pinKBIn6 		4 				// 输入
#define pinUSBTXE 	5 				// 输入
#define pinUSBRXF 	6 				// 输入
#define pinUSBWR 		7 				// 输出

#define iniDDRC		((1<<pinADC_CS1)|(1<<pinADC_CS0)|(1<<pinUSBWR))
#define iniPORTC	((1<<pinPCB_Version)|(1<<pinADC_CS1)|(1<<pinADC_CS0)|(1<<pinUSBRXF)|(1<<pinUSBTXE))

//===================================================================
// PORT D
//===================================================================
/*
#define pinRS485_Int 					0
#define pin485TR 							1
#define pinRXD1 							2
#define pinTXD1 							3
#define pinStrobe4 						4
#define pinStrobe3 						5
#define pinStrobe2 						6
#define pinStrobe1 						7

#define iniDDRD		((1<<pinTXD1)|(1<<pin485TR)|(1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))
#define iniPORTD	((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))
*/

#define pinRS485_Int 					0
#define pin485TR 							1
#define pinRXD1 							2
#define pinTXD1 							3
#define pinNC4 								4
#define pinNC3 								5
#define pinNC2 								6
#define pinLED 								7

#define iniDDRD		((1<<pinTXD1)|(1<<pin485TR)|(1<<pinLED))
#define iniPORTD	((1<<pinLED))

//===================================================================
// PORT E
//===================================================================
/*
#define pinRXD0 							0
#define pinTXD0 							1
#define pinUSBTXE 						2
#define pinM3_Sens1 					3
#define pinM3_Sens2 					4
#define pinUSBRXF 						5
#define pinM1_Pul 						6
#define pinMFault 						7

#define iniDDRE		((1<<pinTXD0))
#define iniPORTE	((1<<pinRXD0)|(1<<pinUSBRXF)|(1<<pinUSBTXE))
*/

#define pinD_Latch 						0
#define pinKBIn4 							1
#define pinKBIn2 							2
#define pinKBIn1 							3
#define pinENC4 							4
#define pinENC5 							5
#define pinM1_Pul 						6
#define pinMFault 						7

#define iniDDRE		((1<<pinD_Latch))
#define iniPORTE	(0x00)

//===================================================================
// PORT F
//===================================================================
/*
#define pinPCB_Version  0
#define pinM2_Close     1
#define pinM2_Open      2
#define pinADC_CS   	3
#define pinJTAG_TCK	4
#define pinJTAG_TMS	5
#define pinJTAG_TDO	6
#define pinJTAG_TDI	7

#define iniDDRF		((1<<pinADC_CS))
#define iniPORTF	((1<<pinPCB_Version)|(1<<pinADC_CS))
*/

#define pinStrobe1 	0
#define pinStrobe2 	1
#define pinStrobe3 	2
#define pinStrobe4 	3
#define pinJTAG_TCK	4
#define pinJTAG_TMS	5
#define pinJTAG_TDO	6
#define pinJTAG_TDI	7

#define iniDDRF		((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))
#define iniPORTF	((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))

//===================================================================
// PORT G
//===================================================================
/*
#define pinD_D_Sel	0
#define pinD_Latch	1
#define	pinUSBRD	2
#define pinM3_Dir       3
#define pinM3_Reset	4

#define iniDDRG		((1<<pinD_D_Sel)|(1<<pinD_Latch)|(1<<pinUSBRD)|(1<<pinM3_Dir)|(1<<pinM3_Reset))
#define	iniPORTG	((1<<pinD_D_Sel))
*/

#define pinDNC0 		0
#define pinDNC1 		1
#define	pinUSBRD 		2
#define pinKBIn3 		3
#define pinM2_Close	4

#define iniDDRG		((1<<pinUSBRD))
#define	iniPORTG	(0x00)

//===================================================================

#define PORT_USB_DATA 				PORTA
#define PIN_USB_DATA 					PINA
#define DIR_USB_DATA 					DDRA

#define PORT_SPI 							PORTB
#define PIN_SPI 							PINB
#define PORT_MOTOR 						PORTB
#define PORT_BEEPER 					PORTB
#define PORT_DROPPER 					PORTB
#define PORT_CS_FRAM 					PORTB
#define PORT_RELAY 						PORTB
#define PIN_RELAY 						PINB

#define PIN_PCB_VER 					PINC
#define PIN_KBD56 						PINC
//#define PORT_KBD        PORTC
//#define PIN_KBD         PINC
#define PORT_CS_ADC 					PORTC
#define PIN_USB_STS 					PINC
#define DIR_USB_STS 					DDRC
#define PORT_USB_WR 					PORTC
#define DIR_USB_WR 						DDRC

#define PORT_RS485_DIR 				PORTD
#define PORT_LED0 						PORTD 		// PCB板上贴片LED指示灯
//#define PORT_LED        PORTD

#define PORT_LED_L 						PORTE
#define PIN_KBD124 						PINE
//#define PIN_USB_STS	PINE
//#define DIR_USB_STS	DDRE
#define PIN_MOTOR_ALR 				PINE
//#define PIN_STEP_STOP   PINE

//#define PORT_CS_ADC 		PORTF
//#define PIN_DROP_IN     PINF
//#define PIN_PCB_VER     PINF
#define PORT_LED 							PORTF 		// 数码管驱动

//#define PORT_LED_SEL    PORTG
//#define PORT_LED_L      PORTG
#define PORT_USB_RD 					PORTG
//#define PORT_STEP_CTL   PORTG
#define DIR_USB_RD 						DDRG
#define PIN_KBD3 							PING
#define PIN_DROP_IN 					PING

#endif
