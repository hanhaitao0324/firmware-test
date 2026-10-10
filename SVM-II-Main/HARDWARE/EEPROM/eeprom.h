#ifndef _EEPROM_H
#define _EEPROM_H

#include "defines.h"
#include "const.h"

#include "system.h"
#include "timedate.h"

extern bool EEPROM_WriteVar(void);
extern void EEPROM_Restore(void);
extern void EEPROM_LoadDefault(void);
extern bool EEPROM_CheckIntegrity(void);
extern void EEPROM_Check(void);
unsigned char readeeprom(unsigned int address);
void writinteep(unsigned int val,unsigned int adr);
void InitEEPROM(unsigned char mode);
unsigned int readinteep(unsigned int adr);
void writeeeprom(unsigned char val, unsigned int address);
//void loaddouble(double *num,unsigned int adr);
//void savedouble(double *num,unsigned int adr);
void LoadDate(DTIM *dt,unsigned int adr);
void WriteDate(DTIM *dt,unsigned int adr);
void loadfloat(float *num,unsigned int adr);
void savefloat(float *num,unsigned int adr);
void writeepbuf(unsigned char *val, unsigned int address,unsigned int length);
void readeepbuf(unsigned char *val, unsigned int address, unsigned int length);

// EEPROM数据地址分配 ------------------
/*
#define ADRSERNUM 						1 				// 8 byte, Serial number，设备ID
#define ADR_ACTIVATION_CODE 	110 			// 6 byte，激活码

#define ADRAUTOPRINT 					65 				// 1 byte，自动打印

#define ADRSOUNDSW 						2000 			// 1 byte，声音开关状态保存地址
*/
#define ADR_HTVER 						0x0000 		// 6 byte, 硬件版本号

#define ADRSERNUM 						0x1000 		// 8 byte, Serial number，设备ID
#define ADR_ACTIVATION_CODE 	0x1010 		// 6 byte，激活码
#define ADRAUTOPRINT 					0x1020 		// 1 byte，自动打印
#define ADRSOUNDSW 						0x1021 		// 1 byte，声音开关状态保存地址
#define ADR_TIMEZONE 					0x1022 		// 1 byte, 时区
#define ADR_DATEFORMAT 				0x1023 		// 1 byte, 日期格式
#define ADR_DAYLIGHTTIME 			0x1024 		// 1 byte, 夏令时

#define ADR_REPSAVE 					0x1028 		// 1 byte, 报告保存标志保存地址，Gavin，2022-12-08 周四 11:08:29

#define ADR_V0_V10 						0x1030 		// 1 byte, 压缩率计算的参考体积，0使用V0，10使用V10
							
#define Password_SW_ADDR			0x1040		// 1 byte, 记录是否开启密码
#define Password_ADDR					0x1041		// 10 byte,密码4-10位
#define Password_NUM_ADDR			0x1052		// 1 byte,记录密码的位数,方便下次读取

#define Password_COD_ADDR			0x1053		// 16 byte,记录激活码是否被使用过,有16个激活码

#define	Update_ADDR						0x1086		// 1 byte,记录是否升级了v1.07版本

#define ADR_Export_Interface	0X1090		// 1 byte，导出设置接口类型,usb,lan,ethernet
#define ADR_Export_Format			0X1091		// 1 byte，导出设置格式,xml,csv,pdf

#define ADR_LWIP_DEV 					0x10A0    // 23 byte，lwip结构体，网络服务器IP
#define ADR_TCP_PORT 					0x10C0 		// 2 byte, 端口号
#define ADR_DHCP_SW 					0x10C2 		// 1 byte, 自动获取IP，开关标志
#define ADR_PRT_IP 						0x10C8 		// 4 byte, 网络打印机IP
#define ADR_PRT_PORT 					0x10CC 		// 2 byte, 打印机端口号，固定为9100，此空间暂时不用，Gavin，2023-01-31 周二 10:28:40
#define ADR_PRT_TYPE 					0x10CE 		// 1 byte, 打印输出类型，0：USB；1：LAN；
#define ADR_PRT_FORMAT 				0x10CF 		// 1 byte, 打印输出格式，0：PCL；1：ASC II；

#define OQ_ST_ADDR 						0x3E00 		// ? byte, OQ结构起始地址

// 测试报告存储地址 --------------------
#define ADR_REPORTSIP 				0x3FFF 		// 1 byte，当前最新报告指针

#define ADR_REPORTS01 				0x4000 		// 512 byte, 第一份报告起始地址
#define ADR_REPORTS02 				0x4200
#define ADR_REPORTS03 				0x4400
#define ADR_REPORTS04 				0x4600
#define ADR_REPORTS05 				0x4800
#define ADR_REPORTS06 				0x4A00
#define ADR_REPORTS07 				0x4C00
#define ADR_REPORTS08 				0x4E00
#define ADR_REPORTS09 				0x5000

#define ADR_REPORTS10 				0x5200 		// 512 byte, 第十份报告起始地址
#define ADR_REPORTS11 				0x5400
#define ADR_REPORTS12 				0x5600
#define ADR_REPORTS13 				0x5800
#define ADR_REPORTS14 				0x5A00
#define ADR_REPORTS15 				0x5C00
#define ADR_REPORTS16 				0x5E00
#define ADR_REPORTS17 				0x6000

#define ADR_REPORTS18 				0x6200 		// 512 byte, 第18份报告起始地址
#define ADR_REPORTS19 				0x6400
#define ADR_REPORTS20 				0x6600

#define ADR_REPORTS21 				0x6800
#define ADR_REPORTS22 				0x6A00
#define ADR_REPORTS23 				0x6C00
#define ADR_REPORTS24 				0x6E00
#define ADR_REPORTS25 				0x7000
#define ADR_REPORTS26 				0x7200
#define ADR_REPORTS27 				0x7400

//#define HWVS 									0x7FF0

#define ADRSIGNATUR 					0x7FFF 		// 1 byte，EEPROM 签名地址
#define SIGNATUR 							0x5B 			// 1 byte，签名内容

/*
//#define ADRAUTODIAM 					6
//#define ADRLANGUAGE 					10
//#define ADRCOUNTH 						12
//#define ADRCOUNTD 						16
//#define ADRMEASUREMETHOD 			20
//#define ADRSENSIBILITY 				22
//#define ADRMEASSPEED 					24
//#define ADRFORCE 							28
//#define ADRFORCELIMIT 				30
//#define ADRDRAGJAW 						32
//#define ADRMEASUREPAUSE 			36
//#define ADRCCD 								64

//#define ADRLEINH 							74
//#define ADRHEINH 							76
//#define ADRPRUEFANZ 					78
//#define ADRDMEING 						80

//#define ADRNWTFAK 						86
//#define ADRBLPOS 							90
//#define ADREICHMAS 						92
//#define ADRVORBREMS 					96 
//#define ADRMMFAK 							102
//#define ADREICHPOS 						106
//#define ADRDRUCKMODE 					110
//#define ADRPRINTERPAPER 			112
//#define ADRPRINTERLANGUAGE 		114
//#define ADRPRINTERFRAMES 			116
//#define ADRDIRDIAM 						118
//#define ADRAUTOPRINT 					124

//#define ADRFULLDAYS 					132
//#define ADRPERIODS 						134
//#define ADRAKTPERIOD 					136
//#define ADRMANUFACT 					138
//#define ADRNOLICENSE 					140
//#define ADRDAYSLEFT 					150
//#define ADRDATEOFFSET 				152
//#define ADRLASTDAY 						164

//#define ADRDIRANZD 						202
//#define ADRDIRANZH 						204

//#define ADRMESSZEIT 					246
//#define ADRMESSDAT 						256
//#define ADRONWT 							266
//#define ADRLBKALO 						270
//#define ADRLBIKALO 						272
//#define ADRLASDIAM 						274
//#define ADRLBKAL 							278
//#define ADRLBIKAL 						280
//#define ADRSERNUM 						284
//#define ADRVALIDAT 						326
//#define ADRKRAFTAN 						344
//#define ADRMESMETH 						346
//#define ADRDIRKORR 						352
//#define ADRDATFORMAT 					356
//#define ADRUEBERLAUF 					360
//#define ADRCHECKWGT1 					364 								// 检测数据的结果？，2016/11/10 14:08:58
//#define ADRCHECKWGT2 					368
//#define ADRCHECKWGT3 					372

//#define ADRHARDON 						384
//#define ADRDIAMON 						388
//#define ADRTHICKON 						392
//#define ADRAUTOMESS 					396

//#define ADRKALDDAT 						400
//#define ADRKALDDATO 					550
//#define ADRKALBDAT 						450
//#define ADRKALBDATO 					600
//#define ADRLASTDATE 					500 								
//#define ADRLASKMOD 						650
//#define ADRKALIMODE 					654
//#define ADRTERMINOLOGIE 			656
//#define ADRJUSBDAT 						660
//#define ADRJUSDDAT 						704
//#define ADRJUSBDATO 					768 								// 上一次硬度校正日期
//#define ADRJUSDDATO 					832 								// 上一次直径或厚度校正日期
//#define ADRKALIVERS 					896
//#define ADRKALIVERSO 					916
//#define ADRDOSNR 							960
//#define ADRDOSNRO 						980
//#define ADRDICKEING 					1000
//#define ADRNULLKORR 					1020
//#define ADRDATENAUSGABE 			1030
*/

/*
//#define ADR_SPRACHE 9		  //1 byte，语言
//#define ADRNOLICENSE 10   //4 byte，许可证
//#define ADRMESSTIME 11		//6 byte，使用时间
//#define ADRVACUUM 17		  //4 byte
//#define ADRMESSZEITL 21	  //4 byte
//#define ADRMESSZEITH 25	  //4 byte
//#define ADRTMODE 29	      //4 byte, Time Mode (Hour/Minute)，时间模式
//#define ADRFULLDAYS 33	  //4 byte
//#define ADRPERIODS 37     //4 byte
//#define ADRAKTPERIOD 41   //4 byte
//#define ADRVACM 45			  //8 byte
//#define ADRVACB 53			  //8 byte

//#define ADRPRINTERFRAMES 	61  	//4 byte
//#define ADRAUTOPRINT 	65        //1 byte，自动打印
//#define ADRPENETRATION 66 	    //4 byte

//#define ADRDAYSLEFT 70	        //4 byte
//#define ADRDATEOFFSET 74	      //8 byte
//#define ADRLASTDAY 82	          //4 bytes

//#define ADRLASTDATE 86	        //6 bytes (packed)
//#define ADRLASTUPDATE 92	      //8 byte

//#define ADREXSICCATOR_SIZE 100  //4 byte
//#define ADRPENETRATION_INVERSE 104 	    //1 byte
*/

#endif
