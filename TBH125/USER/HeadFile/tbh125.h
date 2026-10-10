#ifndef _TBH125_H
#define _TBH125_H

#include <RTL.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>	
#include "stm32f4xx.h"
#include "tbhhard.h"

//----------------------------------------------------------------------------
// PORT A
//----------------------------------------------------------------------------
#define PORT_A       GPIOA
#define MB_LED0			(GPIO_Pin_1)	
#define KB5	    		(GPIO_Pin_2)	
#define KB6		    	(GPIO_Pin_4)	
#define KB4			 		(GPIO_Pin_5)	
#define M_STEP    	(GPIO_Pin_6)	
#define M_RST	    	(GPIO_Pin_7)	
#define	I2C3_SCL		(GPIO_Pin_8)
#define	USB_VBC			(GPIO_Pin_9)
#define	USB_D_up		(GPIO_Pin_11)
#define	USB_D_dowm	(GPIO_Pin_12)
#define	SWDIO				(GPIO_Pin_13)
#define	SWCLK				(GPIO_Pin_14)

//----------------------------------------------------------------------------
// Port B
//----------------------------------------------------------------------------
#define PORT_B    	(GPIOB)
#define M_DIR		  (GPIO_Pin_0)	
#define M_EN		    (GPIO_Pin_1)	
#define BOOT1    		(GPIO_Pin_2)	
#define MB_LED1		  (GPIO_Pin_4)	
#define AD7940_CS		(GPIO_Pin_5)	
#define AD7940_SCLK	(GPIO_Pin_6)	
#define AD7940_SDAT	(GPIO_Pin_7)	
#define CLK	 				(GPIO_Pin_13) 
#define MISO 				(GPIO_Pin_14) 
#define MOSI				(GPIO_Pin_15) 

//----------------------------------------------------------------------------
// Port C
//----------------------------------------------------------------------------
#define PORT_C			(GPIOC)
#define Inp_Bn			(GPIO_Pin_0)
#define POW_CHECK		(GPIO_Pin_1)
#define M_MS1				(GPIO_Pin_4)
#define M_MS2				(GPIO_Pin_5)
#define I2C3_SDA		(GPIO_Pin_9)
#define DS2401_DAT	(GPIO_Pin_13)

//----------------------------------------------------------------------------
// Port D
//----------------------------------------------------------------------------
#define PORT_D    	(GPIOD)
#define USART2_RTS	(GPIO_Pin_4)
#define USART2_TX		(GPIO_Pin_5)
#define USART2_RX		(GPIO_Pin_6)
#define VNC_TXD			(GPIO_Pin_8)
#define VNC_RXD			(GPIO_Pin_9)
#define VNC_DTR			(GPIO_Pin_10)
#define VNC_RTS			(GPIO_Pin_11)
#define VNC_CTS			(GPIO_Pin_12)
#define VNC_DSR			(GPIO_Pin_13)
#define VNC_PROG		(GPIO_Pin_14)
#define VNC_RESET		(GPIO_Pin_15)

//----------------------------------------------------------------------------
// Port E
//----------------------------------------------------------------------------
#define PORT_E    	(GPIOE)
#define KB2					(GPIO_Pin_7)
#define KB1					(GPIO_Pin_8)
#define KB3					(GPIO_Pin_9)
#define MB_Buz0			(GPIO_Pin_10)
#define PD3					(GPIO_Pin_11)
#define PD2					(GPIO_Pin_12)
#define PD1					(GPIO_Pin_13)
#define PD0					(GPIO_Pin_14)
//----------------------------------------------------------------------------
// 移植头文件部分
//----------------------------------------------------------------------------
#define HARDKORR 0.95f;

#define PRELL 		1000
#define MAXMOT 		22000

#define KALIB 		9
#define UPGRADE 	10

#define TLO	1 			// 左上键
#define TLU	2 			// 左下键
#define TMO	3 			// 中上键
#define TMU	4 			// 中下键
#define TRO	5 			// 右上键
#define TRU	6 			// 右下键

#define ENDPCM 'Y'
#define FORCELIMITMAXKMOT 300
#define FORCELIMITMAXGMOT 500

#define FULLSPEEDF 0xAD0			//0x8B0 0x7D0  0x2A0
#define FULLSPEEDB 0xAD0			//0x7D0  0x2A0
#define KRAFTDIFF 5

#define SER_BUF_SIZE 	2048
#define TBUF_SIZE   	2048 		/*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   	2048 

#define TFE     0x01
#define TNF     0x02
#define RNE     0x04
#define RFF     0x08
#define BSY     0x10

typedef struct DateTime
{
	unsigned int day;
	unsigned int month;
	unsigned int year;
	unsigned int hour;
	unsigned int min;
	unsigned int sec;
}DTIM;

extern void InitI2C(void);
extern void writeeeprom(unsigned char, unsigned int);
extern unsigned char readeeprom(unsigned int); 
unsigned int getadDose(void);
unsigned int GetIni(unsigned int);
unsigned int GetIniVal(unsigned int);
void tbhnull(void);
int GetDose(int);								  
void motor(int,unsigned int ,int ,unsigned int);
void GetInReport (void);
void SetOutReport (void);
__irq void def_irq (void);
void com_initialize (void);
void putstr(char *);
void putusbstr(char *);
unsigned int dmmess(unsigned int *);
void backreset(void);
unsigned int blbef(int mode);
unsigned int blebef(void);
void mdbef(int);
void mebef(void);
void writinteep(unsigned int ,unsigned int);
void writlongeep(unsigned long,unsigned int);
unsigned int readinteep(unsigned int);
unsigned long readlongeep(unsigned int);
void LoadFixVal(void);
void beep(void);
unsigned char knacktab(char,unsigned int *);
void spi_init(void);
int com_putchar1 (int);
int com_putchar3 (unsigned char);
unsigned int Vinc_StartProg(void);
void Vinc_Reset(void);
void Vinc_Send(char *);
unsigned int Vinc_Query(unsigned int);
int com_getchar3(void);
int com_put3ww(int c);
int com_putchar2 (int);
void Vinc_QD(unsigned int,char *);
void printheader(unsigned char);
int druckuep(void);
int druckuep32(void);
void com_bufclr2(void);
int Vinc_Prog_Block(int,unsigned char *,unsigned int);
int VincErase(void);
void kontrolle(void);
void calcauswert(void);
void getDTIMdatstr(DTIM ,char *);

unsigned char gettast(void);
unsigned char gettasto(void);
unsigned char gettastTMO(void);
void readeepbuf(unsigned char *, unsigned int,unsigned int); 
void writeepbuf(unsigned char *, unsigned int,unsigned int); 
void InitEEPROM(unsigned char);
void savefloat(float *,unsigned int adr);
void loadfloat(float *,unsigned int adr);
void savedouble(double *,unsigned int adr);
void loaddouble(double *,unsigned int adr);
void Vinc_SC(unsigned int num);
int StartPrinter(void);
int SendPrinter(char *str);
int SendPrinterCC(const char *str);
void StopPrinter(void);
void PrinterInit(void);
void PrinterExit(void);
void eipbef(void);
void Vinc_Extended(void);
void Vinc_GetBuf(char *);
void com_bufclr3(void);
int druckergebnis(void);
int druckergebnis32(void);
void LoadDate(DTIM *,unsigned int );		
void WriteDate(DTIM *,unsigned int);
void setkraftfak(void);
void setlaengfak(void);
void getdatstr(char *);
void gettimstr(char *); 
int druckkp(void);
int druckkp32(void);
void Vinc_Vers(char *);
void Vinc_Dir(char *);
int Vinc_OpenFile(char *);
int Vinc_CloseFile(char *);
int Vinc_WriteFile(int ,char *);
int Vinc_TestStick(void);
int Vinc_TestFile(char *);
int com_getchar21 (void);
void putstr1(char *str);
int com_getchar1 (void);
int GetDoseAvrg(int);
int GetDoseAvrgN(int,int);
void medibef(void);
void mddibef(void);
float round0(float);
float round1(float);
float round2(float);
float round3(float);
int Vinc_GetPrompt(void);
int Vinc_TestAlive(void);
float GetUein(void);
U8 spi_send (U8 outb);
void anzrefresh(void);
void anzout1(char *);
void anzout2(char *);
void logo(void);
void einstell(void);
s32 Ymodem_Receive (u8 *);
int com_getchar2 (void);
void kalibriere(void);
void kalidurchm(unsigned char);
void kalibruch(unsigned char); 
void StartLicManagement(void);
void InitLicManagement(void);
void AKC_RTCSetDate(DTIM dat); 
void AKC_RTCGetDate(DTIM *dat);
void AKC_RTCSetOrgDate(DTIM dat); 
void AKC_RTCGetOrgDate(DTIM *dat);
void ShowDaysLeft(void);
long FCode(char *ID,int art);
void TestLicense(void);
double JulDat(DTIM dtim);
DTIM GregDat(double JulDat);
void getid(volatile char *idn);
void showID(void);
int TestFinger(void);
int CheckPrompt(void);
void SendKalibrator(char *,char *);
int GetLength(void);
void mekabef(void);
int StartKalibrator(void);
float GetKaliVal(float *);
void PrintBold(void);
void PrintNorm(void);
void PrintCondensed(void);
int druckuep(void);
void LoadKalID(char *,unsigned int);		
void WriteKalID(char *,unsigned int);
void LoadDosID(char *,unsigned int);		
void WriteDosID(char *,unsigned int);
int GetDiameter(void);
int GetDick(void);
void SendIntPrinter(char *);
void SendRS232(char *);



typedef struct buf_st
{
  unsigned int in;          	/* Next In Index */
  unsigned int out;         	/* Next Out Index */
  char buf [SER_BUF_SIZE]; 		/* Buffer */
}SBUF;

#define ADRAUTODIAM 					6 				// SETMENU-15.AUTO
#define ADRLANGUAGE 					10 				// SETMENU-10.LANG
#define ADRCOUNTH 						12
#define ADRCOUNTD 						16
#define ADRMEASUREMETHOD 			20 				// SETMENU-13.METH
#define ADRSENSIBILITY 				22 				// SETMENU-xx.THRE，门槛值, Gavin，2022-02-09 周三 10:01:40
#define ADRMEASSPEED 					24
#define ADRFORCE 							28
#define ADRFORCELIMIT 				30
#define ADRDRAGJAW 						32
#define ADRMEASUREPAUSE 			36 				// SETMENU-0.PAUS
#define ADRCCD 								64

#define ADRLEINH 							74 				// SETMENU-3.UNID
#define ADRHEINH 							76 				// SETMENU-2.UNIH
#define ADRPRUEFANZ 					78
#define ADRDMEING 						80

#define ADRNWTFAK 						86
#define ADRBLPOS 							90
#define ADREICHMAS 						92
#define ADRVORBREMS 					96 
#define ADRMMFAK 							102
#define ADREICHPOS 						106 			// SETMENU-1.CPOS
#define ADRDRUCKMODE 					110 			// SETMENU-16.PRIN
#define ADRPRINTERPAPER 			112
#define ADRPRINTERLANGUAGE 		114 			// SETMENU-17.PRLG
#define ADRPRINTERFRAMES 			116
#define ADRDIRDIAM 						118
#define ADRAUTOPRINT 					124

#define ADRFULLDAYS 					132
#define ADRPERIODS 						134
#define ADRAKTPERIOD 					136
#define ADRMANUFACT 					138
#define ADRNOLICENSE 					140
#define ADRDAYSLEFT 					150
#define ADRDATEOFFSET 				152
#define ADRLASTDAY 						164

#define ADRDIRANZD 						202
#define ADRDIRANZH 						204

#define ADRMESSZEIT 					246
#define ADRMESSDAT 						256
#define ADRONWT 							266
#define ADRLBKALO 						270
#define ADRLBIKALO 						272
#define ADRLASDIAM 						274
#define ADRLBKAL 							278
#define ADRLBIKAL 						280
//#define ADRSERNUM 						284
#define ADRVALIDAT 						326
#define ADRKRAFTAN 						344
#define ADRMESMETH 						346
#define ADRDIRKORR 						352
#define ADRDATFORMAT 					356 			// SETMENU-12.DATF，Gavin，2022-02-09 周三 09:56:50
#define ADRUEBERLAUF 					360 			// SETMENU-xx.CYCL，循环时间，Gavin，2022-02-09 周三 09:57:18
#define ADRCHECKWGT1 					364 								// 检测数据的结果？，2016/11/10 14:08:58
#define ADRCHECKWGT2 					368
#define ADRCHECKWGT3 					372

#define ADRHARDON 						384 			// SETMENU-21.HARD
#define ADRDIAMON 						388 			// SETMENU-22.DIAM
#define ADRTHICKON 						392 			// SETMENU-23.THIC
#define ADRAUTOMESS 					396 			// SETMENU-19.AUME

#define ADRKALDDAT 						400
#define ADRKALDDATO 					550
#define ADRKALBDAT 						450
#define ADRKALBDATO 					600
#define ADRLASTDATE 					500 								
#define ADRLASKMOD 						650
#define ADRKALIMODE 					654
#define ADRTERMINOLOGIE 			656 			// SETMENU-14.TERM
#define ADRJUSBDAT 						660
#define ADRJUSDDAT 						704
#define ADRJUSBDATO 					768 								// 上一次硬度校正日期
#define ADRJUSDDATO 					832 								// 上一次直径或厚度校正日期
#define ADRKALIVERS 					896
#define ADRKALIVERSO 					916
#define ADRDOSNR 							960
#define ADRDOSNRO 						980
#define ADRDICKEING 					1000
#define ADRNULLKORR 					1020 			// SETMENU-18.ZECO
#define ADRDATENAUSGABE 			1030 			// SETMENU-20.DOUT

// 增加的密码参数地址，Gavin，206-9，2019-07-25 星期四 14:32:27
#define ADRDATEPW 						1040
#define ADRDATEPWC 						1045 			// 206-9-1, Gavin, 密码校验，2019-08-12 星期一 15:21:29
#define ADRPWMGPW 						1050
#define ADRPWMGPWC 						1055 			// 206-9-1, Gavin, 密码校验，2019-08-12 星期一 15:21:29

#define HWVS 									0x7FF0
#define ADRSIGNATUR 					0x7FFF
#define SIGNATUR 							0x5B

// 增加Verison参数地址	2021/07/21 
#define ADRVERSION						1200			
#define ADRNEWEEPROM					1400			//占用3个字节
#define ADRHARDWAREVER    		1404
//按键宏定义
#define KEY1        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8) 
#define KEY2        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7) 
#define KEY3        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9) 
#define KEY4        GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) 
#define KEY5        GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) 
#define KEY6        GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) 

//位带操作，实现51类似的GPIO控制功能
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
 
//IO口操作，只对单一的IO口！
//确保n的值小于16！
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

//IO操作函数 
//#define IIC_SDA    PAout(8) //SDA
//#define IIC_SCL    PCout(9) //SCL	 
//#define READ_SDA   PAin(8)  //输入SDA
#define IIC_SDA    PCout(9) //SDA
#define IIC_SCL    PAout(8) //SCL	 
#define READ_SDA   PCin(9)  //输入SDA

//SDA方向设置函数
//void SDA_IN(void);					//SDA设置为输入
//void SDA_OUT(void);					//SDA设置为输出

// IO口方向设置 ------------------------------------------------------
#define SDA_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}		// PC9输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} 	// PC9输出模式

//模拟I2C所需的两个延迟函数
void delay_us(u32 nus);					//约5.1us
void delay_ms(u16 nms);					//约5ms

//DS2401的IO方向设置
void OW_PIN_IN(void);			//设置为输入
void OW_PIN_OUT(void);		//设置为输出
//#define OW_PIN_IN()  {GPIOC->MODER&=~(3<<(13*2));GPIOC->MODER|=0<<(13*2);}	//PC13为输入模式
//#define OW_PIN_OUT() {GPIOC->MODER&=~(3<<(13*2));GPIOC->MODER|=1<<(13*2);} //PC13为输出模式

//IO初始化函数
void LED_Init(void);
void Beep_Init(void);
void KEY_Init(void);
void MOTOR_Init(void);
void AD7940_Init(void);
void VNC_Init(void);

void IIC_Init(void);
void USBIO_Init(void);
void TIM3_Init(u16 arr,u16 psc);
void SendDatTo13H(u16 TxDat1, u16 TxDat2);
u16 Get_Adc(u8 ch);
void  Adc_Init(void);
void SET_Baudrate(USART_TypeDef* USARTx, uint32_t baudrate);	//串口设置波特率
u8 My_RTC_Init(void);																				//RTC时钟初始化
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);		//设置时间
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date); 			//设置日历
#endif
