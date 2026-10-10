#include "pins.h"
#include "platform.h"
#include "spi.h"

TBool SPI_Busy=false;
/*
7.SPIE: SPI中断使能，默认0，禁止
6.SPE: SPI使能，默认0，禁止
5.DORD: 数据次序，默认0，高位先发
4.MSTR: 主/从选择，默认0，从机模式
3.CPOL: 时钟极性，默认0，空闲时SCK为低电平
2.CPHA: 时钟相位，默认0，数据在SCK上升沿采样
1.SPR1: 
0.SPR0: SPI时钟速率
SP12X = 0;
00 fosc/4
01 fosc/16
10 fosc/64
11 fosc/128
SP12X = 1, 速率加倍
*/
//------------------------------------------------------------------------------
// SPI_Init - SPI initialization code
//------------------------------------------------------------------------------
void SPI_Init(void)
{
  SPI_Select(SPI_NONE);
  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR0);
//  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
}

//------------------------------------------------------------------------------
// 禁止SPI操作
//------------------------------------------------------------------------------
void SPI_DeInit(void)
{
  SPI_Select(SPI_NONE);
  SPCR=0;
//  SPCR=(1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
}

//------------------------------------------------------------------------------
// SPI_Select - select SPI slave(选择SPI从机)
//------------------------------------------------------------------------------
void SPI_Select(TByte device)
{
  macBitOn(PORT_CS_ADC,pinADC_CS0);
  macBitOn(PORT_CS_ADC,pinADC_CS1);
  macBitOn(PORT_CS_FRAM,pinFM_CS);
  SPCR&=~(1<<SPR0);
  switch (device)
  {
    case SPI_ADC0:
      SPCR|=(1<<SPR0);
      macBitOff(PORT_CS_ADC,pinADC_CS0);
      break;
    case SPI_ADC1:
      SPCR|=(1<<SPR0);
      macBitOff(PORT_CS_ADC,pinADC_CS1);
      break;
    case SPI_FRAM: 
      //SPCR&=~(1<<SPR0);
      //SPCR|=(1<<SPR1);
      macBitOff(PORT_CS_FRAM,pinFM_CS);
      break;
    case SPI_LED:
    	SPCR &= ~(1<<CPOL);
    	SPCR &= ~(1<<CPHA);
    	//SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
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
