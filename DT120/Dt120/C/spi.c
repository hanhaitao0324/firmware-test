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
  SPI_Select(SPI_NONE);
  SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA)|(1<<CPOL);
//  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
}

void SPI_DeInit(void)
{
  SPI_Select(SPI_NONE);
  SPCR=0;
//  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
}


//------------------------------------------------------------------------------
// SPI_Select - select SPI slave(Ñ¡ÔñSPI´Ó»ú)
//------------------------------------------------------------------------------
void SPI_Select(TByte device)
{
  macBitOn(PORT_CS_ADC,pinADC_CS);
  macBitOn(PORT_CS_FRAM,pinFM_CS);
  SPCR&=~(1<<SPR0);
  switch (device)
  {
    case SPI_ADC:
      SPCR|=(1<<SPR0);
      macBitOff(PORT_CS_ADC,pinADC_CS);
      break;
    case SPI_FRAM: 
      //SPCR&=~(1<<SPR0);
      //SPCR|=(1<<SPR1);
      macBitOff(PORT_CS_FRAM,pinFM_CS);
      break;
    case SPI_LED:
      if(globalPCBType == 2)                // DM13H
      {
        SPCR &= ~(1<<CPOL);
    	SPCR &= ~(1<<CPHA);
      }
      break;
  }
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
