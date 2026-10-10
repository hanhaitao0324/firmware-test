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

#define pinFM_CS	0
#define pinSCK		1
#define pinMOSI		2
#define pinMISO		3
#define pinM2_Out 	4
#define pinM1_Out	5
#define pinM3_Step      6
#define pinSound 	7

#define iniDDRB		((1<<pinFM_CS)|(1<<pinSCK)|(1<<pinMOSI)|(1<<pinM2_Out)|(1<<pinM1_Out)|(1<<pinM3_Step)|(1<<pinSound))
#define iniPORTB	((1<<pinFM_CS)|(1<<pinMISO))

//===================================================================
// PORT C
//===================================================================

#define pinKBIn3	0
#define pinKBIn2	1
#define pinKBIn1        2
#define pinKBOut4	3
#define pinKBOut3	4
#define pinKBOut2	5
#define pinKBOut1	6
#define pinUSBWR        7

#define iniDDRC		((1<<pinKBIn3)|(1<<pinKBIn2)|(1<<pinKBIn1)|(1<<pinUSBWR))
#define iniPORTC	(0x00)

//===================================================================
// PORT D
//===================================================================

#define pinRS485_Int    0
#define pin485TR        1
#define pinRXD1		2
#define pinTXD1		3
#define pinStrobe4 	4
#define pinStrobe3	5
#define pinStrobe2	6
#define pinStrobe1	7

#define iniDDRD		((1<<pinTXD1)|(1<<pin485TR)|(1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))
#define iniPORTD	((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))

//===================================================================
// PORT E
//===================================================================

#define pinRXD0		0
#define pinTXD0		1
#define pinUSBTXE       2
#define pinM3_Sens1	3
#define pinM3_Sens2	4
#define pinUSBRXF       5
#define pinM1_Pul       6
#define pinMFault       7

#define iniDDRE		((1<<pinTXD0))
#define iniPORTE	((1<<pinRXD0)|(1<<pinUSBRXF)|(1<<pinUSBTXE))

//===================================================================
// PORT F
//===================================================================

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

//===================================================================
// PORT G
//===================================================================

#define pinD_D_Sel  0       // 由输出引脚改为输入引脚，以判断使用DM133或DM13H
#define pinD_Latch  1
#define	pinUSBRD    2
#define pinM3_Dir   3
#define pinM3_Reset 4

#define iniDDRG0    ((1<<pinD_D_Sel)|(1<<pinD_Latch)|(1<<pinUSBRD)|(1<<pinM3_Dir)|(1<<pinM3_Reset))
#define iniDDRG1    ((1<<pinD_Latch)|(1<<pinUSBRD)|(1<<pinM3_Dir)|(1<<pinM3_Reset))
#define	iniPORTG    ((1<<pinD_D_Sel))

//===================================================================

#define PORT_USB_DATA   PORTA
#define PIN_USB_DATA    PINA
#define DIR_USB_DATA    DDRA

#define PORT_SPI        PORTB
#define PIN_SPI         PINB
#define PORT_MOTOR	PORTB
#define PORT_BEEPER	PORTB
#define PORT_DROPPER    PORTB
#define PORT_CS_FRAM	PORTB

#define PORT_KBD        PORTC
#define PIN_KBD         PINC
#define PORT_USB_WR     PORTC
#define DIR_USB_WR      DDRC

#define PORT_RS485_DIR	PORTD
#define PORT_LED        PORTD

#define PIN_USB_STS	PINE
#define DIR_USB_STS	DDRE
#define PIN_MOTOR_ALR   PINE
#define PIN_STEP_STOP   PINE

#define PORT_CS_ADC	PORTF
#define PIN_DROP_IN     PINF
#define PIN_PCB_VER     PINF

#define PORT_LED_SEL    PORTG
#define PIN_LED_SEL     PING
#define PORT_LED_L      PORTG
#define PORT_USB_RD     PORTG
#define PORT_STEP_CTL   PORTG
#define DIR_USB_RD      DDRG

#endif
