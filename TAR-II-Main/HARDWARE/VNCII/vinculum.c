/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    vinculum.c
 *      Purpose: Vinculum Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
//#include "stm32f10x_conf.h"   							// Device header
#include "stm32f4xx_conf.h"
#include "defines.h"
#include "pins.h"
#include "usart.h"
#include "vinculum.h"
#include "string.h"
#include "timer.h"

extern unsigned char Com2Verg;
extern unsigned int rsstatus2;
extern char buf1[];
extern unsigned int PrinterPort,PrinterPaper,PrinterLanguage,PrinterFrames;
extern unsigned char VincHand;

extern uint8_t CurntPage; 									// 页码
//extern u8 LastUdiskFlag;										//上一次U盘链接状态
extern u8 UdiskFlag;												// U盘链接标志,0表示没有插上到U盘,1表示插上
extern u8 cp_flag;													//页面刷新标志
extern long FileTime;												//U盘导出文件时间变量

bool MartelMode = false;
unsigned char VincHand=0;
int ch;
int zeile;
uint32_t zaehler=0;
char USBbuf[50];

void Vinc_Vers(char *erg);
void Vinc_GetBuf(char *buff); 	
int  Vinc_GetPrompt(void);
void VINC_Reset(void);
void PrinterInit(void);
void PrinterExit(void);
	
/*******************************************************************************
* Function Name  : VINC_Init
* Description    : init the Vinculum GPIO
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void VINC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

//start: f黵 test, wenn vinculum mit rs232 parallel zu STM32 programmiert (angeschlossen) werden soll, wieder einbauen
// VINC GPIO Configuration
//	GPIO_InitStructure.GPIO_Pin   = VINC_DATACK;	//Data Ack from vinculum
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(VINC_GPIOB, &GPIO_InitStructure);
//	
//  GPIO_InitStructure.GPIO_Pin   = VINC_DATAREQ;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(VINC_GPIOB, &GPIO_InitStructure);	

//  GPIO_InitStructure.GPIO_Pin   = VINC_PROG | VINC_RESET;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(VINC_GPIOC, &GPIO_InitStructure);	
//ende: wenn vinculum mit rs232 parallel zu STM32 programmiert werden soll, wieder rein


	// VINC GPIO Configuration
	GPIO_InitStructure.GPIO_Pin   = VINC_DATACK; 							// Data Ack from vinculum
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN; 						// GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 						// 上拉
  GPIO_Init(VINC_GPIOD, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin   = VINC_DATAREQ;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT; 						// GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 						// 上拉
  GPIO_Init(VINC_GPIOD, &GPIO_InitStructure);	

  GPIO_InitStructure.GPIO_Pin   = VINC_PROG | VINC_RESET;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT; 						// GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 						// 上拉
  GPIO_Init(VINC_GPIOD, &GPIO_InitStructure);	

	VINC_Reset();
}



// The VNC1L monitor works in command mode or data mode in a similar way to a modem. Command mode is used to communicate with the VNC1L. Data mode is used to communicate 
// with a slave device on USB Port 1 or 2
// The FTDI Commands are available in all Firmware codes except VDFC Firmware
// Die VDAP Firmware muss geladen sein !
void VINC_Reset(void)
{	
	long i;
	
	VincHand=0;
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_SET);
	osDelay(50);
	
	GPIO_WriteBit(VINC_GPIOD, RTS3, 	Bit_RESET);
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, 	  Bit_SET);
	GPIO_WriteBit(VINC_GPIOD, VINC_PROG, 	Bit_SET);
//	GPIO_WriteBit(VINC_GPIOC, VINC_PROG, 	Bit_RESET);										//f黵 test, wenn vinculum mit rs232 parallel zu STM32 programmiert (angeschlossen) werden soll, wieder einbauen
	osDelay(150);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_RESET);
	osDelay(100);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_SET);
	com_baudrate(VINC_UART, VINC_RESET_BAUD, HARDWARE_FLOW_CONTROL);
	osDelay(100);

	while(com_getchar3(&ch)!=-1);						// return, receive byte via uart3 from Vinculum-II

	com_putchar3('S');											// Switches to the shortened command set (SCS\r)
	com_putchar3('C');											// The SCS command switches the monitor抯 mode to Short Command codes.
	com_putchar3('S');
	com_putchar3(CHAR_CR);									// return
	i=0;

	TIMER_Start(TIMER_Vinculum, TO_100MSEC);
	do{	
		osThreadYield();
	}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

  while(com_getchar3(&ch)!=-1);							// return

	com_putchar3(0x91);											// IPH, switcch to HEX format
	com_putchar3(CHAR_CR);									// return
	i=0;
	while(com_getchar3(&ch)==-1 && i++<150000);// ">" Promt
	while(com_getchar3(&ch)!=-1);							// return

	osDelay(100);
}

void Vinc_Extended(void)
{
	unsigned int i;

	com_putchar3(0x90);
	com_putchar3(CHAR_CR);
	i=0;
	while(com_getchar3(&ch)==-1 && i++<5000);
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x11);
	com_putchar3(CHAR_CR);
	i=0;
	while(com_getchar3(&ch)==-1 && i++<5000);
	while(com_getchar3(&ch)!=-1);
}

int Vinc_TestStick(void)
{
	while(com_getchar3(&ch)!=-1);
	com_putchar3(CHAR_CR);
	Vinc_GetBuf(buf1);
	
	if(buf1[0]==0x3E && buf1[1]==CHAR_CR)
		return(1);
	
	return(0);
}	

int Vinc_TestFile(char *fname)
{
	while(com_getchar3(&ch)!=-1);

	com_putchar3(0x01);
	com_putchar3(0x20);
	while(*fname)
	{
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(CHAR_CR);

	Vinc_GetBuf(buf1);
	
	if(buf1[0]==CHAR_CR && buf1[1]==0x43 && buf1[2]==0x46 && buf1[3]==CHAR_CR)
		return(0);
	
	return(1);
}	

void Vinc_MKDFile(char *fname)
{
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x06);
	com_putchar3(0x20);
	while(*fname)
	{
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(CHAR_CR);
}

void Vinc_CDFile(char *fname)
{
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x02);
	com_putchar3(0x20);
	while(*fname)
	{
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(CHAR_CR);
}

void Vinc_CD(void)
{
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x02);
	com_putchar3(0x20);
	com_putchar3(0x2E);
	com_putchar3(0x2E);
	com_putchar3(CHAR_CR);
}

int Vinc_DeleteFile(char *fname)
{
	while(com_getchar3(&ch)!=-1);

	com_putchar3(0x07);
	com_putchar3(0x20);
	while(*fname)
	{
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(CHAR_CR);

	Vinc_GetBuf(buf1);
	
	if(buf1[0]==0x3E && buf1[1]==CHAR_CR)
		return(1);
	
	return(0);
}	

void Vinc_GetBuf(char *buff) 	
{
	int ch,j;
	long i=0;

	for(i=0;i<50;i++)
		buff[i]=0x00;

	while(com_getchar3(&ch)==-1 && i++<200000);
	if(i<=200000)
	{
		buff[0]=ch;
		j=1;
		do
		{
			i=0;
			while(com_getchar3(&ch)==-1 && i++<100000);
			if(i<100000 && j<50)
				buff[j++]=ch;
		}while(i<100000 && j<50);
	}
	else
		strcpy(buff,"TO\r");
}		

void Vinc_Send(char *bef)
{
	unsigned int i;

	VincHand=1;
	com_bufclr3();
	GPIO_WriteBit(VINC_GPIOD, RTS3, Bit_RESET);
	for(i=0;i<strlen(bef);i++)
		com_put3ww(bef[i]);
	com_putchar3(CHAR_CR);
	VincHand=0;
}
/*******************************************************************************
* Function Name  : Vinc_Prog_Block
* Description    : Programmiert Vinculum chip Flash
* Input          : int block, unsigned char *buf, unsigned int size
* Output         : None
* Return         : int
*******************************************************************************/
int Vinc_Prog_Block(int block, unsigned char *buf, unsigned int size)
{
	signed char endianBuf[PAGESIZE];  					// Data read from the ROM file reversed to accommodate VNC2 endianess
	signed char receiveBuf[PAGESIZE]; 					// Data received from the VNC2
	
	long numwriten;
	int subblocks;
	int i;
	int j = 0;
	unsigned int adr;
	int ch;
	
	adr=block*size;
	subblocks=size/PAGESIZE;

	for(i=0;i<subblocks;i++)
	{
		memset(endianBuf, 0xFF, PAGESIZE);														// Memset the data buffer...
		
		for (j=0; j < PAGESIZE; j += 2)																// Change the data to little endian for VNC2.
		{
			endianBuf[j] 	 = buf[j+1+(i*PAGESIZE)];
			endianBuf[j+1] = buf[j+(i*PAGESIZE)];
		}
		
		osDelay(10);//10

		com_putchar3(0x03);																						// WRITE FORMAT 0x03
		com_putchar3((adr/PAGESIZE)&0xFF);														// Send the FlashAddressLow data address....
		com_putchar3(((adr/PAGESIZE)>>8)&0xFF);												// Send the FlashAddressHigh data address....
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do
		{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02)
				break;
		}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		
		if ((ch & 0x000000FF) != 0x02)
			return(6);											// Check for a response data address
			
		
		for(numwriten=0;numwriten<PAGESIZE;numwriten++)
		{							// Send the data
			com_putchar3(endianBuf[numwriten]);
		}
		
	  osDelay(100);//100
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do
		{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02)
				break;
		}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		
		if ((ch & 0x000000FF) != 0x02)
			return(7);										// Check for a response data has been send
			
		com_bufclr3();
		ch=0xFF;
																																	// Read the data back to make sure that it has been programmed correctly.		
		com_putchar3(0x02);																						// cmd READ FORMAT 0x02
		com_putchar3((adr/PAGESIZE)&0xFF);														// Send the FlashAddressLow data address....
		com_putchar3(((adr/PAGESIZE)>>8)&0xFF);												// Send the FlashAddressHigh data address....		

		osDelay(10);//10
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do
		{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02)
				break;
		}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		
		if ((ch & 0x000000FF) != 0x02)
			return(8);											// Check for a response data address	
		
		if(numwriten < PAGESIZE)
			return -1;														// Make sure that we have 128 bytes to read from the chip!!!
	
																																	// Read 128 bytes back from the chip...
		for(numwriten=0;numwriten<PAGESIZE;numwriten++)
		{							// read the data
			TIMER_Start(TIMER_Vinculum, TO_100MSEC);
			do
			{	
				osThreadYield();
			}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
			
			receiveBuf[numwriten] = (	signed char )ch;
		}
		
		// Compare the data read back from the flash with the data sent....
		// NOTE: The data that is returned is reversed due to the endianess of the VNC2,
		// compare the returned data with the data that we reversed to send to the chip.
		for(j = 0 ; j < PAGESIZE; j++)
		{
			zaehler++;
			if(endianBuf[j] != receiveBuf[j])
			{
				return -1;
			}
		}
		
		adr+=PAGESIZE;
//		if (zaehler >= 210*1024)																		// test um bei letztem Packet anzuhalten
//			__NOP; 																									  //
	}
	
	return(0);
}

/*******************************************************************************
* Function Name  : Vinc_StartProg
* Description    : leitet Programmierung Vinculum chip ein
* Input          : None
* Output         : None
* Return         : unsigned int
*******************************************************************************/
unsigned int Vinc_StartProg(void)
{
	int ch;
	VincHand=1;

	com_baudrate(VINC_UART, VINC_BAUD, HARDWARE_FLOW_CONTROL);			//Die Schnittstelle muss f黵 den Programmiermodus mit 115000 Baud eingestellt werden !
	
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_SET);
	osDelay(50);
	GPIO_WriteBit(VINC_GPIOD, RTS3, 		Bit_RESET);
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, 	  Bit_SET);
	osDelay(150);
	GPIO_WriteBit(VINC_GPIOD, VINC_PROG, 		Bit_RESET);
	osDelay(50);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_RESET);
	osDelay(100);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_SET);
  osDelay(5000);
	
	com_bufclr3();
	
	ch = com_putchar3(0xFF);
	TIMER_Start(TIMER_Vinculum, TO_1SEC);
	do{	
	}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0xFF) return(1);
	
	ch = com_putchar3(0xFB);
	TIMER_Start(TIMER_Vinculum, TO_100MSEC);
	do{	
	}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0xFB) return(2);
	osDelay(100);
	
	com_putchar3(0x01);//Set Data Variable
	com_putchar3(0x70);//TimerLow = 0x70, This command allows the Flash program time and Baud rate to be specified
	com_putchar3(0x17);//TimerHigh = 0x17
	com_putchar3(0x1A);//Baud1, 115200
	com_putchar3(0x00);//Baud2, 115200
	com_putchar3(0x00);//Baud3, 115200
  osDelay(50);
	
	TIMER_Start(TIMER_Vinculum, TO_1SEC);
	do{	
	}while(com_getchar3(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0x02) return(3);
	
	com_baudrate(VINC_UART, VINC_BAUD, HARDWARE_FLOW_CONTROL); 

 	return(0);
}

unsigned int Vinc_Query(unsigned int port)
{
	int i,ch;

	if(port==1)
		com_putchar3(0x2B);
	else 
		com_putchar3(0x2C);
	com_putchar3(CHAR_CR);
	i=0;
	while(com_getchar3(&ch)==-1 && i++<5000);
	while(com_getchar3(&ch)!=-1);
	return(ch);
}


void Vinc_QD(unsigned int num,char *erg)
{
	int ch,j,i=0;

	while(com_getchar3(&ch)!=-1);
	for(j=0;j<32;j++)
		erg[j]=0x00;
	com_putchar3(0x85);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(CHAR_CR);
	for(j=0;j<32;j++)
	{
		i=0;
		while(com_getchar3(&ch)==-1 && i++<50000)
			osThreadYield();
		
		if(i<50000) 
			erg[j]=ch;
	}
	osDelay(10);
	while(com_getchar3(&ch)!=-1);
}

void Vinc_Vers(char *erg)
{
	int ch,j,i=0;

	while(com_getchar3(&ch)!=-1);
	for(j=0;j<32;j++)
		erg[i]=0x00;
	com_putchar3(0x13);
	com_putchar3(CHAR_CR);
	j=0;
	do
	{
		i=0;
		while(com_getchar3(&ch)==-1 && i++<50000);
		if(i<50000) 
			erg[j++]=ch;
	}while(j<32 && i<50000);
	
	erg[j]=0;
	osDelay(10);
	
	while(com_getchar3(&ch)!=-1);
}

void Vinc_Dir(char *erg)
{
	int ch,j,i=0;

	while(com_getchar3(&ch)!=-1);
	for(j=0;j<50;j++) erg[i]=0x00;
	com_putchar3(0x01);
	com_putchar3(CHAR_CR);
	j=0;
	do
	{
		i=0;
		while(com_getchar3(&ch)==-1 && i++<15000);
		if(i<15000) 
			erg[j++]=ch;
	}while(j<50 && i<15000);
	erg[j]=0;
	osDelay(10);
	while(com_getchar3(&ch)!=-1);
}


void Vinc_ReadFile(char *nam,char *buf,int maxbuf)
{
	int ch,j,i=0;

	VincHand=1;

	while(com_getchar3(&ch)!=-1);
	com_put3ww(0x04);
	com_put3ww(0x20);
	while(*nam)
	{
		com_put3ww(*nam);
		nam++;
	}
	com_put3ww(CHAR_CR);
	j=0;
	do
	{
		i=0;
		while(com_getchar3(&ch)==-1 && i++<15000);
		if(i<15000)
		{ 
			buf[j]=ch;
			j++;
		}
	}while(j<maxbuf && i<15000);
	VincHand=0;
}	

int Vinc_OpenFile(char *nam)
{
	int ret;

	VincHand=1;

	while(com_getchar3(&ch)!=-1);
	com_put3ww(0x09);
	com_put3ww(0x20);
	while(*nam)
	{
		com_put3ww(*nam);
		nam++;
	}
	//增加写入日期		格式09 20 file 20 time 0D	时间分年月日参考VNC2命令集28页
	com_put3ww(0x20);
	com_put3ww(FileTime>>24);							
	com_put3ww((FileTime&0x00FF0000)>>16);   
	com_put3ww((FileTime&0x0000FF00)>>8); 
	com_put3ww(FileTime&0x000000FF); 
	
	com_put3ww(CHAR_CR);
	ret=Vinc_GetPrompt();
	VincHand=0;
	return(ret);
}	

int Vinc_TestAlive(void)
{
	int ret,ch;
	
	while(com_getchar3(&ch)!=-1);
	com_put3ww('E'); 				// 0x45
	com_put3ww(0x0D);
	ret=Vinc_GetPrompt();
	
	//if(ret!=12) 
	if(ret!=10)
		VINC_Reset();
	else 
		return(1);
	
	while(com_getchar3(&ch)!=-1);
	com_put3ww('E');
	com_put3ww(0x0D);
	ret=Vinc_GetPrompt();
	if(ret==12) 
		return(1);	
	
	return(0);
}

int Vinc_GetPrompt(void)
{
	char pbuf[20];
	unsigned long i=0;
	
	int ind=0,ch;

	while(com_getchar3(&ch)==-1 && i++<300000) osThreadYield();

	if(i>=300000) return(11);
	pbuf[ind]=ch;
	ind++;
	while(com_getchar3(&ch)!=-1)
	{
		pbuf[ind]=ch;
		if(ind<20) ind++;
	}
	pbuf[ind]=0;
	if(pbuf[0]==0x3E && ind==1) return(0);
	if(pbuf[0]=='B' && pbuf[1]=='C') return(1);	  //Bad Command
	if(pbuf[0]=='C' && pbuf[1]=='F') return(2);		//Command failed
	if(pbuf[0]=='D' && pbuf[1]=='F') return(3);		//Disk full
	if(pbuf[0]=='F' && pbuf[1]=='I') return(4);		//Invalid try openimg a directory
	if(pbuf[0]=='R' && pbuf[1]=='O') return(5);		//Read only
	if(pbuf[0]=='F' && pbuf[1]=='O') return(6);		//File Open
	if(pbuf[0]=='N' && pbuf[1]=='E') return(7);		//Dir not empty
	if(pbuf[0]=='F' && pbuf[1]=='N') return(8);		//Filename invalid
	if(pbuf[0]=='N' && pbuf[1]=='U') return(9);		//No upgrade
	return(10);																		//unknown error
}

int Vinc_CloseFile(char *nam)
{
	int ret=0;

	VincHand=1;

	while(com_getchar3(&ch)!=-1);
	com_put3ww(0x0A);
	com_put3ww(0x20);
	while(*nam)
	{
		com_put3ww(*nam);
		nam++;
	}
	com_put3ww(CHAR_CR);
	VincHand=0;
	return(ret);
}	

int Vinc_WriteFile(int num,char *buf)
{
	int i;
	VincHand=1;

	while(com_getchar3(&ch)!=-1);
	com_put3ww(0x08);
	com_put3ww(0x20);
	com_put3ww(0x00);   
	com_put3ww(0x00);   
	com_put3ww(num>>8);
	com_put3ww(num & 0x00FF);
	com_put3ww(CHAR_CR);

	for(i=0;i<num;i++)
	{
		com_put3ww(buf[i]);
	}
	
	i=Vinc_GetPrompt();
	VincHand=0;
	return(i);
}	

void Vinc_SC(unsigned int num)
{
 	int i;

	com_putchar3(0x86);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(CHAR_CR);
	i=0;
	while(com_getchar3(&ch)==-1 && i++<15000);
	while(com_getchar3(&ch)!=-1);
}

unsigned char GetPrinterStatus(void)
{
 	int i;
	int ch;
	
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x81);
	com_putchar3(CHAR_CR);
	i=0;
	while(com_getchar3(&ch)==-1 && i++<5000);
	while(com_getchar3(&ch)!=-1);
	return(ch);
}

int SetDataMode(void)
{
	int i=0;
	
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, Bit_RESET);
	
	while(GPIO_ReadInputDataBit(VINC_GPIOD, VINC_DATACK)!=0 && i++<100)
		osDelay(1);
	
	if(i>=100)
		return(0);
	
	return(1);
}

///*
// 返回参数：返回0，启动打印机失败；返回非0，启动成功；2021-02-03 星期三 17:01:27
int StartPrinter(void)
{
	int ret=0;
	int i;
	char qd_buf[50];
	
	MartelMode = false;
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, Bit_SET);
	osDelay(100);
	
	//if(!Vinc_TestAlive()) 
	//	return(0);
	
	for(i=0;i<16;i++)
	{
		Vinc_QD(i,qd_buf);
		if((qd_buf[7] & 0x04) == 0x04 || (qd_buf[7] & 0x10) == 0x10 )
		{
			Vinc_SC(i);
			
			if((qd_buf[7] & 0x10) == 0x10)
			{
				MartelMode = true;
				ret = SetDataMode();
			}
			else
				ret = 1;
			
			break;
		}
		
		osDelay(100);
	}
	
	//ret = 1;
	if(ret && !MartelMode)
	{
		i=GetPrinterStatus();
		PrinterInit();
	}
	
	return(ret);
}
//*/

void StopPrinter(void)
{
	PrinterExit();
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, Bit_SET);	
}

void SendPrinter(char *str)
{
	int ch,i,aktsend,index=0;

	VincHand=1;

	aktsend=strlen(str);
	while(aktsend>32)
	{
		if(!MartelMode)
		{
			com_put3ww(0x83);
			com_put3ww(0x20);
			com_put3ww(32);
			
			com_put3ww(CHAR_CR);
		}
		
		for(i=0;i<32;i++)
		{
			if(str[index]==0x0A) zeile++;
			//if((str[index] & 0x80) == 0x80 && !PrinterFrames && str[index]!='?' && str[index]!='?')
			if((str[index] & 0x80) == 0x80 && !PrinterFrames && str[index]!='\x9A' && str[index]!='\x81')
				com_put3ww(' ');
			else
				com_put3ww(str[index]);
			index++;
			osThreadYield();
		}
		
		if(!MartelMode)
		{
			i=0;
			while(com_getchar3(&ch)==-1 && i++<50000)
				osThreadYield();
			while(com_getchar3(&ch)!=-1)
				osThreadYield();
		}
		else
			osDelay(500);
		
		aktsend-=32;
		osDelay(100);
	}
	
	if(aktsend>0)
	{
		if(!MartelMode)
		{
			com_put3ww(0x83);
			com_put3ww(0x20);
			
			com_put3ww(aktsend);
			com_put3ww(CHAR_CR);
		}
		
		for(i=0;i<aktsend;i++)
		{
			if(str[index]==0x0A) zeile++;
			if((str[index] & 0x80) == 0x80 && !PrinterFrames)
				com_put3ww(' ');
			else
				com_put3ww(str[index]);
			index++;
			osThreadYield();
		}
		
		if(!MartelMode)
		{
			i=0;
			while(com_getchar3(&ch)==-1 && i++<50000)
				osThreadYield();
			while(com_getchar3(&ch)!=-1)
				osThreadYield();
		}
		else
			osDelay(500);
	}
	
	VincHand=0;
}
 
void SendPrinterCC(const char *str)
{
	int ch,i,aktsend,index=0;

	VincHand=1;

	aktsend=strlen(str);
	while(aktsend>32)
	{
		if(!MartelMode)
		{
			com_put3ww(0x83);
			com_put3ww(0x20);
			com_put3ww(32);
			
			com_put3ww(CHAR_CR);
		}
		
		for(i=0;i<32;i++)
		{
			if(str[index]==0x0A)
				zeile++;
			
			if((str[index] & 0x80) == 0x80 && !PrinterFrames)
				com_put3ww(' ');
			else
				com_put3ww(str[index]);
			
			index++;
			osThreadYield();	
		}
		
		if(!MartelMode)
		{
			i=0;
			while(com_getchar3(&ch)==-1 && i++<50000)
				osThreadYield();
			
			while(com_getchar3(&ch)!=-1)
				osThreadYield();
		}
		else
			osDelay(500);
		
		aktsend-=32;
		osDelay(100);
	}
	
	if(aktsend>0)
	{
		if(!MartelMode)
		{
			com_put3ww(0x83);
			com_put3ww(0x20);
			com_put3ww(aktsend);
			com_put3ww(CHAR_CR);
		}
		
		for(i=0;i<aktsend;i++)
		{
			if(str[index]==0x0A) zeile++;
			if((str[index] & 0x80) == 0x80 && !PrinterFrames)
				com_put3ww(' ');
			else
				com_put3ww(str[index]);
			index++;
			osThreadYield();
		}
		
		if(!MartelMode)
		{
			i=0;
			while(com_getchar3(&ch)==-1 && i++<50000)
				osThreadYield();
			
			while(com_getchar3(&ch)!=-1)
				osThreadYield();
		}
		else
			osDelay(500);
	}
	
	VincHand=0;
}
 
void PrinterInit(void)
{
	if(!MartelMode)
	{
		SendPrinter("\033E\033&l26A");
		SendPrinter("\033&a5L\033&k10.35H");
	}
}

void PrinterExit(void)
{
	if(!MartelMode)
		SendPrinter("\033&l1T\033E");
}

//检测U盘--------------------------------------------------	2022/1/5 星期三 16:09:43
void DetectionUdisk(void)
{
	u8 tdat = 0; 													// 预设U盘连接状态为断开
	
	/*if(CurntPage == 0)																					//只有在首页的时候回去扫描U盘
	{
		for(int i=0;i<16;i++)
		{
			Vinc_QD(i,(char *)USBbuf);
			if((USBbuf[7] & 0x20) == 0x20)												
			{	
				if(((USBbuf[11]==0x08) && (USBbuf[13]==0x50)))
				{	
					if(UdiskFlag==1)																		//上一次是插上的,这一次还是插上的
						return;
					else
					{
						cp_flag =1;																				//刷新页面
						UdiskFlag = 1;																		//表示U盘已经连接上
						LastUdiskFlag = 1;																//记录
						return;
					}
				}	
			}		
		}
		
		if(UdiskFlag==1)																					//上一次是插上的，现在拔了
		{
			UdiskFlag = 0;																					//表示U盘已经断开
			cp_flag =1;																							//刷新页面
		}
		else
			UdiskFlag = 0;																					//表示U盘已经断开
	}*/
	
	for(int i=0;i<16;i++)
	{
		Vinc_QD(i,(char *)USBbuf);
		if((USBbuf[7] & 0x20) == 0x20)												
		{
			if(((USBbuf[11]==0x08) && (USBbuf[13]==0x50)))
			{
				tdat = 1; 											// U盘连接状态
				i = 16; 												// 退出循环
			}	
		}		
	}
	
	if(UdiskFlag != tdat)
	{
		UdiskFlag = tdat;
		cp_flag =1;													//刷新页面
	}
}

// End -----------------------------------------------------
