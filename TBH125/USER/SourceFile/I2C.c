#include "tbh125.h"

extern float MeasSpeed;
extern float NwtFak;
extern int Force;
extern int Sensibility,terminologie,autodiam;
extern unsigned int MeasureMethod,HardOn,DiamOn,ThickOn,AutoMess,DatenAusgabe;
extern int ForceLimit;
extern float DragJaw;
extern unsigned int MeasurePause;
extern unsigned int LaeEinh;
extern unsigned int HardEinh;
extern unsigned int aktLanguage;
extern unsigned int AutoPrint,ueberlauf;
extern unsigned int blpos,eichpos,PrinterPaper,PrinterLanguage,PrinterFrames,MaxNachzieh,DatFormat;
extern float eichmas,vorbrems;
extern long counth,countd; 
extern unsigned char vers[];
short NullKorr;

void IIC_Stop(void);

/*------------------------------------------------------------------------------
I2C初始化
------------------------------------------------------------------------------*/
void IIC_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							// 普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 设为推挽模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							// 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// IO口最大速度
	
	GPIO_InitStructure.GPIO_Pin = I2C3_SCL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2C3_SDA;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);												//初始化高电平
	GPIO_SetBits(GPIOC,GPIO_Pin_9);												//初始化高电平
	
	// 给一个停止信号, 复位I2C总线上的所有设备到待机模式
	IIC_Stop();
}
/*------------------------------------------------------------------------------
I2C产生起始信号
------------------------------------------------------------------------------*/
void IIC_Start(void)
{
	SDA_OUT();     			//SDA线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	
	delay_us(5);
	
 	IIC_SDA=0;					//START:when CLK is high,DATA change form high to low 
	
	delay_us(5);
	
	IIC_SCL=0;					//钳住I2C总线,准备发送或接收数据
	
	delay_us(5);
}	  
/*------------------------------------------------------------------------------
产生I2C停止信号
------------------------------------------------------------------------------*/
void IIC_Stop(void)
{
	SDA_OUT();					//SDA线输出
	IIC_SCL=0;
	IIC_SDA=0;					//STOP:when CLK is high DATA change form low to high
	delay_us(5);
	IIC_SCL=1; 
	delay_us(5);
	IIC_SDA=1;					//发送I2C总线结束信号
	delay_us(5);
}
/*------------------------------------------------------------------------------
等待应答信号到来
返回值:1,接收应答失败
			 0,接收应答成功
------------------------------------------------------------------------------*/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	IIC_SDA=1;
	SDA_IN();      			//SDA设置为输出
	delay_us(5);
	
	IIC_SCL=1;
	delay_us(5); 
	
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	
	IIC_SCL=0;					//时钟输出为0
	delay_us(5);
	
	return 0;  
} 
/*------------------------------------------------------------------------------
产生ACK应答
------------------------------------------------------------------------------*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}
/*------------------------------------------------------------------------------
不产生ACK应答
------------------------------------------------------------------------------*/    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(5);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}					 				     
/*------------------------------------------------------------------------------
I2C发送一个字节
返回从机有无应答
1,有应答
0,无应答
------------------------------------------------------------------------------*/	  
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	
	SDA_OUT(); 	    
  IIC_SCL=0;									//拉低时钟开始数据传输
	for(t=0;t<8;t++)
  {              
		IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	  
		delay_us(5);   					
		IIC_SCL=1;
		delay_us(5); 
		IIC_SCL=0;	
		delay_us(5);
  }	 
} 	    
/*------------------------------------------------------------------------------
读一个字节,ACK = 1时,发送ACK,ACK = 0,发送nACK
------------------------------------------------------------------------------*/    
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	SDA_IN();													//SDA设置为输出
  for(i=0;i<8;i++ )
	{
		IIC_SCL=0; 
		delay_us(5);
		IIC_SCL=1;
    receive<<=1;
    if(READ_SDA)receive++;   
		delay_us(5);
  }					 
  
  if (!ack)
		IIC_NAck();											//发送nACK
  else
    IIC_Ack(); 											//发送ACK   
  
  return receive;
}

void writeeeprom(unsigned char val, unsigned int address)
{
	tsk_lock();
	IIC_Start();
	IIC_Send_Byte(0XA0);	    				//发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(address>>8);				//发送高地址
	IIC_Wait_Ack();	   
    IIC_Send_Byte(address%256);   	//发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(val);     					//发送字节						   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();												//产生一个停止条件 
	
	//delay_ms(100); 									// 时间延时2.9619mS，2021-07-03 星期六 16:38:44
	//delay_ms(5); 										// 5mS
	delay_ms(20); 										// 原先延时不够增加至20ms
	tsk_unlock();
}


void writeepbuf(unsigned char *val, unsigned int address,unsigned int length)
{
	unsigned int i;
	
	tsk_lock();

	for(i=0;i<length;i++) 
	{
		writeeeprom(*val,address);
		address++;
		val++;
	}

	tsk_unlock();
}


void readeepbuf(unsigned char *val, unsigned int address,unsigned int length) 
{
	unsigned int i;
	
	tsk_lock();
	for(i=0;i<length;i++)
	{
		*val++=readeeprom(address++);
	}
	
	tsk_unlock();
}

unsigned char readeeprom(unsigned int address) 		//读EEPEROM
{
	unsigned char value;
	unsigned int i;
	
	tsk_lock();
	
	IIC_Start();
	
	IIC_Send_Byte(0XA0);	   			//发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(address>>8);		//发送高地址
	IIC_Wait_Ack();
	IIC_Send_Byte(address%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式		   
	IIC_Wait_Ack();	 
    value=IIC_Read_Byte(1);		   
    IIC_Stop();//产生一个停止位   	
	tsk_unlock();
	
	for(i=0;i<800;i++);						
	return(value);
}


void writinteep(unsigned int val,unsigned int adr)
{
	writeepbuf((unsigned char*)&val,adr,2);
}

void writlongeep(unsigned long val,unsigned int adr)
{
	writeepbuf((unsigned char*)&val,adr,4);
}

/***********************************************************************/
/*Integer aus EEPROM auslesen                                          */
/***********************************************************************/
unsigned int readinteep(unsigned int adr)
{
	unsigned int hil=0;
	
	readeepbuf((unsigned char *)&hil,adr,2);
	
	return(hil);
}


/***********************************************************************/
/*long aus EEPROM auslesen                                          */
/***********************************************************************/
unsigned long readlongeep(unsigned int adr)
{
	unsigned long hil=0;

	readeepbuf((unsigned char *)&hil,adr,4);
	return(hil);
}


/*************************************************************************/
/* Float in in EEPROM schreiben                                          */
/*************************************************************************/

void savefloat(float *num,unsigned int adr)
{
	writeepbuf((unsigned char*)num,adr,4);
}

/*************************************************************************/
/* float aus EEPROM laden                                                */
/*************************************************************************/

void loadfloat(float *num,unsigned int adr)
{
	readeepbuf((unsigned char *)num,adr,4);
	if(!isfinite(*num)) *num=0.0;
}

/*************************************************************************/
/* Double in in EEPROM schreiben                                          */
/*************************************************************************/

void savedouble(double *num,unsigned int adr)
{
	writeepbuf((unsigned char*)num,adr,sizeof(double));
}

/*************************************************************************/
/* Double aus EEPROM laden                                                */
/*************************************************************************/

void loaddouble(double *num,unsigned int adr)
{
	readeepbuf((unsigned char *)num,adr,sizeof(double));
	if(!isfinite(*num)) *num=0.0;
}



void InitEEPROM(unsigned char mode)
{
	int i;
	char ccd[8];
	DTIM dat;

	srand(123);
	i=rand();

	aktLanguage=0;
	writinteep(aktLanguage,ADRLANGUAGE);
	MeasureMethod=0;
	writinteep(MeasureMethod,ADRMEASUREMETHOD);
	terminologie=1;
	writinteep(terminologie,ADRTERMINOLOGIE);
	autodiam=1;
	writinteep(autodiam,ADRAUTODIAM);
	HardOn=1;
	writinteep(HardOn,ADRHARDON);
	DiamOn=1;
	writinteep(DiamOn,ADRDIAMON);
	ThickOn=1;
	writinteep(ThickOn,ADRTHICKON);
	AutoMess=0;
	writinteep(AutoMess,ADRAUTOMESS);
  DatenAusgabe=0;
  writinteep(DatenAusgabe,ADRDATENAUSGABE);
  Sensibility=10;
	writinteep(Sensibility,ADRSENSIBILITY);
  MeasSpeed=2.3;
	savefloat(&MeasSpeed,ADRMEASSPEED);
  Force=20;
	writinteep(Force,ADRFORCE);
	if(vers[49]==0x31)     //#ifdef GRMOT
    ForceLimit=FORCELIMITMAXGMOT;
	else
    ForceLimit=FORCELIMITMAXKMOT;
	writinteep(ForceLimit,ADRFORCELIMIT);
  DragJaw=0;
	savefloat(&DragJaw,ADRDRAGJAW);
	MeasurePause=0;
	writinteep(MeasurePause,ADRMEASUREPAUSE);
	HardEinh=0;
	writinteep(HardEinh,ADRHEINH);
	LaeEinh=0;
	writinteep(HardEinh,ADRLEINH);
	if(mode==0) {
		strcpy(ccd,"000000");
		writeepbuf((unsigned char*)ccd,ADRCCD,7);
	}
	NwtFak=19.0;
	savefloat(&NwtFak,ADRNWTFAK);
	AutoPrint=0;
	writinteep(AutoPrint,ADRAUTOPRINT);
	blpos=26000;
	writinteep(blpos,ADRBLPOS);
	eichmas=10.0;
	savefloat(&eichmas,ADREICHMAS);		
	vorbrems=0.5;
	savefloat(&vorbrems,ADRVORBREMS);
	eichpos=0;
	writinteep(eichpos,ADREICHPOS);

	DatFormat=0;
	writinteep(DatFormat,ADRDATFORMAT);

	PrinterPaper=0;
	writinteep(PrinterPaper,ADRPRINTERPAPER);
	PrinterLanguage=0;
	writinteep(PrinterLanguage,ADRPRINTERLANGUAGE);
	PrinterFrames=1;
	writinteep(PrinterFrames,ADRPRINTERFRAMES);
	NullKorr=0;
	writinteep(NullKorr,ADRNULLKORR);

/*	strcpy(ccd,"123456");
	for(i=0;i<6;i++) {
		writeeeprom(ccd[i],ADRSERNUM+i);
	}*/
	dat.day=1;
	dat.month=1;
	dat.year=2009;
	dat.hour=0;
	dat.min=0;
	dat.sec=0;
	WriteDate(&dat,ADRKALDDAT);
	WriteDate(&dat,ADRKALBDAT);
	WriteDate(&dat,ADRKALDDATO);
	WriteDate(&dat,ADRKALBDATO);
	WriteDate(&dat,ADRVALIDAT);

	countd=0;
	writlongeep(countd,ADRCOUNTD);
	counth=0;
	writlongeep(counth,ADRCOUNTH);
	i=0;
  writinteep(i,ADRLBIKAL);
	writinteep(i,ADRLBKAL);
	writinteep(i,ADRLBIKALO);
	writinteep(i,ADRLBKALO);  		
	ueberlauf=40;
	writinteep(ueberlauf,ADRUEBERLAUF);

	// ????,Gavin,2020-03-14 ??? 16:43:34
	writeeeprom(SIGNATUR,ADRSIGNATUR);
}

void LoadDate(DTIM *dt,unsigned int adr)		
{
	readeepbuf((unsigned char *)dt,adr,sizeof(DTIM));
	if(dt->day>31 || dt->month>12) {
		dt->day=1;
		dt->month=1;
		dt->year=2010;
	}
}

void WriteDate(DTIM *dt,unsigned int adr)
{
	writeepbuf((unsigned char*)dt,adr,sizeof(DTIM));
}

void LoadKalID(char *dt,unsigned int adr)		
{
	readeepbuf((unsigned char *)dt,adr,14);
	dt[14]=0;
}

void WriteKalID(char *dt,unsigned int adr)
{
	writeepbuf((unsigned char*)dt,adr,14);
	dt[14]=0;
}

void LoadDosID(char *dt,unsigned int adr)		
{
	int i;

	readeepbuf((unsigned char *)dt,adr,8);
	for(i=0;i<8;i++) {
		if(dt[i]<0x30 || dt[i]>0x39) dt[i]=0x30;
	}
	dt[8]=0;
}

void WriteDosID(char *dt,unsigned int adr)
{
	writeepbuf((unsigned char*)dt,adr,8);
}

/*
void SDA_IN(void)					//SDA方向设置为输入
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = I2C3_SDA;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void SDA_OUT(void)				//SDA方向设置为输出
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = I2C3_SDA;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}
*/

//模拟I2C所需的两个延迟函数
void I2C_delay_us(u32 nus)					//约5.1us
{		
	u16 i = 0;
	while(nus--)
	{
		i = 30;
		while(i--);
	}

}

void I2C_delay_ms(u16 nms)					//约5ms
{	 	
		u16 i  = 0;
		while(nms--)
		{
			i = 1000;
			while(i--);
		}
}
