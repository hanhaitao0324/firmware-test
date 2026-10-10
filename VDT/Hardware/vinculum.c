/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    vinculum.c
 *      Purpose: Vinculum Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"   							// Device header
#include "defines.h"
#include "pins.h"
#include "uart.h"
#include "vinculum.h"
#include "string.h"
#include "timer.h"

extern unsigned char Com2Verg;
extern unsigned int rsstatus2;
extern char buf1[];
extern unsigned int PrinterPort,PrinterPaper,PrinterLanguage,PrinterFrames;
extern unsigned char VincHand;

bool MartelMode = false;
unsigned char VincHand=0;
int ch;
int zeile;
uint32_t zaehler=0;

void com_bufclr2(void);
void Vinc_Vers(char *erg);
void Vinc_GetBuf(char *buff); 	
int  Vinc_GetPrompt(void);
void VINC_Reset(void);
void com_put2ww(int c);
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
	GPIO_InitStructure.GPIO_Pin   = VINC_DATACK;	//Data Ack from vinculum
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VINC_GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin   = VINC_DATAREQ;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(VINC_GPIOB, &GPIO_InitStructure);	

  GPIO_InitStructure.GPIO_Pin   = VINC_PROG | VINC_RESET;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(VINC_GPIOC, &GPIO_InitStructure);	

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
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_SET);
	osDelay(50);
	GPIO_WriteBit(VINC_GPIOB, RTS3, 	Bit_RESET);
	GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, 	  Bit_SET);
	GPIO_WriteBit(VINC_GPIOC, VINC_PROG, 	Bit_SET);
//	GPIO_WriteBit(VINC_GPIOC, VINC_PROG, 	Bit_RESET);										//f黵 test, wenn vinculum mit rs232 parallel zu STM32 programmiert (angeschlossen) werden soll, wieder einbauen
	osDelay(150);
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_RESET);
	osDelay(100);
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_SET);
	com_baudrate(VINC_UART, VINC_RESET_BAUD, HARDWARE_FLOW_CONTROL);
	osDelay(100);

	while(com_getchar2(&ch)!=-1);						// return, receive byte via uart3 from Vinculum-II

	com_putchar2('S');											// Switches to the shortened command set (SCS\r)
	com_putchar2('C');											// The SCS command switches the monitor抯 mode to Short Command codes.
	com_putchar2('S');
	com_putchar2(CHAR_CR);									// return
	i=0;

	TIMER_Start(TIMER_Vinculum, TO_100MSEC);
	do{	
		osThreadYield();
	}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

  while(com_getchar2(&ch)!=-1);							// return

	com_putchar2(0x91);											// IPH, switcch to HEX format
	com_putchar2(CHAR_CR);									// return
	i=0;
	while(com_getchar2(&ch)==-1 && i++<150000);// ">" Promt
	while(com_getchar2(&ch)!=-1);							// return

	osDelay(100);
}

void Vinc_Extended(void)
{
	unsigned int i;

	com_putchar2(0x90);
	com_putchar2(CHAR_CR);
	i=0;
	while(com_getchar2(&ch)==-1 && i++<5000);
	while(com_getchar2(&ch)!=-1);
	com_putchar2(0x11);
	com_putchar2(CHAR_CR);
	i=0;
	while(com_getchar2(&ch)==-1 && i++<5000);
	while(com_getchar2(&ch)!=-1);
}

int Vinc_TestStick(void)
{
	while(com_getchar2(&ch)!=-1);
	com_putchar2(CHAR_CR);
	Vinc_GetBuf(buf1);
   	if(buf1[0]==0x3E && buf1[1]==CHAR_CR) return(1);
	return(0);
}	

int Vinc_TestFile(char *fname)
{
	while(com_getchar2(&ch)!=-1);

	com_putchar2(0x01);
	com_putchar2(0x20);
	while(*fname) {
		com_putchar2(*fname);
		fname++;
	}
	com_putchar2(CHAR_CR);

	Vinc_GetBuf(buf1);
   	if(buf1[0]==CHAR_CR && buf1[1]==0x43 && buf1[2]==0x46 && buf1[3]==CHAR_CR) return(0);
	return(1);
}	

int Vinc_DeleteFile(char *fname)
{
	while(com_getchar2(&ch)!=-1);

	com_putchar2(0x07);
	com_putchar2(0x20);
	while(*fname) {
		com_putchar2(*fname);
		fname++;
	}
	com_putchar2(CHAR_CR);

	Vinc_GetBuf(buf1);
   	if(buf1[0]==0x3E && buf1[1]==CHAR_CR) return(1);
	return(0);
}	

void Vinc_GetBuf(char *buff) 	
{
	int ch,j;
	long i=0;

	for(i=0;i<50;i++)
		buff[i]=0x00;

	while(com_getchar2(&ch)==-1 && i++<200000);
	if(i<=200000) {
		buff[0]=ch;
		j=1;
		do {
			i=0;
			while(com_getchar2(&ch)==-1 && i++<100000);
			if(i<100000 && j<50)
				buff[j++]=ch;
		}while(i<100000 && j<50);
	}
	else strcpy(buff,"TO\r");
}		

void Vinc_Send(char *bef)
{
	unsigned int i;

	VincHand=1;
	com_bufclr2();
	GPIO_WriteBit(VINC_GPIOB, RTS3, 		Bit_RESET);
	for(i=0;i<strlen(bef);i++)
		com_put2ww(bef[i]);
	com_putchar2(CHAR_CR);
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

	for(i=0;i<subblocks;i++) {
		memset(endianBuf, 0xFF, PAGESIZE);														// Memset the data buffer...
		
		for (j=0; j < PAGESIZE; j += 2)																// Change the data to little endian for VNC2.
		{
			endianBuf[j] 	 = buf[j+1+(i*PAGESIZE)];
			endianBuf[j+1] = buf[j+(i*PAGESIZE)];
		}
		
		osDelay(10);//10

		com_putchar2(0x03);																						// WRITE FORMAT 0x03
		com_putchar2((adr/PAGESIZE)&0xFF);														// Send the FlashAddressLow data address....
		com_putchar2(((adr/PAGESIZE)>>8)&0xFF);												// Send the FlashAddressHigh data address....
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02) break;
		}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		if ((ch & 0x000000FF) != 0x02) return(6);											// Check for a response data address
			
		
		for(numwriten=0;numwriten<PAGESIZE;numwriten++){							// Send the data
			com_putchar2(endianBuf[numwriten]);
		}
		
	  osDelay(100);//100
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02) break;
		}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		if ((ch & 0x000000FF) != 0x02)	return(7);										// Check for a response data has been send
			
		com_bufclr2();
		ch=0xFF;
																																	// Read the data back to make sure that it has been programmed correctly.		
		com_putchar2(0x02);																						// cmd READ FORMAT 0x02
		com_putchar2((adr/PAGESIZE)&0xFF);														// Send the FlashAddressLow data address....
		com_putchar2(((adr/PAGESIZE)>>8)&0xFF);												// Send the FlashAddressHigh data address....		

		osDelay(10);//10
		
		TIMER_Start(TIMER_Vinculum, TO_1SEC);
		do{	
			osThreadYield();
			if ((ch & 0x000000FF) == 0x02) break;
		}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
		if ((ch & 0x000000FF) != 0x02) return(8);											// Check for a response data address	
		
		if(numwriten < PAGESIZE) return -1;														// Make sure that we have 128 bytes to read from the chip!!!
	
																																	// Read 128 bytes back from the chip...
		for(numwriten=0;numwriten<PAGESIZE;numwriten++){							// read the data
			TIMER_Start(TIMER_Vinculum, TO_100MSEC);
			do{	
				osThreadYield();
			}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));
			receiveBuf[numwriten] = (	signed char )ch;
		}
		
		// Compare the data read back from the flash with the data sent....
		// NOTE: The data that is returned is reversed due to the endianess of the VNC2,
		// compare the returned data with the data that we reversed to send to the chip.
		for(j = 0 ; j < PAGESIZE; j++){
			zaehler++;
			if(endianBuf[j] != receiveBuf[j])		{
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
	
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_SET);
	osDelay(50);
	GPIO_WriteBit(VINC_GPIOB, RTS3, 		Bit_RESET);
	GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, 	  Bit_SET);
	osDelay(150);
	GPIO_WriteBit(VINC_GPIOC, VINC_PROG, 		Bit_RESET);
	osDelay(50);
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_RESET);
	osDelay(100);
	GPIO_WriteBit(VINC_GPIOC, VINC_RESET, 	Bit_SET);
  osDelay(5000);
	
	com_bufclr2();
	
	ch = com_putchar2(0xFF);
	TIMER_Start(TIMER_Vinculum, TO_1SEC);
	do{	
	}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0xFF) return(1);
	
	ch = com_putchar2(0xFB);
	TIMER_Start(TIMER_Vinculum, TO_100MSEC);
	do{	
	}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0xFB) return(2);
	osDelay(100);
	
	com_putchar2(0x01);//Set Data Variable
	com_putchar2(0x70);//TimerLow = 0x70, This command allows the Flash program time and Baud rate to be specified
	com_putchar2(0x17);//TimerHigh = 0x17
	com_putchar2(0x1A);//Baud1, 115200
	com_putchar2(0x00);//Baud2, 115200
	com_putchar2(0x00);//Baud3, 115200
  osDelay(50);
	
	TIMER_Start(TIMER_Vinculum, TO_1SEC);
	do{	
	}while(com_getchar2(&ch) ==-1 && !TIMER_TimeOut(TIMER_Vinculum));

	if ((ch & 0x000000FF) != 0x02) return(3);
	
	com_baudrate(VINC_UART, VINC_BAUD, HARDWARE_FLOW_CONTROL); 

 	return(0);
}

unsigned int Vinc_Query(unsigned int port)
{
	int i,ch;

	if(port==1)
		com_putchar2(0x2B);
	else 
		com_putchar2(0x2C);
	com_putchar2(CHAR_CR);
	i=0;
	while(com_getchar2(&ch)==-1 && i++<5000);
	while(com_getchar2(&ch)!=-1);
	return(ch);
}


void Vinc_QD(unsigned int num,char *erg)
{
	int ch,j,i=0;

	while(com_getchar2(&ch)!=-1);
	for(j=0;j<32;j++)
		erg[j]=0x00;
	com_putchar2(0x85);
	com_putchar2(0x20);
	com_putchar2(num);
	com_putchar2(CHAR_CR);
	for(j=0;j<32;j++) {
		i=0;
		while(com_getchar2(&ch)==-1 && i++<50000)
			osThreadYield();
		if(i<50000) 
			erg[j]=ch;
	}
	osDelay(10);
	while(com_getchar2(&ch)!=-1);
}

void Vinc_Vers(char *erg) {
	int ch,j,i=0;

	while(com_getchar2(&ch)!=-1);
	for(j=0;j<32;j++) erg[i]=0x00;
	com_putchar2(0x13);
	com_putchar2(CHAR_CR);
	j=0;
	do {
		i=0;
		while(com_getchar2(&ch)==-1 && i++<50000);
		if(i<50000) 
			erg[j++]=ch;
	}while(j<32 && i<50000);
	
	erg[j]=0;
	osDelay(10);
	
	while(com_getchar2(&ch)!=-1);
}

void Vinc_Dir(char *erg) {
	int ch,j,i=0;

	while(com_getchar2(&ch)!=-1);
	for(j=0;j<50;j++) erg[i]=0x00;
	com_putchar2(0x01);
	com_putchar2(CHAR_CR);
	j=0;
	do {
		i=0;
		while(com_getchar2(&ch)==-1 && i++<15000);
		if(i<15000) 
			erg[j++]=ch;
	}while(j<50 && i<15000);
	erg[j]=0;
	osDelay(10);
	while(com_getchar2(&ch)!=-1);
}


void Vinc_ReadFile(char *nam,char *buf,int maxbuf)
{
	int ch,j,i=0;

	VincHand=1;

	while(com_getchar2(&ch)!=-1);
	com_put2ww(0x04);
	com_put2ww(0x20);
	while(*nam) {
		com_put2ww(*nam);
		nam++;
	}
	com_put2ww(CHAR_CR);
	j=0;
	do {
		i=0;
		while(com_getchar2(&ch)==-1 && i++<15000);
		if(i<15000) { 
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

	while(com_getchar2(&ch)!=-1);
	com_put2ww(0x09);
	com_put2ww(0x20);
	while(*nam) {
		com_put2ww(*nam);
		nam++;
	}
	com_put2ww(CHAR_CR);
	ret=Vinc_GetPrompt();
	VincHand=0;
	return(ret);
}	

int Vinc_GetPrompt(void)
{
	char pbuf[20];
	unsigned long i=0;
	
	int ind=0,ch;

	while(com_getchar2(&ch)==-1 && i++<300000) osThreadYield();

	if(i>=300000) return(11);
	pbuf[ind]=ch;
	ind++;
	while(com_getchar2(&ch)!=-1){
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

	while(com_getchar2(&ch)!=-1);
	com_put2ww(0x0A);
	com_put2ww(0x20);
	while(*nam) {
		com_put2ww(*nam);
		nam++;
	}
	com_put2ww(CHAR_CR);
	VincHand=0;
	return(ret);
}	

int Vinc_WriteFile(int num,char *buf)
{
	int i;
	VincHand=1;

	while(com_getchar2(&ch)!=-1);
	com_put2ww(0x08);
	com_put2ww(0x20);
	com_put2ww(0x00);   
	com_put2ww(0x00);   
	com_put2ww(num>>8);
	com_put2ww(num & 0x00FF);
	com_put2ww(CHAR_CR);

	for(i=0;i<num;i++) {
		com_put2ww(buf[i]);
	}
	i=Vinc_GetPrompt();
	VincHand=0;
	return(i);
}	

void Vinc_SC(unsigned int num)
{
 	int i;

	com_putchar2(0x86);
	com_putchar2(0x20);
	com_putchar2(num);
	com_putchar2(CHAR_CR);
	i=0;
	while(com_getchar2(&ch)==-1 && i++<15000);
	while(com_getchar2(&ch)!=-1);
}

unsigned char GetPrinterStatus(void)
{
 	int i;
	int ch;


	while(com_getchar2(&ch)!=-1);
	com_putchar2(0x81);
	com_putchar2(CHAR_CR);
	i=0;
	while(com_getchar2(&ch)==-1 && i++<5000);
	while(com_getchar2(&ch)!=-1);
	return(ch);
}

int SetDataMode(void)
{
    int i=0;

    GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, 	  Bit_RESET);	
		while(GPIO_ReadInputDataBit(VINC_GPIOB, VINC_DATACK)!=0 && i++<100)
        osDelay(1);
    if(i>=100) return(0);
    return(1);
}

// 2022-11-25 周五 16:09:30，Gavin，后补v2.09的内容
int StartPrinter(void)
{
	int ret=0;
	int i,j=0;
	
	while((ret == 0)&&(j < 5))
	{
		MartelMode=false;
		GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, Bit_SET);	
		osDelay(1000);
		
		for(i=0;i<16;i++) 
		{
			Vinc_QD(i,buf1);
			if((buf1[7] & 0x04) == 0x04)
			{
				Vinc_SC(i);
				ret = 1;
				break;
			}
			
			if((buf1[7] & 0x10) == 0x10)
			{
				Vinc_SC(i);
				MartelMode=true;
				ret=SetDataMode();
				break;
			}
			/*
			if((buf1[7] & 0x04) == 0x04 || (buf1[7] & 0x10) == 0x10 )
			{
				Vinc_SC(i);
				if((buf1[7] & 0x10) == 0x10) 
				{
					MartelMode=true;
					ret=SetDataMode();
					
				}
				else
					ret=1;
				
				break;
			}
			*/
			osDelay(100);
		}
		
		if(ret == 1) 
		{
			PrinterInit();
			break;
		}
		else
		{
			VINC_Reset();
			osDelay(1000);
			GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, Bit_SET);	
			osDelay(1000);
			SendPrinter("\x1B@");
			osDelay(1000);
		}
		
		j++;
	}
	
	return(ret);
}

void StopPrinter(void)
{
	PrinterExit();
	GPIO_WriteBit(VINC_GPIOB, VINC_DATAREQ, 	  Bit_SET);	
}

void SendPrinter(const char *str)
{
	int ch,i,aktsend,index=0;

	VincHand=1;

	aktsend=strlen(str);
	while(aktsend>32) {
        if(!MartelMode) {
            com_put2ww(0x83);
            com_put2ww(0x20);
            com_put2ww(32);
            com_put2ww(CHAR_CR);
        }
		for(i=0;i<32;i++) {
			if(str[index]==0x0A) zeile++;
			if((str[index] & 0x80) == 0x80 && !PrinterFrames && str[index]!='\x9A' && str[index]!='\x81')
				com_put2ww(' ');
			else
				com_put2ww(str[index]);
			index++;
			osThreadYield();
		}
        if(!MartelMode) {
            i=0;
            while(com_getchar2(&ch)==-1 && i++<50000)
                osThreadYield();
            while(com_getchar2(&ch)!=-1)
                osThreadYield();
        }
        else
            osDelay(500);
		aktsend-=32;
		osDelay(100);
	}
	if(aktsend>0) {
        if(!MartelMode) {
            com_put2ww(0x83);
            com_put2ww(0x20);
            com_put2ww(aktsend);
            com_put2ww(CHAR_CR);
        }
		for(i=0;i<aktsend;i++) {
			if(str[index]==0x0A) zeile++;
			if((str[index] & 0x80) == 0x80 && !PrinterFrames)
				com_put2ww(' ');
			else
				com_put2ww(str[index]);
			index++;
			osThreadYield();
		}
        if(!MartelMode) {
            i=0;
            while(com_getchar2(&ch)==-1 && i++<50000)
                osThreadYield();
            while(com_getchar2(&ch)!=-1)
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
	while(aktsend>32) {
        if(!MartelMode) {
            com_put2ww(0x83);
            com_put2ww(0x20);
            com_put2ww(32);
            com_put2ww(CHAR_CR);
        }
		for(i=0;i<32;i++) {
			if(str[index]==0x0A) zeile++;
			// if((str[index] & 0x80) == 0x80 && !PrinterFrames)
			// 	com_put2ww(' ');
			// else
				com_put2ww(str[index]);
			index++;
			osThreadYield();	
		}
        if(!MartelMode) {
            i=0;
            while(com_getchar2(&ch)==-1 && i++<50000)
                osThreadYield();
            while(com_getchar2(&ch)!=-1)
                osThreadYield();
        }
        else
            osDelay(500);
		aktsend-=32;
		osDelay(100);
	}
	if(aktsend>0) {
        if(!MartelMode) {
            com_put2ww(0x83);
            com_put2ww(0x20);
            com_put2ww(aktsend);
            com_put2ww(CHAR_CR);
        }
		for(i=0;i<aktsend;i++) {
			if(str[index]==0x0A) zeile++;
			// if((str[index] & 0x80) == 0x80 && !PrinterFrames)
			// 	com_put2ww(' ');
			// else
				com_put2ww(str[index]);
			index++;
			osThreadYield();
		}
        if(!MartelMode) {
            i=0;
            while(com_getchar2(&ch)==-1 && i++<50000)
                osThreadYield();
            while(com_getchar2(&ch)!=-1)
                osThreadYield();
        }
        else
            osDelay(500);
	}
	VincHand=0;
}

// 2022-11-25 周五 16:09:30，Gavin，后补v2.09的内容
void PrinterInit(void)
{
	if(!MartelMode)
	{
		SendPrinter("\033E\033&l26A"); 			// 复位+A4纸张
		SendPrinter("\033(10U"); 						// 默认符号集
		SendPrinter("\033&a5L\033&k10.35H");// 左边距=第五列 +
	}
	else
	{
		//SendPrinter("\x1B@");
		//osDelay(1000);
		SendPrinter("\x1B!\x02\r\n");
	}
}

// 2022-11-25 周五 16:09:30，Gavin，后补v2.09的内容
void PrinterExit(void)
{
	if(!MartelMode)
		//SendPrinter("\033&l1T\033E");
		SendPrinter("\033&l1T\033*rC\033E");
		
	else
		//SendPrinter("\x1B!\x02\r\n");
		SendPrinter("\x1B@");
}

