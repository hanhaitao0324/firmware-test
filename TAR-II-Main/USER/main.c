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
#include "pwm.h"
#include "rtc.h"
#include "system.h"
#include "eeprom.h"
#include "string.h"
#include "version.h"

#include "updater.h"
//#include "usbd_cdc_vcp.h"
//#include "usbd_usr.h"

#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
//#include "tcp_client_demo.h"
//#include "tcp_server_demo.h"
#include "udp_demo.h"
//#include "httpd.h"
#include "lan8720.h"

// SVM_MB
// 库函数版本
// QIHUAN
// GAVIN

extern char DeviceSerialNb[]; 					// 设备序列号
extern u8 SoundSw; 											// 键盘音开关
extern u8 ReportSave; 									// 报告保存标志，Gavin，2022-12-08 周四 11:14:20
extern u8 PasswordSW;										// 密码开关 2021/10/25 12:01:46
extern uint8_t ReportsIp; 							// 最新测试报告指针
extern int8_t TimeZone; 								// 时区
extern u8 DateFormat; 									// 日期格式
extern u8 DaylightTime; 								// 夏令时
extern u8 CompressibilityPt; 						// 压缩率计算带入参数

extern OQ_ST OQ_PtGp; 									// OQ

extern u8 ExportInterface; 							// 报告输出途径，USB，LAN
extern u8 ExportFormat; 								// 报告输出格式
extern u16 TcpPort; 										// 端口号
extern u8 DhcpSw; 											// DHCP开关
extern u8 PrtIP; 												// 网络打印机IP
extern u16 PrtPort;											// 网络打印机端口号
extern u8 PrintType; 										// 打印类型，0：USB；1：LAN；Gavin，2023-01-30 周一 14:54:34
extern u8 PrintFormat; 									// 打印格式，0：PCL；1：ASC-II
extern u8 BalanceModel; 								// 电子秤型号，0：Mettler Me 54；1：Sartorius Practum；Gavin，2023-08-04 周五 12:57:13

typedef struct
{
	uint32_t id[3];
}ChipID; 																// 芯片ID

uint32_t M407ID = 0; 										// 96转10位ID

const uint32_t StId[16] = 							// 十六组随机数
{
	499530, 566640, 752395, 120907,
	471186, 100345, 362137, 742371,
	290819, 500840, 689457, 241507,
	315809, 755724, 800502, 285197,
};

const uint32_t PsId[40] = 							// 40组随机数，用于重置密码激活码		2021/10/26 星期二 16:04:18
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
uint32_t Activation_Code[6]; 						// 第六个是正在使用的，前5个是计算出来的 
uint32_t tmpActivationCode; 						// 临时激活码

uint32_t Password_Code[20];							// 20组重置密码激活码
uint32_t tmpPasswordCode; 							// 临时重置密码激活码

extern uint8_t DeviceID;
extern char ConfigCode[];
extern char ConfigCodeSv[];

// 记录重置码用的数量

char CodeNum = 0;
// 获取芯片ID号 --------------------------------------------
void Get_ChipID(void);

// 验证重置密码激活码 --------------------------------------
char VerifyCode(char *);

extern void Display_Handler(void);
extern void Communicate_Handler(void);
//extern void Can_Handler(void);
extern void Print_Handler(void);

void Display_thread(void const *args);
void Communicate_thread(void const *args);
//void Can_thread(void const *args);
void PrintReprot_thread(void const *args);

void Udisk_thread(void const *args);			//2022/1/5 星期三 16:28:13

osThreadId DispTask;                        /* assigned task id of task:  DispTask */
osThreadId CommTask; 												/* assigned task id of task:  CommTask */
//osThreadId Can_Task; 												// 已分配任务Id的任务：Can_Task
osThreadId PrintTask;
osThreadId UdiskTask;											//用于U盘的检测  2022/1/5 星期三 16:08:45

osThreadDef(Display_thread, 		osPriorityHigh, 1, 2048 );                       
osThreadDef(Communicate_thread, osPriorityNormal, 1, 2048 );
//osThreadDef(Can_thread, osPriorityRealtime, 1, 2048);
osThreadDef(PrintReprot_thread, osPriorityNormal, 1, 2048);
osThreadDef(Udisk_thread, osPriorityNormal, 1, 512);	//2021/11/23 星期二 10:30:48

extern u8 UsbInUse; 												// USB占用标志
void Udisk_thread(void const *args)					//任务只用于U盘的检测2021/11/23 星期二 10:30:17
{
	while(1)
	{
		if(UPDATER_Busy)
			osThreadTerminate(PrintTask);
		
		if(!UsbInUse) 											// USB接口空闲时
			DetectionUdisk();
		
		osDelay(500);
	}
}

void Display_thread(void const *args)
{
	//u8 usbstatus=0;//连接状态
	//u8 len;	
	//u8 t;
	//u16 times=0;//延时计数
	
  //logo();
  while (1)
  {
  	if(UPDATER_Busy)
			osThreadTerminate(PrintTask);
		
    Display_Handler();
    osDelay(100);
    
    // USB虚拟串口
		/*if(usbstatus!=bDeviceState) 				// USB连接状态发生了改变.
		{
			usbstatus=bDeviceState; 					// 记录新的状态
			if(usbstatus==CONFIGURED) 				// 提示USB连接成功
			{
				LED1_ON; 												// DS1亮
			}
			else 															// 提示USB断开
			{
				LED1_OFF; 											// DS1灭
			}
		}
		
		if(USB_USART_RX_STA&0x8000)
		{
			len=USB_USART_RX_STA&0x3FFF; 			// 得到此次接收到的数据长度
			usb_printf("\r\n您发送的消息为:%d\r\n\r\n",len);
			for(t=0;t<len;t++)
			{
				VCP_DataTx(USB_USART_RX_BUF[t]);// 以字节方式,发送给USB 
			}
			usb_printf("\r\n\r\n"); 					// 插入换行
			USB_USART_RX_STA=0;
		}
		else
		{
			times++;
			if(times%5000==0)
			{
				usb_printf("\r\n战舰STM32开发板USB虚拟串口实验\r\n");
				usb_printf("正点原子@ALIENTEK\r\n\r\n");
			}
			
			if(times%200==0)
				usb_printf("请输入数据,以回车键结束\r\n");
			
			if(times%30==0)
				LED0=!LED0; 										// 闪烁LED,提示系统正在运行.
			
			delay_ms(10);   
		}*/
  }
}

void Communicate_thread(void const *args)
{ 
  while(1) 
  {
  	// Signal flags that are reported as event are automatically cleared.
  	osSignalWait(0x1, osWaitForever);
    Communicate_Handler();
    osDelay(1);
  }
}

/*
void Can_thread(void const *args)
{
	while(1)
	{
		Can_Handler(); 											// Can信息处理
		
		osDelay(1);
	}
}
*/

void PrintReprot_thread(void const *args)
{
	while(1)
	{
		if(UPDATER_Busy)
			osThreadTerminate(PrintTask);
		
		// Signal flags that are reported as event are automatically cleared.
  	osSignalWait(0x2, osWaitForever);
    Print_Handler();
    osDelay(1);
  }
}

int main(void)
{ 
	u8 u8dat = 0;
	
	// 有引导程序时，中断向量偏移量修改在system_stm32f4xx.c中
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 					// 设置系统中断优先级分组2
	
	INP_Init();									// 外部接近开关，4个，第一个为TAR位置；
	
	// CAN初始化普通/环回模式，波特率500Kbps -----------------------
  CAN_ModeInit();
  
	//定时器时钟84M，分频系数8400，所以84M/84=1Mhz的计数频率，计数1000次为1ms
	TIM4_Int_Init(1000-1,84-1);	
	USART1_Init();				//Usart2 for RS232 Port
  USART3_Init();				//Usart3 for Vinculum-II
	LED_Init();
	BEEPER_Init();
	My_RTC_Init();
	AT24CXX_Init();
	MOTOR_Init();
	//TIM8CH2_PWM_Init(500-1,84-1);
	//TIM8CH2_SetCompare(50);
	
	mymem_init(SRAMIN);		//初始化内部内存池
	
	readeepbuf((unsigned char *)MAIN_HARDWARE_VERSION, ADR_HTVER, HVER_MAX_LEN);
	if((MAIN_HARDWARE_VERSION[0] < 0x30)&&
		 (MAIN_HARDWARE_VERSION[0] > 0x39))
	{
		strcpy(MAIN_HARDWARE_VERSION,"1.00.0");
		writeepbuf((unsigned char *)MAIN_HARDWARE_VERSION, ADR_HTVER, HVER_MAX_LEN);
	}
	
	//usbd_cdc_vcp_Init(); 									// 初始化USB
	
	//__enable_irq(); 	// 开启总中断
	
	VINC_Init();													// Vinculum
	
	Get_ChipID();
	
	readeepbuf((unsigned char *)DeviceSerialNb, ADRSERNUM, 6); 				// 设备序列号
	DeviceSerialNb[6] = 0;
	SoundSw = readeeprom(ADRSOUNDSW); 							// 读取声音开关状态
	ReportSave = readeeprom(ADR_REPSAVE); 					// 读取报告保存标志，Gavin，2022-12-08 周四 11:13:32
	PasswordSW = readeeprom(Password_SW_ADDR);			// 读取密码开关状态 2021/10/25 12:00:56
	ReportsIp = readeeprom(ADR_REPORTSIP); 					// 最新测试报告指针
	TimeZone = readeeprom(ADR_TIMEZONE); 						// 时区读取
	DateFormat = readeeprom(ADR_DATEFORMAT); 				// 日期格式
	DaylightTime = readeeprom(ADR_DAYLIGHTTIME); 		// 夏令时
	CompressibilityPt = readeeprom(ADR_V0_V10); 		// 压缩率计算的带入值，V0/V10
	
	readeepbuf((unsigned char *)&OQ_PtGp, OQ_ST_ADDR, 18); 				// OQ
	
	// Gavin，2023-03-13 周一 14:16:43 -----------------------------------------------------
	ExportInterface = readeeprom(ADR_Export_Interface);
	if(ExportInterface > 1) ExportInterface = 0;
	ExportFormat = readeeprom(ADR_Export_Format);
	ExportFormat = 0; 										// 默认XML
	
	readeepbuf((unsigned char *)&eepLwipDev, ADR_LWIP_DEV, 23); 				// LWIP结构体
	readeepbuf((unsigned char *)&TcpPort, ADR_TCP_PORT, 2); 						// 端口号
	DhcpSw = readeeprom(ADR_DHCP_SW); 																	// DHCP
	
	readeepbuf((unsigned char *)&PrtIP, ADR_PRT_IP, 4); 								// 网络打印机IP地址
	PrintType = readeeprom(ADR_PRT_TYPE);
	PrintFormat = readeeprom(ADR_PRT_FORMAT);
	
	BalanceModel = readeeprom(ADR_BAL_MODEL); 			// 读取电子秤型号
	if(BalanceModel > 1) 														// 型号上限检查
	{
		BalanceModel = 0; 														// 默认型号，0.Sartorius Practum，1.Mettler Me 54
		writeeeprom(BalanceModel, ADR_BAL_MODEL);
	}
	
	u8dat = lwip_comm_init();
	if(u8dat == 0)
	{
		if(DhcpSw) 								// 自动获取IP
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
	
	// End，2023-03-13 周一 14:17:07 -------------------------------------------------------
	
	BEEPER_PlayTones(tones_startup);
	//BEEPER_PlayTones(tones_SMB);
	PowOnDelay = 1500;
	//while(1);
	
 	osKernelInitialize ();                    // initialize CMSIS-RTOS
 	DispTask = osThreadCreate(osThread(Display_thread), NULL);
	CommTask = osThreadCreate(osThread(Communicate_thread), NULL);
	//Can_Task = osThreadCreate(osThread(Can_thread), NULL);
	PrintTask = osThreadCreate(osThread(PrintReprot_thread), NULL);
	
	UdiskTask = osThreadCreate(osThread(Udisk_thread), NULL);

	osKernelStart ();                         // start thread execution 
	
}

// 获取芯片ID号 --------------------------------------------
void Get_ChipID(void)
{
	uint8_t udat,i,Psdat;
	ChipID chipid = {0};
	//char tbuf[10];
	
	chipid.id[0] = *(__I uint32_t *)(0x1FFF7A10 + 0x00);
	chipid.id[1] = *(__I uint32_t *)(0x1FFF7A10 + 0x04);
	chipid.id[2] = *(__I uint32_t *)(0x1FFF7A10 + 0x08);
	
	//M407ID = chipid.id[0]+chipid.id[1]+chipid.id[2];
	M407ID = chipid.id[0]^chipid.id[1]^chipid.id[2];
	
	for(i=0;i<8;i++)
	{
		udat = (uint8_t)(0x0000000f&(M407ID>>(7-i)*4));
		if(udat != 0)
			break;
	}
	
	Psdat = udat;							
					
	for(i=0;i<5;i++)
	{
		Activation_Code[i] = StId[udat]^M407ID;
		udat++;
		if(udat > 15)
			udat = 0;
		Activation_Code[i] = Activation_Code[i]%1000000;
	}
	
	for(i=0;i<16;i++)																	//根据ID号获取重置密码的激活码
	{
		Password_Code[i] = PsId[Psdat]^M407ID;
		Psdat++;
		Password_Code[i] = Password_Code[i]%1000000;
	}
	
//	//初始上电检查是否更新v1.07版本---------- 				2021/11/5 星期五 10:21:51
//	if(readeeprom(Update_ADDR)!=0x23)									//如果没有升级v1.07版本
//	{
//		writeeeprom(0x23,Update_ADDR);									//写入0x23，下次再进程序时就不执行这一段
//		
//		writeeeprom(0, Password_SW_ADDR);								//关闭密码 2021/11/5 星期五 10:16:46
//		//writeepbuf("9999",Password_ADDR,4);						//初次升级密码为9999
//		//writeeeprom(4,Password_NUM_ADDR);							//记录保存了4位数密码
//	}
	
	
	for(i=0;i<6;i++) ConfigCode[i]=readeeprom(ADR_ACTIVATION_CODE+i);
	ConfigCode[6] = '\0';
	strcpy(ConfigCodeSv, ConfigCode);
	sscanf(ConfigCode,"%u",&tmpActivationCode);
	Activation_Code[5] = tmpActivationCode;
	
	if(Activation_Code[5] == Activation_Code[0]) 			// 300rpm，1个工位；1个鼓
	{
		DeviceID = 121;
	}
	else
	if(Activation_Code[5] == Activation_Code[1]) 			// 250rpm，1个工位；1个鼓+电子称
	{
		DeviceID = 122;
	}
	else
	if(Activation_Code[5] == Activation_Code[2]) 			// 300rpm，2个工位；2个鼓
	{
		DeviceID = 221;
	}
	else
	if(Activation_Code[5] == Activation_Code[3]) 			// 250rpm，2个工位；2个鼓+电子称
	{
		DeviceID = 222;
	}
	else
	//if(Activation_Code[5] == Activation_Code[4]) 			// 250/300rpm，2个工位
	//{
	//	DeviceID = 223;
	//}
	//else
	{
		DeviceID = 0;
	}
}
// 验证重置密码激活码 ------------------------------------------2021/10/26 星期二 18:09:25
char VerifyCode(char *temKeyStr)
{
	sscanf(temKeyStr,"%u",&tmpPasswordCode);
	for(int i=0;i<16;i++)
	{
		if(Password_Code[i]==tmpPasswordCode)				//先验证激活码是否正确
		{
			if(readeeprom(Password_COD_ADDR+i)==1)		//验证激活码是否被使用，1是使用，否则没有
				return 0;
			else																			//未被使用过的激活码	
			{     
				writeeeprom(0x01,Password_COD_ADDR+i);	//记录该激活码使用过了
				return 1;	
			}																	
		}
		
	}
	return 0;
	
}

// End -----------------------------------------------------


