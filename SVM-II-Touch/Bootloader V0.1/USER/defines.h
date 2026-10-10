#ifndef _DEFINES_H
#define _DEFINES_H

#ifndef false
  #define false 0x00
#endif
#ifndef true
  #define true  0xFF
#endif

#define pinOn(REG, BIT) (REG|=(0x01<<BIT))
#define pinOff(REG, BIT) (REG&=(~(0x01<<BIT)))
#define pinTest(REG, BIT) (REG&(1<<BIT))

//==================================================================
// Common types
//==================================================================

typedef unsigned char TByte;

typedef unsigned short TWord;
typedef signed short TsWord;

typedef unsigned int TDWord;

typedef float TReal32;

typedef double TReal64;

typedef unsigned char TBool;

//==================================================================
// Fixed-point types
//==================================================================
typedef signed int TsFix16;

typedef unsigned int TFix16;

typedef signed long int TsFix32;

typedef unsigned long int TFix32;

//==================================================================
// Complex value type
//==================================================================
typedef struct
  {
    TsFix16 re;
    TsFix16 im;
  } TComplex;

#ifndef HIBYTE
  #define HIBYTE(a) (*((TByte*)&a + 1))
#endif

#ifndef LOBYTE
  #define LOBYTE(a) (*((TByte*)&(a)))
#endif

#define macBitOn(REG,BIT) (REG|=(1<<BIT))
#define macBitOff(REG,BIT) (REG&=(~(1<<BIT)))
#define macBitTest(REG,BIT) (REG&(1<<BIT))

#endif
