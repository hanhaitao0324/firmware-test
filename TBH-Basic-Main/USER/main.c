
/************************************************
 �?堃电子�?�技
 作者：Gavin
************************************************/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx_conf.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "vinculum.h"
#include "beeper.h"
#include "24cxx.h"
#include "INP.h"
#include "can.h"
#include "motor.h"
#include "AD7190.h"
#include "pwm.h"
#include "rtc.h"
#include "system.h"
#include "eeprom.h"
#include "w25qxx.h"
#include "string.h"
#include "version.h"

#include "updater.h"

#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
#include "udp_demo.h"
#include "lan8720.h"

// SVM_MB
// 库函数版�?
// QIHUAN
// GAVIN

extern u8 SoundSw; 										// 键盘音开关
extern u8 ReportSave; 									// 报告保存标志
extern int8_t TimeZone; 								// 时区
extern u8 DateFormat; 									// 日期格式
extern u8 DaylightTime; 								// 夏令时

extern OQ_ST OQ_PtGp,PQ_PtGp,Password_PtGp; 			// OQ，PQ，用户密码

extern u8 ExportInterface; 								// 报告输出途径，USB，LAN
extern u16 TcpPort; 									// 端口号
extern u8 DhcpSw; 										// DHCP开关
extern u8 PrtIP; 										// 网络打印机IP
extern u8 PrintType; 									// 打印类型 0：USB 1：LAN
extern u8 BalanceModel;

typedef struct
{
	uint32_t id[3];
}ChipID; 												// 芯片ID

uint32_t M407ID = 0; 									// 96转10位ID

const uint32_t StId[16] = 							// 十六组随机数
{
	499530, 566640, 752395, 120907,
	471186, 100345, 362137, 742371,
	290819, 500840, 689457, 241507,
	315809, 755724, 800502, 285197,
};

const uint32_t PsId[40] = 							// 40组随机数，用于重置密码激活码
{
	934769, 456434, 338702, 617318, 737614,
  	744255, 719715, 244117, 837043, 756451,
  	934141, 127039, 580191, 323951, 980898,
  	621566, 935678, 114353, 521098, 980640,
  	144494, 382308, 109701, 140036, 642865,
  	408337, 721120, 940467, 454553, 971846,
  	969139, 453618, 919225, 629771, 711496,
  	464426, 189122, 716139, 990195, 656344
};

uint32_t Password_Code[20];							// 20组重置密码激活码
uint32_t tmpPasswordCode; 							// 临时重置密码激活码

// 获取芯片ID --------------------------------------------
void Get_ChipID(void);

// 验证重置密码激活码 --------------------------------------
char VerifyCode(char *);

extern void Display_Handler(void);
extern void Communicate_Handler(void);
extern void RS485_Handler(void);
extern void Balance_Handler(void);
extern void Print_Handler(void);

void Display_thread(void const *args);
void Communicate_thread(void const *args);
void PrintReprot_thread(void const *args);

void Udisk_thread(void const *args);

osThreadId DispTask;                        /* assigned task id of task:  DispTask */
osThreadId CommTask; 												/* assigned task id of task:  CommTask */
osThreadId PrintTask;
osThreadId UdiskTask;											//用于U盘的检测

osThreadDef(Display_thread, 	osPriorityHigh, 1, 2048 );                      
osThreadDef(Communicate_thread, osPriorityNormal, 1, 1024 );
osThreadDef(PrintReprot_thread, osPriorityNormal, 1, 2048);
osThreadDef(Udisk_thread, osPriorityNormal, 1, 512);

extern u8 UsbInUse; 												// USB占用标志
void Udisk_thread(void const *args)
{
	while(1)
	{
		if(UPDATER_Busy)
			osThreadTerminate(UdiskTask);
		
		// if(!UsbInUse) 											// USB接口空闲
		// 	DetectionUdisk();
		
		osDelay(500);
	}
}

void Display_thread(void const *args)
{
  while (1)
  {
  	if(UPDATER_Busy)
		osThreadTerminate(DispTask);
		
    Display_Handler();
    osDelay(100);
  }
}

void Communicate_thread(void const *args)
{ 
  while(1) 
  {
  	if(RS232_RxFlag)
  	{
  		RS232_RxFlag = 0;
  		Communicate_Handler();
  	}
  	else
  	if(RS485_RxFlag)
  	{
  		RS485_RxFlag = 0;
  		RS485_Handler();
  	}
  	else
  	if(Balance_RxFlag)
  	{
  		Balance_RxFlag = 0;
  		Balance_Handler();
  	}
  	else
  		osSignalWait(0x1, osWaitForever);
  	
    osDelay(1);
  }
}

void PrintReprot_thread(void const *args)
{
	while(1)
	{		
		// Signal flags that are reported as event are automatically cleared.
		osSignalWait(0x2, osWaitForever);
		Print_Handler();
		osDelay(100);
  	}
}

//ADC初始化,ADC1的通道10,硬件连接PC0
void  Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	//先初始化ADC1通道10IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PC0 通道10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频,ADCCLK=PCLK2/4 = 84/4 = 21Mhz,ADC时钟最好不要超过36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;//10位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测,使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化

	ADC_Cmd(ADC1, ENABLE);//开启AD转换器
}

int main(void)
{
	u8 u8dat = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	INP_Init();									// 外部接近开关
	Adc_Init();
	// CAN初始化普通/环回模式，波特率500Kbps -----------------------
  	CAN_ModeInit();
  
	//定时器时钟84M，分频系数8400，所以84M/84=1Mhz的计数频率，计数1000次为1ms
	TIM4_Int_Init(1000-1,84-1);	
	USART1_Init(); 							// Usart1 for RS232 Port
  	USART3_Init(); 							// Usart3 for Vinculum-II
  	USART6_Init(); 							// Usart6 for Balance
	LED_Init();
	BEEPER_Init();
	My_RTC_Init();
	AT24CXX_Init();
	W25QXX_Init();			//W25QXX初始化
	MOTOR_Init();
	ad7190_Init();
	
	mymem_init(SRAMIN);		//初始化内部内存池
	
	readeepbuf((unsigned char *)MAIN_HARDWARE_VERSION, ADR_HTVER, HVER_MAX_LEN);
	if((MAIN_HARDWARE_VERSION[0] < 0x30)||
		 (MAIN_HARDWARE_VERSION[0] > 0x39))
	{
		strcpy(MAIN_HARDWARE_VERSION,"1.00.0");
		writeepbuf((unsigned char *)MAIN_HARDWARE_VERSION, ADR_HTVER, HVER_MAX_LEN);
	}
	
	VINC_Init();													// Vinculum
	
	Get_ChipID();
	
	// 读取内部参数 start----------------------------------------------------------------------------------
	readeepbuf((unsigned char *)DeviceSerialNb, ADRSERNUM, 6); 				// 设备序列号
	DeviceSerialNb[6] = '\0';
	SoundSw = readeeprom(ADRSOUNDSW);
	//ReportSave = readeeprom(ADR_REPSAVE); 					// 读取报告保存标志
	ReportsIp = readeeprom(ADR_REPORTSIP); 					// 最新测试报告指针
	if(ReportsIp>20) ReportsIp = 0;
	GenReportsIp = readeeprom(ADR_GENREPORTSIP);
	if(GenReportsIp>20) GenReportsIp = 0;
	TimeZone = readeeprom(ADR_TIMEZONE); 						// 时区读取
	if(TimeZone > 37 || TimeZone < 0) TimeZone = 14;
	DateFormat = readeeprom(ADR_DATEFORMAT); 				// 日期格式
	if(DateFormat>2) DateFormat = 0;
	DaylightTime = readeeprom(ADR_DAYLIGHTTIME); 		// 夏令时
	
	LongTabhold = readeeprom(ADRLongTabhold);
	if(LongTabhold > 1) LongTabhold = 0;

	if(LongTabhold == 1) 
		LengthAdd = 30.0f;
	else 
		LengthAdd = 0;

	ConLidSensor = readeeprom(ADRConLidSensor);
	if(ConLidSensor > 1) ConLidSensor = 0;

	loadfloat(&HardTole,ADRHardTole);
	if(HardTole > 10.0f || HardTole < 1.0f) HardTole = 1.0f;
	loadfloat(&DiamTole,ADRDiamTole);
	if(DiamTole > 1.0f || DiamTole < 0.03f) DiamTole = 0.05f;
	loadfloat(&WeightTole,ADRWeightTole);
	if(WeightTole > 1.0f || WeightTole < 0.1f) WeightTole = 0.1f;
	
	readeepbuf((unsigned char *)&OQ_PtGp, OQ_ST_ADDR, 18); 				// OQ
	readeepbuf((unsigned char *)&PQ_PtGp, PQ_ST_ADDR, 18); 				// PQ
	readeepbuf((unsigned char *)&Password_PtGp, Password_ST_ADDR, 18); 				// Password
	
	User_Num = readeeprom(ADRUserNum);
	if(User_Num > 103 || User_Num < 3) User_Num = 3;

	readeepbuf((unsigned char *)&User_datatemp1, ADR_User_admin, 20);
	if(strcmp(User_datatemp1.UD_Username,"admin")!=0)
	{
		strcpy(User_datatemp1.UD_Username,"admin");
		strcpy(User_datatemp1.UD_Password,"Admin321");
		strcpy(User_datatemp1.UD_FirstName,"ERWEKA");
		strcpy(User_datatemp1.UD_LastName,"Administrator");
		User_datatemp1.UD_UserStatus = 1;
		User_datatemp1.UD_FixedUserID = 1;

		User_datatemp1.Perm_Admin = 1;
		User_datatemp1.Perm_UserManage = 1;
		User_datatemp1.Perm_Reports = 0;
		User_datatemp1.Perm_Quali = 0;
		User_datatemp1.Perm_TestManage = 0;
		User_datatemp1.Perm_Tests = 0;
		User_datatemp1.Perm_X = 0;
		User_datatemp1.Perm_Y = 0;

		writeepbuf((unsigned char *)&User_datatemp1, ADR_User_admin, UserData_Long);
	}

	readeepbuf((unsigned char *)&User_datatemp1, ADR_User_operator, 20);
	if(strcmp(User_datatemp1.UD_Username,"operator")!=0)
	{
		strcpy(User_datatemp1.UD_Username,"operator");
		strcpy(User_datatemp1.UD_Password,"Operator321");
		strcpy(User_datatemp1.UD_FirstName,"ERWEKA");
		strcpy(User_datatemp1.UD_LastName,"Operator");
		User_datatemp1.UD_UserStatus = 1;
		User_datatemp1.UD_FixedUserID = 2;

		User_datatemp1.Perm_Admin = 0;
		User_datatemp1.Perm_UserManage = 0;
		User_datatemp1.Perm_Reports = 1;
		User_datatemp1.Perm_Quali = 1;
		User_datatemp1.Perm_TestManage = 1;
		User_datatemp1.Perm_Tests = 1;
		User_datatemp1.Perm_X = 0;
		User_datatemp1.Perm_Y = 0;

		writeepbuf((unsigned char *)&User_datatemp1, ADR_User_operator, UserData_Long);
	}

	readeepbuf((unsigned char *)&User_datatemp1, ADR_User_service, 20);
	if(strcmp(User_datatemp1.UD_Username,"service")!=0)
	{
		strcpy(User_datatemp1.UD_Username,"service");
		strcpy(User_datatemp1.UD_Password,"Service321");
		strcpy(User_datatemp1.UD_FirstName,"ERWEKA");
		strcpy(User_datatemp1.UD_LastName,"Service");
		User_datatemp1.UD_UserStatus = 1;
		User_datatemp1.UD_FixedUserID = 3;

		User_datatemp1.Perm_Admin = 1;
		User_datatemp1.Perm_UserManage = 1;
		User_datatemp1.Perm_Reports = 1;
		User_datatemp1.Perm_Quali = 1;
		User_datatemp1.Perm_TestManage = 1;
		User_datatemp1.Perm_Tests = 1;
		User_datatemp1.Perm_X = 1;
		User_datatemp1.Perm_Y = 1;

		writeepbuf((unsigned char *)&User_datatemp1, ADR_User_service, UserData_Long);
	}

	ErrorLogNum = readinteep(ADRErrorNum);
	if(ErrorLogNum > 4000) ErrorLogNum = 0;

	Product_Num = readeeprom(ADRProductNum);
	if(Product_Num > 100) Product_Num = 0;
	ProductID = readinteep(ADRProductID);

	Method_Num = readeeprom(ADRMethodNum);
	if(Method_Num > 100) Method_Num = 0;
	MethodID = readinteep(ADRMethodID);

	//Gavin�?2023-03-13 周一 14:16:43 -----------------------------------------------------
	AutoPrint = readeeprom(ADRAutoPrint);
	if(AutoPrint > 1) AutoPrint = 0;
	AutoExport = readeeprom(ADRAutoExport);
	if(AutoExport > 1) AutoExport = 0;

	ExportFlag = readeeprom(ADRExportFlag);
	if(ExportFlag > 1) ExportFlag = 0;
	ExportInterface = readeeprom(ADR_Export_Interface);
	if(ExportInterface > 1) ExportInterface = 0;
	
	readeepbuf((unsigned char *)&eepLwipDev, ADR_LWIP_DEV, 23); 				// LWIP结构体
	readeepbuf((unsigned char *)&TcpPort, ADR_TCP_PORT, 2); 						// 端口号
	DhcpSw = readeeprom(ADR_DHCP_SW); 																	// DHCP
	
	readeepbuf((unsigned char *)&PrtIP, ADR_PRT_IP, 4); 								// 网络打印机IP地址
	PrintType = readeeprom(ADR_PRT_TYPE);
	
	BalanceModel = readeeprom(ADR_BAL_MODEL); 			// 读取电子秤型号
	if(BalanceModel > 2)
	{
		BalanceModel = 0;
		writeeeprom(BalanceModel, ADR_BAL_MODEL);
	}
	
	ContactForce = readinteep(ADRCONTACTFORCE);
	if(ContactForce > 10 || ContactForce < 3) ContactForce = 10;
	Sensitivity = readinteep(ADRSENSIBILITY);
	if(Sensitivity > 20 || Sensitivity < 3) Sensitivity = 10;
	loadfloat(&FastMoveSP,ADRFASTMOVESP);
	if(FastMoveSP > 45.0f || FastMoveSP < 3.0f) FastMoveSP = 10.0f;
	blops = readinteep(ADRBLPOS);
	if(blops > 900000 || blops < 800000) blops = 857600;
	loadfloat(&DiaMeasSpeed,ADRDIAMEASSPEED);
	if(DiaMeasSpeed > 3.0f || DiaMeasSpeed < 1.0f) DiaMeasSpeed = 2.3f;
	loadfloat(&mmfactor,ADRMMFAK);
	if(mmfactor > 26200.0f || mmfactor < 25200.0f) mmfactor = 25600.0f;

	ForceSenserType=readeeprom(ADRSenserType);
	if(ForceSenserType != 50 && ForceSenserType != 100) ForceSenserType = 50;

	LoadCellMeaRange = readeeprom(ADRLoadCellMeaRange);
	if(LoadCellMeaRange > 1) LoadCellMeaRange = 0;
	if(ForceSenserType == 50 && LoadCellMeaRange == 1) LoadCellMeaRange = 0;
	
	if(LoadCellMeaRange == 0) SenMeaRange = 500;
	else if(LoadCellMeaRange == 1) SenMeaRange = 1000;

	loadfloat(&factor,ADRNWTFAK);
	if(ForceSenserType == 50)
	{
		if(factor > 35.0f || factor < 30.0f) factor = 33.3f;
	}
	else if(ForceSenserType == 100)
	{
		if(factor > 70.0f || factor < 60.0f) factor = 66.7f;
	}

	TestRepNum = readinteep(ADRTestRepNum);
	AdjReportNum = readinteep(ADRAdjReportNum);

	HardAdjRptIp = readeeprom(ADR_HardAdjRptIp); 					// 最新硬度调整报告指针
	if(HardAdjRptIp>20) HardAdjRptIp = 0;
	DiamAdjRptIp = readeeprom(ADR_DiamAdjRptIp); 					// 最新长度调整报告指针
	if(DiamAdjRptIp>20) DiamAdjRptIp = 0;
	HardCalRptIp = readeeprom(ADR_HardCalRptIp); 					// 最新硬度校正报告指针
	if(HardCalRptIp>20) HardCalRptIp = 0;
	DiamCalRptIp = readeeprom(ADR_DiamCalRptIp); 					// 最新长度校正报告指针
	if(DiamCalRptIp>20) DiamCalRptIp = 0;
	WeigCalRptIp = readeeprom(ADR_WeigCalRptIp); 					// 最新重量校正报告指针
	if(WeigCalRptIp>20) WeigCalRptIp = 0;

	SysPwIndex = readeeprom(ADRSysPwIndex);
	if(SysPwIndex>15)
	{
		SysPwIndex = 0;
		writeeeprom(SysPwIndex,ADRSysPwIndex);
	}

	// 读取内部参数 end------------------------------------------------------------------------

	u8dat = lwip_comm_init();
	if(u8dat == 0)
	{
		if(DhcpSw) 								// �?动获取IP
		{
			//LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
			//等待DHCP获取 
		 	//LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
		 	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
			{
				lwip_periodic_handle();
			}
		}
		
		EthInitStatus = ETH_SUCCESS;
	}
	else
	{
		if(u8dat != 1)
		{
			ETH_Mem_Free();
			lwip_comm_mem_free();
		}
	}
	
	// End�?2023-03-13 周一 14:17:07 -------------------------------------------------------
	
	BEEPER_PlayTones(tones_startup);
	PowOnDelay = 1500;
	
 	osKernelInitialize ();                    // initialize CMSIS-RTOS
 	DispTask = osThreadCreate(osThread(Display_thread), NULL);
	CommTask = osThreadCreate(osThread(Communicate_thread), NULL);
	PrintTask = osThreadCreate(osThread(PrintReprot_thread), NULL);
	
	//UdiskTask = osThreadCreate(osThread(Udisk_thread), NULL);

	osKernelStart ();                         // start thread execution 	
}

// 获取�?片ID�? --------------------------------------------
void Get_ChipID(void)
{
	uint8_t udat,i,Psdat;
	ChipID chipid = {0};
	
	chipid.id[0] = *(__I uint32_t *)(0x1FFF7A10 + 0x00);
	chipid.id[1] = *(__I uint32_t *)(0x1FFF7A10 + 0x04);
	chipid.id[2] = *(__I uint32_t *)(0x1FFF7A10 + 0x08);
	
	M407ID = chipid.id[0]^chipid.id[1]^chipid.id[2];
	
	for(i=0;i<8;i++)
	{
		udat = (uint8_t)(0x0000000f&(M407ID>>(7-i)*4));
		if(udat != 0)
			break;
	}
	
	Psdat = udat;
	
	for(i=0;i<16;i++)																	//根据ID号获取重�?密码的激活码
	{
		Password_Code[i] = PsId[Psdat]^M407ID;
		Psdat++;
		Password_Code[i] = Password_Code[i]%1000000;
	}	
}
// 验证重置密码激活码 ------------------------------------------2021/10/26 星期�? 18:09:25
char VerifyCode(char *temKeyStr)
{
	sscanf(temKeyStr,"%u",&tmpPasswordCode);
	for(int i=0;i<16;i++)
	{
		if(Password_Code[i]==tmpPasswordCode)			// 先验证激活码是否正确
		{
			if(readeeprom(Password_COD_ADDR+i)==1)		// 激活码已使用
				return 0;
			else
			{     
				writeeeprom(0x01,Password_COD_ADDR+i);	// 激活码未使用，记录该激活码使用过了
				return 1;	
			}																	
		}	
	}
	return 0;
}

// End -----------------------------------------------------


