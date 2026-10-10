#ifndef _DEFINES_H
#define _DEFINES_H

#define CHAR_CR 	0x0D
#define CHAR_NL		0x0A
#define CHAR_ESC 	0x1B
#define CHAR_PRMT 0x3E

#ifndef false
  #define false 0x00
#endif
#ifndef true
  #define true  0xff
#endif

// NULL declaration
#ifndef NULL
#define NULL  ((void *)0)
#endif	

#include <stdint.h>
#include <stdbool.h>

//==================================================================
// Common types
//==================================================================
typedef int             S32;

#ifndef HIBYTE
  #define HIBYTE(a) (*((TByte*)&a + 1))
#endif

#ifndef LOBYTE
  #define LOBYTE(a) (*((TByte*)&(a)))
#endif


// PCB类型，Gavin， 2022-06-08 16:20
enum PCB_TYPES
{
  PCB_TLC5927,
  PCB_DM13H, 				// Gavin，2021-12-22 星期三 13:27:15
  PCB_TYPE_LAST
};


#endif
