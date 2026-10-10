#ifndef _SPI_H
  #define _SPI_H

#include "defines.h"

#define SPI_EMPTY_BYTE  (0xff)

// SPI_Init - init module (SPI mode 3)
extern void SPI_Init(void);
// SPI_DeInit - Release SPI from Ports
extern void SPI_DeInit(void);
// SPI_IOByte - I/O a byte on SPI
extern TByte SPI_IOByte(TByte data);

#endif
