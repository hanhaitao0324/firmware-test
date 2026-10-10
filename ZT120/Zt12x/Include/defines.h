#ifndef _DEFINES_H
  #define _DEFINES_H

#ifndef __IAR_SYSTEMS_ICC__
#define	__flash
#define	__farflash
#define __root
#define __eeprom
#define __monitor
#define __disable_interrupt()
#define __enable_interrupt()
#define __C_task
#define __interrupt
#define strcpy_P	strcpy
#define strcat_P	strcat
#define strcmp_P	strcmp
#define memcpy_P	memcpy
#define strlen_P	strlen
#define strncpy_P	strncpy
#define strncmp_P	strncmp
#define sprintf_P	sprintf
#endif

#ifndef false
  #define false 0x00
#endif
#ifndef true
  //#define true  0xff   // GAVIN
  #define true  1      // GAVIN
#endif

#define macBitOn(REG,BIT) (REG|=(0x01<<BIT))
#define macBitOff(REG,BIT) (REG&=(~(0x01<<BIT)))
#define macBitTest(REG,BIT) (REG&(1<<BIT))

//==================================================================
// Common types
//==================================================================

#if defined __IAR_SYSTEMS_ICC__

typedef unsigned char TByte;

typedef unsigned int TWord;
typedef signed int TsWord;

typedef unsigned long int TDWord;

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

#endif

#if defined __BORLANDC__
//==================================================================
// Borland C++ Builder definitions
//==================================================================
typedef unsigned __int8 TByte;

typedef unsigned __int16 TWord;
typedef signed __int16 TsWord;

typedef unsigned __int32 TDWord;

typedef float TReal32;

typedef double TReal64;

typedef unsigned char TBool;


//==================================================================
// Fixed-point types
//==================================================================
typedef signed __int16 TsFix16;

typedef unsigned __int16 TFix16;

typedef signed __int32 TsFix32;

typedef unsigned __int32 TFix32;

//==================================================================
// Complex value type
//==================================================================
#pragma pack(1)
typedef struct
  {
    TsFix16 re;
    TsFix16 im;
  } TComplex;
#pragma pack()
#endif


#if defined __ADSPBLACKFIN__

#include <fract.h>
#include <complex.h>

typedef unsigned char TByte;

typedef unsigned short TWord;
typedef signed short TsWord;

typedef unsigned long int TDWord;

typedef float TReal32;

typedef double TReal64;


//==================================================================
// Fixed-point types
//==================================================================
typedef fract16 TsFix16;

typedef fract32 TsFix32;

//==================================================================
// Complex value type
//==================================================================
typedef complex_fract16 TComplex;

#endif

#if ((defined __TURBOC__) && (!defined __BORLANDC__))

typedef unsigned char TByte;

typedef unsigned int TWord;
typedef signed int TsWord;

typedef unsigned long int TDWord;

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

typedef TsFix16 fract16;
typedef TsFix32 fract32;

#endif

#if defined __GNUC__

typedef unsigned char TByte;

typedef unsigned int TWord;
typedef signed int TsWord;

typedef unsigned long int TDWord;

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

#endif

#ifndef HIBYTE
  #define HIBYTE(a) (*((TByte*)&a + 1))
#endif

#ifndef LOBYTE
  #define LOBYTE(a) (*((TByte*)&(a)))
#endif

#endif
