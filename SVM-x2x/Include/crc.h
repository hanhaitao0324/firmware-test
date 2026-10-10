#ifndef _CRC_H
  #define _CRC_H

#include "defines.h"
#include "platform.h"

extern __flash TWord crc16tab[256];

#define CalcCRC16_Macro(c,crc) (crc16tab[(((TWord)crc >> 8) & 0xff) ^ (TByte)c] ^ ((TWord)crc << 8))

#endif
