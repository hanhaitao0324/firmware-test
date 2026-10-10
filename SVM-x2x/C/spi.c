#include "pins.h"
#include "platform.h"
#include "spi.h"
#include "global.h"

TBool SPI_Busy=false;

//------------------------------------------------------------------------------
// SPI_Init - SPI initialization code
//------------------------------------------------------------------------------
void SPI_Init(void)
{
  //SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA)|(1<<CPOL);
//  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);

	// 7 SPIE: 
	// 6 SPE: SPI EN
	// 5 DORD: 数据次序
	// 4 MSTR: 主从模式
	// 3 CPOL: 时钟极性
  // 2 CPHA: 时钟相位
	// 1 SPR1: 时钟速率选择
	// 0 SPR0: 时钟速率选择
	if(globalPCBType != PCB_DM13H-1) 			// PCB_OLD,PCB_NEW
	{
		SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA)|(1<<CPOL); 		// 使能，主模式，
	}
	else 																	// PCB_DM13H
	{
		SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0); 	//|(1<<CPHA)|(1<<CPOL); 		// 使能，主模式，
	}
}

void SPI_DeInit(void)
{
  SPCR=0x00;
}

//------------------------------------------------------------------------------
// SPI_IOByte - send/receive byte via SPI
//------------------------------------------------------------------------------
TByte SPI_IOByte(TByte data)
{ 
  //read status register
  SPSR;
  //and data register to clear the interrupt on data transfer end
  SPDR;
  //transmit data byte
  SPDR=data;
  //wait for end of transmission
  while (!(SPSR&(1<<SPIF)))
    continue;
  //return received byte
  return SPDR;
}
