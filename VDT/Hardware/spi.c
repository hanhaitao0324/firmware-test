 /*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    spi.c
 *      Purpose: Spi driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"
#include "defines.h"
#include "pins.h"
#include "spi.h"
#include "led.h"

extern u8 globalPCBType;

/*******************************************************************************
* Function Name  : SPI1_Init
* Description    : Initialize SPI1 for LED Display
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Init(void) 
{	
	SPI_InitTypeDef SPI_InitStructure;
  
	SPI_I2S_DeInit(SPI1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	// Enable SPI1 peripheral
	SPI_StructInit(&SPI_InitStructure);
	SPI_Cmd(SPI1, DISABLE);
	
	if(globalPCBType == PCB_TLC5927)
	{
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //72MHz / mit Teiler 16 geht es nicht ?
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
	}
	else
	if(globalPCBType == PCB_DM13H)
	{
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //72MHz / mit Teiler 16 geht es nicht ?
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
	}
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_CalculateCRC(SPI1, DISABLE);

	// Enable SPI1
	SPI_Cmd(SPI1, ENABLE);
}

//------------------------------------------------------------------------------
// SPI_DeInit - 
//------------------------------------------------------------------------------
void SPI1_DeInit(void)
{
}
	
//------------------------------------------------------------------------------
// SPI_IOByte - send/receive byte via SPI
//------------------------------------------------------------------------------
uint16_t SPI1_IOByte(uint16_t data)
{ 
	SPI_I2S_SendData(SPI1, data);
	//SPI_I2S_ReceiveData(SPI1);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {}
    
  return 0;
}
