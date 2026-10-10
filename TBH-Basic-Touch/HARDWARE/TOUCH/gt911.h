#ifndef __GT911_H
#define __GT911_H	
#include "sys.h"	

#define TOUCH_HJ 	1 							// 鸿佳触摸屏，启用
#define TOUCH_ST 	0 							// 深拓触摸屏，禁用
//#define TOUCH_HJ 	0 							// 鸿佳触摸屏，禁用
//#define TOUCH_ST 	1 							// 深拓触摸屏，启用

//IO操作函数	 
#define GT_RST  PIout(8) //GT9147复位引脚
#define GT_INT  PHin(7)  //GT9147中断引脚	

#define GT911_RST_1()  PIout(8)=1	/* RST = 1 */ 
#define GT911_RST_0()  PIout(8)=0	/* RST = 0 */ 

#define GT911_INT_1()  	PHin(7)=1/* INT = 1 */ 
#define GT911_INT_0()  	PHin(7)=0/* INT = 0 */ 
 
 
 
//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令
  
//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  
 
 
 //一下未使用
#define GT911_READ_XY_REG 					0x814E	/* 坐标寄存器 */ 
#define GT911_CLEARBUF_REG 					0x814E	/* 清除坐标寄存器 */ 
#define GT911_CONFIG_REG 						0x8047	/* 配置参数寄存器 */ 
#define GT911_COMMAND_REG 					0x8040 	/* 实时命令 */ 
#define GT911_PRODUCT_ID_REG 				0x8140 	/*productid*/ 
#define GT911_VENDOR_ID_REG 				0x814A 	/* 当前模组选项信息 */ 
#define GT911_CONFIG_VERSION_REG 		0x8047 	/* 配置文件版本号 */ 
#define GT911_CONFIG_CHECKSUM_REG 	0x80FF 	/* 配置文件校验码 */ 
#define GT911_FIRMWARE_VERSION_REG	0x8144 	/* 固件版本号 */  
 
 
 
u8 GT911_Send_Cfg(u8 mode);
u8 GT911_WR_Reg(u16 reg,u8 *buf,u8 len);
void GT911_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 GT911_Init(void);
u8 GT911_Scan(u8 mode); 


void GT911_Reset_Sequence(uint8_t ucAddr);//未使用
void GT911_Soft_Reset(void);//未使用
#endif













