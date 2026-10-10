/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    spi.h
 *      Purpose: spi driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
// Define to prevent recursive inclusion -------------------------------------
#ifndef _SPI_H
#define _SPI_H

#include "defines.h"

#define SPI_EMPTY_BYTE  (0xff)

//enum SPI_DEVICES
//  {
//    SPI_LED,
//    SPI_NONE
//  };

extern bool SPI1_Busy;

// SPI_Init - init module (SPI mode 3)
extern void SPI1_Init(void);
// SPI_DeInit - Allow PIN Access
extern void SPI1_DeInit(void);
// SPI_IOByte - I/O a byte on SPI
extern uint16_t SPI1_IOByte(uint16_t data);

#endif
