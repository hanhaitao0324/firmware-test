#include "TBH125.h"

#define VNC1 1
#define VNC2 2


extern unsigned char Com2Verg;
extern unsigned int rsstatus2;
extern unsigned char vers[];
//extern char rsbuf2[];
extern char buf1[50];
extern unsigned int PrinterPaper,PrinterLanguage,PrinterFrames;
int zeile;
extern unsigned char VincHand;
extern float NwtFak,dmeing;
extern int ForceLimit;
extern char kalivers[];
extern char dosnr[];
extern BOOL StepSperr;
extern BOOL RTS_TimeOut;
extern unsigned int druckmode,Printerlanguage;
extern struct buf_st tbuf2;
unsigned char endianbuf[129];

volatile int VincType=VNC1;

void Vinc_Reset(void)
{
	long i;

	VincHand=0;
	GPIO_WriteBit(GPIOD, VNC_CTS, Bit_RESET);
	
	GPIO_WriteBit(GPIOD, VNC_DSR, Bit_SET);
	
	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_RESET);
	os_dly_wait(10);
	
	GPIO_WriteBit(GPIOD, VNC_PROG, Bit_SET);
	os_dly_wait(150);

	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_SET);
	SET_Baudrate(USART3,9600);
	os_dly_wait(3500);
	
	i=0;
	while(com_getchar3()!=-1 && i++<100)
		os_dly_wait(10);
	 
	com_putchar3('S');
	com_putchar3('C');
	com_putchar3('S');
	com_putchar3(0x0D);
	Vinc_GetPrompt();

	com_putchar3(0x91);
	com_putchar3(0x0D);
   Vinc_GetPrompt();	

	os_dly_wait(100);
}

void Vinc_Extended(void)
{
	unsigned int i;

	com_putchar3(0x90);
	com_putchar3(0x0D);
	i=0;
	while(com_getchar3()==-1 && i++<50)
		os_dly_wait(1);
	while(com_getchar3()!=-1);
	com_putchar3(0x11);
	com_putchar3(0x0D);
	i=0;
	while(com_getchar3()==-1 && i++<50)
		os_dly_wait(1);
	while(com_getchar3()!=-1);
}

int Vinc_TestStick(void)
{
	while(com_getchar3()!=-1);
	com_putchar3(0x0D);
	Vinc_GetBuf(buf1);
   	if(buf1[0]==0x3E && buf1[1]==0x0D) return(1);
	return(0);
}	

int Vinc_TestFile(char *fname)
{
	while(com_getchar3()!=-1);

	com_putchar3(0x01);
	com_putchar3(0x20);
	while(*fname) {
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(0x0D);

	Vinc_GetBuf(buf1);
   	if(buf1[0]==0x0D && buf1[1]==0x43 && buf1[2]==0x46 && buf1[3]==0x0D) return(0);
	return(1);
}	

int Vinc_DeleteFile(char *fname)
{
	while(com_getchar3()!=-1);

	com_putchar3(0x07);
	com_putchar3(0x20);
	while(*fname) {
		com_putchar3(*fname);
		fname++;
	}
	com_putchar3(0x0D);

	Vinc_GetBuf(buf1);
   	if(buf1[0]==0x3E && buf1[1]==0x0D) return(1);
	return(0);
}	


void Vinc_GetBuf(char *buff) 	
{
	int ch,j;
	long i=0;

	for(i=0;i<50;i++)
		buff[i]=0x00;

	while((ch=com_getchar3())==-1 && i++<200)
		os_dly_wait(1);
	if(i<=200) {
		buff[0]=ch;
		j=1;
		do {
			i=0;
			while((ch=com_getchar3())==-1 && i++<1000)
				os_dly_wait(1);
			if(i<1000 && j<50)
				buff[j++]=ch;
		}while(i<1000 && j<50);
//		buff[i]=0x0D;
	}
	else strcpy(buff,"TO\r");
}		


void Vinc_Send(char *bef)
{
	unsigned int i;

	VincHand=1;
	com_bufclr3();
	GPIO_WriteBit(GPIOD, VNC_CTS, Bit_RESET);
	for(i=0;i<strlen(bef);i++)
		com_put3ww(bef[i]);
	com_putchar3(0x0D);
	VincHand=0;
}

int VincErase(void)
{
	long i;

	VincHand=1;
	SET_Baudrate(USART3,115200);
	GPIO_WriteBit(GPIOD, VNC_CTS, Bit_RESET);
	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_RESET);
	os_dly_wait(10);
	GPIO_WriteBit(GPIOD, VNC_PROG, Bit_RESET);
	os_dly_wait(150);
	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_SET);
	os_dly_wait(100);

	com_putchar3(0xFF);
	i=0;
	while(com_getchar3()==-1 && i++<50)
		os_dly_wait(1);;
	if(i>=50) return(1);

	com_putchar3(0xFA);
	i=0;
	while(com_getchar3()==-1 && i++<10)
		os_dly_wait(10);
	if(i<10){
		VincType=VNC1;
	}
	else {
		com_putchar3(0xFB);
		i=0;
		while(com_getchar3()==-1 && i++<50)
			os_dly_wait(10);
		if(i>=50) return(2);
		else VincType=VNC2;
	}

	if(VincType==VNC1) {
		com_putchar3(0x01);
		com_putchar3(0x70);
		com_putchar3(0x17);
		com_putchar3(0x1A);
		com_putchar3(0x00);
		com_putchar3(0x00);
		i=0;
		while(com_getchar3()==-1 && i++<50)
			os_dly_wait(1);
		if(i>=50) return(3);
	}
	else {
		com_putchar3(0x01);
		com_putchar3(0x01);
		i=0;
		while(com_getchar3()==-1 && i++<150)
			os_dly_wait(10);
		os_dly_wait(100);
		if(i>=150) return(3);
	}
	return(0);
}

int Vinc_Prog_Block(int block,unsigned char *buf,unsigned int size)
{
	long i;
	int subblocks,j,k,ret=0;
	unsigned int adr;
	if(VincType==VNC1) 
	{
		adr=block*size;
		subblocks=size/128;
	
		for(j=0;j<subblocks;j++) 
		{
		
			os_dly_wait(50);
			com_putchar3(0x03);
			com_putchar3(adr);
			com_putchar3(adr>>8);
			com_putchar3(128);
			i=0;
			while(com_getchar3()==-1 && i++<5000)
				os_dly_wait(2);
			if(i>=5000) 
			{
				ret=6;
				break;
			}
			os_dly_wait(20);
			for(i=0;i<128;i++)
			{
				com_putchar3(buf[i+(j*128)]);
			}
			i=0;
			while(com_getchar3()==-1 && i++<2000)
				os_dly_wait(2);
			if(i>=2000) 
			{
				ret=7;
				break;
			}
			adr+=128;
		}
	}
	else 
	{
		adr=(block*size)/128;
		subblocks=size/128;
		for(j=0;j<subblocks;j++) 
		{
			for(k=0;k<128;k+=2) 
			{
				endianbuf[k]=buf[k+(j*128)+1];
				endianbuf[k+1]=buf[k+(j*128)];
			}
			os_dly_wait(50);
			com_putchar3(0x03);

			com_putchar3(adr);

			com_putchar3(adr>>8);
			i = 0;
			while(com_getchar3()==-1&& i++<5000)
				os_dly_wait(2);
			if(i>=5000) 
			{
				ret=6;
				break;
			}
			while(com_getchar3()!=-1);
			os_dly_wait(20);
			for(i=0;i<128;i++)
			{
				com_putchar3(endianbuf[i]);
			}
			i=0;
			while(com_getchar3()==-1&& i++<2000)
				os_dly_wait(2);
			if(i>=2000) 
			{
				ret=7;
				break;
			}
			while(com_getchar3()!=-1);
			os_dly_wait(1);
			adr++;
		}

	}
	return(ret);
}


unsigned int Vinc_StartProg(void)
{
	long i;

	VincHand=1;
	SET_Baudrate(USART3,115200);
	GPIO_WriteBit(GPIOD, VNC_CTS, Bit_RESET);
	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_RESET);
	os_dly_wait(10);
	GPIO_WriteBit(GPIOD, VNC_PROG, Bit_RESET);
	os_dly_wait(150);
	GPIO_WriteBit(GPIOD, VNC_RESET, Bit_SET);
	os_dly_wait(100);

	com_putchar3(0xFF);
	i=0;
	while(com_getchar3()==-1 && i++<50)
		os_dly_wait(1);
	if(i>=50) return(1);

	while(com_getchar3()!=-1);
	com_putchar3(0xFA);
	i=0;
	while(com_getchar3()==-1 && i++<50)
		os_dly_wait(10);
	if(i<50)
	{
		VincType=VNC1;
	}
	else 
	{
		com_putchar3(0xFB);
		i=0;
		while(com_getchar3()==-1 && i++<50)
			os_dly_wait(1);
		if(i>=50) 
			return(2);
		else 
			VincType=VNC2;
	}
	
	if(VincType==VNC1) 
	{
		com_putchar3(0x01);
		com_putchar3(0x70);
		com_putchar3(0x17);
		com_putchar3(0x1A);
		com_putchar3(0x00);
		com_putchar3(0x00);
		i=0;
		while(com_getchar3()==-1 && i++<50)
			os_dly_wait(1);
		if(i>=50) return(3);
	}
	else 
	{
		com_putchar3(0x01);
		com_putchar3(0x01);

		i=0;
		while(com_getchar3()==-1 && i++<150)
			os_dly_wait(10);
		os_dly_wait(100);
		if(i>=150) 
			return(3);
	}

 	return(0);
}

unsigned int Vinc_Query(unsigned int port)
{
	int i,ch;

	if(port==1)
		com_putchar3(0x2B);
	else 
		com_putchar3(0x2C);
	com_putchar3(0x0D);
	i=0;
	while((ch=com_getchar3())==-1 && i++<5000);
	while(com_getchar3()!=-1);
	return(ch);
}


void Vinc_QD(unsigned int num,char *erg)
{
	static int ch,j,i;

	while(com_getchar3()!=-1);
	for(j=0;j<32;j++)
		erg[j]=0x00;
	com_putchar3(0x85);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);
	
	for(j=0;j<32;j++) 
	{
		/*i=0;
		while((ch=com_getchar3())==-1 && i++<2500)
			//os_tsk_pass();
			os_dly_wait(1); 									// Gavin, 2021-05-24 星期一 17:26:41
		
		if(i<2500) 
			erg[j]=ch;
		*/
		//i = 6000;
		i = 10000; // 5500
		while((ch=com_getchar3())==-1 && i--)
			os_tsk_pass();
		
		if(i > 0) 
			erg[j]=ch;
	}
	
	os_dly_wait(15);
	while(com_getchar3()!=-1);
}

void Vinc_Vers(char *erg) 
{
	int ch,j,i=0;

	while(com_getchar3()!=-1);
	for(j=0;j<32;j++) erg[i]=0x00;
	com_putchar3(0x13);
	com_putchar3(0x0D);
	j=0;
	do {
		i=0;
		while((ch=com_getchar3())==-1 && i++<150)
			os_dly_wait(1);
		if(i<150) 
			erg[j++]=ch;
	}while(j<32 && i<150);
	erg[j]=0;
	os_dly_wait(10);
	while(com_getchar3()!=-1);
}

void Vinc_Dir(char *erg) 
{
	int ch,j,i=0;

	while(com_getchar3()!=-1);
	for(j=0;j<50;j++) erg[i]=0x00;
	com_putchar3(0x01);
	com_putchar3(0x0D);
	j=0;
	do {
		i=0;
		while((ch=com_getchar3())==-1 && i++<15000);
		if(i<15000) 
			erg[j++]=ch;
	}while(j<50 && i<15000);
	erg[j]=0;
	os_dly_wait(10);
	while(com_getchar3()!=-1);
}


void Vinc_ReadFile(char *nam,char *buf,int maxbuf)
{
	int ch,j,i=0;

	VincHand=1;

	while(com_getchar3()!=-1);
	com_put3ww(0x04);
	com_put3ww(0x20);
	while(*nam) {
		com_put3ww(*nam);
		nam++;
	}
	com_put3ww(0x0D);
	j=0;
	do {
		i=0;
		while((ch=com_getchar3())==-1 && i++<15000);
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

	while(com_getchar3()!=-1);
	com_put3ww(0x09);
	com_put3ww(0x20);
	while(*nam) {
		com_put3ww(*nam);
		nam++;
	}
	com_put3ww(0x0D);
	ret=Vinc_GetPrompt();
	VincHand=0;
	return(ret);
}	


int Vinc_TestAlive(void)
{
	int ret;
	
	while(com_getchar3()!=-1);
	com_put3ww('E'); 				// 0x45
	com_put3ww(0x0D);
	ret=Vinc_GetPrompt();
	
	if(ret!=12) 
		Vinc_Reset();
	else 
		return(1);
	
	while(com_getchar3()!=-1);
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
	int i=0;
	
	int ind=0,ch;

	while((ch=com_getchar3())==-1 && i++<5000)				//原程序是500
		os_dly_wait(1);
	
	if(i>=5000) return(11);
	pbuf[ind]=ch;
	ind++;
	do 
	{
		//i=300;
		i = 7200;
		while((ch=com_getchar3())==-1 && i--)
			os_tsk_pass();
		
		if(i>0)
		{
			pbuf[ind]=ch;
			if(ind<20) ind++;
		}
	}while(i>0);
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
	if(pbuf[0]=='E' && pbuf[1]==0x0D && pbuf[2]=='E' && pbuf[3]==0x0D) return(12);		//No upgrade
	
	return(10);										//unknown error

}

int Vinc_CloseFile(char *nam)
{
	int ret=0;

	VincHand=1;

	while(com_getchar3()!=-1);
	com_put3ww(0x0A);
	com_put3ww(0x20);
	while(*nam) {
		com_put3ww(*nam);
		nam++;
	}
	com_put3ww(0x0D);
//	ret=Vinc_GetPrompt();
	VincHand=0;
	return(ret);
}	

int Vinc_WriteFile(int num,char *buf)
{
	int i;

	VincHand=1;

	while(com_getchar3()!=-1);
	com_put3ww(0x08);
	com_put3ww(0x20);
	com_put3ww(0x00);   
	com_put3ww(0x00);   
	com_put3ww(num>>8);
	com_put3ww(num & 0x00FF);
	com_put3ww(0x0D);

	for(i=0;i<num;i++) {
		com_put3ww(buf[i]);
	}
	i=Vinc_GetPrompt();
	VincHand=0;
	return(i);
}	


void Vinc_SC(unsigned int num)
{
	com_putchar3(0x86);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);
	Vinc_GetPrompt();
}


void Vinc_SF(unsigned int num)
{
	com_putchar3(0x87);
	com_putchar3(0x20);
	com_putchar3(num);
	com_putchar3(0x0D);
	Vinc_GetPrompt();
}

unsigned char GetPrinterStatus(void)
{
 	int i;
	int ch;


	while(com_getchar3()!=-1);
	ch=com_put3ww(0x81);
	ch=com_put3ww(0x0D);
	i=0;
	while((ch=com_getchar3())==-1 && i++<10)
		os_dly_wait(5);
	while(com_getchar3()!=-1);
	return(ch);

}

const unsigned char prinit[22]={0x1B,0x09,0x1B,0x09,0x1B,0x23,0xBB,0x1B,0x21,0x00,0x1C,0x2E,0x1B,0xFD,0x83,0x1B,0x63,0x00,0x1B,0x15,0x1B,0x15};
//const unsigned char prinit[22]={0x1B,0x09,0x1B,0x09,0x1B,0x21,0x00,0x1C,0x2E,0x1B,0x63,0x00};

volatile unsigned char PrintType = 0; 	// 打印机类型：0，台式机；1，微打

int StartPrinter(void)
{
	int ret=0,i;
	char buf1[50];
	char str[6];
	
	//VINC_DATREQ=VINC_DATREQ_H; 				// 直传打印数据；VINC_DATREQ_H;
	GPIO_WriteBit(GPIOD, VNC_DSR, Bit_SET);
	os_dly_wait(100);
	
	if(druckmode==1) 
	{
		if(vers[47]==0x30) // 0，德国打印机； 1，中国打印机
		{
			str[0]=0x1B;
			str[1]='!';
			str[2]=0x02;
			str[3]=0;

			SendIntPrinter(str);
			return(0);
		}
		else 
		{
			for(i=0;i<22;i++)
				com_putchar2(prinit[i]);
			os_dly_wait(1000); 
	 		return(0);
		}
	}
	
	ret=1;
	if(!Vinc_TestAlive()) 
		return(0);
	
	for(i=0;i<16;i++) 
	{
		Vinc_QD(i,buf1);
		if((buf1[7] & 0x04) == 0x04) 	// 台式打印机
		{
			Vinc_SC(i);
			ret=0;
			PrintType = 0; 							// 台式打印机标志
			break;
		}
		
		if(buf1[7] == 0x10)
		{
			Vinc_SC(i);
			ret=0;
			PrintType = 1; 							// 微型打印机标志
			
			//VINC_DATREQ = 0; 						// 直传打印数据；VINC_DATREQ_H;
			GPIO_WriteBit(GPIOD, VNC_DSR, Bit_RESET);
			os_dly_wait(100);
			break;
		}
	}
	
	if(ret==0) 
	{
//		i=GetPrinterStatus();
//		if((i & 0x20) == 0x00) {
		//if(PrinterLanguage==1) 			// 2015.05.29
			PrinterInit();
		
/*		} 
		else {
			ret=2;
		}*/
	}
	return(ret);
}

void StopPrinter(void)
{
//	static int i;
  	struct buf_st *p;

	if(druckmode==1){
//		com_baudrate (0,115200);
		return;
	}

	p = &tbuf2;
	while (p->in != p->out) 
		os_dly_wait(100);

//	i=GetPrinterStatus();
	PrinterExit();
	//VINC_DATREQ=VINC_DATREQ_H;
	GPIO_WriteBit(GPIOD, VNC_DSR, Bit_SET);
}

int SendPrinter(char *str)
{
	int i,aktsend,index=0,abr;

	if(druckmode==1) 					// 打印机模式, 1.INT内部打印机；0.EXT外部打印机
	{
		SendIntPrinter(str);
		return(0);
	}
	
	if(PrintType == 1) 				// 打印机类型：0，台式机；1，微型打印机
	{
		//VINC_DATREQ = 0; 						// 直传打印数据；VINC_DATREQ_H;
		
		aktsend=strlen(str);
		for(i=0;i<aktsend;i++)
		{
			com_put3ww(str[i]);
		}
		
		os_dly_wait(100);
		
		return(0);
	}
	else
	{
		VincHand=1;
		if(RTS_TimeOut)
		{
			RTS_TimeOut=__FALSE;
			Vinc_Reset();
		}	
		abr=0;
		aktsend=strlen(str);
		while(aktsend>32) 
		{
			if(com_put3ww(0x83)==1)
			{
				abr=1;
				break;
			}
			
			if(com_put3ww(0x20)==1) 
			{
				abr=2;
				break;
			}
			
			if(com_put3ww(32)==1) 
			{
				abr=3;
				break;
			}
			
			if(com_put3ww(0x0D)==1)
			{
				abr=4;
				break;
			}
			
			if(abr==0) 
			{
				for(i=0;i<32;i++) 
				{
					if(str[index]==0x0A) 
						zeile++;
					
					//if((str[index] & 0x80) == 0x80 && !PrinterFrames && str[index]!='š' && str[index]!='')
					if((str[index] & 0x80) == 0x80 && !PrinterFrames && str[index]!='\x9a' && str[index]!='\x81')
					{
						if(com_put3ww(' ')==1)
						{
							abr=5;
							break;
						}
					}
					else 
					{
						if(com_put3ww(str[index])==1)
						{
							abr=6;
							break;
						}
					}
					index++;
					os_dly_wait(1);
				}
				Vinc_GetPrompt();
	/*			i=0;
				while(com_getchar3()==-1 && i++<500)
					os_dly_wait(2);
				while(com_getchar3()!=-1)
					os_tsk_pass();*/
				aktsend-=32;
				os_dly_wait(100);
			}
			if(abr!=0) break;
		}
		
		if(aktsend>0 && abr==0) 
		{
			if(com_put3ww(0x83)==1)
			{
				abr=7;
			}	
			
			if(abr==0) 
			{
				if(com_put3ww(0x20)==1) 
				{
					abr=8;
				}
			}		
			
			if(abr==0) 
			{
				if(com_put3ww(aktsend)==1)
				{
					abr=9;
				}
			}
			
			if(abr==0) 
			{
				if(com_put3ww(0x0D)==1)
				{
					abr=10;
				}
			}
			
			if(abr==0) 
			{
				for(i=0;i<aktsend;i++) 
				{
					if(str[index]==0x0A) zeile++;
					if((str[index] & 0x80) == 0x80 && !PrinterFrames) 
					{
						if(com_put3ww(' ')==1)
						{
							abr=11;
							break;
						}
					}
					else 
					{
						if(com_put3ww(str[index])==1)
						{
							abr=12;
							break;
						}
					}
					index++;
					os_dly_wait(1);
				}
				i=0;
				while(com_getchar3()==-1 && i++<500)
					os_dly_wait(2);
				while(com_getchar3()!=-1)
					os_tsk_pass();
			}
		}
		
		if(abr>0) 
		{
			os_dly_wait(1000);
		}
		
		VincHand=0;
		return(abr);
	}
}

int SendPrinterCC(const char *str)
{
	return(SendPrinter((char*) str));

}

 
void PrinterInit(void)
{
//	SendPrinter("\033%%-12345X");
//	SendPrinter("@PJL ENTER LANGUAGE = PCL\r\n");
	
	if(PrintType == 0) // 打印机标志，1，微型打印机标志；0，台式打印机
	{
		if(PrinterLanguage==1 && druckmode==0) 
		{
			SendPrinter("\033E\033&l26A");
			SendPrinter("\033(10U");
			SendPrinter("\033&a5L\033&k10.35H");
		}
	}
	else
	{
		SendPrinter("\x1B!\x02");
	}

//	SendPrinter("\033E\033&l26A");
//	SendPrinter("\033&a5L\033&k10.35H");
}

void PrinterExit(void)
{
	if(druckmode==0) 			// 0，EXT模式；1，INT模式
	{
		if(PrintType == 0) 		// 打印机标志，1，微型打印机标志；0，台式打印机
		{
			if(PrinterLanguage==1)
				SendPrinter("\033&l1T\033E");
			else
				SendPrinter("\f");
		}
		else
		{
			SendPrinter("\x1B!\x02");
		}
	}
}

// 测量参数获取，Gavin，2019-07-27 星期六 10:12:21
int StartKalibrator(void)
{
	int ret=0,i;
	static char answ[30];
	static unsigned char buf1[50];

	for(i=0;i<16;i++) 
	{
		Vinc_QD(i,(char *)buf1);
		// 2016/12/3 11:17:25 	Gavin;
		//if((buf1[7] & 0x01) == 0x01 && buf1[16]==0x0A && buf1[17]==0xA1) // U盘校验工具，旧模式，PID-0xA10A
		//if((buf1[7] & 0x01) == 0x01 && buf1[16]==0x01 && buf1[17]==0x60) 	// U盘校验工具，新模式，PID-0x6001
		if((buf1[7] & 0x01) == 0x01)
		{
			if(((buf1[16]==0x0A) && (buf1[17]==0xA1))||
			   ((buf1[16]==0x01) && (buf1[17]==0x60)))
			{
				Vinc_SC(i);
				Vinc_SF(i);
				ret=1;
				break;
			}
		}
		
		os_tsk_pass();
	}
	
	if(ret) 
	{
		//VINC_DATREQ=VINC_DATREQ_H;
		GPIO_WriteBit(GPIOD, VNC_DSR, Bit_SET);
		com_put3ww(0x18);						//BaudRate 9600
		com_put3ww(0x20);
		com_put3ww(0x38);
		com_put3ww(0x41);
		com_put3ww(0x00);
		com_put3ww(0x0D);
		Vinc_GetPrompt();

		com_put3ww(0x1A);
		com_put3ww(0x20);
		com_put3ww(0x08);
		com_put3ww(0x00);
		com_put3ww(0x0D);
		Vinc_GetPrompt();

		com_put3ww(0x1B);
		com_put3ww(0x20);
		com_put3ww(0x04);
		com_put3ww(0x0D);
		Vinc_GetPrompt();

		SendKalibrator("KNID\r",answ);
		strncpy(kalivers,&answ[6],14);
		SendKalibrator("KHDNR\r",answ);
		strncpy(dosnr,&answ[7],8);
	}
	
	return(ret);
}

void SendKalibrator(char *str,char *answer)
{
	int i,j,aktsend,index=0;
	int ch;

	VincHand=1;
	
	do
	{
		com_put3ww(0x84);
		com_put3ww(0x0D);
		aktsend=GetLength();
		for(i=0;i<aktsend;i++)
		{
//			j=5000;
			j = 50000;

			while((ch=com_getchar3())==-1 && j--)
				os_tsk_pass();
		}
		
		Vinc_GetPrompt();
	}while(aktsend>0);
	
	aktsend=strlen(str);
	while((ch=com_getchar3())!=-1);
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
		if(Vinc_GetPrompt())
		{
			while((ch=com_getchar3())!=-1);
			
			do
			{
				com_put3ww(0x84);
				com_put3ww(0x0D);
				aktsend=GetLength();
				if(aktsend==0)
				{
					Vinc_GetPrompt();
					os_dly_wait(15);
				}
			}while(aktsend==0);
			
			j=0;
			do
			{
				i = 50000;
//				i = 12000;

				while((ch=com_getchar3())==-1 && i--)
					os_tsk_pass();
				
				if(i>0)
					answer[index++]=ch;
				
				j++;
			}while(j<aktsend);
		}
	}while(answer[index-1]!=0x0D);
	
	Vinc_GetPrompt();
	answer[index]=0;
}

// 测量参数获取，Gavin，2019-07-27 星期六 09:49:56
float GetKaliVal(float *sollw)
{
	float dmsave,ret=0,rsoll=0;
	int forcesave,i;
	char answ[20];

	if(StartKalibrator()) 
	{
		dmsave=dmeing;
		forcesave=ForceLimit;

		dmeing=20.0f;
		ForceLimit=(int)*sollw;
		SendKalibrator("KNULL\r",answ);
		mekabef();
		os_dly_wait(100);
		i=0;
		do 
		{
			SendKalibrator("KGETVAL\r",answ);
			if(strncmp(answ,"KGETVAL",7)==0) 
			{
				ret+=atof(&answ[8]);
				rsoll+=GetDoseAvrg(0)/NwtFak;
				i++;
			}
		}while(i<5);
		*sollw=rsoll/5;
		StepSperr=__FALSE;
		//STEP_ENA=STEP_ENA_H;
		GPIO_WriteBit(PORT_B, M_EN, Bit_SET);
		
		backreset();		
		dmeing=dmsave;
		ForceLimit=forcesave;
	}
	return(ret/5);
}

int GetLength(void)
{
	int i,ch,ch1,ret=0;

//	i=50000;
	i=120000;

	while((ch=com_getchar3())==-1 && i--)
		os_tsk_pass();
	
	if(i>0)
	{
			i = 50000;
//		i = 12000;

		while((ch1=com_getchar3())==-1 && i--)
			os_tsk_pass();
		
		if(ch1==0x0D)
			ret=ch;
	}
	
	return(ret);	
}

void PrintBold(void)
{
	if(druckmode==1) 
	{
//#ifndef CHINAPRINTER
		if(vers[47]==0x30) 
		{
			SendPrinter("\033G");
		}
//#endif
	}
	else 
	{
		if(PrintType == 0) 						// 0, 台式打印机 1，微型打印机标志
		{
			if(PrinterLanguage==1) 
			{
				SendPrinter("\033(s3B\033&k10.35H");
			}
		}
		else
		{
			SendPrinter("\x1B!\x02");
		}
	}		
}

void PrintCondensed(void)
{
//	if(PrinterLanguage==0)
//		SendPrinter("\033(s4S");
}


void PrintNorm(void)
{
	if(druckmode==1)  							// 1，INT内部打印机
	{
//#ifndef CHINAPRINTER
		if(vers[47]==0x30)  					// 0x30, 德国打印机
		{
			SendPrinter("\033H");
		}
//#endif
	}
	else  													// 0，EXT外部打印机
	{
		if(PrintType == 0) 						// 0, 台式打印机 1，微型打印机标志
		{
			if(PrinterLanguage==1)
			{
				SendPrinter("\033(s0B\033&k10.35H");
		//		SendPrinter("\033(s4S");
			}
		}
		else
		{
			SendPrinter("\x1B!\x02");
		}
	}
}

