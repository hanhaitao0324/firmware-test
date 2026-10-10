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

extern char UpdataFlag;


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

#define CorValue			0.00001f	// 单精度浮点数显示所用的修正值

#define WeiRangeL 0.0001f	// 重量公差输入范围(g)
#define WeiRangeR 52.0000f
#define WeiRangeLMg 0.1f	// 重量公差输入范围(g)
#define WeiRangeRMg 52000.0f

#define DimRangeL 0.01f		// 长度公差输入范围(mm)
#define DimRangeR 30.0f

#define HarRangeL 0.1f		// 硬度公差输入范围(N)
#define HarRangeR 500.0f

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

#define REPORT_MAX 18 									// 最多保存20份报告
#define CALI_REPORT_MAX 18
#define REPORT_LONG 6036 								// 一份测试报告的字节长度
#define REPORT_CALI_LONG 353 							// 一份校正报告的字节长度

#pragma pack(1)
typedef struct
{
	RTC_DateTypeDef RPT_Date; 						// 1 报告生成日期
	RTC_TimeTypeDef RPT_Time; 						// 2 报告生成时间
	u8 RPT_Type; 									// 3 报告类型，1：autocal校准报告，2：砝码校准报告，3：长度校准报告，4：直接测试报告 5：一般数据测量报告

	char RPT_DeviceSerialNb[10]; 					// 4 设备ID
	char RPT_MbVersion[10]; 						// 5 主板软件版本
	char RPT_DbVersion[10]; 						// 6 显示板软件版本	

	u32 RPT_xxxxx;	// 
	char RPT_LoginName[20];							// 用户名
	char RPT_LastName[20];							// 姓
	char RPT_FirstName[20];							// 名 sum-103

	// 测试产品初始信息 ---------------------------
	RTC_DateTypeDef RPT_StartDate; 						// 测试开始日期
	RTC_TimeTypeDef RPT_StartTime; 						// 测试开始时间
	u32 RPT_TestRepNum;	// 报告编号
	u8 RPT_TestStatus;									// 测试状态
	char RPT_ProName[30]; 								// 产品名称
	char RPT_ProNumber[30]; 							// 产品编号
	char RPT_ProBatchID[30]; 							// 批次编号
	char RPT_ProInfomation[128]; 						// 产品信息 sum-231
	
	u8 RPT_MeasureMeth;									// 测量方式：0恒速/1恒力
	float RPT_ConstSpeed;								// 恒速值
	int RPT_ConstForce;									// 恒力值
	float RPT_ForceLimit;								// 力传感器测量量程：500N/1000N
	int RPT_Sensitivity;								// 硬度测试接触力：5-20N
	u8 RPT_Dimen;										// 药片形状：0：Diameter 1：Length & Width
	u8 RPT_SizeDetermin;								// 获取长度方式： 0：手动 1：自动
	u8 RPT_NextTablet;									// 下一项测试方式： 0：手动 1：自动

	float RPT_NomiWeight;
	float RPT_NomiThick;
	float RPT_NomiWidth;
	float RPT_NomiLength;
	float RPT_NomiHard; // sum-40

	unsigned short RPT_CountTabletNum;					// 累计测试样品的数量

	u8 RPT_TotalHard;		// 总硬度次数
	u8 RPT_TotalThick;		// 总厚度次数
	u8 RPT_TotalLength;		// 总长度次数
	u8 RPT_TotalWidth;		// 总宽度次数
	u8 RPT_TotalWeight;		// 总重量次数

	u8 RPT_StatiHard;		// 有效硬度次数
	u8 RPT_StatiThick;		// 有效厚度次数
	u8 RPT_StatiLength;		// 有效长度次数
	u8 RPT_StatiWidth;		// 有效宽度次数
	u8 RPT_StatiWeight;		// 有效重量次数

	u8 RPT_WUnit;	// 重量单位
	u8 RPT_LUnit;	// 长度单位
	u8 RPT_HUnit;	// 硬度单位
	float RPT_EVALUATION_WEIGHT[6];					// 最小值，最大值，最大最小差值，平均值，标准偏差值，标准偏差率
	float RPT_EVALUATION_THICKNESS[6];
	float RPT_EVALUATION_WIDTH[6];
	float RPT_EVALUATION_LENGTH[6];
	float RPT_EVALUATION_HARDNESS[6]; // sum-135

	float RPT_RESULTS_WEIGHT[200];
	float RPT_RESULTS_THICKNESS[200];
	float RPT_RESULTS_WIDTH[200];
	float RPT_RESULTS_LENGTH[200];
	float RPT_RESULTS_HARDNESS[200]; // sum-4000
	
	// 时区，夏令时，OQ状态 ------------------------
	char RPT_TimeZone[16]; 								// 设备所处时区
	char RPT_SummerTime[6]; 							// 设备所处季节，夏令时
	char RPT_OQ[10]; 									// 设备校准状态 sum-32

	u8 RPT_Tolerance_WEIGHT[200];
	u8 RPT_Tolerance_THICKNESS[200];
	u8 RPT_Tolerance_WIDTH[200];
	u8 RPT_Tolerance_LENGTH[200];
	u8 RPT_Tolerance_HARDNESS[200]; // sum-1000

	u8 RPT_Tolerance_All[200];	// 1:FM

	u32 PM_FixedUserID;	// 产品ID，不能重复，增加新产品时自动叠加
	u32 PM_Version;		// 产品版本号
	char RPT_MethName[30]; 	// 方法名称
	u32 MM_FixedUserID;	// ID，不能重复，增加时自动叠加
	u32 MM_Version;		// 版本号 

	u32 RPT_UserID;		// 用户ID sum-250

	u8 MM_Evaluation;		//0:Average 1:Nominal
	u8 MM_ToleranceRule;	// 0:T1T2T3 1:MinMaxPL
	u8 MM_Deviation;		// 0:绝对值 1:百分比
	u8 MM_ToleranceType;	// 0:对称 1：非对称		sum-4

	float MM_T1_Left[5];	// Wight Thick Width Length Hard
	float MM_T1_Right[5];
	float MM_T2_Left[5];
	float MM_T2_Right[5];
	float MM_T3_Left[5];	
	float MM_T3_Right[5];	
	float MM_MinMax_Left[5];
	float MM_MinMax_Right[5];
	float MM_PL_Left[5];
	float MM_PL_Right[5];	// sum-200

	char RPT_TabletPress[30]; 		// 
	u8 RPT_RetryEnable;
	u8 RPT_FHard;		// 初始硬度测量次数，不包含retry
	u8 RPT_FThick;		// 初始厚度测量次数，不包含retry
	u8 RPT_FLength;		// 初始长度测量次数，不包含retry
	u8 RPT_FWidth;		// 初始宽度测量次数，不包含retry
	u8 RPT_FWeight;		// 初始重量测量次数，不包含retry
	
	u8 RPT_TestOption;

	uint32_t CRC32; 									// 校验 sum-4 total:6036
}REPORTS_ST;
#pragma pack()

#pragma pack(1)
typedef struct
{
	RTC_DateTypeDef RPT_Date; 						// 1 报告生成日期
	RTC_TimeTypeDef RPT_Time; 						// 2 报告生成时间
	u8 RPT_Type; 									// 3 报告类型，1：动态调整报告，2：静态调整报告，3：长度调整报告 4：动态硬度校正 5：静态硬度校正 6:长度校正 7:重量校正

	char RPT_DeviceSerialNb[10]; 					// 4 设备ID
	char RPT_MbVersion[10]; 						// 5 主板软件版本
	char RPT_DbVersion[10]; 						// 6 显示板软件版本

	u32 RPT_AdjReportNum;	// 调整和校正报告编号
	char RPT_LoginName[20];							// 用户名
	char RPT_LastName[20];							// 姓
	char RPT_FirstName[20];							// 名 sum-103

	// 调整-------------------------------
	RTC_DateTypeDef RPT_NewDate; 						//	最新调整日期
	RTC_TimeTypeDef RPT_NewTime; 						//	最新调整时间

	RTC_DateTypeDef RPT_LastDate; 						//	上次调整日期
	RTC_TimeTypeDef RPT_LastTime; 						//	上次调整时间

	float RPT_TargetValue;								// 目标值
	float RPT_ActualValue;								// 实际值

	float RPT_NewFactor;								// 新系数
	float RPT_OldFactor;								// 旧系数 sum-32

	// 校正--------------------------------
	u8 RPT_Status;	// 调整和校正结果，succeed/failed
	// 硬度校正----------------------------
	float RPT_Tolerance;	// 公差
	float RPT_SetValue[5];		// 设置值，传感器读数
	float RPT_ActValue[5];		// 测量值，autocal读数
	char RPT_BlockSerStr[128];	// 测量工具序列号
	u8 RPT_Result[5];			// 计算结果是否符合公差	sum-178

	
	// 时区，夏令时，OQ状态 ------------------------
	char RPT_TimeZone[16]; 								// 设备所处时区
	char RPT_SummerTime[6]; 							// 设备所处季节，夏令时
	char RPT_OQ[10]; 									// 设备校准状态

	u32 RPT_UserID;		// 操作用户的ID

	uint32_t CRC32; 									// 校验 sum-40 total:353
}REPORTS_CalAdj;	// 调整和校正报告
#pragma pack()

typedef struct
{
	RTC_DateTypeDef QUAL_Date; 						// 1 日期
	RTC_TimeTypeDef QUAL_Time; 						// 2 时间
	char QUAL_TimeZone[16]; 						// 3 时区
}QUAL_ST;

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

// User Management------------------------------------------
#define UserData_Long 		102
#define UserData_LongAct	120		// 用户数据大小(加上预留)
#define User_MAX 103 

#pragma pack(1)
typedef struct
{
	char UD_Username[20];							// 用户名	
	char UD_Password[20];							// 密码
	char UD_FirstName[20];							// 名
	char UD_LastName[20];							// 姓

	char UD_UserStatus;		// 用户状态
	
	u32 UD_FixedUserID;	// 系统生成的用户ID，不能重复，增加新用户时自动叠加

	// 用户权限
	char Perm_Admin; 		// Administration管理员权限，可以进入Control panel页面更改设置参数
	char Perm_UserManage;	// User management权限，可以管理用户及其信息
	char Perm_Reports;		// Reports权限，可以进入Report报告列表查看、打印及导出各种报告
	char Perm_Quali;		// Qualification权限，可以对设备进行调整和校正，也可以进入主页上的OQ状态灯页面
	char Perm_TestManage;	// Test management权限，可以进入产品方法管理页面
	char Perm_Tests;		// Test权限，可以进入产品测试页面
	char Perm_X;			// 
	char Perm_Y;			// 

	u8 PasswordType;		// 密码类型：0：系统生成  1：用户自设
	RTC_DateTypeDef PW_Dead_Date; 		// 密码有效截止日期
	RTC_DateTypeDef PW_Alarm_Date; 		// 密码有效报警日期
}User_Data;		// 占用空间：102字节
#pragma pack()

// Product Management------------------------------------------
 #define ProductMana_Long 		349
 #define Product_MAX 100 

#pragma pack(1)
typedef struct
{
	u8 PM_Status;		// 产品状态
	char PM_ProName[30]; 								// 产品名称
	char PM_ProNumber[30]; 								// 产品编号
	char PM_ProBatchID[30]; 							// 批次编号
	char PM_ProInfomation[128]; 						// 产品信息

	u8 PM_Dimen;	// 产品形状

	u8 PM_WeightUnit;	// 重量单位
	u8 PM_LengthUnit;	// 长度单位
	u8 PM_HardUnit;	// 硬度单位
	float PM_NomiWeight;		// 名义参数
	float PM_NomiThick;
	float PM_NomiWidth;
	float PM_NomiLength;	// length
	float PM_NomiHard;
	
	u32 PM_FixedUserID;	// 产品ID，不能重复，增加新产品时自动叠加
	u32 PM_Version;		// 产品版本号

	char PM_Press[30];

	char PM_LoginName[20];							// 维护人用户名
	char PM_LastName[20];							// 姓
	char PM_FirstName[20];							// 名
	RTC_DateTypeDef PM_Date; 						// 维护日期
	RTC_TimeTypeDef PM_Time; 						// 维护时间

}Product_Mana;		// 占用空间：字节
#pragma pack()
// Method Management------------------------------------------
 #define MethodMana_Long 		343
 #define Method_MAX 100 

#pragma pack(1)
typedef struct
{
	u8 MM_Status;		// 状态
	char MM_MethName[30]; 	// 名称
	u8 MM_MethShape;	// 形状	count-32

	u8 MM_TestType;		// 测试方法：0恒速/1恒力
	float MM_TestSpeed;		// 恒速大小
	short MM_ForceLimit;	// 力上限
	u8 MM_Sensitivity;		// 灵敏度
	u8 MM_VisibleCrack;		// 可见裂缝	0:OFF 1:ON
	u8 MM_Nexttablet;		// 测量方法：自动/手动
	short MM_PauseSec;		// 自动测量时间间隔 count-12

	u8 MM_FixCounts;
	u8 MM_TestOption;
	u16 MM_WeightNum;
	u16 MM_ThickNum;
	u16 MM_WidthNum;
	u16 MM_LengthNum;
	u16 MM_HardNum;	// count-12

	u8 MM_Evaluation;	// 0:Average 1:Nominal
	u8 MM_ToleranceRule;	// 0:T1T2T3 1:MinMaxPL
	u8 MM_Deviation;	// 0:绝对值 1：相对值
	u8 MM_ToleranceType;	// 0:对称 1：不对称   count-4

	float MM_T1_Left[5];	// Wight Thick Width Length Hard
	float MM_T1_Right[5];
	float MM_T2_Left[5];
	float MM_T2_Right[5];
	float MM_T3_Left[5];	
	float MM_T3_Right[5];	
	float MM_MinMax_Left[5];
	float MM_MinMax_Right[5];
	float MM_PL_Left[5];
	float MM_PL_Right[5];	// count-200

	u8 MM_WeightUnit;	// 重量单位
	u8 MM_LengthUnit;	// 长度单位
	u8 MM_HardUnit;	// 硬度单位
	
	u32 MM_FixedUserID;	// ID，不能重复，增加时自动叠加
	u32 MM_Version;		// 版本号

	char MM_LoginName[20];							// 维护人用户名
	char MM_LastName[20];							// 姓
	char MM_FirstName[20];							// 名
	RTC_DateTypeDef MM_Date; 						// 维护日期
	RTC_TimeTypeDef MM_Time; 						// 维护时间

	short MM_TestForce;		// 恒力大小
	u8 MM_RetryEnable;		// retry开关	0:No 1:Yes
	u8 MM_RetryNum;			// retry最大次数
}Method_Mana;
#pragma pack()

#pragma pack(1)
typedef struct
{
	u8 status;
	char CI_Company[30];
	char CI_PhoneNumber[30];
	char CI_Email[30];
	char CI_Info[128];
}ContactInfo;
#pragma pack()

// 错误列表信息----------------------------------
extern u32 ErrorLogNum;
#define ELMaxLength   		128
#define ELActualLength   	86
#define ELNum				4000

#pragma pack(1)
typedef struct
{
	char EL_Username[20];							// 用户名
	char EL_FirstName[20];							// 名
	char EL_LastName[20];							// 姓
	u32 EL_UserID;									// 用户ID
	u32 EL_ErrorCode;								// 错误码
	
	RTC_DateTypeDef EL_Date; 						// 日期
	RTC_TimeTypeDef EL_Time; 						// 时间
	char EL_TimeZone[10]; 							// 时区
}ErrorLog;
#pragma pack()

// 电子称
extern float BalanceWeight; 						// 电子称获取的重量
extern u8 BalanceUnit;
extern u8 WeightFlag; 								// 获取重量标志
extern u8 EBalanceTxCnt; 							// 电子称通信发送计数器
extern u8 EBalanceRxCnt; 							// 电子称通信接收计数器
extern char BalanceSn[]; 							// 电子称序列号字符串

float GetWeight(void);

extern User_Data User_data,User_datatemp1,User_datatemp2;
extern Product_Mana Pro_data;
extern ContactInfo Contact_info;

// 变量声明-------------------------------------------------
extern const uint32_t StId[16];

extern char DeviceSerialNb[]; 					// 设备序列号

extern uint8_t ReportsIp,GenReportsIp;
extern u8 HardAdjRptIp;
extern u8 DiamAdjRptIp;
extern u8 HardCalRptIp;
extern u8 DiamCalRptIp;
extern u8 WeigCalRptIp;

extern u8 User_Num;
extern u8 SysPwIndex;

extern u8 Product_Num;	// 产品数量
extern u32 ProductID;	// 最新产品ID

extern u8 Method_Num;	// 产品数量
extern u32 MethodID;	// 最新产品ID

// 函数声明 ------------------------------------------------
void VDT_System_init(void);
void StartBootloader(void);
void Display_Handler(void);
void logo(void);
void getid(char *buf);
void showID(void);
uint8_t EDIT_Advanced(char *title, char *tStr, uint8_t len, bool sound);
bool ENGINEERING_DevSNum(void);

u8 BalanceInit(void);
void WeiCheReportSave(void);
void TestReportSave(void);
void CanDataHandle(void);

char ExportQual(void);
char ExportTest(void);
void ErrorSave(u32 error);
char* DateTrans(RTC_DateTypeDef str);

void Display_Menu(void);
void Touch_Menu(void);
u8 Get_ServiceMenu(u8 line0,u8 line1);
u8 Get_CPMenu(u8 line0,u8 line1); 			// ConPan Menu 刷新
u8 Get_QualMenu(u8 line0,u8 line1);			// Qualification Meun 刷新
u8 Get_TestReportsList(u8 line0,u8 line1);
u8 Get_ReportsMenu(u8 line0,u8 line1);		// reports Meun 刷新
u8 Get_ReportsList(u8 line0,u8 line1);
u8 Get_UserList(u8 line0,u8 line1);
u8 Get_ProductList(u8 line0,u8 line1);
u8 Get_MethList(u8 line0,u8 line1);

float round0(float);
float round1(float);
float round2(float);
float round3(float);
float round4(float);

double round0d(double);
double round1d(double);
double round2d(double);
double round3d(double);
double round4d(double);

void AbortHandle(void);

extern unsigned char LengthUnit,HardUnit,WeightUnit;
extern u32 TestRepNum,AdjReportNum;
extern u8 ForceSenserType;
extern u8 LongTabhold,ConLidSensor,LoadCellMeaRange;
extern float LengthAdd;
extern float HardTole,DiamTole,WeightTole;
extern short SenMeaRange;
extern short ForceLimit;
extern u8 ExportFlag;
extern u8 AutoPrint,AutoExport;

void setHardUnitFact(u8 unit,float *fact);		// 设置硬度单位
void setLengthUnitFact(u8 unit,float *fact);		// 设置长度单位
void setWeightUnitFact(u8 unit,float *fact);		// 设置重量单位

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

unsigned int GetIni(unsigned int num);
u16 Get_Adc(u8 ch);

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

#endif


