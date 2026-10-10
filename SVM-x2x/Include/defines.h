#ifndef _DEFINES_H
  #define _DEFINES_H

#ifdef __BORLANDC__
class QObject
  {
    public:
      QObject(void){};
      ~QObject(void){};
  };

class QMutex
  {
    public:
      QMutex(){};
      ~QMutex(){};
      void lock(){};
      void unlock(){};
  };
#define Q_OBJECT
#define QTimer	int
#define slots
#define QString int
#define QFile	int
#endif

#ifndef false
  #define false 0
#endif
#ifndef true
  #define true  (!false)
#endif

#ifdef _WINDOWS
#define PATH_SEPARATOR	"\\"
#else
#define PATH_SEPARATOR	"/"
#endif

#define macBitOn(REG,BIT) (REG|=(0x01<<BIT))
#define macBitOff(REG,BIT) (REG&=(~(0x01<<BIT)))
#define macBitTest(REG,BIT) (REG&(1<<BIT))


#ifndef __TMS320C28XX__
typedef unsigned char TByte;
typedef unsigned char TBitField;
#else
typedef unsigned short TByte;
typedef int TBitField;
#endif

#ifndef __BORLANDC__

//==================================================================
// Common types
//==================================================================
typedef unsigned short TWord;

typedef unsigned long int TDWord;

typedef float TReal32;

typedef double TReal64;

typedef unsigned char TBool;

//==================================================================
// Fixed-point types
//==================================================================
typedef signed short TsFix16;

typedef unsigned short TFix16;

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

#else

//==================================================================
// Borland C++ Builder definitions
//==================================================================
typedef unsigned __int8 TByte;

typedef unsigned __int16 TWord;

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
