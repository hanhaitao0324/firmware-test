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


// 颜色代码 ----------------------------
//画笔颜色,(1000 0)(100 001)(1 0000)
#define WHITE 			0xFFFF 		// 白色
#define BLACK 			0x0000	  // 黑色
#define BLUE 				0x001F  	// 蓝色
#define BRED 				0XF81F 		// 
#define GRED 				0XFFE0
#define GBLUE 			0X07FF
#define RED 				0xF800
#define MAGENTA 		0xF81F
#define GREEN 			0x07E0
#define CYAN 				0x7FFF
#define YELLOW 			0xFFE0
#define BROWN 			0XBC40 		//棕色
#define BRRED 			0XFC07 		//棕红色
#define GRAY 				0X8430 		//灰色，(1000 0)(100 001)(1 0000)
#define GRAY1 			0XC638
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


// 颜色定义 2020-11-28 星期六 12:37:39 --------------------------------
#define GRAY_575757 	0x52AA
#define GRAY_656565 	0x632C
#define GRAY_878787 	0x8430
#define GRAY_A6A6A6 	0xA534
#define GRAY_A7A7A7 	0xA534
#define GRAY_B3B3B3 	0xB596
#define GRAY_C6C6C6 	0xC638
#define GRAY_D7D7D7 	0xD6BA
#define GRAY_DADADA 	0xDEDB
#define GRAY_E8E8E8 	0xEF5D
#define GRAY_EDEDED 	0xEF7D
#define GRAY_F6F6F6 	0xF7BE
#define GRAY_F8F8F8 	0XFFDF
#define GREEN_3AAA35 	0x3D46
#define GREEN_95C11F 	0x9603
#define GREEN_42AB3E 	0x4547
#define BLUE_26A5D1 	0x253A
#define BLUE_2EA6D0 	0x2D3A
#define ORANGE_D16F00 0xD360
#define ORANGE_E94E1B 0xEA63

// 字体定义 ----------------------------
#define Font_Rbm20 20
#define Font_Rbm22 22
#define Font_Rbm24 24
#define Font_Rbr27 27
#define Font_Rbb28 28
#define Font_Rbb34 34
#define Font_Hnb48 48

// 特殊键码定义 ------------------------
#define BACKSPACE_KEY 0x08
#define ENTER_KEY 		0x0d
#define SHIFT_KEY 		0x82
#define TO123_KEY 		0x80
#define TOABC_KEY 		0x81
#define CLEAR_KEY 		0x83

// 图片编号定义 ------------------------
//#define gImage_LiangTong 			0 				// 彩色大量筒
//#define gImage_035 						1 				// 杠杆表
#define gImage_039 						2 				// 秒表
//#define gImage_BackSpace 			3 				// 
//#define gImage_Empty 					4
//#define gImage_To123 					5
//#define gImage_Space123 			6
//#define gImage_Space 					7
//#define gImage_Smile 					8
//#define gImage_RShift 				9
//#define gImage_LShift 				10
//#define gImage_Toabc 					11
//#define gImage_BackSpace123 	12
//#define gImage_EarthK 				13
//#define gImage_Empty123 			14
//#define gImage_Enter 					15
//#define gImage_Enter123 			16
//#define gImage_KeyP 					17
//#define gImage_KeyP123 				18



extern uint32_t M407ID; 								// 96转10位ID


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

extern void Get_ChipID(void);

#endif
