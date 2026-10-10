#include "tbh125.h"

unsigned char gettasto(void);
unsigned char gettast(void);

extern __irq void USBLP_Handler(void);

unsigned int nullwert,blpos,blepos,eichpos;
unsigned int StepCounter,StepRampe,RampDiv,AktStep,flenable=1;
unsigned long StepSpeed,KraftSpeed;
unsigned int mesenable=0;
float mmfak,Diameter,Thickness;
unsigned int PrinterPaper,PrinterLanguage,PrinterFrames;
extern long counth,countd;  
extern char befbuf1[80];
extern unsigned char vers[];
void Brighten(void);																	//控制数码管亮度
int Goaheadnum;																		//通过判断这个值改变定时器4中断时间
int Goaheadnum2;
float vorbrems=0.5;
float dmeing=5;
float dickeing=5;
long counth,countd;
float Hardness;
float eichmas;
float laengfak,kraftfak;
unsigned int ueberlauf=40;
unsigned int DatFormat;
unsigned int MaxDose,inueb=0;
extern char buf[150];

extern int dmena,dickena,terminologie,autodiam;

extern unsigned int aktLanguage;

extern float MeasSpeed;
extern float NwtFak;
extern int Force;
extern int Sensibility;
extern unsigned int MeasureMethod,HardOn,DiamOn,ThickOn,AutoMess,DatenAusgabe;
extern int ForceLimit;
extern float DragJaw;
extern unsigned int MeasurePause;
extern unsigned int LaeEinh;
extern unsigned int HardEinh;
extern unsigned int AutoPrint,pruefanz;
extern BOOL RemoteFlg;
extern BOOL BusyFlg;
extern unsigned int rsstatus1;
unsigned int dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
unsigned int anzpoi,MesAbr,druckmode=1,PrinterLanguage;
extern volatile int AD12Bit;
extern short NullKorr;

//BIT Blanking; 								// 显示隐藏标志，1-隐藏，0-显示；
char Blanking; 								// 显示隐藏标志，1-隐藏，0-显示；

BOOL StepSperr=__FALSE;

const unsigned int font[48] = 
{
	0x69C5,0x4801,0x708C,0x700D,0x5109,0x310D,0x318D,0x6001,0x718D,0x710D, 	// 0,1,2,3,4,5,6,7,8,9
	0x1008,0x1A58,0x0000,0x0000,0x0000,0x0000,0x0000, 											// -,*,
	0x7189,0x642D,0x2184,0x6425,0x3184,0x3180,0x218D,0x5189,0x2424,0x4085, 	// A,B,C,D,E,F,G,H,I,J
	0x1990,0x0184,0x4B81,0x4391,0x6185,0x7188,0x6195,0x7198,0x310D,0x2420, 	// K,L,M,N,O,P,Q,R,S,T
	0x4185,0x09C0,0x41D1,0x0A50,0x0A20,0x2844 															// U,V,W,X,Y,Z
};

static unsigned int anzdat[2][4];

U8 InReport;
U8 OutReport;

I2C_InitTypeDef I2C_Struct;

//----------------------------------------------------------------------------
// IO配置
//----------------------------------------------------------------------------
void LED_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MB_LED1;					//测试灯初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = PD0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PD1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PD2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PD3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

void Beep_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
}

void KEY_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = KB1|KB2|KB3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KB4|KB5|KB6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MB_LED0;					//测试灯初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void MOTOR_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = M_STEP|M_RST;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = M_DIR|M_EN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = M_MS1|M_MS2;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);									//拉高禁止DMOS输出
}

void AD7940_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = AD7940_CS|AD7940_SCLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD7940_SDAT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


void VNC_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin = VNC_CTS|VNC_DSR|VNC_PROG|VNC_RESET;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VNC_DTR|VNC_RTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void OW_PIN_OUT(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS2401_DAT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}
void OW_PIN_IN(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS2401_DAT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);							//使能TIM3时钟
	TIM_TimeBaseInitStrue.TIM_Period = arr;													//自动重装载
	TIM_TimeBaseInitStrue.TIM_Prescaler = psc;											//定时器分频
	TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;			//向上计数模式
	TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);									//初始化TIM3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 									//定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 			//强占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 						//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_Cmd(TIM3,ENABLE); 																					//使能定时器3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);												//允许定时器3更新中断
}
//定时器4
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);							//使能TIM4时钟
	TIM_TimeBaseInitStrue.TIM_Period = arr;													//自动重装载
	TIM_TimeBaseInitStrue.TIM_Prescaler = psc;											//定时器分频
	TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;			//向上计数模式
	TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStrue);									//初始化TIM4
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 									//定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; 			//强占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 						//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_Cmd(TIM4,ENABLE); 																					//使能定时器4
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);												//允许定时器4更新中断
}
//ADC初始化,ADC1的通道10,通道11,硬件连接PC0,PC1
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIO时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

  //先初始化ADC1通道10和通道11 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|POW_CHECK;//PC0 通道5
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
//USB的IO配置
void USBIO_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = USB_VBC;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

//RTC时钟初始化
u8 My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	
	u16 retry=0X1FFF; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器时钟
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE开启    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
				retry++;
				delay_ms(5);
			}
		if(retry==0)return 1;		//LSE开启失败
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(rtcclk),选择LES作为RTC时钟  
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1-0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
			RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
    RTC_Init(&RTC_InitStructure);
 
		RTC_Set_Time(9,02,56,RTC_H12_AM);	//设置时间
		RTC_Set_Date(21,5,8);		//设置日期
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 
 
	return 0;
}

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化失败
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}

//RTC日期设置
//year,month,date:年(0-99),月(1-12),日(0-31)
//week:星期(1-7,0,非法)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化失败
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=0;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}
//----------------------------------------------------------------------------
// 移植部分
//----------------------------------------------------------------------------
void Init_Device(void)
{
	My_RTC_Init();								//RTC时钟初始化
	LED_Init();										//数码管显示IO配置
	Beep_Init();									//蜂鸣器IO配置
	
	KEY_Init();										//按键IO配置
	MOTOR_Init();									//电机IO配置
	AD7940_Init();								//AD7940 IO配置
	IIC_Init();										//模拟I2C IO配置
	VNC_Init();										//VNC IO配置
	OW_PIN_OUT();									//DS2401 IO配置
	Adc_Init();										//ADC IO配置
	USBIO_Init();									//USB IO配置
//	TIM3_Init(80,719);						//定时器3初始化   2021/11/22 修改定时器3，加快定时器3中断速度，原先是26，419
	TIM3_Init(120,719);						//定时器3初始化 2021/12/16	针对个别板子修改定时器3
	TIM4_Init(27,719);						//26，719是240us
	
}	


void beep(void)
{
	GPIO_SetBits(GPIOE,GPIO_Pin_10);
	os_dly_wait(40);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
}

unsigned char gettast(void)
{
	unsigned char ch;

	while(gettasto()!=0);
	while((ch=gettasto())==0);
	beep();
	return(ch);
}

unsigned char gettasto(void)
{
	if(((KEY1 & 0x01) && (KEY2 & 0x02) && (KEY3 & 0x04) && (KEY4 & 0x08) && (KEY5 & 0x10) && (KEY6 & 0x20)) || RemoteFlg)
	{
		os_dly_wait(10);
		return(0);
	}
	
	os_dly_wait(20);
	
	if(KEY1 ==0)	return(TLO);															//左上键
	if(KEY2 ==0)	return(TLU);															//左下键
	if(KEY3 ==0)	return(TMO);															//中上键
	if(KEY4 ==0)	return(TMU);															//中下键
	if(KEY5 ==0)	return(TRO);															//右上键
	if(KEY6 ==0)	return(TRU);															//右下键
	
	return(0);
} 

unsigned char gettastTMO(void)
{
	if(KEY3==0) return(TMO); 			// 中上键
	
	return(0);
}

void motor(int dir,unsigned int step,int speed,unsigned int mode)
{
	if((dir==0 && (GetIni(1)==1)) || GPIO_ReadOutputDataBit(GPIOB,M_EN)==0) return;
	
	if(dir!=0)
		GPIO_WriteBit(PORT_B, M_DIR, Bit_RESET);       
	else
		GPIO_WriteBit(PORT_B, M_DIR, Bit_SET);
	
 	StepCounter=step;
	if(speed==0)
	{
		if(dir==0)
		{
			KraftSpeed=FULLSPEEDB;
			StepSpeed=FULLSPEEDB;
		}
		else
		{
			KraftSpeed=FULLSPEEDF;
			StepSpeed=FULLSPEEDF;
		}
		
		StepRampe=23;
	}
	else
	{
		StepSpeed=15000.00f / MeasSpeed ;                          //0x19798;
		KraftSpeed=15000;
		StepRampe=13;
	}
	
	switch(mode)
	{
		case 0:
			GPIO_WriteBit(PORT_C, M_MS1, Bit_RESET);
			GPIO_WriteBit(PORT_C, M_MS2, Bit_RESET);
			break;
		case 1:
			GPIO_WriteBit(PORT_C, M_MS1, Bit_SET);
			GPIO_WriteBit(PORT_C, M_MS2, Bit_RESET);
			StepSpeed/=2;
			KraftSpeed/=2;
			break;
		case 2:
			GPIO_WriteBit(PORT_C, M_MS1, Bit_RESET);
			GPIO_WriteBit(PORT_C, M_MS2, Bit_SET);
			KraftSpeed/=4;
			StepSpeed/=4;
			break;
		case 3:
			GPIO_WriteBit(PORT_C, M_MS1, Bit_SET);
			GPIO_WriteBit(PORT_C, M_MS2, Bit_SET);
			KraftSpeed/=16;
			StepSpeed/=16;
			break;
	}
	
	AktStep=0;
	GPIO_WriteBit(PORT_B, M_EN, Bit_RESET);
}

unsigned int getadDose(void)
{
	unsigned char i;
	unsigned int erg=0;

	GPIO_WriteBit(PORT_B, AD7940_CS, Bit_RESET);
	for(i=0;i<15;i++)
	{
		if(GPIO_ReadInputDataBit(GPIOB,AD7940_SDAT) !=0)
			erg |= 0x01;
		
		GPIO_WriteBit(PORT_B, AD7940_SCLK, Bit_RESET);
		erg=erg << 1;
		
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); 
		
		GPIO_WriteBit(PORT_B, AD7940_SCLK, Bit_SET);
	}
	
	GPIO_WriteBit(PORT_B, AD7940_SCLK, Bit_RESET);
	__nop();
	GPIO_WriteBit(PORT_B, AD7940_SCLK, Bit_SET);
	
	GPIO_WriteBit(PORT_B, AD7940_CS, Bit_SET);
	
	return(erg);
}

void tbhnull(void)
{
	unsigned int i;
	unsigned long sum=0;

	for(i=0;i<10000;i++)
	{ 
		sum += getadDose();
		os_tsk_pass();
	}
	sum /= 10000;
	i=(int)sum;
	nullwert=i;
}

int TestFinger(void)
{
	unsigned int i;
	unsigned int sum=0;

	for(i=0;i<100;i++)
	{ 
		sum += getadDose();
		os_tsk_pass();
	}
	
	sum /= 100;
	if(nullwert>200)
		i=nullwert-200;
	else
		i=nullwert/2;
	
	if(sum<i) return(1);
	
	return(0);
}

unsigned int GetIni(unsigned int num)
{
	unsigned int i;

	switch(num)
	{
		case 1:
			
			i=Get_Adc(ADC_Channel_10) & 0x03FF;
			break;
		case 2:
			i=Get_Adc(ADC_Channel_11) & 0x03FF;
			break;
		case 3:
			i =0;
			break;
		case 4:
			i =0;
			break;
		default:
			i=0;
			break;
	}
	
	if(i>850) return(1);
	
	return(0);
}

float GetUein(void)						//没有用到的函数
{
	float ret;
	unsigned int i;
	
//	i=ADC->DR6 & 0x03FF;
	ret=(float) i;
	ret /= 28.2093f;
	
	return(ret);
}

unsigned int GetIniVal(unsigned int num)
{
	unsigned int i;
	
	switch(num)
	{
		case 1:
			i=Get_Adc(ADC_Channel_10) & 0x03FF;
			break;
		case 2:
			i=Get_Adc(ADC_Channel_11) & 0x03FF;
			break;
		case 3:
			i=0;
			break;
		case 4:
			i=0;
			break;
		default:
			i=0;
			break;
	}
	
	return(i);
}

int GetDose(int mode)
{
	unsigned int i;
	int ret;
	
	i=getadDose();
	if(mode==1)
	{
		ret=i-nullwert;
		if(ret<0) ret=0;
		return(ret);
	}
	
	if(i>nullwert) return(i-nullwert);
	
	return(0);
}

int GetDoseAvrg(int mode)
{
	int i;
	long sum=0;

	for(i=0;i<10000;i++)
	{
		sum += GetDose(mode);
		os_tsk_pass();
	}
	
	sum /= 10000;
	i=(int)sum;
	
	return(i);
}


int GetDoseAvrgN(int mode,int n)
{
	int i;
	long sum=0;

	for(i=0;i<n;i++)
	{ 
		sum += GetDose(mode);
		os_tsk_pass();
	}
	
	sum /= n;
	i=(int)sum;
	
	return(i);
}

unsigned int dmmess(unsigned int *SCou)
{
	int mew,abcou,schwelle;			
	unsigned int steps;
	unsigned char rammel;

	schwelle=(int)((float)Sensibility * NwtFak);
	rammel=0;
	counth++;
	countd++;
	writlongeep(countd,ADRCOUNTD);
	writlongeep(counth,ADRCOUNTH);
	tbhnull();
	tbhnull();
	abcou=0;

	steps=blpos-((int)((dmeing+vorbrems)*mmfak));

	motor(1,steps,0,2);
	
	while(GPIO_ReadOutputDataBit(GPIOB,M_EN) == 0)
	{
		mew=GetDoseAvrgN(0,20);

		if(mew>schwelle && StepRampe==0)
		{
			
			if(abcou++>3)
			{							//2
				GPIO_WriteBit(PORT_B, M_EN, Bit_SET);
				rammel=1;
			}
		}
		else
		{
			abcou=0;									//
		}
		
		os_tsk_pass();
	}
	
	*SCou=AktStep;
	return(rammel);
}

// Gavin，2022-02-09 周三 11:05:35 ---------------------------------
const unsigned int ramptab[24]=
{ 0, 7,10,15,22,33,50,76,114,171,256,385,578,867,1300,1959,2926,4389,6584,9876,14814,22222,33333,50000};
const unsigned int vergtab[24]=
{30,30,30,30,30,30,30,30, 30, 30, 30, 24, 24, 24,  24,  24,  24,  18,  18,  18,   18,    9,    9,    9};

int ForcFlag = 0; 											// 进入硬度测试的推挤动作标志
int ForcFlag1 = 0; 											// 1秒内推力到达标志，电机停止
int ForcSec = 0; 												// 步进电机脉冲计数器，多少个脉冲到1秒
int ForceAdx0, ForceAdx1; 							// 上一次推力值，当前推力差
unsigned int ForceOc1r; 								// 定时计数器的初值
int ForceStep; 													// 恒力参数，牛顿/秒

unsigned char knacktab(char mode,unsigned int *count)
{
	unsigned int steps,ueberl,uebverg,tbadd,maxval,abfval,ACou,schwelle;
	int adx,adxx,maxmew=0;
	
	// "SSW "参数：Sensibility，范围3~50，默认10；
	schwelle=(int)((float)Sensibility * NwtFak);
	if(dmmess(&ACou))
	{
		Hardness=99998;
		Diameter=99998;
		Thickness=99998;
		
		return(0);
	}
	
	os_dly_wait(100);
	if(MeasureMethod==0 || mode==0) 			// 恒速测量
	{
		steps=(int)((vorbrems+3)*mmfak);		//*2

		motor(1,steps,1,2);
		uebverg=1;
	}
	else																	//恒力测试
	{
		steps=(int)((vorbrems+3)*mmfak*4);	//*4
		motor(1,steps,1,3);
		uebverg=1;
	}
	
	ueberl=0;
	
	os_dly_wait(50);
	
	
	while(GetDoseAvrgN(0,10) < schwelle & GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		if(gettastTMO()==TMO && MesAbr==0) 	// 中上键
		{
			MesAbr=1;
			beep();
		}
		
		os_tsk_pass();
	}
		
	tbadd=AktStep;
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)	//步进电机使能端无效
	{
		Hardness=99998;
		Diameter=99998;
		Thickness=99998;
		
		return(0);
	}
	
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);	
	
	if(MeasureMethod==1 && mode==1)
		tbadd/=4;
	
	Diameter=(double)(blpos-(ACou + tbadd));
	Diameter /= mmfak;
	Diameter += DragJaw;
	//Diameter = round2(Diameter);
	
	*count=ACou + tbadd;
	
	os_dly_wait(5);
	if(mode==0)
		return(1);
	
	if(MeasureMethod==0) 									// 恒速测量
	{
		motor(1,MAXMOT-(ACou + tbadd),1,2);
		uebverg=ueberlauf;
	}
	else 																	// 恒力测量
	{
		motor(1,(MAXMOT-(ACou + tbadd))*4,1,3);
		uebverg=ueberlauf*2;					//8
	}
	
	if(Force<100)													//恒力参数
	{
		KraftSpeed *= (100-Force);
	}
	else
	{
		KraftSpeed /= (Force/100);
	}
	
	if(KraftSpeed>65500)
		KraftSpeed=65500;
	
	ueberl=0;
	mesenable=0;
	maxmew=0;
	if(ForceLimit<=MaxDose)							 //ForceLimit = 300 or 500;大小电机的力上限 MaxDose = 500;
	{
		maxval=ForceLimit*NwtFak/HARDKORR; //NwtFak(NFCT,牛顿系数),HARDKORR(0.95)
	}
	else
	{
		maxval=MaxDose*NwtFak/HARDKORR;
	}
	
	abfval=schwelle/2;   
	
	ForceAdx0 = ForceAdx1 = 0; 						// 推力寄存器复位
	ForceStep = Force*NwtFak/HARDKORR; 		// 恒力参数转换
	ForceStep = (float)ForceStep; 				// 每秒钟应达到的力，恒力参数转换
	//ForceStep = (float)ForceStep/200; 		// 每5毫秒应达到的力(200Hz)
	//ForceStep = (float)ForceStep/150; 		// 每6.6毫秒应达到的力(150Hz)
	ForceStep = (float)ForceStep/100; 		// 每10毫秒应达到的力(100Hz)
	//ForceStep = (float)ForceStep/80; 			// 每10毫秒应达到的力(80Hz)，Gavin，2021-10-31 星期日 13:53:06
	//ForceStep = (float)ForceStep/60; 			// 每10毫秒应达到的力(60Hz)
	
	ForcFlag = 1; 												// 进入硬度测试的推挤过程标志置位
	
	// ueberl恒速：40；恒力：80；
	while(ueberl<uebverg && maxmew<maxval && GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_SET);
		//__nop(); __nop(); __nop();
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_RESET);
		
		while(!mesenable)
		{
			if(gettastTMO()==TMO && MesAbr==0) 					// 中上键
			{
				MesAbr=1;
				beep();
			}
			
			os_tsk_pass();
		}
		
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_SET);
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_RESET);
		
		mesenable=0;
		adx=GetDose(0);
		if(adx>maxmew)
		{
			maxmew=adx;
		}
		
		ForceAdx1 = adx - ForceAdx0; 									// 当前推力差 = 当前推力-上一次推力
		if(ForceAdx1 >= ForceStep) 										// 当前推理差 >= 恒力参数设定值（转换过）
		{
			ForceAdx0 = adx-(ForceAdx1-ForceStep); 			// 保存上一次推力
			ForcFlag1 = 1; 															// 1秒内推力上限已经到达标志，电机停转
		}
		
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_RESET);
		
		// 破碎判断？---------------------------------
		adxx=maxmew-abfval;
		if(adxx<0)
			adxx=0;
		if(adx<adxx)
		{
			ueberl++;
		}
		else
		{
			ueberl=0;
		}
		
		os_tsk_pass();
		
		// 测试点 --------------------------
		//GPIO_WriteBit(PORT_B, GPIO_Pin_4, Bit_RESET);
		
	}
	
	// Gavin，2022-02-09 周三 11:14:21 ----------------------
	ForcFlag = 0; 												// 推挤硬度测试部分已经结束，恢复标志
	ForcFlag1 = 0; 												// 恢复电机驱动标志
	
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)
	{
		Hardness=99997;
		Diameter=99997;
		return(0);
	}
	
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);
	if(maxmew>maxval && flenable==1)
	{
		Hardness=99996;
	}
	else
	{
		Hardness=(float)maxmew;
		Hardness /= NwtFak;
		Hardness *= HARDKORR;
		Hardness=round0(Hardness);
	}
	
	return(1);
}

/*------------------------------------------------------------------------------
            Default Interrupt Service Routine (necessary for STR91x)
------------------------------------------------------------------------------*/
/*          */
__irq void def_irq (void)
{
//	VIC0->VAR = 0;
//	VIC1->VAR = 0;
//	                                              /* Do nothing                  */ 
}

//定时器3中断处理数码管显示
void TIM3_IRQHandler(void)
{
	/*
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET) //溢出中断
	{
		GPIO_WriteBit(PORT_E, PD0, Bit_SET);
		GPIO_WriteBit(PORT_E, PD1, Bit_SET);
		GPIO_WriteBit(PORT_E, PD2, Bit_SET);
		GPIO_WriteBit(PORT_E, PD3, Bit_SET);										//屏蔽数码管,消除重影
		
		anzpoi++;									
		if(anzpoi > 3)
			anzpoi = 0;
		
		SendDatTo13H(anzdat[1][anzpoi], anzdat[0][anzpoi]);
		
		switch(anzpoi)
		{
			case 3: 
			{
				GPIO_WriteBit(PORT_E, PD3, Bit_RESET);
			}break;
			case 2:
			{
				GPIO_WriteBit(PORT_E, PD2, Bit_RESET);
			}break;
			case 1:
			{
				GPIO_WriteBit(PORT_E, PD1, Bit_RESET);
			}break;
			case 0: 
			{
				GPIO_WriteBit(PORT_E, PD0, Bit_RESET);
			}break;
			default:
			{
				GPIO_WriteBit(PORT_E, PD0, Bit_SET);
				GPIO_WriteBit(PORT_E, PD1, Bit_SET);
				GPIO_WriteBit(PORT_E, PD2, Bit_SET);
				GPIO_WriteBit(PORT_E, PD3, Bit_SET);
			}break;
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志
	}
	*/
	
	///*
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET) 	// 溢出中断
	{
		if(Blanking)
		{
			Blanking = 0;
			
			switch(anzpoi)											// 屏蔽数码管,消除重影
			{
				case 0: GPIO_WriteBit(PORT_E, PD0, Bit_SET); break;
				case 1: GPIO_WriteBit(PORT_E, PD1, Bit_SET); break;
				case 2: GPIO_WriteBit(PORT_E, PD2, Bit_SET); break;
				case 3: GPIO_WriteBit(PORT_E, PD3, Bit_SET); break;
				default: 
					GPIO_WriteBit(PORT_E, PD0, Bit_SET);
					GPIO_WriteBit(PORT_E, PD1, Bit_SET);
					GPIO_WriteBit(PORT_E, PD2, Bit_SET);
					GPIO_WriteBit(PORT_E, PD3, Bit_SET);
				break;
			}
			
			//SendDatTo13H(0x0000, 0x0000);
		}
		else
		{
			Blanking = 1;
			
			anzpoi++; 
			if(anzpoi > 3) 
				anzpoi = 0;
			SendDatTo13H(anzdat[1][anzpoi], anzdat[0][anzpoi]);
			
			switch(anzpoi)
			{
				case 0: GPIO_WriteBit(PORT_E, PD0, Bit_RESET); break;
				case 1: GPIO_WriteBit(PORT_E, PD1, Bit_RESET); break;
				case 2: GPIO_WriteBit(PORT_E, PD2, Bit_RESET); break;
				case 3: GPIO_WriteBit(PORT_E, PD3, Bit_RESET); break;
				default:
					GPIO_WriteBit(PORT_E, PD0, Bit_SET);
					GPIO_WriteBit(PORT_E, PD1, Bit_SET);
					GPIO_WriteBit(PORT_E, PD2, Bit_SET);
					GPIO_WriteBit(PORT_E, PD3, Bit_SET);
				break;
			}
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 		// 清除中断标志
	}
	///*/
}

//定时器4中断处理电机相关
void TIM4_IRQHandler(void)
{
	unsigned int ini;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET) //溢出中断
	{
		if(!StepSperr)
		{
			if(GPIO_ReadOutputDataBit(GPIOB,M_EN)!=0) 	// 步进电机使能，0有效；1无效；
			{ 
				Goaheadnum = 0;                             
				RampDiv=0;
			}
			else 																				// 电机使能有效
			{
				if(MeasureMethod == 0) 										// 恒速测量
				{
					mesenable=1; 															// 步进电机走步标志，置位
					GPIO_WriteBit(PORT_A, M_STEP, Bit_RESET);	// 步进电机走步脉冲下降沿
					
					/*
					if(MeasureMethod==0)										 //恒速测量
					{
						TIM4->ARR = (StepSpeed + ramptab[StepRampe]/16)/(25.62);
						TIM4->PSC = 719;
					}
					else																			//恒力测量
					{
						TIM4->PSC = 719;			//前进低速	540us
						TIM4->ARR=(KraftSpeed + ramptab[StepRampe]/16)/(25.62);
					}
					*/
					
					TIM4->ARR = (StepSpeed + ramptab[StepRampe]/16)/(25.62);
					TIM4->PSC = 719;
					
					if(StepRampe>0) 							// 数组指针，斜率？
					{
						if(RampDiv++>vergtab[StepRampe])
						{		//90	
							StepRampe--;
							RampDiv=0;
						}
					}
					
					GPIO_WriteBit(PORT_A, M_STEP, Bit_SET); // 走步脉冲上升沿
		
					ini=GetIni(1); 								// 位置开关检测
					if(StepCounter>0&&ini==0)
					{
						StepCounter--; 							// 步进电机走步计数器
					}	
					
					// 推杆返回，碰到位置开关 ------------------
					if(GPIO_ReadOutputDataBit(GPIOB,M_DIR)==1 && ini==1)
					{
						if(inueb++>40) 												// 延时40步
						GPIO_WriteBit(PORT_B, M_EN, Bit_SET); // 步进电机使能，失效
					}
					
					// 步进电机走步完成 ------------------------
					if(StepCounter ==0)
					{
						GPIO_WriteBit(PORT_B, M_EN, Bit_SET); // 步进电机使能，失效
					}
					
					// 离开位置开关 ----------------------------
					if(ini==0)
					{
						AktStep++; 									// 测试次数？
						inueb=0; 										// 延时清零
					}
				}
				else 														// 恒力测量
				{
					if(ForcFlag1 == 0) 							// 1秒内推力到达标志
					{
						mesenable=1; 															// 步进电机走步标志，置位
						GPIO_WriteBit(PORT_A, M_STEP, Bit_RESET);	// 步进电机走步脉冲下降沿
					}
					
					/*
					if(MeasureMethod==0)										 //恒速测量
					{
						TIM4->ARR = (StepSpeed + ramptab[StepRampe]/16)/(25.62);
						TIM4->PSC = 719;
					}
					else																			//恒力测量
					{
						TIM4->PSC = 719;			//前进低速	540us
						TIM4->ARR = (KraftSpeed + ramptab[StepRampe]/16)/(25.62);
					}
					*/
					
					TIM4->PSC = 719;			//前进低速	540us
					if(ForcFlag == 0)
					{
						TIM4->ARR = ForceOc1r = (KraftSpeed + ramptab[StepRampe]/16)/(25.62);
					}
					else 
					{
						TIM4->ARR = ForceOc1r; 			// 计数器赋初值
						
						ForcSec++; 										// 脉冲计数器
						//if(ForcSec >= 3250) 					// 1秒多少个脉冲
						//if(ForcSec >= 16) 						// 5毫秒(200Hz)多少个脉冲
						//if(ForcSec >= 21) 						// 6.6毫秒(150Hz)多少个脉冲
						if(ForcSec >= 32) 						// 10毫秒(100Hz)多少个脉冲
						//if(ForcSec >= 40) 						// 12.5毫秒(80Hz)，Gavin，2021-10-31 星期日 13:54:24
						//if(ForcSec >= 54) 						// 16.7毫秒(60Hz)
						{
							ForcSec = 0; 								// 脉冲计数器复位
							ForcFlag1 = 0; 							// 1秒内推力到达标志复位
							mesenable = 1; 							// 步进电机走步标志，置位
						}
					}
					
					if(ForcFlag1 == 0) 							// 1秒内推力到达
					{
						if(StepRampe>0) 							// 数组指针，斜率？
						{
							if(RampDiv++>vergtab[StepRampe])
							{		//90	
								StepRampe--;
								RampDiv=0;
							}
						}
						
						GPIO_WriteBit(PORT_A, M_STEP, Bit_SET); // 走步脉冲上升沿
						
						ini=GetIni(1); 								// 位置开关检测
						if(StepCounter>0&&ini==0)
						{
							StepCounter--; 							// 步进电机走步计数器
						}	
						
						// 推杆返回，碰到位置开关 ------------------
						if(GPIO_ReadOutputDataBit(GPIOB,M_DIR)==1 && ini==1)
						{
							if(inueb++>40) 												// 延时40步
								GPIO_WriteBit(PORT_B, M_EN, Bit_SET); // 步进电机使能，失效
						}
						
						// 步进电机走步完成 ------------------------
						if(StepCounter ==0)
						{
							GPIO_WriteBit(PORT_B, M_EN, Bit_SET); // 步进电机使能，失效
						}
						
						// 离开位置开关 ----------------------------
						if(ini==0)
						{
							AktStep++; 									// 测试次数？
							inueb=0; 										// 延时清零
						}
					}
				}
			}
		}
		else
		{
			
			TIM4_Init(27,719);
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志

	}

}

void backreset(void)
{
	if(GetIni(1)==1)
		return;	
	motor(0,60000,0,1);
	while(GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{	
		if(TestFinger())
		{

			GPIO_WriteBit(PORT_B, M_EN, Bit_SET);
			break;
		}
		
		if(gettastTMO()==TMO && MesAbr==0) 	// 中上键
		{
			MesAbr=1;
			beep();
		}
		
		os_tsk_pass();
	}
}

unsigned int blbef(int mode)
{
	float dmsave,mmsave,vbsave,speedsave,hil;	
	unsigned int rammel,sensisave;
	
	backreset();
	mmsave=mmfak;
	dmsave=dmeing;
	vbsave=vorbrems;
	speedsave=MeasSpeed;
	sensisave=Sensibility;
	MeasSpeed=2.3;
	
	if(vers[49]==0x31)
	{     //#ifdef GRMOT
		mmfak=400;
		blpos=11170;		//11670 10870;                          //9120;				//26000  30000 bei Rollenantrieb;
	}
	else
	{
		mmfak=666;
		blpos=19432;				//18100;                          //9120;				//26000  30000 bei Rollenantrieb;
	}
	
	hil=(float)NullKorr;
	hil/=10.0f;
	hil*=(float)mmfak;
	blpos += (short)hil;
	
	dmeing=2;
	Sensibility=10;
	vorbrems=0;
	rammel=knacktab(0,&blpos);
	dmeing=dmsave;
	vorbrems=vbsave;
	mmfak=mmsave;
	Sensibility=sensisave;
	MeasSpeed=speedsave;
	os_dly_wait(10);
	writinteep(blpos,ADRBLPOS);
	os_dly_wait(200);
	
	if(mode==0)
		backreset();
	
	return(rammel);
}

unsigned int blebef(void)
{
	float dmsave,vbsave;	
	unsigned int rammel;
	
	backreset();
	dmsave=dmeing;
	
	dmsave=dmeing;
	vbsave=vorbrems;
	if(vers[49]==0x31)     //#ifdef GRMOT
		mmfak=400;
	else
		mmfak=666;
	
	dmeing=eichmas;
	vorbrems=0.5;
	rammel=knacktab(0,&blepos);
	dmeing=dmsave;
	vorbrems=vbsave;
	os_dly_wait(200);
	backreset();
	
	if(rammel)
	{
		mmfak=(double)(blpos-blepos);
		mmfak/=eichmas;
		if(vers[49]==0x31)
		{    //#ifdef GRMOT
			if(mmfak<360 || mmfak>410) mmfak=380;
		}
		else
		{
			if(mmfak<620 || mmfak>700) mmfak=666;
		}
		
		savefloat(&mmfak,ADRMMFAK);
	}
	
	return(rammel);
}

void mdbef(int mod)
{
	unsigned int count;

	backreset();
	if(mod==0)
		Diameter=0;
	else 
		Thickness=0;
	
	knacktab(0,&count);
	if(mod==0)
	{
		if(Diameter<90000)
		{
			Diameter=(double)(blpos-count);
			Diameter /= mmfak;
		}
	}
	else
	{
		if(Thickness<90000)
		{
			Thickness=(double)(blpos-count);
			Thickness /= mmfak;
			Thickness += 0.03f;
		}
	}
	
	os_dly_wait(200);
	backreset();
}

void mebef(void)
{
	unsigned int count;
	 
	backreset(); 								// 回零

	Diameter = 0;
	Hardness = 0;
	knacktab(1,&count); 				// 测试开始
	os_dly_wait(200);
	backreset(); 								// 回零
}

void setkraftfak()
{
	kraftfak=1.0;

	switch(HardEinh)
	{
		case 0:										//牛顿，N
			kraftfak=1.0;						
			break;
		case 1:										//公斤，Kp					
			kraftfak=9.81;			
			break;
		case 2:									  //？,Sc
			kraftfak=6.874;
			break;
	}
}

void setlaengfak()
{
	laengfak=1.0;
	switch(LaeEinh)
	{	
		case 0:										//公制，mm
			laengfak=1.0;
			break;
		case 1:										//英制，in
			laengfak=25.4;
			break;
	}
}

void LoadFixVal(void)
{
	if(readeeprom(ADRSIGNATUR)!=SIGNATUR)
	{
		InitEEPROM(0);
	}
	
	aktLanguage=readinteep(ADRLANGUAGE);
	if(aktLanguage>1) aktLanguage=0;
	
	MeasureMethod=readinteep(ADRMEASUREMETHOD);
	if(MeasureMethod>1)	MeasureMethod=0;
	
	terminologie=readinteep(ADRTERMINOLOGIE);
	if(terminologie<0 || terminologie>1) terminologie=1;
	
	autodiam=readinteep(ADRAUTODIAM);
	if(autodiam<0 || autodiam>1) autodiam=1;
	
	HardOn=readinteep(ADRHARDON);
	if(HardOn>1) HardOn=0;
	
	AutoMess=readinteep(ADRAUTOMESS);
	if(AutoMess>1) AutoMess=0;
  
	DatenAusgabe=readinteep(ADRDATENAUSGABE);
  if(DatenAusgabe>1) DatenAusgabe=0;
	
	DiamOn=readinteep(ADRDIAMON);
	if(DiamOn>1 || !dmena) DiamOn=0;
	
	ThickOn=readinteep(ADRTHICKON);
	if(ThickOn>1 || !dickena) ThickOn=0;
	
	Sensibility=readinteep(ADRSENSIBILITY);
	if(Sensibility<3 || Sensibility>50) Sensibility=10;
  
	loadfloat(&MeasSpeed,ADRMEASSPEED);
	if(MeasSpeed<0.1f || MeasSpeed>3.0f) MeasSpeed=2.3f;
  
	Force=readinteep(ADRFORCE);
	if(Force<5 || Force > 200) Force=20;
  
	NullKorr=(short)readinteep(ADRNULLKORR);
	if(NullKorr<-25 || NullKorr > 25) NullKorr=0;
	
	if(vers[49]==0x31)      //#ifdef GRMOT
		ForceLimit=FORCELIMITMAXGMOT;
	else
		ForceLimit=FORCELIMITMAXKMOT;
	
	MaxDose=500;
	loadfloat(&DragJaw,ADRDRAGJAW);
	if(DragJaw<0 || DragJaw>10.0f) DragJaw=0;
	
	MeasurePause=readinteep(ADRMEASUREPAUSE);
	if(MeasurePause>10) MeasurePause=0;
	
	HardEinh=readinteep(ADRHEINH);
	if(HardEinh>2) HardEinh=0;
	
	LaeEinh=readinteep(ADRLEINH);									//2021/9/29 9:57增加上电读取长度单位
	if(LaeEinh>1) LaeEinh=0;											//2021/10/19 星期二 9:55:26	读取硬度保存SC
	
	DatFormat=readinteep(ADRDATFORMAT);
	if(DatFormat>1) DatFormat=0;
	
	loadfloat(&NwtFak,ADRNWTFAK);
  if(NwtFak<16.0f || NwtFak>25.0f) NwtFak=19.0f;
	
	AutoPrint=readinteep(ADRAUTOPRINT);
	if(AutoPrint>1) AutoPrint=0;
	
	blpos=readinteep(ADRBLPOS);
	if(blpos<18000 || blpos>32000) blpos=26000;
	
	loadfloat(&eichmas,ADREICHMAS);
	if(eichmas<3.0f || eichmas>16.0f) eichmas=10.0f;
	
	loadfloat(&vorbrems,ADRVORBREMS);
	if(vorbrems<0.1f || vorbrems>2) vorbrems=0.5;
	
	loadfloat(&mmfak,ADRMMFAK);

	if(vers[49]==0x31)
	{    //#ifdef GRMOT
		if(mmfak<360 || mmfak>410) mmfak=380;
	}
	else
	{
		if(mmfak<620 || mmfak>700) mmfak=666;
	}
	
	eichpos=readinteep(ADREICHPOS);
	if(eichpos >25) eichpos=0;
	
	druckmode=readinteep(ADRDRUCKMODE);
	if(druckmode>1) druckmode=0;
	//if(vers[47]==0x30) 									// Gavin，2021-08-25 星期三 12:03:29
	//	druckmode=0;
	//if(vers[47]==0x31)
	//	druckmode=1;
	
	PrinterLanguage=readinteep(ADRPRINTERLANGUAGE);
	if(PrinterLanguage>1) PrinterLanguage=0;

	PrinterPaper=readinteep(ADRPRINTERPAPER);
	if(PrinterPaper>1) PrinterPaper=0;
	
	PrinterLanguage=readinteep(ADRPRINTERLANGUAGE);
	if(PrinterLanguage>1) PrinterLanguage=0;
	
	PrinterFrames=readinteep(ADRPRINTERFRAMES);
	if(PrinterFrames>1) PrinterFrames=0;
	
	ueberlauf=readinteep(ADRUEBERLAUF); 											// 加载循环时间
	//if(ueberlauf<10 || ueberlauf >120) ueberlauf=40;
	if(ueberlauf<10 || ueberlauf >120) ueberlauf=20; 					// 修改默认值，Gavin，2022-02-09 周三 10:34:25
	
	pruefanz=readinteep(ADRPRUEFANZ);
	if(pruefanz>100) pruefanz=10;
	
	countd=readlongeep(ADRCOUNTD);
	counth=readlongeep(ADRCOUNTH);
	loadfloat(&dmeing,ADRDMEING);
	setlaengfak();
	setkraftfak();
}

void eipbef(void)
{
	unsigned int i,schwelle;

	if(eichpos==0) return;
	schwelle=(int)((float)Sensibility * NwtFak);
	backreset();
	i=eichpos*mmfak;

	motor(1,i,0,2);
	while(GPIO_ReadOutputDataBit(GPIOB,M_EN) == 0)
	{
		if(GetDose(0)>schwelle+50 && StepRampe==0)
		{
			GPIO_WriteBit(PORT_B, M_EN, Bit_SET);
		}
		
		os_dly_wait(1);
	}
}

void getdatstr(char *dbu)
{
	unsigned int year;
 	RTC_DateTypeDef RTC_DateStructure;

	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure); 
	year=RTC_DateStructure.RTC_Year + 2000;
	if(DatFormat==0)
		sprintf(dbu,"%02d.%02d.%d",RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_Month,year);
	else
		sprintf(dbu,"%02d/%02d/%d",RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,year);
}

void getDTIMdatstr(DTIM dat,char *dbu)
{
	if(DatFormat==0)
		sprintf(dbu,"%02d.%02d.%d",dat.day,dat.month,dat.year);
	else
		sprintf(dbu,"%02d/%02d/%d",dat.month,dat.day,dat.year);
}	

void gettimstr(char *tbu) 
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	sprintf(tbu,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);
}

// DM13H----------------------------------------------------
void Delay13h(u32 count)
{
	u32 i=0;
	
	for(;i<count;i++);
}

//#define TWN 5 																					// DM13H延时
//#define TWN 10 																						// DM13H延时
//#define TWN 20 																						// DM13H延时，Gavin，2022-02-25 周五 09:45:28
#define TWN 2 																						// DM13H延时，Gavin，2022-02-25 周五 09:45:28

// 发送数据到13H数码管驱动芯片，两片级联
void SendDatTo13H(u16 TxDat1, u16 TxDat2)
{
	u8 i,j;
	u16 dis_buf[2];
	//LED_Init();
	dis_buf[0] = TxDat1;
	dis_buf[1] = TxDat2;
	
	for(i=0;i<2;i++) 											// 传送显示数据
	{
		for(j=0;j<16;j++)
		{
			if(dis_buf[i]&(1<<(15-j)))
				GPIO_WriteBit(PORT_B, MOSI, Bit_SET);
			else
				GPIO_WriteBit(PORT_B, MOSI, Bit_RESET);
			
			//Delay13h(TWN);
			GPIO_WriteBit(PORT_B, CLK, Bit_SET); 								// 时钟
			//Delay13h(TWN);
			GPIO_WriteBit(PORT_B, CLK, Bit_RESET);
		}
	}
	
	//Delay13h(TWN);
	GPIO_WriteBit(PORT_B, MISO, Bit_SET); 										// 锁存
	//Delay13h(TWN);
	GPIO_WriteBit(PORT_B, MISO, Bit_RESET); 
}

void anzout1(char *str)
{
	unsigned char i;
	for(i=0;i<4;i++)
	{
		if(str[i] < 0x30)
		{
			anzdat[0][i] = 0x0000;
		}
		else
		{	
			anzdat[0][i] = font[(str[i] - 0x30)];
			if((i==0 && dp10) || (i==1 && dp20) || (i==2 && dp30) || (i==3 && dp40)) 
				//anzdat[0][i] |= 0x8000;
				anzdat[0][i] |= 0x0002;
		}	
	}
}

void anzout2(char *str)
{
	unsigned char i;
	for(i=0;i<4;i++)
	{
		if(str[i]=='-')
			str[i]=0x3A;
		else
		if(str[i]=='*') 					// Gavin, v2.07, 2019-09-17 星期二 18:43:49
			str[i]=0x3B;
		
		if(str[i]<0x30)
		{
			anzdat[1][i]=0x0000;
		}
		else
		{
			anzdat[1][i] = font[(str[i] - 0x30)];
			if((i==0 && dp1) || (i==1 && dp2) || (i==2 && dp3) || (i==3 && dp4)) 
				//anzdat[1][i] |= 0x8000;
				anzdat[1][i] |= 0x0002;
		}
	}
}

void anzrefresh(void)
{
	//空函数
}


//保留小数位模式，*10^n + 0.5,除10^n-------------------
float round0(float in) 							//浮点数四舍五入，不保留小数
{
	float fdat;
	
	fdat = (int)in;
	return fdat;
}

float round1(float in) 							//浮点数四舍五入，保留1位小数
{
	float fdat;
	
	fdat = (int)(in*10+0.5f)/10.0f;
	return fdat;
}

float round2(float in) 							//浮点数四舍五入，保留2位小数
{
	float fdat;
	
	fdat = (int)(in*100+0.5f)/100.0f;
	return fdat;
}

float round3(float in) 							//浮点数四舍五入，保留3位小数
{
	float fdat;
	
	fdat = (int)(in*1000+0.5f)/1000.0f;
	return fdat;
}

void mekabef(void)
{
	unsigned int ueberl,steps,tbadd,maxval,ACou,schwelle;
	int adx,maxmew=0;

	steps=(int)((vorbrems+6)*mmfak);
	schwelle=(int)((float)Sensibility * NwtFak);
	if(dmmess(&ACou))
	{
		return;
	}
	
	os_dly_wait(100);
	motor(1,steps,1,2);

	ueberl=0;
	while(ueberl<4 & GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		adx=GetDoseAvrgN(0,10);
		if(adx > schwelle)
			ueberl++;
		else
			ueberl=0;
		
		os_tsk_pass();
	}
	tbadd=AktStep;

	ueberl=0;
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)
	{
		return;
	}
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);	

	os_dly_wait(5);

	mesenable=0;
	maxmew=0;
	if(ForceLimit<=MaxDose)
	{
		maxval= ForceLimit*NwtFak;
	}
	else
	{
		maxval=MaxDose*NwtFak;			///HARDKORR2;
	}
	
	motor(1,MAXMOT-(ACou + tbadd),1,2);
	while(maxmew<maxval && GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		while(!mesenable) os_tsk_pass();
		mesenable=0;
		adx=GetDoseAvrgN(0,50); //		adx=GetDose();
		if(adx>maxmew)
		{
			maxmew=adx;
		}
		
		os_tsk_pass();
	}
	
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)
	{
		return;
	}
	
	StepSperr=__TRUE;
	GPIO_WriteBit(PORT_B, M_EN, Bit_RESET);
}

int GetDiameter(void)
{
	unsigned int ueberl,schwelle,methsave;
	int adx;
	float speedsave;

	speedsave=MeasSpeed;
	methsave=MeasureMethod;
	MeasureMethod=0;
	MeasSpeed=3.0;
	schwelle=(int)((float)Sensibility * NwtFak);
	motor(1,blpos,1,2);

	ueberl=0;
	while(ueberl<4 & GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		adx=GetDoseAvrgN(1,10);
		if(adx > schwelle) ueberl++;
		else ueberl=0;
		os_tsk_pass();
	}
	
	ueberl=0;
	MeasSpeed=speedsave;
	MeasureMethod=methsave;
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)
	{
		return(0);
	}
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);	

	dmeing=(double)(blpos-AktStep);
	dmeing/=mmfak;
	dmeing+=0.3f;
	backreset();
	return(1);
}

int GetDick(void)
{
	unsigned int ueberl,schwelle,methsave;
	int adx;
	float speedsave;

	speedsave=MeasSpeed;
	methsave=MeasureMethod;
	MeasureMethod=0;
	MeasSpeed=3.0;
	schwelle=(int)((float)Sensibility * NwtFak);
	motor(1,blpos,1,2);
	
	ueberl=0;
	while(ueberl<4 & GPIO_ReadOutputDataBit(GPIOB,M_EN)==0)
	{
		adx=GetDoseAvrgN(1,10);
		if(adx > schwelle) ueberl++;
		else ueberl=0;
		os_tsk_pass();
	}
	
	ueberl=0;
	MeasSpeed=speedsave;
	MeasureMethod=methsave;
	if(GPIO_ReadOutputDataBit(GPIOB,M_EN)==1)
	{
		return(0);
	}
	
	GPIO_WriteBit(PORT_B, M_EN, Bit_SET);

	dickeing=(double)(blpos-AktStep);
	dickeing/=mmfak;
	dickeing+=0.3f;
	backreset();
	return(1);
}

u16 Get_Adc(u8 ch)   
{
	  	//设置指定ADC的规则组通道,一个序列,采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精准度	    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

