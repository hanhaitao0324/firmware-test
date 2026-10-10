#include "xtea.h"

TByte XTEA_NumRounds=NUMBER_OF_ROUNDS;

void XTEA_SetNumRounds(TByte num_rounds)
{
  XTEA_NumRounds=num_rounds;
  if (XTEA_NumRounds<MIN_NUMBER_OF_ROUNDS)
    XTEA_NumRounds=MIN_NUMBER_OF_ROUNDS;
  if (XTEA_NumRounds>MAX_NUMBER_OF_ROUNDS)
    XTEA_NumRounds=MAX_NUMBER_OF_ROUNDS;
}

void XTEA_Encrypt(TDWord* v, TDWord* k) 
{
  TDWord v0=v[0], v1=v[1], i;
  TDWord sum=0, delta=0x9E3779B9;

  for(i=0; i<XTEA_NumRounds; i++) 
  {
    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    sum += delta;
    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
  }
  v[0]=v0; 
  v[1]=v1;
}

void XTEA_Decrypt(TDWord* v, TDWord* k) 
{
  TDWord v0=v[0], v1=v[1], i;
  TDWord delta=0x9E3779B9, sum=delta*XTEA_NumRounds;
  
  for(i=0; i<XTEA_NumRounds; i++) 
  {
    v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
    sum -= delta;
    v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
  }
  v[0]=v0; 
  v[1]=v1;
}
