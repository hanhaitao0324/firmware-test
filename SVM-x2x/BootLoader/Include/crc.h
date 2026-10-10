#ifndef CRC_Def_
#define CRC_Def_

#ifndef NULL
  #define NULL (0)
#endif

#define CalcCRC16_Macro(c, crc) (crc16tab[(((unsigned int)crc >> 8) & 0xff) ^ (unsigned char)c] ^ ((unsigned int)crc << 8))
  // макрос вычисления CRC16

unsigned int CalcCRC16_Byte(unsigned char Smb, unsigned int CRC16); // вычисление CRC16

#endif
