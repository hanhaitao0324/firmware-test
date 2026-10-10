/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    pins.h
 *      Purpose: Controller Pin defines
 *			Controller: STM32F103RBT6
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef _PINS_H
#define _PINS_H

//----------------------------------------------------------------------------
// PORT A
//----------------------------------------------------------------------------
#define PORT_A       GPIOA
#define RELAY 			(GPIO_Pin_0)	//out, ACPUMPE
#define ACPUMP			RELAY
#define PUMP				(GPIO_Pin_1)	//out, DC Pumpe, Zusatzventil zur Pumpenentlastung auf DC Pumpen Motorausgang ab FW Version 1.03
#define D_SEL	    	(GPIO_Pin_4)	//out, ANZEIGE
#define D_SCK	    	(GPIO_Pin_5)	//out, ANZEIGE
#define D_LATCH 		(GPIO_Pin_6)	//out, ANZEIGE
#define D_DATA    	(GPIO_Pin_7)	//out, ANZEIGE
#define TXD1	    	(GPIO_Pin_9)	//out, RS232 Port
#define RXD1	    	(GPIO_Pin_10)	//in, RS232 Port

//----------------------------------------------------------------------------
// Port B
//----------------------------------------------------------------------------
#define PORT_B    	(GPIOB)
//Hardware V2.01
//#define SB1		      (GPIO_Pin_0)	//in
//#define SB2		      (GPIO_Pin_1)	//in
//#define SB3		      (GPIO_Pin_2)	//in
//#define SB4		      (GPIO_Pin_3)	//in
//#define SB5		      (GPIO_Pin_4)	//in
//#define SB6		      (GPIO_Pin_5)	//in
//#define BEEP	 			(GPIO_Pin_8)	//out, Piezo

//HW V2.10.0
#define SB1		      (GPIO_Pin_0)	//in
#define SB2		      (GPIO_Pin_1)	//in
#define NOTUSED     (GPIO_Pin_2)	//in
#define SB4		      (GPIO_Pin_3)	//in
#define SB5		      (GPIO_Pin_4)	//in
#define SB6		      (GPIO_Pin_5)	//in
#define BEEP	 			(GPIO_Pin_8)	//out, Piezo

#define TXD3 					(GPIO_Pin_10) //out, VINC Pin32 in	(RX)
#define RXD3 					(GPIO_Pin_11) //in,  VINC Pin31 out	(TX)
#define VINC_DATAREQ	(GPIO_Pin_12) //out, VINC Pin36 in  (DSR)
#define CTS3 					(GPIO_Pin_13) //in,  VINC Pin33	out (RTS)
#define RTS3 					(GPIO_Pin_14) //out, VINC Pin34 in  (CTS)
#define VINC_DATACK 	(GPIO_Pin_15) //in,  VINC Pin35 out (DTR) Data acknowledge 
//#define VINC_CTS			(RTS3)
//#define VINC_RTS			(CTS3)

#define M24XXX_GPIO				       (PORT_B)
#define M24XXX_PIN_SCL           (GPIO_Pin_6) //out
#define M24XXX_PIN_SDA           (GPIO_Pin_7) //I/O

//----------------------------------------------------------------------------
// Port C
//----------------------------------------------------------------------------
#define PORT_C			(GPIOC)
//Hardware V2.01
//#define STROBE1			(GPIO_Pin_6)
//#define STROBE2			(GPIO_Pin_7)
//#define STROBE3			(GPIO_Pin_3)
//#define STROBE4			(GPIO_Pin_4)

//HW V2.10.0
#define STROBE1			(GPIO_Pin_6)
#define STROBE2			(GPIO_Pin_7)
#define STROBE3			(GPIO_Pin_8)
#define STROBE4			(GPIO_Pin_9)

#define VENTIL			(GPIO_Pin_5) //out, Ventil
//Hardware V2.01
//#define VNC_RESET 	(GPIO_Pin_8) //out, VINC
//#define VNC_PROG 		(GPIO_Pin_9) //out, VINC

//HW V2.10.0
#define VNC_PROG 		(GPIO_Pin_1) //out, VINC
#define VNC_RESET 	(GPIO_Pin_2) //out, VINC
#define SB3		      (GPIO_Pin_4) //in
#define MCP3201_CLK			(GPIO_Pin_10)	//out, 
#define MCP3201_DOUT 		(GPIO_Pin_11) //in,
#define MCP3201_CS			(GPIO_Pin_12)	//out, 
#define MCP3201_PORT		(GPIOC) 

#endif
