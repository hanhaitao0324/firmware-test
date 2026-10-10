#ifndef _PINS_H
  #define _PINS_H

#define macPinOn(PORT,PIN)\
PORT|=(0x01<<PIN)

#define macPinOff(PORT,PIN)\
PORT&=~(0x01<<PIN)

//===================================================================
// PORT A
//===================================================================

#define pinNC_A0	0
#define pinKB1          1
#define pinKB2          2
#define pinKB3          3
#define pinKB4          4
#define pinKB6          5
#define pinPCB_Version  6
#define pinNC_A7        7

#define iniDDRA	    (0x00)
//#define iniPORTA    ((1<<pinPCB_Version))
#define iniPORTA    ((1<<pinPCB_Version)|(1<<pinNC_A7)) 		// Gavin£¬2021-12-22 ÐÇÆÚÈý 14:59:44

//===================================================================
// PORT B
//===================================================================

#define pinD_sel	0
#define pinSCK		1
#define pinMOSI		2
#define pinMISO		3
#define pinNC_B4 	4
#define pinReverse      5
#define pinM_Out	6
#define pinSound 	7

#define iniDDRB		((1<<pinD_sel)|(1<<pinSCK)|(1<<pinMOSI)|(1<<pinReverse)|(1<<pinM_Out)|(1<<pinSound))
#define iniPORTB	((1<<pinD_sel)|(1<<pinMISO))

//===================================================================
// PORT C
//===================================================================

#define pinUSBD0	0
#define pinUSBD1	1
#define pinUSBD2	2
#define pinUSBD3	3
#define pinUSBD4	4
#define pinUSBD5	5
#define pinUSBD6	6
#define pinUSBD7	7

#define iniDDRC		(0x00)
#define iniPORTC	(0x00)

//===================================================================
// PORT D
//===================================================================

#define pinNC_D0        0
#define pinUSBRXF       1
#define pinSensor       2
#define pinM_Pul	3
#define pinNC_D4        4
#define pinLED_PCB      5
#define pinLED1         6
#define pinKB5          7

#define iniDDRD		((1<<pinLED_PCB)|(1<<pinLED1))
#define iniPORTD	((1<<pinSensor)|(1<<pinLED1))

//===================================================================
// PORT E
//===================================================================

#define pinRXD0		0
#define pinTXD0		1
#define pinNC_E2	2
#define pinNC_E3        3
#define pinNC_E4	4
#define pinNC_E5        5
#define pinMFault       6
#define pinD_latch      7

#define iniDDRE		((1<<pinTXD0)|(1<<pinD_latch))
#define iniPORTE	((1<<pinRXD0))

//===================================================================
// PORT F
//===================================================================

#define pinStrobe1 	0
#define pinStrobe2	1
#define pinStrobe3	2
#define pinStrobe4	3
#define pinTCK	        4
#define pinTMS	        5
#define pinTDO	        6
#define pinTDI	        7

#define iniDDRF		((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))
#define iniPORTF	((1<<pinStrobe4)|(1<<pinStrobe3)|(1<<pinStrobe2)|(1<<pinStrobe1))

//===================================================================
// PORT G
//===================================================================

#define pinUSBWR        0
#define	pinUSBRD	1
#define pinUSBTXE       2  
#define pin485TR        3
#define pinNC_G4        4

#define iniDDRG		((1<<pinUSBWR)|(1<<pinUSBRD)|(1<<pin485TR))
#define	iniPORTG	((1<<pinUSBTXE))

//===================================================================
// Ports
//===================================================================

#define PIN_KBD             PINA
#define PIN_PCB_VER         PINA

#define PORT_LED_SEL        PORTB
#define PORT_SPI            PORTB
#define PIN_SPI             PINB
#define PORT_REVERSE	    PORTB
#define PORT_MOTOR	    PORTB
#define PORT_BEEPER	    PORTB

#define PORT_USB_DATA       PORTC
#define DIR_USB_DATA        DDRC
#define PIN_USB_DATA        PINC

#define PIN_USB_RXF         PIND
#define DIR_USB_RXF         DDRD
#define PIN_SENSOR          PIND
#define PORT_LED_1          PORTD
#define PIN_KBD_5           PIND

#define PIN_MOTOR_ALR       PINE
#define PORT_LED_L          PORTE

#define PORT_LED            PORTF

#define PORT_USB_CTRL       PORTG
#define DIR_USB_CTRL        DDRG
#define PIN_USB_TXE         PING
#define DIR_USB_TXE         DDRG
#define PORT_RS485_DIR      PORTG

#endif
