#ifndef _FONT_H
#define _FONT_H

#include "defines.h"

#define SEG_A   0x2000
#define SEG_B   0x4000
#define SEG_C   0x0001
#define SEG_D   0x0004
#define SEG_E   0x0080
#define SEG_F   0x0100
#define SEG_G1  0x1000
#define SEG_G2  0x0008
#define SEG_J   0x0200
#define SEG_K   0x0400
#define SEG_L   0x0800
#define SEG_M   0x0010
#define SEG_N   0x0020
#define SEG_P   0x0040
#define SEG_DP  0x0002
#define SEG_EMPTY 0x0000

#define SEG_H   0x0008

#define SEG7_A  0x04
#define SEG7_B  0x08
#define SEG7_C  0x10
#define SEG7_D  0x20
#define SEG7_E  0x80
#define SEG7_F  0x01
#define SEG7_G1 0x02
#define SEG7_G2 0x40

#define EMPTY7  10

#define CHARACTER_SET7_LEN	(11)
#define CHARACTER_SET14_LEN	(128)

extern const uint8_t CHARACTER_SET7[CHARACTER_SET7_LEN];
extern const uint16_t CHARACTER_SET14[CHARACTER_SET14_LEN];

#endif
