#include "eeprom_utils.h"
#include "crc.h"

//------------------------------------------------------------------------------
TWord CalcCRC16(TByte *src, TByte len)
{
  TWord rpar=0xffff;
  TByte pos;
  TByte data;

  pos=0;
  while (pos<len)
  {
    data=src[pos];
    pos++;
    rpar=CalcCRC16_Macro(data,rpar);
  }
  return rpar;
}

//------------------------------------------------------------------------------
// EEPROM_ReadString - read a string from the EEPROM
//------------------------------------------------------------------------------
void EEPROM_ReadString(char *dest, char __eeprom *src, TByte length)
  {
    TByte pos;
    
    pos=0;
    while (pos<length)
      {
        dest[pos]=src[pos];  
        pos++;
      }
  }

//------------------------------------------------------------------------------
// EEPROM_CalcCRC16 - calculate CRC16 of EEPROM area with length len
//------------------------------------------------------------------------------
TWord EEPROM_CalcCRC16(TByte __eeprom *src, TByte len)
{
  TWord rpar=0xffff;
  TByte pos;
  TByte data;

  pos=0;
  while (pos<len)
  {
    data=src[pos];
    pos++;
    rpar=CalcCRC16_Macro(data,rpar);
  }
  return rpar;
}

//------------------------------------------------------------------------------
// EEPROM_WriteBlock - write block with length len from EEPROM to EEPROM
//------------------------------------------------------------------------------
void EEPROM_WriteBlock(TByte __eeprom *dest, TByte __eeprom *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}

//------------------------------------------------------------------------------
// EEPROM_WriteBlock_P - write block with length len from flash to EEPROM
//------------------------------------------------------------------------------
void EEPROM_WriteBlock_P(TByte __eeprom *dest, TByte __flash *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}

//------------------------------------------------------------------------------
// EEPROM_WriteBlock_R - write block with length len from RAM to EEPROM
//------------------------------------------------------------------------------
void EEPROM_WriteBlock_R(TByte __eeprom *dest, TByte *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}
