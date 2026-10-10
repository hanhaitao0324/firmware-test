/***************************************************************************

 Author        : Neil Zhao - CAST

 Date          : Feb 20th, 2009

 File          : AD7190_IO.c

 Hardware      : ADuC7026 and AD7190

 Description   : Use the GPIO to simulate the SPI communication of AD7190
	
***************************************************************************/
#include "AD7190.h"
//#include "delay.h"
//#include "lcd.h"
#include "math.h"
#include "usart.h"

//#define V_Ref 3 		// 参考电压


#define AD7190_Write 				0
#define AD7190_Read 				1

//寄存器选择
#define ComState_register 				(0)<<3	//
#define Mode_register 						(1)<<3
#define Config_register 					(2)<<3
#define Data_register 						(3)<<3
#define ID_register 							(4)<<3
#define GPOCON_register 					(5)<<3
#define Disorders_register 				(6)<<3
#define FullScale_register 				(7)<<3

//模式寄存器
#define MODE_ADC_OneByOne 				(0)<<21
#define MODE_ADC_OnlyOne 					(1)<<21
#define MODE_ADC_Free 						(2)<<21
#define MODE_ADC_SavePower 				(3)<<21
#define MODE_ADC_AdjustZero 			(4)<<21
#define MODE_ADC_AdjustFull 			(5)<<21
#define MODE_ADC_SysAdjustZero 		(6)<<21
#define MODE_ADC_SysAdjustFull 		(7)<<21
#define MODE_MCLK_OUTosc 					(0)<<18
#define MODE_MCLK_OUTclo 					(1)<<18
#define MODE_MCLK_IN 							(2)<<18
#define MODE_MCLK_INcloOut 				(3)<<18
#define MODE_SINC3 								(1)<<15
#define MODE_ENPAR 								(1)<<13
#define MODE_Single 							(1)<<11
#define MODE_REJ60 								(1)<<10
//#define MODE_Filter_Speed		0

//配置寄存器
#define Config_Chop_EN 						(1)<<23
#define Config_REFSEL_IO 					(1)<<20
#define Config_Burn_EN 						(1)<<7
#define Config_REFDET_EN 					(1)<<6
#define Config_BUF_EN 						(1)<<4
#define Config_UB_EN 							(1)<<3

#define Config_Ch0_A1A2 					(1)<<8
#define Config_Ch1_A3A4 					(1)<<9
#define Config_Ch2_temp 					(1)<<10
#define Config_Ch3_A2A2 					(1)<<11
#define Config_Ch4_A1AC 					(1)<<12
#define Config_Ch5_A2AC 					(1)<<13
#define Config_Ch6_A3AC 					(1)<<14
#define Config_Ch7_A4AC 					(1)<<15

#define Config_ADC_Gain_1 		0
#define Config_ADC_Gain_8 		3
#define Config_ADC_Gain_16 		4
#define Config_ADC_Gain_32 		5
#define Config_ADC_Gain_64 		6
#define Config_ADC_Gain_128 	7


typedef struct 
{
	u32 ADC_Mode;
	u32 Return_state;
	u32 ADC_SCLK;
	u32 SINC3_EN;
	u32 ENPAR;
	u32 Single_EN;
	u32 REJ60_EN;
	u32 Filter;
}AD7190_MODE_SET;

typedef struct 
{
	u32 Config_Channel;
	u32 Config_ADC_Gain;	
	u32 Config_Chop;			//斩波使能
	u32 Config_REFSEL;
	u32 Config_Burn;
	u32 Config_REFDET;
	u32 Config_BUF;
	u32 Config_UB;
}AD7190_Config_SET;

u8 ADC_Channel,text;
u8 shuju;
u8 ADC_Gain;

void AD7190_Connect_Set(u8 Write_EN,u8 WR,u8 dat);

//u8 WaitDataRDY(void)
//{
//	u8 buf;
//	AD7190_Connect_Set(0,AD7190_Read,ComState_register);
//	ReadFromAD7190(1,&buf);
//	
//	if((buf&0x40))
//		return 0;
//	else if(!(buf&0x80))
//		return 1;
//	
//	return 0;
//}

u8 WaitDataRDY(void)
{
	u8 buf;
	
	while(1)
	{
		AD7190_Connect_Set(0,AD7190_Read,ComState_register);
		ReadFromAD7190(1,&buf);
		
		if((buf&0x40))
		{			
			delay_ms(10);
			return 0;
		}
		else if(!(buf&0x80))
		{
			return 1;
		}
		
		delay_ms(1);
	}
//	while(SPI_DOUT); //here miso(Dout) also work as !DRD
}

//---------------------------------
//void WriteToAD7190(unsigned char count,unsigned char *buf);
//---------------------------------
//Function that writes to the AD7190 via the SPI port. 
//--------------------------------------------------------------------------------
void WriteToAD7190(unsigned char count, unsigned char *buf)
{
	unsigned char ValueToWrite = 0;
	unsigned char i = 0;
	unsigned char j = 0;
	
	SET_SCL();
	delay_us(1);
	SET_CS();
	delay_us(1);
	CLR_CS();
	delay_us(1);
	
	for(i=count;i>0;i--)
 	{
	 	ValueToWrite = *(buf + i - 1);
		for(j=0; j<8; j++)
		{
			CLR_SCL();
			if(0x80 == (ValueToWrite & 0x80))
			{
				SET_SDO();	  //Send one to SDO pin
			}
			else
			{
				CLR_SDO();	  //Send zero to SDO pin
			}
			delay_us(1);
			SET_SCL();
			delay_us(1);
			ValueToWrite <<= 1;	//Rotate data
		}
	}
	SET_CS();
}


//---------------------------------
//void ReadFromAD7190(unsigned char count,unsigned char *buf)
//---------------------------------
//Function that reads from the AD7190 via the SPI port. 
//--------------------------------------------------------------------------------
void ReadFromAD7190(unsigned char count, unsigned char *buf)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char RotateData = 0;
	
	SET_SCL();
	delay_us(1);
	SET_CS();
	delay_us(1);
	CLR_CS();
	delay_us(1);
	
	for(j=count; j>0; j--)
	{
		for(i=0; i<8; i++)
		{
			CLR_SCL();
			RotateData <<= 1;		//Rotate data
			delay_us(1);
			RotateData |= GP0DAT;			//Read SDI of AD7190
			SET_SCL();
			delay_us(1);
		}
		
		*(buf + j - 1)= RotateData;
		RotateData=0;
	}
	
	SET_CS();
} 

unsigned long GET_AD7190(void)
{
	u32 DAC_Dat=0;
	u8 buf[3];
	
	ReadFromAD7190(3,buf);
	
	AD7190_Connect_Set(0,AD7190_Read,ComState_register);
	ReadFromAD7190(1,&text);
	ADC_Channel=text&0x07;
	
	DAC_Dat=buf[2]*256*256+buf[1]*256+buf[0];
	
	return DAC_Dat;
}

unsigned long GET_AD7190_1(void)
{
	u32 DAC_Dat=0;
	u8 buf[4];
	
	ReadFromAD7190(4,buf);
	DAC_Dat=buf[3]*256*256*256+buf[2]*256*256+buf[1]*256+buf[0];
	
	return DAC_Dat;
}

void AD7190_Connect_Set(u8 Write_EN,u8 WR,u8 dat)
{
	u8 buf=dat;
	
	if(Write_EN)
		buf=1<<7;
	
	if(WR)
		buf=1<<6;
	
	WriteToAD7190(1,&buf);
}

void AD7190_GPOCON_Set(u8 dat)
{
	u8 buf = dat;
	u8 Dat;
	
	Dat = GPOCON_register;
	WriteToAD7190(1,&Dat);
	WriteToAD7190(1,&buf);
}

void AD7190_Mode_Set(AD7190_MODE_SET *Mode)
{
	u32 buf=Mode->ADC_Mode;
	u8 Write_dat[3],Dat;
	
	buf|=Mode->ADC_SCLK;
	buf|=Mode->Filter;
	
	if(Mode->Return_state)
		buf|=(1<<20);
	
	if(Mode->SINC3_EN)
		buf|=MODE_SINC3;
	
	if(Mode->ENPAR)
		buf|=MODE_ENPAR;
	
	if(Mode->Single_EN)
		buf|=MODE_Single;
	
	if(Mode->REJ60_EN)
		buf|=MODE_REJ60;
	
	Write_dat[2]=buf>>16;
	Write_dat[1]=buf>>8;
	Write_dat[0]=buf;
	Dat=Mode_register;
	WriteToAD7190(1,&Dat);
	WriteToAD7190(3,Write_dat);
}


void AD7190_Config_Set(AD7190_Config_SET *Mode)
{
	u32 buf=Mode->Config_Channel|Mode->Config_ADC_Gain;
	u8 Write_dat[3],Dat;
	
	ADC_Gain=pow(2,Mode->Config_ADC_Gain);
	if(Mode->Config_Chop)
		buf|=Config_Chop_EN;
	
	if(Mode->Config_REFSEL)
		buf|=Config_REFSEL_IO;
	
	if(Mode->Config_Burn)
		buf|=Config_Burn_EN;
	
	if(Mode->Config_REFDET)
		buf|=Config_REFDET_EN;
	
	if(Mode->Config_BUF)
		buf|=Config_BUF_EN;
	
	if(Mode->Config_UB)
		buf|=Config_UB_EN;
	
	Write_dat[2]=buf>>16;
	Write_dat[1]=buf>>8;
	Write_dat[0]=buf;
	Dat=Config_register;
	WriteToAD7190(1,&Dat);
	WriteToAD7190(3,Write_dat);
}

long int DisordersData = 0;
long int DisordersTemp = 0;
void AD7190_Init(void)
{
	u8 buf[5];
	AD7190_MODE_SET 		Mode;
	AD7190_Config_SET   Config;

	Config.Config_Channel		=		Config_Ch0_A1A2;
	Config.Config_ADC_Gain		=		Config_ADC_Gain_64;		// 增益从128改为64后，测量结果接近真实值
	Config.Config_Chop			=		0; 				// 斩波禁用
	Config.Config_REFSEL		=		0; 				// 在REFIN1(+)与REFIN1(-)之间施加的外部基准电压
	Config.Config_Burn			=		0; 				// 禁用激励电流
	Config.Config_REFDET		=		1; 				// 使能基准电压检测
	Config.Config_BUF			=		0; 				// 模拟输入不缓冲
	Config.Config_UB			=		0; 				// 双极性
	
	Mode.ADC_Mode			=	MODE_ADC_AdjustZero;	// 内部零电平校准
	Mode.Return_state		=	0;
	Mode.ADC_SCLK			=	MODE_MCLK_IN; 		// 内部4.92MHz时钟
	Mode.SINC3_EN			=	0; 								// 使用sinc4滤波器（默认值）
	Mode.ENPAR				=	0; 								// 禁用奇偶校验
	Mode.Single_EN			=	0; 								// 禁用单周期转换
	Mode.REJ60_EN			=	0; 								// 60Hz陷波禁用
	Mode.Filter				=	1; 								// 滤波器设置
	
	
	AD7190_Config_Set(&Config);
	WaitDataRDY();
	AD7190_Mode_Set(&Mode);
	WaitDataRDY();
	Mode.ADC_Mode			=	MODE_ADC_AdjustFull;	// 内置满量程校准
	AD7190_Mode_Set(&Mode);
	WaitDataRDY();
	Mode.ADC_Mode			=	MODE_ADC_OneByOne;		// 连续转换模式
	//Mode.Filter				=	10;
	Mode.Filter				=	1;
	AD7190_Mode_Set(&Mode);
	
	AD7190_GPOCON_Set(0x40); 							// 打开电桥开关
	
	AD7190_Connect_Set(0,AD7190_Read,Disorders_register);
	ReadFromAD7190(3,&buf[0]);
	DisordersData = (((u32)buf[0])<<16)|(((u32)buf[1])<<8)|(((u32)buf[2]));
	DisordersTemp = (DisordersData&0X007FFFFF)^0X007FFFFF;
//	DisordersTemp = (DisordersData&0XFF7FFFFF)^0XFF800000+1;
	DisordersData = (DisordersData>=0X00800000)?(-(DisordersTemp+1)):(DisordersData);	
}

void ad7190_Init(void)
{
	u8 buf[3];
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 							// 使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 											// 输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 											// 推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 									// IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure); 															// 根据设定参数初始化GPIO
	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6); 							// PB356 输出高
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 												// 普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 												// 上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 															// 根据设定参数初始化GPIO
	
	buf[0] = 0xff;
	WriteToAD7190(1,buf);

	buf[0] = 0xff;
	WriteToAD7190(1,buf);

	buf[0] = 0xff;
	WriteToAD7190(1,buf);

	buf[0] = 0xff;
	WriteToAD7190(1,buf);

	buf[0] = 0xff;
	WriteToAD7190(1,buf);

//	buf[0] = 0xff;
//	WriteToAD7190(1,buf);

	AD7190_Init();

//	WaitDataRDY();
//	AD7190_Connect_Set(0,AD7190_Read,Config_register);
//	GET_AD7190();
//	WaitDataRDY();
//	AD7190_Connect_Set(0,AD7190_Read,Mode_register);
//	GET_AD7190();
	
	
////--------------------------------------------------------------------------------------------
//	buf[0] = 0x10;
//	WriteToAD7190(1,buf);	  	//write communication register 0x10 to control the progress to write configuration register 

//	buf[2] = 0x80;				//sinc4 enable;ref=refin1+ refin1-.
//	buf[1] = 0x01;				//通道1，差分AN1+ AN1-输入。
//	buf[0] = 0x0f;				//no buf,dan jixing  ,gain=128.
//	WriteToAD7190(3,buf);		//write configuration register,unipolar operation,gain=1,channel:AIN1 to AIN2
////--------------------------------------------------------------------------------------------
//	WaitDataRDY();
////--------------------------------------------------------------------------------------------
//	buf[0] = 0x08;
//	WriteToAD7190(1,buf);		//write communication register 0x08 to control the progress to write mode register 

//	buf[2] = 0x08;				//internal 4.92MHz clock//c8,wai 0;e8,wai full;88 nei 0;a8 nei full
//	buf[1] = 0x80;				//sinc4;filter word=5,data rate=240Hz
//	buf[0] = 0x10;				//
//	WriteToAD7190(3,buf);		//write mode register,internal 4.92MHz clock,output data rate=4.7Hz
}

float ADC_Votage(int n)
{
	int tempn;
	float Vo;
	
	tempn = Get_ADC_AvrgNum(n);
	Vo=((tempn/(float)8388608)-1)*V_Ref*1000/ADC_Gain;	// 单位：mV
	if(Vo<0)
	Vo = -Vo;
	
	return Vo;
}

u32 ADC_Num(void)
{
	u8 buf[3];
	u32 tempn;
	
	WaitDataRDY();

 	buf[0] = 0x58;
	WriteToAD7190(1,buf); 			// write communication register 0x58 to control the progress to read data register
	
	tempn = GET_AD7190();
	
	return tempn;
}

u32 Get_ADC_AvrgNum(int n)
{
	int i;
	long int sum = 0;

	for (i = 0; i < n; i++)
	{
		sum += ADC_Num();
	}
	sum /= n;
	i = (u32)sum;
	return(i);
}
