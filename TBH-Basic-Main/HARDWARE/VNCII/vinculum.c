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
#include "stdlib.h"
#include "defines.h"
#include "pins.h"
#include "usart.h"
#include "vinculum.h"
#include "string.h"
#include "timer.h"
#include "motor.h"
#include "ad7190.h"

extern char buf1[];
extern unsigned int PrinterFrames;
extern unsigned char VincHand;

extern u8 cp_flag;													//页面刷新标志
extern u8 PopWindow;
extern long FileTime;												//U盘导出文件时间变量

bool MartelMode = false;		// 打印机模式：0 台式打印机 1 微型打印机
unsigned char VincHand=0;
int ch;
int zeile;
uint32_t zaehler=0;
char USBbuf[50];

void LCD_ShowSpStr(u16 x,u16 y,u16 width,u16 height,u16 pcolor,u16 bcolor,u8 size,char *p,u8 mode);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
	
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

void VINC_Reset(void)
{
	long i;
	
	VincHand=0;
	
	GPIO_WriteBit(VINC_GPIOD, RTS3, 	Bit_RESET);
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, 	  Bit_SET);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_RESET);	
	osDelay(10);

	GPIO_WriteBit(VINC_GPIOD, VINC_PROG, 	Bit_SET);
	osDelay(150);
	GPIO_WriteBit(VINC_GPIOD, VINC_RESET, 	Bit_SET);
	
	com_baudrate(VINC_UART, VINC_RESET_BAUD, HARDWARE_FLOW_CONTROL);
	osDelay(3500);

	i=0;
	while(com_getchar3(&ch)!=-1 && i++<100)						// return, receive byte via uart3 from Vinculum-II
		osDelay(10);

	com_putchar3('S');											// Switches to the shortened command set (SCS\r)
	com_putchar3('C');											// The SCS command switches the monitor�?? mode to Short Command codes.
	com_putchar3('S');
	com_putchar3(CHAR_CR);									// return
	Vinc_GetPrompt();

	com_putchar3(0x91);											// IPH, switcch to HEX format
	com_putchar3(CHAR_CR);									// return
	Vinc_GetPrompt();

	osDelay(100);
}

void Vinc_Extended(void)	// Switches to the extended command set 切换到扩展命令集
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

void Vinc_MKDFile(char *fname)	// Make a new subdirectory in the current directory 在当前目录中创建一个子目录
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

void Vinc_CDFile(char *fname)	// Change current directory 更改当前目录
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

void Vinc_CD(void)	// Move up one directory level 向上移动一个目录级别
{
	while(com_getchar3(&ch)!=-1);
	com_putchar3(0x02);
	com_putchar3(0x20);
	com_putchar3(0x2E);
	com_putchar3(0x2E);
	com_putchar3(CHAR_CR);
}

int Vinc_DeleteFile(char *fname) // Delete a file 删除文件
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

	com_baudrate(VINC_UART, VINC_BAUD, HARDWARE_FLOW_CONTROL);			//Die Schnittstelle muss f�?? den Programmiermodus mit 115000 Baud eingestellt werden !
	
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

unsigned int Vinc_Query(unsigned int port)	// Display a status of devices connected to a USB port 显示连接到USB端口的设备的状态
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

// Query Device:验证设备编号，识别USB设备类型，autocal工具或者打印机
void Vinc_QD(unsigned int num,char *erg)		
{
	int ch,j,i=0;

	while(com_getchar3(&ch)!=-1)
		osThreadYield();
	for(j=0;j<32;j++)
		erg[j]=0x00;
	com_putchar3(0x85);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);

	for(j=0;j<32;j++)
	{
		i=0;
		while(com_getchar3(&ch)==-1 && i++<50000)
			osDelay(1);
		
		if(i<50000) 
			erg[j]=ch;
	}

	osDelay(10);
	while(com_getchar3(&ch)!=-1)
		osThreadYield();
}

void Vinc_QD_Old(unsigned int num,char *erg)
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
	//增加写入日期		格式09 20 file 20 time 0D	时间分年月日参考VNC2命令�??28�??
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
	
	//if(ret!=10)
	if(ret!=12) 
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

int Vinc_GetPrompt(void)		// 接收命令执行结果
{
	char pbuf[20];
	int i=0;
	
	int ind=0,ch;

	while(com_getchar3(&ch)==-1 && i++<5000)
		osDelay(1);

	if(i>=5000) return(11);
	pbuf[ind]=ch;
	ind++;

	do 
	{
		i = 7200;
		while((com_getchar3(&ch))==-1 && i--)
			osThreadYield();
		
		if(i>0)
		{
			pbuf[ind]=ch;
			if(ind<20) ind++;
		}
	}while(i>0);

	pbuf[ind]=0;

	if(pbuf[0]==0x3E && ind==2) return(0);
	if(pbuf[0]=='B' && pbuf[1]=='C') return(1);	  //Bad Command
	if(pbuf[0]=='C' && pbuf[1]=='F') return(2);		//Command failed
	if(pbuf[0]=='D' && pbuf[1]=='F') return(3);		//Disk full
	if(pbuf[0]=='F' && pbuf[1]=='I') return(4);		//Invalid try openimg a directory
	if(pbuf[0]=='R' && pbuf[1]=='O') return(5);		//Read only
	if(pbuf[0]=='F' && pbuf[1]=='O') return(6);		//File Open
	if(pbuf[0]=='N' && pbuf[1]=='E') return(7);		//Dir not empty
	if(pbuf[0]=='F' && pbuf[1]=='N') return(8);		//Filename invalid
	if(pbuf[0]=='N' && pbuf[1]=='U') return(9);		//No upgrade
	if(pbuf[0]=='E' && pbuf[1]==0x0D && pbuf[2]=='E' && pbuf[3]==0x0D) return(12);		//No upgrade

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

void Vinc_SC(unsigned int num)		// Set Current: 在从设备发送或接收数据之前，指定要使用的设备接口
{
	com_putchar3(0x86);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);
	CheckPrompt();
}

void Vinc_SC_Old(unsigned int num)
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

// 将设备视为FTDI设备
void Vinc_SF(unsigned int num)
{
	com_putchar3(0x87);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);
	CheckPrompt();
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

// 返回参数：返回0，启动打印机失败；返回非0，启动成功；2021-02-03 星期三 17:01:27
int StartPrinter(void)
{
	int ret=0;
	int i;
	char qd_buf[50];
	
	MartelMode = false;
	GPIO_WriteBit(VINC_GPIOD, VINC_DATAREQ, Bit_SET);
	osDelay(100);
	
	for(i=0;i<16;i++)
	{
		Vinc_QD_Old(i,qd_buf);
		if((qd_buf[7] & 0x04) == 0x04 || (qd_buf[7] & 0x10) == 0x10 )		// 打印机类设备
		{
			Vinc_SC_Old(i);
			
			if((qd_buf[7] & 0x10) == 0x10)	// CDC 类设备
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
	
	if(ret && !MartelMode)
	{
		i=GetPrinterStatus();
		PrinterInit();
	}
	
	return(ret);
}

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
			if((str[index] & 0x80) == 0x80 && !PrinterFrames)	// 是否打印扩展ascii字符
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
		SendPrinter("\033%-12345X");
		SendPrinter("\033E");
		SendPrinter("\033E");
		SendPrinter("\033E");
		delay_ms(500);

		SendPrinter("\033E");	// <ESC>E 重置
		SendPrinter("\033&l26A");	// <ESC>&l26A 纸张大小A4
		SendPrinter("\033&a5L");	// <ESC>&a5L Left Margin
		SendPrinter("\033&k10.35H");	// <ESC>&k10.35H  Horizontal Motion Index
	}
}

void PrinterExit(void)
{
	if(!MartelMode)
	{
		SendPrinter("\033&l1T");	// <ESC>&l1T 工作分离
		SendPrinter("\033E");
		SendPrinter("\033%-12345X");
	}
}

//检测U盘--------------------------------------------------
char DetectionUdisk(void)
{
	char tdat = 0;
	
	for(int i=0;i<16;i++)
	{
		Vinc_QD(i,(char *)USBbuf);
		if((USBbuf[7] & 0x20) == 0x20)												
		{
			if(((USBbuf[11]==0x08) && (USBbuf[13]==0x50)))
			{
				tdat = 1;
				i = 16;
			}
		}		
	}
	
	return tdat;	// u盘连接状态 0：未连接 1：已连接
}

char AutocalId[20],dosnr[10];
char Pid2[2];

// 检测u盘模�??
int StartKalibrator(void)
{
	int ret=0,i;
	static char answ[30];
	static unsigned char buf2[50];

	VINC_Reset();

	for(i=0;i<16;i++) 
	{
		Vinc_QD(i,(char *)buf2);
		if((buf2[7] & 0x01) == 0x01)
		{
			if(((buf2[16]==0x0A) && (buf2[17]==0xA1))||((buf2[16]==0x01) && (buf2[17]==0x60)))	 // PID
			{
				Pid2[0] = buf2[16];
				Pid2[1] = buf2[17];
				Vinc_SC(i);
				Vinc_SF(i);
				ret=1;
				break;
			}
		}

		//osThreadYield();
		osDelay(1);
	}
	
	if(ret) 
	{
		GPIO_WriteBit(GPIOD, VINC_DSR, Bit_SET);
		// FBD  设置FTDI设备的波特率：18 20 divisor 0d
		com_put3ww(0x18);						//BaudRate 9600
		com_put3ww(0x20);
		com_put3ww(0x38);
		com_put3ww(0x41);
		com_put3ww(0x00);
		com_put3ww(0x0D);
		CheckPrompt();

		// FSD  设置FTDI设备的数据特征：数据位、奇偶校验位、停止位
		com_put3ww(0x1A);
		com_put3ww(0x20);
		com_put3ww(0x08);
		com_put3ww(0x00);
		com_put3ww(0x0D);
		CheckPrompt();

		// FFC  设置FTDI设备的流控制选项：
		com_put3ww(0x1B);
		com_put3ww(0x20);
		com_put3ww(0x04);
		com_put3ww(0x0D);
		CheckPrompt();

		TIMER_Start(TIMER_GENERAL, TO_30SEC);						// 启动延时定时器
		i=0;
		do
		{			
			SendKalibrator("KNID\r",answ);
			i++;
		} while (strlen(answ) != 21 && i<3);
		
		strncpy(AutocalId,&answ[6],14);

		SendKalibrator("KHDNR\r",answ);	
		strncpy(dosnr,&answ[7],8);	
	}
	
	return(ret);
}

char SendKalibrator(char *str,char *answer)
{
	int i,j,aktsend,index=0;
	int ch;

	VincHand=1;
	
	do
	{
		// DRD  从usb设备读取待处理数据
		com_put3ww(0x84);
		com_put3ww(0x0D);
		aktsend=GetLength();
		for(i=0;i<aktsend;i++)
		{
			j = 50000;

			while((com_getchar3(&ch))==-1 && j--)
				osThreadYield();
		}
		CheckPrompt();
	}while(aktsend>0);

	aktsend=strlen(str);
	while((com_getchar3(&ch))!=-1);

	// DSD  将数据包发送到USB设备
	com_put3ww(0x83);
	com_put3ww(0x20);
	com_put3ww(aktsend);
	com_put3ww(0x0D);
	for(i=0;i<aktsend;i++)
	{
		com_put3ww(str[i]);
	}

	index=0;
	do
	{
		// 此处做超时退出处理		
		if(TIMER_TimeOut(TIMER_GENERAL)) 						// 判断是否超时
		{
			return 1;
		}

		if(CheckPrompt())
		{
			while((com_getchar3(&ch))!=-1);
			
			do
			{
				com_put3ww(0x84);
				com_put3ww(0x0D);
				aktsend=GetLength();
				if(aktsend==0)
				{
					CheckPrompt();
					osDelay(15);
				}
			}while(aktsend==0);
			
			j=0;
			do
			{
				i = 50000;

				while((com_getchar3(&ch))==-1 && i--)
					osThreadYield();
				
				if(i>0)
					answer[index++]=ch;
				
				j++;
			}while(j<aktsend);
		}
	}while(answer[index-1]!=0x0D);
	
	CheckPrompt();
	answer[index]=0;

	return 0;
}

// 获取autocal读数
float GetAutocalVal(float *weight)
{
	float dmsave,ret=0,rsoll=0;
	int forcesave,i;
	char answ[20];

	dmsave=NomiLength;
	forcesave=ForceLimit;

	NomiLength=20.0f;
	ForceLimit=(short)*weight;

	if(SendKalibrator("KNULL\r",answ))	// 去皮
		return 0;	// 超时未连接

	mekabef();

	i=0;
	do 
	{
		if(SendKalibrator("KGETVAL\r",answ))	// 获取硬度
			return 0;	// 超时未连接

		if(strncmp(answ,"KGETVAL",7)==0)
		{
			ret += atof(&answ[8]);
			rsoll += (abs(Get_ADC_AvrgNum(100)-nullwert))/NwtFak;
			i++;
		}
	}while(i<5);
	*weight=rsoll/5;
	
	motor_moveTo(-blops,51200*3);
	do
	{
		step = (int)readMotDat(TMC5160_XACTUAL, &MotSpiStatus);
	}while(step != (-blops));		
	NomiLength=dmsave;
	ForceLimit=forcesave;

	return(ret/5);
}

int GetLength(void)
{
	int i,ch,ch1,ret=0;

	i=50000;

	while((com_getchar3(&ch))==-1 && i--)
		osThreadYield();
	
	if(i>0)
	{
		i = 5000;

		while((com_getchar3(&ch1))==-1 && i--)
			osThreadYield();
		
		if(ch1==0x0D)
			ret=ch;
	}
	
	return(ret);	
}

int CheckPrompt(void)
{
	int i,ch,ret=0;

	i=0;
	while((com_getchar3(&ch))==-1 && i++<50000)
		osThreadYield();
	if(i<50000 && ch==0x3E)
	{
		i=0;
		while((com_getchar3(&ch))==-1 && i++<50000)
			osThreadYield();
		if(ch==0x0D) ret=1;
	}
	return(ret);
}

void PrintBold(void)
{
	SendPrinter("\033(s3B\033&k10.35H");
}

void PrintNorm(void)
{
	SendPrinter("\033(s0B\033&k10.35H");
}

// End -----------------------------------------------------
