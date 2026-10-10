#ifndef _SPI_H
  #define _SPI_H

#include "defines.h"

#define SPI_EMPTY_BYTE  (0xff)

enum SPI_DEVICES
  {
    SPI_ADC,
    SPI_FRAM,
    SPI_LED,
    SPI_NONE
  };

extern TBool SPI_Busy;

// SPI_Init - init module (SPI mode 3)
extern void SPI_Init(void);
// SPI_DeInit - Allow PIN Access
extern void SPI_DeInit(void);
// SPI_IOByte - I/O a byte on SPI
extern TByte SPI_IOByte(TByte data);
// SPI_Select - select a SPI device
extern void SPI_Select(TByte device);

#endif
