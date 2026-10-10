#ifndef _EEPROM_H
#define _EEPROM_H

#include "defines.h"
#include "const.h"

#include "system.h"
#include "timedate.h"

bool EEPROM_WriteVar(void);
void EEPROM_Restore(void);
void EEPROM_LoadDefault(void);
bool EEPROM_CheckIntegrity(void);
void EEPROM_Check(void);
unsigned char readeeprom(unsigned int address);
void writinteep(unsigned int val,unsigned int adr);
void InitEEPROM(unsigned char mode);
unsigned int readinteep(unsigned int adr);
void writeeeprom(unsigned char val, unsigned int address);
void LoadDate(DTIM *dt,unsigned int adr);
void WriteDate(DTIM *dt,unsigned int adr);
void loadfloat(float *num,unsigned int adr);
void savefloat(float *num,unsigned int adr);
void writeepbuf(unsigned char *val, unsigned int address,unsigned int length);
void readeepbuf(unsigned char *val, unsigned int address, unsigned int length);

// EEPROM数据地址分配 ------------------
#define ADR_HTVER 						0x0000 		// 6 byte, 硬件版本号

#define ADRSERNUM 						0x1000 		// 8 byte, Serial number，设备ID
#define ADR_ACTIVATION_CODE 	        0x1010 		// 6 byte，激活码
#define ADRAUTOPRINT 					0x1020 		// 1 byte，自动打印
#define ADRSOUNDSW 						0x1021 		// 1 byte，声音开关状态保存地址
#define ADR_TIMEZONE 					0x1022 		// 1 byte, 时区
#define ADR_DATEFORMAT 				    0x1023 		// 1 byte, 日期格式
#define ADR_DAYLIGHTTIME 			    0x1024 		// 1 byte, 夏令时

#define ADR_REPSAVE 					0x1028 		// 1 byte, 报告保存标志保存地址，Gavin，2022-12-08 周四 11:08:29

#define Password_COD_ADDR			    0x1053		// 16 byte,记录激活码是否被使用过,有16个激活码

#define	Update_ADDR						0x1086		// 1 byte,记录是否升级了v1.07版本

#define ADR_Export_Interface	        0X1090		// 1 byte，导出设置接口类型,usb,lan,ethernet
#define ADR_Export_Format			    0X1091		// 1 byte，导出设置格式,xml,csv,pdf

// Gavin，2023-03-13 周一 14:35:55
#define ADR_LWIP_DEV 					0x10A0    // 23 byte，lwip结构体，网络服务器IP
#define ADR_TCP_PORT 					0x10C0 		// 2 byte, 端口号
#define ADR_DHCP_SW 					0x10C2 		// 1 byte, 自动获取IP，开关标志
#define ADR_PRT_IP 						0x10C8 		// 4 byte, 网络打印机IP
#define ADR_PRT_PORT 					0x10CC 		// 2 byte, 打印机端口号，固定为9100，此空间暂时不用，Gavin，2023-01-31 周二 10:28:40
#define ADR_PRT_TYPE 					0x10CE 		// 1 byte, 打印输出类型，0：USB；1：LAN；
#define ADR_PRT_FORMAT 				    0x10CF 		// 1 byte, 打印输出格式，0：PCL；1：ASC II；
#define ADR_BAL_MODEL 				    0x10D0 		// 1 byte, 电子秤型号，0：Mettler Me 54；1：Sartorius Practum；Gavin，2023-08-04 周五 12:51:53

#define OQ_ST_ADDR 						0x3E00 		// ? byte, OQ结构起始地址
#define PQ_ST_ADDR 						0x3E12 		// ? byte, PQ结构起始地址
#define Password_ST_ADDR                0x3E24      // Password

// 报告存储地址 --------------------
#define ADR_GENREPORTSIP 				0x3FFE      // 1 byte 最新一般数据测试报告指针
#define ADR_REPORTSIP 				    0x3FFF 		// 1 byte，当前最新测试报告指针
#define ADR_HardAdjRptIp                0x3FF0      // 硬度调整报告最新指针
#define ADR_DiamAdjRptIp                0x3FF1
#define ADR_HardCalRptIp                0x3FF2
#define ADR_DiamCalRptIp                0x3FF3
#define ADR_WeigCalRptIp                0x3FF4


// 用户数据存储地址---------------------
#define ADR_User_admin          0x4000
#define ADR_User_operator       0x4078
#define ADR_User_service        0x40F0
#define ADR_User_number001      0x4168


#define ADR_User_number100      0x7048


//#define HWVS 									0x7FF0

#define ADRSIGNATUR 					0x27FFF 		// 1 byte，EEPROM 签名地址
#define SIGNATUR 						0x5B 			// 1 byte，签名内容

//
#define ADRAUTODIAM 					6       // 自动获取名义尺寸
#define ADRSENSIBILITY 				    10      // 硬度测量灵敏度
#define ADRCONTACTFORCE 				14      // 长度测量接触力
#define ADRLANGUAGE 					18      // 语言
#define ADRMEASUREMETHOD 			    22      // 测量方式
#define ADRMEASSPEED 					26      // 恒速速度
#define ADRFORCE 						30      // 恒力力度
#define ADRFORCELIMIT 				    34      // 最大力上限
#define ADRDMEING 						38      // 名义尺寸
#define ADRNWTFAK 						42      // 传感器系数
#define ADRBLPOS 						46      // 轨道行程
#define ADRVORBREMS 					50      // 偏移尺寸
#define ADRMMFAK 						54      // 电机位移距离系数
#define ADRFASTMOVESP                   58      // 轨道快速移动速度
#define ADRDIAMEASSPEED                 62      // 直径测量速度
#define ADRHARDUNIT                     66      // 硬度单位
#define ADRLENGTHUNIT                   67      // 长度单位
#define ADRWEIGHTUNIT                   68      // 重量单位
#define ADRTestRepNum                   69      // 测试报告编号
#define ADRAdjReportNum                 73      // 调整和校正报告编号
#define ADRSenserType                   77      // 力传感器型号，50kg/100kg
#define ADRDevLanguage                  78      // 设备语言
#define ADRNumFormat                    79      // 数据千位分隔符格式
#define ADRLongTabhold                  80      // 长药片开关
#define ADRConLidSensor                 81      // 收集盒传感器开关
#define ADRLoadCellMeaRange             82      // 力传感器量程
#define ADRHardTole                     83      // 硬度偏差
#define ADRDiamTole                     87      // 直径偏差
#define ADRWeightTole                   91      // 重量偏差
#define ADRExportFlag                   99      // 数据导出开关
#define ADRErrorNum                     100     // 错误记录数量 4byte
#define ADRAutoPrint                    104     // 自动打印 1byte
#define ADRAutoExport                   105     // 自动导出 1byte

#define ADRUserNum                      200     // 用户数量 1byte
#define ADRUserFixedID                  201     // 用户编号 4byte
#define ADRSysPwIndex                   206     // 自动分配密码索引 1byte
#define ADRProductNum                   207     // 产品数量 1byte
#define ADRProductID                    208     // 产品编号 4byte
#define ADRMethodNum                    212     // 方法数量 1byte
#define ADRMethodID                     213     // 方法编号 4byte
#define ADRContactInfo                  217     // 联系信息 218byte

#define ADRHradAdjDate 					700 	// 上一次硬度调整日期
#define ADRDiamAdjDate 					724 	// 上一次直径或厚度调整日期
#define ADRHardCalDate                  748     // 上一次硬度校正日期
#define ADRDiamCalDate                  772     // 上一次直径或厚度校正日期
#define ADRWeiCalDate                   796     // 上一次重量校正日期


#define MaxEEPROMAdr                0x8000      // 24c256地址末尾






// Flash 存储地址-------------------------------------------------

// 错误列表存储地址
#define ADR_ErrorList_001 0x0000
#define ADR_ErrorList_002 0x0080
#define ADR_ErrorList_003 0x0100
#define ADR_ErrorList_004 0x0180
#define ADR_ErrorList_005 0x0200
#define ADR_ErrorList_006 0x0280
#define ADR_ErrorList_007 0x0300
#define ADR_ErrorList_008 0x0380
#define ADR_ErrorList_009 0x0400
#define ADR_ErrorList_010 0x0480


#define ADR_ErrorList_end 0x7F000

// 产品和方法管理地址---------------------------------------------
// Product Management
#define ADR_Product_Management01   0x80000
#define ADR_Product_Management02   0x81000
#define ADR_Product_Management03   0x82000
// .............................
#define ADR_Product_Management100  0xE4000

// Method Management
#define ADR_Method_Management01   0xF0000
#define ADR_Method_Management02   0xF1000
#define ADR_Method_Management03   0xF2000
// .............................
#define ADR_Method_Management100  0x154000

// 报告起始地址--------------------------------------------------------------

// 直接测试报告
#define ADR_REPORTS01 				0x800000 		// 5120 byte, 第一份报告起始地址
#define ADR_REPORTS02 				0x802000
#define ADR_REPORTS03 				0x804000
#define ADR_REPORTS04 				0x806000
#define ADR_REPORTS05 				0x808000
#define ADR_REPORTS06 				0x80A000
#define ADR_REPORTS07 				0x80C000
#define ADR_REPORTS08 				0x80E000
#define ADR_REPORTS09 				0x810000
#define ADR_REPORTS10 				0x812000      // 5120 byte, 第十份报告起始地址
#define ADR_REPORTS11 				0x814000
#define ADR_REPORTS12 				0x816000
#define ADR_REPORTS13 				0x818000
#define ADR_REPORTS14 				0x81A000
#define ADR_REPORTS15 				0x81C000
#define ADR_REPORTS16 				0x81E000
#define ADR_REPORTS17 				0x820000
#define ADR_REPORTS18 				0x822000     // 5120 byte, 第十八份报告起始地址
#define ADR_REPORTS19 				0x824000
#define ADR_REPORTS20 				0x826000
#define ADR_REPORTSEND 				0x828000

// 一般数据测试报告
#define ADR_GENREPORTS01 		0x830000 		// 5120 byte, 第一份报告起始地址
#define ADR_GENREPORTS02 		0x832000
#define ADR_GENREPORTS03 		0x834000
#define ADR_GENREPORTS04 		0x836000
#define ADR_GENREPORTS05 		0x838000
#define ADR_GENREPORTS06 		0x83A000
#define ADR_GENREPORTS07 		0x83C000
#define ADR_GENREPORTS08 		0x83E000
#define ADR_GENREPORTS09 		0x840000
#define ADR_GENREPORTS10 		0x842000     // 5120 byte, 第十份报告起始地址
#define ADR_GENREPORTS11 		0x844000
#define ADR_GENREPORTS12 		0x846000
#define ADR_GENREPORTS13 		0x848000
#define ADR_GENREPORTS14 		0x84A000
#define ADR_GENREPORTS15 		0x84C000
#define ADR_GENREPORTS16 		0x84E000
#define ADR_GENREPORTS17 		0x850000
#define ADR_GENREPORTS18 		0x852000     // 5120 byte, 第十八份报告起始地址
#define ADR_GENREPORTS19 		0x854000
#define ADR_GENREPORTS20 		0x856000
#define ADR_GENREPORTSEND 		0x858000

// 调整校正报告-----------------------------------------------
// 硬度调整
#define ADR_HardADJ_REPORTS01      0x860000
#define ADR_HardADJ_REPORTS02      0x861000
#define ADR_HardADJ_REPORTS03      0x862000
#define ADR_HardADJ_REPORTS04      0x863000
#define ADR_HardADJ_REPORTS05      0x864000
#define ADR_HardADJ_REPORTS06      0x865000
#define ADR_HardADJ_REPORTS07      0x866000
#define ADR_HardADJ_REPORTS08      0x867000
#define ADR_HardADJ_REPORTS09      0x868000
#define ADR_HardADJ_REPORTS10      0x869000
#define ADR_HardADJ_REPORTS11      0x86A000
#define ADR_HardADJ_REPORTS12      0x86B000
#define ADR_HardADJ_REPORTS13      0x86C000
#define ADR_HardADJ_REPORTS14      0x86D000
#define ADR_HardADJ_REPORTS15      0x86E000
#define ADR_HardADJ_REPORTS16      0x86F000
#define ADR_HardADJ_REPORTS17      0x870000
#define ADR_HardADJ_REPORTS18      0x871000
#define ADR_HardADJ_REPORTS19      0x872000
#define ADR_HardADJ_REPORTS20      0x873000
#define ADR_HardADJ_REPORTSEND     0x874000

// 长度调整
#define ADR_DiaADJ_REPORTS01        0x880000
#define ADR_DiaADJ_REPORTS02        0x881000
#define ADR_DiaADJ_REPORTS03        0x882000
#define ADR_DiaADJ_REPORTS04        0x883000
#define ADR_DiaADJ_REPORTS05        0x884000
#define ADR_DiaADJ_REPORTS06        0x885000
#define ADR_DiaADJ_REPORTS07        0x886000
#define ADR_DiaADJ_REPORTS08        0x887000
#define ADR_DiaADJ_REPORTS09        0x888000
#define ADR_DiaADJ_REPORTS10        0x889000
#define ADR_DiaADJ_REPORTS11        0x88A000
#define ADR_DiaADJ_REPORTS12        0x88B000
#define ADR_DiaADJ_REPORTS13        0x88C000
#define ADR_DiaADJ_REPORTS14        0x88D000
#define ADR_DiaADJ_REPORTS15        0x88E000
#define ADR_DiaADJ_REPORTS16        0x88F000
#define ADR_DiaADJ_REPORTS17        0x890000
#define ADR_DiaADJ_REPORTS18        0x891000
#define ADR_DiaADJ_REPORTS19        0x892000
#define ADR_DiaADJ_REPORTS20        0x893000
#define ADR_DiaADJ_REPORTSEND       0x894000

// 硬度校正
#define ADR_HardCALI_REPORTS01      0x8A0000
#define ADR_HardCALI_REPORTS02      0x8A1000
#define ADR_HardCALI_REPORTS03      0x8A2000
#define ADR_HardCALI_REPORTS04      0x8A3000
#define ADR_HardCALI_REPORTS05      0x8A4000
#define ADR_HardCALI_REPORTS06      0x8A5000
#define ADR_HardCALI_REPORTS07      0x8A6000
#define ADR_HardCALI_REPORTS08      0x8A7000
#define ADR_HardCALI_REPORTS09      0x8A8000
#define ADR_HardCALI_REPORTS10      0x8A9000
#define ADR_HardCALI_REPORTS11      0x8AA000
#define ADR_HardCALI_REPORTS12      0x8AB000
#define ADR_HardCALI_REPORTS13      0x8AC000
#define ADR_HardCALI_REPORTS14      0x8AD000
#define ADR_HardCALI_REPORTS15      0x8AE000
#define ADR_HardCALI_REPORTS16      0x8AF000
#define ADR_HardCALI_REPORTS17      0x8B0000
#define ADR_HardCALI_REPORTS18      0x8B1000
#define ADR_HardCALI_REPORTS19      0x8B2000
#define ADR_HardCALI_REPORTS20      0x8B3000
#define ADR_HardCALI_REPORTSEND     0x8B4000

// 长度校正
#define ADR_DiaCALI_REPORTS01       0x8C0000
#define ADR_DiaCALI_REPORTS02       0x8C1000
#define ADR_DiaCALI_REPORTS03       0x8C2000
#define ADR_DiaCALI_REPORTS04       0x8C3000
#define ADR_DiaCALI_REPORTS05       0x8C4000
#define ADR_DiaCALI_REPORTS06       0x8C5000
#define ADR_DiaCALI_REPORTS07       0x8C6000
#define ADR_DiaCALI_REPORTS08       0x8C7000
#define ADR_DiaCALI_REPORTS09       0x8C8000
#define ADR_DiaCALI_REPORTS10       0x8C9000
#define ADR_DiaCALI_REPORTS11       0x8CA000
#define ADR_DiaCALI_REPORTS12       0x8CB000
#define ADR_DiaCALI_REPORTS13       0x8CC000
#define ADR_DiaCALI_REPORTS14       0x8CD000
#define ADR_DiaCALI_REPORTS15       0x8CE000
#define ADR_DiaCALI_REPORTS16       0x8CF000
#define ADR_DiaCALI_REPORTS17       0x8D0000
#define ADR_DiaCALI_REPORTS18       0x8D1000
#define ADR_DiaCALI_REPORTS19       0x8D2000
#define ADR_DiaCALI_REPORTS20       0x8D3000
#define ADR_DiaCALI_REPORTSEND      0x8D4000

// 重量校正
#define ADR_WeiCALI_REPORTS01       0x8E0000
#define ADR_WeiCALI_REPORTS02       0x8E1000
#define ADR_WeiCALI_REPORTS03       0x8E2000
#define ADR_WeiCALI_REPORTS04       0x8E3000
#define ADR_WeiCALI_REPORTS05       0x8E4000
#define ADR_WeiCALI_REPORTS06       0x8E5000
#define ADR_WeiCALI_REPORTS07       0x8E6000
#define ADR_WeiCALI_REPORTS08       0x8E7000
#define ADR_WeiCALI_REPORTS09       0x8E8000
#define ADR_WeiCALI_REPORTS10       0x8E9000
#define ADR_WeiCALI_REPORTS11       0x8EA000
#define ADR_WeiCALI_REPORTS12       0x8EB000
#define ADR_WeiCALI_REPORTS13       0x8EC000
#define ADR_WeiCALI_REPORTS14       0x8ED000
#define ADR_WeiCALI_REPORTS15       0x8EE000
#define ADR_WeiCALI_REPORTS16       0x8EF000
#define ADR_WeiCALI_REPORTS17       0x8F0000
#define ADR_WeiCALI_REPORTS18       0x8F1000
#define ADR_WeiCALI_REPORTS19       0x8F2000
#define ADR_WeiCALI_REPORTS20       0x8F3000
#define ADR_WeiCALI_REPORTSEND      0x8F4000

#define ADR_Flash_End           0x1000000
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

/*
case 9:  temp = ErwekaIcon64_48A[t]; 	break; 				// 三横
case 10: temp = ErwekaIcon235_48A[t]; 	break; 				// ERWEKA
case 11: temp = ErwekaIcon112_128A[t]; 	break; 				// Quick Test
case 12: temp = ErwekaIcon112_128B[t]; 	break; 				// EPUSP Test
case 13: temp = ErwekaIcon64_64A[t]; 	break; 				// 圆圈i
case 14: temp = ErwekaIcon112_128C[t]; 	break; 				// 右箭头,Next
case 15: temp = ErwekaIcon44_48A[t]; 	break; 				// 圆圈?,Help
case 16: temp = ErwekaIcon136_136A[t]; 	break; 				// 循环双箭头大,Tap count
case 17: temp = ErwekaIcon136_136B[t]; 	break; 				// 手表大,Time
case 18: temp = ErwekaIcon112_128D[t]; 	break; 				// 铜钱，大圆小方,Abort
case 19: temp = ErwekaIcon44_48B[t]; 	break; 				// 循环双箭头小
case 20: temp = ErwekaIcon112_128E[t]; 	break; 				// 左箭头,Back
case 21: temp = ErwekaIcon44_48C[t]; 	break; 				// 手表小，计时
case 22: temp = ErwekaIcon112_128F[t]; 	break; 				// 大圆右三角，START
case 23: temp = ErwekaIcon256_256A[t]; 	break; 				// 大图标量具
case 24: temp = ErwekaIcon112_128J[t]; 	break; 				// 打印机图标, Test Page
case 25: temp = ErwekaIcon112_128G[t]; 	break; 				// 方叉关闭图标，Close
case 26: temp = ErwekaIcon80_80A[t]; 	break; 				// 对勾，选中
case 27: temp = ErwekaIcon112_128H[t]; 	break; 				// Enter
case 28: temp = ErwekaIcon112_128I[t]; 	break; 				// Save
case 29: temp = ErwekaIcon24_24A[t]; 	break; 				// 小三角上拉
case 30: temp = ErwekaIcon112_128K[t]; 	break; 				// Start

case 31: temp = ErwekaIcon64_64[t]; 		break; 				// 小房子
case 32: temp = ErwekaIcon48_48[t]; 		break; 				// 小X
case 33: temp = ErwekaIcon32_24A[t]; 		break; 				// 半表针
case 34: temp = ErwekaIcon16_32[t]; 		break; 				// 上下双箭头
case 35: temp = ErwekaIcon32_32A[t]; 		break; 				// 逆时针
case 36: temp = ErwekaIcon32_24B[t]; 		break; 				// 三角波
case 37: temp = ErwekaIcon24_32[t]; 		break; 				// 小秒表
case 38: temp = ErwekaIcon32_32B[t]; 		break; 				// 小天平
case 39: temp = ErwekaIcon24_24B[t]; 		break; 				// 起子扳手
case 40: temp = ErwekaIcon32_32C[t]; 		break; 				// 小表盘
case 41: temp = ErwekaIcon32_32D[t]; 		break; 				// 小打印机
case 42: temp = ErwekaIcon24_24C[t]; 		break; 				// 小喇叭
case 43: temp = ErwekaIcon32_32E[t]; 		break; 				// 小苗
case 44: temp = ErwekaIcon32_32F[t]; 		break; 				// 大表盘
case 45: temp = ErwekaIcon32_16[t]; 		break; 				// 小三角下拉
case 46: temp = ErwekaIcon24_24D[t]; 		break; 				// 小电话
case 47: temp = ErwekaIcon32_32G[t]; 		break; 				// 上下循环双箭头
case 48: temp = ErwekaIcon32_64[t]; 		break; 				// 小温度计
case 49: temp = ErwekaIcon56_56[t]; 		break; 				// 大X
case 50: temp = ErwekaIcon144_168[t]; 	break; 				// 大秒表
case 51: temp = ErwekaIcon204_168[t]; 	break; 				// 大天平
case 52: temp = ErwekaIcon152_184[t]; 	break; 				// 二维码
case 53: temp = ErwekaIcon468_88[t]; 	break; 				// 开机画面1

case 54: temp = ErwekaIcon112_128L[t]; 	break; 				// 上下双循环箭头，Start/Stop
case 55: temp = ErwekaIcon112_128M[t]; 	break; 				// 温度计，Start

case 56: temp = Erweka24Icon32X32[t]; 	break; 				// 小叉
case 57: temp = Erweka25Icon64X64[t]; 	break; 				// 小上箭头
case 58: temp = Erweka26Icon64X64[t]; 	break; 				// 小下箭头

case 59: temp = Erweka27Icon24X24[t]; 	break; 				// 水平双箭头
case 60: temp = ErwekaIcon32_32H[t]; 	break; 				// 'X'编辑标

case 61: temp = Erweka28Icon24X24[t]; 	break; 				// 打印管理图标
case 62: temp = Erweka51Icon112X128[t]; break; 				// 单页打印图标
case 63: temp = Erweka52Icon112X128[t]; break; 				// 全部打印图标
case 64: temp = Erweka29Icon24X24[t]; 	break; 				// 压缩率参考体积

case 66: temp = Erweka31Icon48X48[t]; 	break; 				// 量筒运行
case 67: temp = Erweka32Icon48X48[t]; 	break; 				// 量筒暂停
case 68: temp = Erweka18bIcon64X64[t]; 	break; 				// 循环双箭头大号
    
case 69: temp = Erweka69bIcon42X42[t];	break;				// 解锁						2021/10/29 星期五 9:00:22
case 70: temp = Erweka70bIcon112X128[t];break;				// Export(大)
case 71: temp = Erweka71Icon42X42[t];	break;				// Export(小)

case 72: temp = Erweka39Icon112X128[t]; break; 				// 左侧上下双箭头，右侧三横，去皮图标Tare

case 73: temp = Erweka01cIcon112_128N[t]; break; 			// 测试助手，TestAssist
case 75: temp = ErwekaXunhuan64x64[t]; break; 				// 上下循环双圆弧箭头
case 76: temp = ErwekaShizhong64x64[t]; break; 				// 时钟，4点
case 77: temp = ErwekaM0Mf64x64[t]; break; 						// M0Mf
case 78: temp = ErwekaDuigou48x48[t]; break; 					// 小对勾
case 79: temp = ErwekaDrum64x64[t]; break; 						// 小鼓Drum，Move up
case 80: temp = ErwekaBigDrum256x256[t]; break; 			// 大鼓Drum，运行绿色图标

case 81: temp = ErwekaSmallDrum64x40[t]; break; 			// 小鼓Drum，
case 82: temp = ErwekaMidDrum144x160[t]; break; 			// 中鼓Drum；

case 83: temp = ErwekaLouDou80x104[t]; break; 				// 漏斗定时器
case 84: temp = ErwekaJiaoNang88x96[t]; break; 				// 胶囊10/6.5g
case 85: temp = ErwekaTianPing80x64[t]; break; 				// 中号天平

case 86: temp = ErwekaJiaoNang48x40[t]; break; 				// 小胶囊
case 87: temp = ErwekaMSDrum80x88[t]; break; 					// 中小鼓
case 88: temp = ErwekaBrush80x88[t]; break; 					// 斜刷子

case 89: temp = ErwekaSensor32x32[t]; break; 					// 位置传感器
case 90: temp = ErwekaLouDou80x104A[t]; break; 				// 漏斗定时器，上下颠倒

case 91: temp = ErwekaLAN48X48A[t]; break; 						// 网络断开
case 92: temp = ErwekaLAN48X48B[t]; break; 						// 网络连接
case 93: temp = ErwekaLAN48X48C[t]; break; 						// 网络断开，黑块
case 94: temp = ErwekaLAN48X48D[t]; break; 						// 网络连接，客户提供的新图标
case 95: temp = ErwekaUDisk48X48[t]; break; 					// U盘连接图标

case 96: temp = ErwekaLAN32X32[t]; break; 						// 网络图标

case 97: temp = ErwekaLogin64X56[t]; break; 					// 登录按钮图标
case 98: temp = ErwekaLan52X40[t]; break; 						// 网络连接图标
case 100: temp = ErwekaGear88X72[t]; break; 					// 三齿轮图标
case 101: temp = ErwekaEye32X32[t]; break; 						// 眼睛图标

case 102: temp = ErwekaExit64X56[t]; break; 					// 退出图标
case 103: temp = ErwekaUser32X24[t]; break; 					// 用户管理图标
case 104: temp = ErwekaDatMng32X24[t]; break; 				// 数据管理图标
case 105: temp = ErwekaAdj32X24[t]; break; 						// 校正图标

case 106: temp = ErwekaDT32X32[t]; break; 						// 直径厚度图标
case 107: temp = ErwekaHard48X24[t]; break; 					// 硬度图标
case 108: temp = Erweka_i32X32[t]; break; 						// "i"图标
case 109: temp = Erweka_move112X128[t];break;					// move图标
case 110: temp = Erweka_x48X48[t];break;					// move图标

case 136: temp = Erweka_Direct_48x48[t];break; 					// Direct Measurement
case 137: temp = Erweka_General_48x48[t];break; 				// General Data Measurement
case 111: temp = Erweka_hardness48X48[t];break;					// 硬度
case 112: temp = Erweka_diameter48X48[t];break;					// 直径
case 113: temp = Erweka_thickness48X48[t];break;				// 厚度
case 114: temp = Erweka_weight48X48[t];break;					// 重量

case 115: temp = Erweka_Dynamic136x136[t];break;				// Dynamic
case 116: temp = Erweka_Static136x136[t];break;					// Static

case 117: temp = Erweka_Thickness83x64[t];break;				//	厚度
case 118: temp = Erweka_Diameter71x64[t];break;					//	直径

case 120: temp = Erweka_rotare154x160[t];break;					//	旋转机器
case 121: temp = Erweka_rotare_CW160x154[t];break;				//	旋转机器CW90

case 122: temp = Erweka_NextCW90_128x112[t];break;				//	Next CW90
case 123: temp = Erweka_AbortCW90_128x112[t];break;				//	Abort CW90	
case 124: temp = Erweka_keyboardCW90_377x256[t];break;			//	keyboard CW90

case 125: temp = Erweka_Zero_112x128[t];break;					//	校准清零

case 126: temp = Erweka_Direct_136x168[t];break;				//	Direct Measurement
case 127: temp = Erweka_General_136x168[t];break;				//	General Data Measurement
case 128: temp = Erweka_Weight_96x96[t];break;					//	Weight
case 129: temp = Erweka_Thick_96x96[t];break;					//	Thickness
case 130: temp = Erweka_Width_96x96[t];break;					//	Width
case 131: temp = Erweka_Diam_96x96[t];break;					//	Length/diameter
case 132: temp = Erweka_Hard_96x96[t];break;					//	Hardness


case 133: temp = Erweka_OQ_171x64[t]; 	break; 				// OQ指示灯
case 134: temp = Erweka_dkoe_303x64[t]; 	break; 			// dkoetterheinrich
case 135: temp = Erweka_service_64x64[t]; 	break; 			// 服务菜单

case 140: temp = Erweka_mcnetmode_48x48[t]; 	break; 		// MC.NET mode
case 141: temp = Erweka_switch_112x128[t]; 	break; 			// 开关

case 142: temp = Erweka_unit_48x46[t]; 		break; 			// 单位设置
case 143: temp = Erweka_network_48x46[t]; 	break; 			// 网络设置
case 144: temp = Erweka_config_48x46[t]; 	break; 			// 设备配置
case 145: temp = Erweka_setting_48x46[t]; 	break; 			// 调整参数设置
case 146: temp = Erweka_oqpq_48x46[t]; 		break; 			// OQ/PQ设置

case 147: temp = Erweka_success_40x40[t]; 	break; 			// 对号
case 148: temp = Erweka_fail_40x40[t]; 		break; 			// 错号
case 149: temp = Erweka_successCW90_40x40[t];break; 		// 对号CW90
case 150: temp = Erweka_failCW90_40x40[t]; 	break; 			// 错号CW90

case 151: temp = Erweka_zero_112x128[t]; 	break; 			// 清零
case 152: temp = Erweka_creatuser_112x128[t]; 	break; 		// 添加用户
case 153: temp = Erweka_password_112x128[t]; break; 		// 密码设置
case 154: temp = Erweka_zeroCW90_128x112[t]; break; 		// 清零CW90

case 155: temp = Erweka_shalou_256x256[t]; 	break; 			// 初始化沙漏
case 156: temp = Erweka_shalouCW45_256x256[t]; 	break; 		// 初始化沙漏CW45
case 157: temp = Erweka_success_256x256[t]; 	break; 		// 初始化成功
case 158: temp = Erweka_fail_256x256[t]; 	break; 			// 初始化失败

case 159: temp = Erweka_saveCW90_128x112[t]; 	break; 			// 保存CW90

case 160: temp = Erweka_tanhao_80x80[t]; 	break; 			// 感叹号
case 161: temp = Erweka_duihao_32x32[t]; 	break; 			// 对号

case 162: temp = Erweka_delete_112x128[t]; 	break; 			// 删除
case 163: temp = Erweka_succeed_112x128[t]; break; 			// 初始化成功
case 164: temp = Erweka_failed_112x128[t]; 	break; 			// 初始化失败
case 165: temp = Erweka_shalou_112x128[t]; 	break; 			// 初始化沙漏
case 166: temp = Erweka_retry_112x128[t]; 	break; 			// 重新初始化

case 167: temp = Erweka_product_136x168[t]; break; 			// 产品
case 168: temp = Erweka_method_136x168[t]; break; 			// 方法
case 169: temp = Erweka_create_112x128[t]; break; 			// 创建新的
case 170: temp = Erweka_edit_112x128[t]; break; 			// 编辑	
case 171: temp = Erweka_notfilter_112x128[t]; break; 		// 清除过滤
case 172: temp = Erweka_copy_112x128[t]; break; 			// 复制
case 173: temp = Erweka_filter_48x48[t]; break; 			// 过滤 
*/

#endif
