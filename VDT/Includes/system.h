/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    system.h
 *      Purpose: system defines & constants
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
// Define to prevent recursive inclusion -------------------------------------
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "const.h"
#include "mcp3201.h"
#include "timedate.h"

/**
 * The STM32 factory-programmed UUID memory.
 * Three values of 32 bits each starting at this address
 * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
 */
#define STM32_UUID ((uint32_t *)0x1FFFF7E8)

extern const char DEVICE_VERSION[];
extern const char DEVICE_BETAVERSION[];
extern const char DEVICE_SONDERVERSION[];
extern const char DEVICE_FWDATE[];
#define SERIAL_NUMBER_LEN   6

#define PRESSURE_RISE				600	// Druckanstieg Timeout sec x10
#define MAXVACUM						900 // 900mbar
#define MAXVACUM_EXISCCATOR 700 // Exisccator Size > 250mm

#define RAM_KEY_ADDR        (0x20002000)
#define BOOTLOADER_KEY1     (0x12345678)
#define BOOTLOADER_KEY2     (0x43211234)

#define SILENT_MODE 2
#define VERBOSE_MODE 1
#define NORMAL_EXECUTION 0

#define COMMUNICATION_UART			USART1
#define COMMUNICATION_BAUD			UART1_BAUD

typedef struct
{
  char      SerialNumber[SERIAL_NUM_MAX_LEN+2];//8
////  uint8_t			  DeviceType;//1
////  uint16_t     	Password;//2
	uint8_t 	  Language;//1
//	uint8_t				NoLicens;//1
//	char			messTime[6];//6
//	int				messZeitL;//4
//	int				messZeitH;//4
//	int				tmode;//4
//	uint32_t				FullDays;//4
//	uint32_t				Periods;//4
//	uint32_t				AktPeriods;//4
//	uint32_t				PrinterFrames;//4
//	uint8_t				Autoprint;//1
//	int 			Penetration;//4
////	uint32_t				DaysLeft;//4
////	double		dateoffset;//8
////	int				LastDay;//4
////	DTIM 			LastDate;//6
	uint32_t	Exisccator_size;//4
	uint8_t		Penetration_inverse;//1
//	
  uint8_t   Empty[7];
  uint32_t  CRC32;
} Platform_Data_s;

typedef struct{
	 Platform_Data_s 	Platform_Data;
	 MCP3201_Data_s		MCP_3201_Data;
}SYSTEM_Data_s;

extern SYSTEM_Data_s SYSTEM;
extern const MCP3201_Data_s MCP3201_Default;

extern Platform_Data_s *Platform_Data;
extern MCP3201_Data_s	*ADC_Data;

void VDT_System_init(void);
void StartBootloader(void);
void Display_Handler(void);
void logo(void);
void getid(char *buf);
void showID(void);
uint8_t EDIT_Advanced(char *title, char *tStr, uint8_t len, bool sound);
bool ENGINEERING_DevSNum(void);

enum Chancel_Reason
{
  CHANCEL_NO,
  CHANCEL_USER,
  CHANCEL_PRESS_TIMEOUT,
  CHANCEL_FINISH
};

//typedef struct DateTime {
//	unsigned int day;
//	unsigned int month;
//	unsigned int year;
//	unsigned int hour;
//	unsigned int min;
//	unsigned int sec;
//}DTIM;

//#define PRELL 1000
//#define ENDPCM 'Y'

#define ADRSERNUM 1		    //8 byte
#define ADR_SPRACHE 9		  //1 byte
#define ADRNOLICENSE 10   //4 byte
//#define ADRMESSTIME 11	//6 byte
#define ADRVACUUM 17		  //4 byte
#define ADRMESSZEITL 21	  //4 byte
#define ADRMESSZEITH 25	  //4 byte
#define ADRTMODE 29	      //4 byte, Time Mode (Hour/Minute)
#define ADRFULLDAYS 33	  //4 byte
#define ADRPERIODS 37     //4 byte
#define ADRAKTPERIOD 41   //4 byte
#define ADRVACM 45			  //8 byte
#define ADRVACB 53			  //8 byte

//#define ADRPRINTERFRAMES 	61  //4 byte
#define ADRAUTOPRINT 	65        //1 byte
#define ADRPENETRATION 66 	    //4 byte

#define ADRDAYSLEFT 70	        //4 byte
#define ADRDATEOFFSET 74	      //8 byte
#define ADRLASTDAY 82	          //4 bytes

#define ADRLASTDATE 86	        //6 bytes (packed)
#define ADRLASTUPDATE 92	      //8 byte

#define ADREXSICCATOR_SIZE 100  //4 byte
#define ADRPENETRATION_INVERSE 104 	    //1 byte

#define ADRSIGNATUR 0x7FFF	    //1 byte
#define SIGNATUR 0x57				    //1 byte

//double JulDat(DTIM dtim);
//DTIM GregDat(double JulDat);

#endif


