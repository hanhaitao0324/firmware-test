#ifndef _UTILS_H
#define _UTILS_H

#include "defines.h"

extern uint16_t UTILS_ToHexDec(uint8_t num);
uint8_t ReverseBits(uint8_t c);
//extern void UTILS_TimeToStr(TDWord time, char *str, TBool point);

void gettimstr(char *tbu);
void getdatstr(char *dbu);

/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str, S32 intnum);
uint32_t Str2Int(uint8_t *inputstr, S32 *intnum);
uint32_t SerialKeyPressed(uint8_t *key);
void SerialPutChar(uint8_t c);

/* Common routines */
#define IS_AF(c)	((c >= 'A') && (c <= 'F'))
#define IS_af(c)	((c >= 'a') && (c <= 'f'))
#define IS_09(c)	((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)	IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)	IS_09(c)
#define CONVERTDEC(c)	(c - '0')

#define CONVERTHEX_alpha(c)	(IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#endif
