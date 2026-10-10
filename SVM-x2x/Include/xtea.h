#ifndef _XTEA_H
  #define _XTEA_H

#include "defines.h"

#define MIN_NUMBER_OF_ROUNDS  (16)
#define MAX_NUMBER_OF_ROUNDS  (64)

#define NUMBER_OF_ROUNDS      (32)

extern void XTEA_SetNumRounds(TByte num_rounds);
extern void XTEA_Encrypt(TDWord* v, TDWord* k);
extern void XTEA_Decrypt(TDWord* v, TDWord* k); 

#endif
