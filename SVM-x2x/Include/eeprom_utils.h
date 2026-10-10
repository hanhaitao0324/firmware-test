#ifndef _EEPROM_UTILS_H
  #define _EEPROM_UTILS_H

#include "const.h"
#include "defines.h"
#include "platform.h"

#define EEPROM_ERRORS   (3)

extern TWord CalcCRC16(TByte *src, TByte len);
// EEPROM_ReadString - read a string from the EEPROM
extern void EEPROM_ReadString(char *dest, char __eeprom *src, TByte length);
// EEPROM_CalcCRC16 - calculate CRC16 of EEPROM area with length len
extern TWord EEPROM_CalcCRC16(TByte __eeprom *src, TByte len);
// EEPROM_WriteBlock - write block with length len from EEPROM to EEPROM
extern void EEPROM_WriteBlock(TByte __eeprom *dest, TByte __eeprom *src, TByte len);
// EEPROM_WriteBlock_P - write block with length len from flash to EEPROM
extern void EEPROM_WriteBlock_P(TByte __eeprom *dest, TByte __flash *src, TByte len);
// EEPROM_WriteBlock_R - write block with length len from RAM to EEPROM
extern void EEPROM_WriteBlock_R(TByte __eeprom *dest, TByte *src, TByte len);

#endif
