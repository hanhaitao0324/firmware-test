/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    system.h
 *      Purpose: system defines & constants
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
// Define to prevent recursive inclusion -------------------------------------
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "const.h"
//#include "mcp3201.h"
#include "timedate.h"
#include "stm32f4xx.h" 

/**
 * The STM32 factory-programmed UUID memory.
 * Three values of 32 bits each starting at this address
 * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
 */
#define STM32_UUID ((uint32_t *)0x1FFFF7E8)

extern const char DEVICE_VERSION[];
extern const char DEVICE_BETAVERSION[];
extern const char DEVICE_SONDERVERSION[];
extern const char DEVICE_FWDATE[];
#define SERIAL_NUMBER_LEN   6

#define PRESSURE_RISE				600	// Druckanstieg Timeout sec x10
#define MAXVACUM						900 // 900mbar
#define MAXVACUM_EXISCCATOR 700 // Exisccator Size > 250mm

#define RAM_KEY_ADDR        (0x20002000)
#define BOOTLOADER_KEY1     (0x12345678)
#define BOOTLOADER_KEY2     (0x43211234)

#define SILENT_MODE 2
#define VERBOSE_MODE 1
#define NORMAL_EXECUTION 0

#define COMMUNICATION_UART			USART1
#define COMMUNICATION_BAUD			UART1_BAUD

enum PRINT_REPORT_LINE_NB
{
	PRT_01,
	PRT_02,
};

enum XML_REPORT_LINE_NB
{
	XML_VER_UTF,
	XML_DATA_0,
		
		XML_VER_01,
		XML_DATE_01,
		XML_TYPE_01,
	
		XML_INFO_0,
			XML_DEV_01,
			XML_SN_01,
			XML_VERFMB_01,
			XML_VERFTB_01,
			XML_MDUSP_01,
			XML_NB_STATION_01,
			XML_QUALI_01,
		XML_INFO_1,
		
		XML_STATION1_0,
			XML_NAME1_01,
			XML_PRD1_0,
				XML_PDNAME1_01,
				XML_BCHNB1_01,
				XML_CYLNB1_01,
				XML_INFOMATION1_01,
			XML_PRD1_1,
			XML_PAR1_0,
				XML_USPMHD1_01,
				XML_SPEED1_01,
				XML_VWGT1_01,
				XML_VVOL1_01,
				XML_TAPHEIGHT1_01,
			XML_PAR1_1,
			XML_TEST1_0,
				XML_STEP1_0,
					XML_PM11_01,
					XML_LM11_01,
					XML_PM12_01,
					XML_LM12_01,
					XML_PM13_01,
					XML_LM13_01,
					XML_PM14_01,
					XML_LM14_01,
					XML_PM15_01,
					XML_LM15_01,
				XML_STEP1_1,
				XML_DENT1_01,
				XML_SPEEDMIN1_01,
				XML_SPEEDMAX1_01,
				XML_TAPS1_01,
				XML_ENDDT1_01,
				XML_STATUS1_01,
			XML_TEST1_1,
		XML_STATION1_1,
		
		XML_STATION2_0,
			XML_NAME2_01,
			XML_PRD2_0,
				XML_PDNAME2_01,
				XML_BCHNB2_01,
				XML_CYLNB2_01,
				XML_INFOMATION2_01,
			XML_PRD2_1,
			XML_PAR2_0,
				XML_USPMHD2_01,
				XML_SPEED2_01,
				XML_VWGT2_01,
				XML_VVOL2_01,
				XML_TAPHEIGHT2_01,
			XML_PAR2_1,
			XML_TEST2_0,
				XML_STEP2_0,
					XML_PM21_01,
					XML_LM21_01,
					XML_PM22_01,
					XML_LM22_01,
					XML_PM23_01,
					XML_LM23_01,
					XML_PM24_01,
					XML_LM24_01,
					XML_PM25_01,
					XML_LM25_01,
				XML_STEP2_1,
				XML_DENT2_01,
				XML_SPEEDMIN2_01,
				XML_SPEEDMAX2_01,
				XML_TAPS2_01,
				XML_ENDDT2_01,
				XML_STATUS2_01,
			XML_TEST2_1,
		XML_STATION2_1,
	
	XML_DATA_1
};

extern uint8_t CurntPage; 							// 页码

typedef struct
{
  char      SerialNumber[SERIAL_NUM_MAX_LEN+2];//8
////  uint8_t			  DeviceType;//1
////  uint16_t     	Password;//2
	uint8_t 	  Language;//1
//	uint8_t				NoLicens;//1
//	char			messTime[6];//6
//	int				messZeitL;//4
//	int				messZeitH;//4
//	int				tmode;//4
//	uint32_t				FullDays;//4
//	uint32_t				Periods;//4
//	uint32_t				AktPeriods;//4
//	uint32_t				PrinterFrames;//4
//	uint8_t				Autoprint;//1
//	int 			Penetration;//4
////	uint32_t				DaysLeft;//4
////	double		dateoffset;//8
////	int				LastDay;//4
////	DTIM 			LastDate;//6
	uint32_t	Exisccator_size;//4
	uint8_t		Penetration_inverse;//1
//	
  uint8_t   Empty[7];
  uint32_t  CRC32;
} Platform_Data_s;

//typedef struct{
//	 Platform_Data_s 	Platform_Data;
//	 MCP3201_Data_s		MCP_3201_Data;
//}SYSTEM_Data_s;

//extern SYSTEM_Data_s SYSTEM;
//extern const MCP3201_Data_s MCP3201_Default;

extern Platform_Data_s *Platform_Data;
//extern MCP3201_Data_s	*ADC_Data;

#define REPORT_PAGE_MAX 1 							// 每页显示9条报告记录，根据报告份数计算最大页码，0算一页
#define REPORT_MAX 18 									// 最多保存20份报告
#define REPORT_LONG 500 								// 一份报告的字节长度
typedef struct
{
	RTC_DateTypeDef RPT_Date; 						// 1 报告日期
	RTC_TimeTypeDef RPT_Time; 						// 2 报告时间
	uint16_t RPT_Type; 										// 3 报告类型，快速测试(计次数/计时间)，压缩率测试

	char RPT_DeviceSerialNb[10]; 					// 4 设备ID
	char RPT_MbVersion[10]; 							// 5 主板软件版本
	char RPT_DbVersion[10]; 							// 6 显示板软件版本
	
	uint16_t RPT_DeviceType; 							// 7 设备类型，121/122/221/222/223
	
	// 站1测试产品初始信息 ---------------------------
	char RPT_S1PName[16]; 								// 8 产品名称
	char RPT_S1PInfo[78]; 								// 9 产品备注
	char RPT_S1PBatchNb[16]; 							// 10 产品批号
	char RPT_S1CylinderNb[16]; 						// 11 量筒编号
	char RPT_S1CylinderSize[10]; 					// 12 量筒尺寸
	
	uint16_t RPT_S1Method; 								// 13 测试方法
	uint16_t RPT_S1Speed; 								// 14 电机转速
	uint16_t RPT_S1SetTapsNb; 						// 15 设置拍击次数
	uint16_t RPT_S1SetRunTime; 						// 16 设置运行时间
	uint16_t RPT_S1ReferenceVolume; 			// 17 参考体积，V0/V10
	
	uint16_t RPT_S1InitialVolume; 				// 18 初始体积
	uint16_t RPT_S1SampleMass; 						// 19 样品质量
	
	// 站2测试产品初始信息 -----------------------------
	char RPT_S2PName[16]; 								// 20 产品名称
	char RPT_S2PInfo[78]; 								// 21 产品备注
	char RPT_S2PBatchNb[16]; 							// 22 产品批号
	char RPT_S2CylinderNb[16]; 						// 23 量筒编号
	char RPT_S2CylinderSize[10]; 					// 24 量筒尺寸
	
	uint16_t RPT_S2Method; 								// 25 测试方法
	uint16_t RPT_S2Speed; 								// 26 电机转速
	uint16_t RPT_S2SetTapsNb; 						// 27 设置拍击次数
	uint16_t RPT_S2SetRunTime; 						// 28 设置运行时间
	uint16_t RPT_S2ReferenceVolume; 			// 29 参考体积，V0/V10
	
	uint16_t RPT_S2InitialVolume; 				// 30 初始体积
	uint16_t RPT_S2SampleMass; 						// 31 样品质量
	
	// 时区，夏令时，OQ状态 ------------------------
	char RPT_TimeZone[16]; 								// 32 设备所处时区
	char RPT_SummerTime[6]; 							// 33 设备所处季节，夏令时
	char RPT_OQ[10]; 											// 34 设备校准状态
	
	// 站1测试数据 ---------------------------------
	uint16_t RPT_S1V10; 									// 35 拍击10次体积
	uint16_t RPT_S1V500; 									// 36 拍击500次体积
	uint16_t RPT_S1V1250; 								// 37 拍击1250次体积
	
	uint16_t RPT_S1Delay[12]; 						// 38 拍击延迟1次体积
	uint16_t RPT_TESTENDS1; 							// 39 拍击循环次数
	
	float RPT_S1InitialDensity; 					// 40 初始密度
	float RPT_S1TappedDensity; 						// 41 完成密度
	float RPT_S1HausnerRatio; 						// 42 压缩比
	float RPT_S1Compressibility; 					// 43 压缩率
	
	float RPT_S1MinSpeed; 								// 44 测试速度下限
	float RPT_S1MaxSpeed; 								// 45 测试速度上限
	
	// 站2测试数据 ---------------------------------
	uint16_t RPT_S2V10; 									// 46 拍击10次体积
	uint16_t RPT_S2V500; 									// 47 拍击500次体积
	uint16_t RPT_S2V1250; 								// 48 拍击1250次体积
	
	uint16_t RPT_S2Delay[12]; 						// 49 拍击延迟1次体积
	uint16_t RPT_TESTENDS2; 							// 50 拍击循环次数
	
	float RPT_S2InitialDensity; 					// 51 初始密度
	float RPT_S2TappedDensity; 						// 52 完成密度
	float RPT_S2HausnerRatio; 						// 53 压缩比
	float RPT_S2Compressibility; 					// 54 压缩率
	
	float RPT_S2MinSpeed; 								// 55 测试速度下限
	float RPT_S2MaxSpeed; 								// 56 测试速度上限
	
	uint16_t RPT_RealTapsNb; 							// 57 实际拍击次数
	uint16_t RPT_RealRunTime; 						// 58 实际运行时间
	uint16_t RPT_TestStatus; 							// 59 测试状态，完成/中断
	uint16_t RPT_V0_V10; 									// 60 压缩率测试参数，初始体积带入V0还是V10
	
	uint32_t CRC32; 											// 61 校验
}REPORTS_ST; 

// OQ ------------------------------------------------------
typedef struct
{
	u16 OQ_sw; 														// 2 OQ使能标志
	u16 OQ_epd; 													// 2 OQ有效期（天数）
	u16 OQ_wbed; 													// 2 OQ报警期（剩余天数）
	
	RTC_DateTypeDef OQ_PON_dst; 					// 4 OQ开始日期
	RTC_DateTypeDef OQ_EON_dst; 					// 4 OQ截止日期
	RTC_DateTypeDef OQ_AON_dst; 					// 4 OQ报警日期
}OQ_ST;

// Control Panel 
#define CONTROL_PANEL_PAGE_MAX 1 				// 每页显示9条项目记录，根据项目份数计算最大页码，0算一页

// 函数声明 ------------------------------------------------
void VDT_System_init(void);
void StartBootloader(void);
void Display_Handler(void);
void logo(void);
void getid(char *buf);
void showID(void);
uint8_t EDIT_Advanced(char *title, char *tStr, uint8_t len, bool sound);
bool ENGINEERING_DevSNum(void);

enum Chancel_Reason
{
  CHANCEL_NO,
  CHANCEL_USER,
  CHANCEL_PRESS_TIMEOUT,
  CHANCEL_FINISH
};

//typedef struct DateTime {
//	unsigned int day;
//	unsigned int month;
//	unsigned int year;
//	unsigned int hour;
//	unsigned int min;
//	unsigned int sec;
//}DTIM;

//#define PRELL 1000
//#define ENDPCM 'Y'

//double JulDat(DTIM dtim);
//DTIM GregDat(double JulDat);




//0,不支持ucos
//1,支持ucos
//#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 


// ConPan Menu 刷新-------------------------------
u8 Get_CPMenu(u8 line0,u8 line1);

#endif


