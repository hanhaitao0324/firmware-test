/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    io.c
 *      Purpose: IO DRIVER
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_gpio.h"    // Device header
#include "defines.h"
#include "pins.h"
#include "io.h"

/*******************************************************************************
* Function Name  : io_Init
* Description    : io configuration for General i/o operations
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin   = PUMP | RELAY;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PORT_A, &GPIO_InitStructure);
	
	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin   = VENTIL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PORT_C, &GPIO_InitStructure);	
  
  // GPIO Configuration
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin 	= PCBSW0;
  GPIO_Init(PORT_C, &GPIO_InitStructure);	
  
}

/*******************************************************************************
* Function Name  : SetDcPump
* Description    : DC Pumpe, Zusatzventil zur Pumpenentlastung auf DC Pumpen Motorausgang ab FW Version 1.03
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IO_SetDcPump(BitAction val)
{
  GPIO_WriteBit(PORT_A, PUMP, val);		
}

/*******************************************************************************
* Function Name  : SetAcPump
* Description    : AC Pumpe
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IO_SetAcPump(BitAction val)
{
  GPIO_WriteBit(PORT_A, RELAY, val);		
}

/*******************************************************************************
* Function Name  : SetVentil
* Description    : Ventil, Entlüftungsventil 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IO_SetVentil(BitAction val)
{
  GPIO_WriteBit(PORT_C, VENTIL, val);		
}
