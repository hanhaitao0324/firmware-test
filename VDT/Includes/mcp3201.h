/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    mcp3201.h
 *      Purpose: ADC Microchip
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
// Define to prevent recursive inclusion -------------------------------------
#ifndef _MCP3201_H
#define _MCP3201_H

//#include "stm32f10x_gpio.h"    // Device header
#include "defines.h"
#include "pins.h"

#define ADC_HANDLER_UPD_RATE    (250)		// Thread update in ms

typedef struct
{
  float     VACM;
  float     VACB;
	int 			Vacuum;	
	uint32_t  CRC32;
} MCP3201_Data_s;


// Function prototypes
void MCP3201_Init(void);
void set_MCP3201_default(void);
uint16_t MCP3201_read(void);
void ADC_Handler(void);
uint16_t ADC_get_val(void);
unsigned int GetVacAvrg(void);
unsigned int getadavrg(void);
void set_VacM(float val);
void set_VacB(float val);

#endif // _MCP3201_H
