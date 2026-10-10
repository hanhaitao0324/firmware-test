#include "tbh125.h"
#include "usart.h"
#include "delay.h"

#include "usbd_cdc_vcp.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

OS_TID DispTask;                        /* assigned task id of task: phase_a */
OS_TID LEDTask;
OS_TID CommTask; 
OS_TID BusyTask;
OS_TID USART3Task;

extern void init_240064(void);
extern void Init_Device(void);

__task void display(void);
__task void LEDRefresh(void);
__task void Communicate(void);
__task void BusyComm(void);
extern __task void UART3Send(void);

extern BOOL BusyFlg;
extern const unsigned char Init_vers[];
extern void USB_Init(void);

extern unsigned int ueberlauf; 					// 循环时间，Gavin, 2022-02-09 周三 10:09:21

//#define DHCP_TOUT  50                        /* DHCP timeout 5 seconds      */

float mebuD[100],mebuH[100],mebuT[100];


BOOL MessAbr=__FALSE;
BOOL MessPause=__FALSE;
BOOL MessFlg=__FALSE;
BOOL JawZeroed=__FALSE;
//U32  dhcp_tout;

unsigned int aktLanguage=0;
unsigned int LaeEinh=0;
unsigned int HardEinh=0;
float MeasSpeed=2.3;
float NwtFak=18.0;
int Force=20;
int AutoPrint;
unsigned int MeasurePause=0;
int Sensibility=10;
unsigned int MeasureMethod=0,HardOn=1,DiamOn=1,ThickOn=1,AutoMess=0,DatenAusgabe=0;
int ForceLimit;
float DragJaw=0;
float DirectDiam=5;
unsigned int DirectNumD=10;
unsigned int DirectNumH=10;
//extern char rsbuf2[];
//extern unsigned int rsstatus2;
extern char id[];
extern int eichpos,terminologie,autodiam;
unsigned int pruefanz,pruefanzd,pruefanzdi;
volatile unsigned int abranz; 
extern float laengfak,kraftfak;
int LastMeasAnz=0,dmena=1,dickena=0;
extern char kalimode,bueb,dueb;
extern unsigned int insttype;
extern unsigned int druckmode,PrinterLanguage;

unsigned char sp=0;
extern unsigned char dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
extern float dmeing,dickeing;
extern char buf[150];
extern float Hardness,Diameter,Thickness;
extern float eichmas;
extern char messDate[11];
extern char messTime[6];
extern char kalivers[],dosnr[];
extern char kaliverso[],dosnro[];

extern char messDate[11];
extern char messTime[6];
unsigned int insttype;

extern float minb,mind,mint,maxb,maxt,maxd,mittd,mittb,mittt,srelb,srelt,sreld;
extern float sabwb,sabwd,sabwt;


extern unsigned int PrinterPaper,PrinterLanguage,PrinterFrames,MesAbr,DatFormat;
extern char bkal,dkal;
extern float sollb[3],istb[3],solld[3],istd[3],lasbnwt,lasdiam;
extern short NullKorr;

//volatile int AD12Bit=0;

char *text0[4]={"ANZL","NUMB","NUMB","NUMB"};
char *text1[4]={"DURM","DIAM","DIAM","DIAM"};
char *text2a[4]={"ERGB","REST","REST","REST"};
char *text2b[4]={"AUSW","STAT","STAT","STAT"};
char *text2[4]={"GESW","SPED","SPED","SPED"};
char *text2c[4]={"KRAF","FORC","FORC","FORC"};
char *text3[4]={"DRUK","PRIN","PRT ","PRT "};
char *text4[4]={"KALI","CALI","KALI","KALI"};
char *text5[4]={"KONT","CONT","CONT","CONT"};

char *text6[4]={"EINS","SETT","SETT","SETT"};
char *text7[4]={"BRUC","HARD","HARD","HARD"};
char *text8[4]={"DRUC","PRIN","PRIN","PRIN"};
char *text9[4]={"KEN ","TING","TING","TING"};
char *text10[4]={"ENDM","BLCK","BLCK","BLCK"};

char *text11[4]={"NULL","ZERO","ZERO","ZERO"};
char *text12[4]={"EN  ","JAW ","JAW ","JAW "};
char *text13[4]={"KON1","CON1","CON1","CON1"};
char *text14[4]={"KON2","CON2","CON2","CON2"};
char *text15[4]={"KON3","CON3","CON3","CON3"};

// SETT设置菜单的子菜单---------------------------
char *text16[4]={"PAUS","PAUS","PAUS","PAUS"};
char *text17[4]={"KPOS","CPOS","CPOS","CPOS"};
char *text18[4]={"GEWT","WEIT","WEIT","WEIT"};

char *text19[4]={"EINB","UNIH","UNIH","UNIH"};
char *text20[4]={"EIND","UNID","UNID","UNID"};

char *text21[4]={"STUN","HOUR","HOOR","HOUR"};
char *text22[4]={"MINU","MINU","MINU","MINU"};
char *text23[4]={"TAG ","DAY ","DAY ","DAY "};
char *text24[4]={"MONA","MONT","MONT","MONT"};
char *text25[4]={"JAHR","YEAR","YEAR","YEAR"};
char *text28[4]={"NFKT","NFCT","NFCT","NFCT"};

char *text26[4]={"DEMO","DISM","DISM","DISM"};
char *text27[4]={"NTAG","OUNT","OUNT","OUNT"};

char *text29[4]={"DICK","THIC","THIC","THIC"};

char *text30[4]={"SPRA","LANG","LANG","LANG"};
char *text31[4]={"DEUT","GERM","GERM","GERM"};
char *text32[4]={"ENGL","ENGL","ENGL","ENGL"};
char *text33[4]={"VERS","VERS","VERS","VERS"};
char *text34[4]={"DATF","DATF","DATF","DATF"};
char *text35[4]={"EURO","EURO","EURO","EURO"};
char *text36[4]={"USA ","USA ","USA ","USA "};
char *text37[4]={"METH","METH","METH","METH"};
char *text38[4]={"GESW","SPEE","SPEE","SPEE"};
char *text39[4]={"KRAF","FORC","FORC","FORC"};

char *text40[4]={"BRUC","HARD","HARD","HARD"};
char *text41[4]={"DURC","DIAM","DIAM","DIAM"};
char *text42[4]={"DICK","THIC","THIC","THIC"};

char *text43[4]={" EIN","  ON","  ON","  ON"};
char *text44[4]={" AUS"," OFF"," OFF"," OFF"};
char *text45[4]={"DOSE","CELL","CELL","CELL"};

char *text46[4]={"TERM","TERM","TERM","TERM"};
char *text47[4]={"KALI","CALI","CALI","CALI"};
char *text48[4]={"JUST","ADJU","ADJU","ADJU"};

char *text49[4]={"JUST","ADJU","ADJU","ADJU"};
char *text50[4]={"KALI","CALI","CALI","CALI"};

char *text51[4]={"KEIN","NO P","NO P","NO P"}; 					// 没有链接打印机
char *text52[4]={"DRUC","RINT","RINT","RINT"};

char *text53[4]={"WIED","REPE","REPE","REPE"};
char *text54[4]={"ERHO","AT  ","AT  ","AT  "};

char *text55[4]={"AUTO","AUTO","AUTO","AUTO"};

char *text56[4]={"DRUC","PRIN","PRIN","PRIN"};
char *text57[4]={"INT ","INT ","INT ","INT "};
char *text58[4]={"EXT ","EXT ","EXT ","EXT "};

char *text59[4]={"DSPR","PRLG","PRLG","PRLG"};
char *text60[4]={"ASCI","ASCI","ASCI","ASCI"};
char *text61[4]={"PCL ","PCL ","PCL ","PCL "};
char *text62[4]={"NUKO","ZECO","ZECO","ZECO"};

char *text63[4]={"PAPI","NO P","NO P","NO P"}; 					// 打印机缺纸
char *text64[4]={"ER  ","APER","APER","APER"};

char *text65[4]={"DRU ","PRI ","PRI ","PRI "};
char *text66[4]={"ERRO","ERRO","ERRO","ERRO"};

char *text67[4]={"AUME","AUME","AUME","AUME"};
char *text68[4]={"DAUS","DOUT","DOUT","DOUT"};

// 增加菜单,206-1-1,GAVIN,2019-07-24 星期三 20:51:29
char *text69[4]={"DATE","DATE","DATE","DATE"};
char *text70[4]={"PWMG","PWMG","PWMG","PWMG"};
char *text71[4]={"PWST","PWST","PWST","PWST"};

// 增加菜单，Gavin，2022-02-09 周三 09:40:01
char *text72[4]={"SCHW","THRE","THRE","THRE"};
char *text73[4]={"ZYKL","CYCL","CYCL","CYCL"};

char *texts1[4]={"BMIN","HMIN","HMIN","HMIN"};
char *texts2[4]={"BMAX","HMAX","HMAX","HMAX"};
char *texts3[4]={"BM-M","HM-M","HM-M","HM-M"};
char *texts4[4]={"BMIT","HAVR","HAVR","HAVR"};
char *texts5[4]={"BSTA","HSTA","HSTA","HSTA"};
char *texts6[4]={"BREL","HREL","HREL","HREL"};

char *texts7[4]={"DMIN","DMIN","DMIN","DMIN"};
char *texts8[4]={"DMAX","DMAX","DMAX","DMAX"};
char *texts9[4]={"DM-M","DM-M","DM-M","DM-M"};
char *texts10[4]={"DMIT","DAVR","DAVR","DAVR"};
char *texts11[4]={"DSTA","DSTA","DSTA","DSTA"};
char *texts12[4]={"DREL","DREL","DREL","DREL"};

char *texts13[4]={"TMIN","TMIN","TMIN","HMIN"};
char *texts14[4]={"TMAX","TMAX","TMAX","HMAX"};
char *texts15[4]={"TM-M","TM-M","TM-M","HM-M"};
char *texts16[4]={"TMIT","TAVR","TAVR","HAVR"};
char *texts17[4]={"TSTA","TSTA","TSTA","HSTA"};
char *texts18[4]={"TREL","TREL","TREL","HREL"};


char **stexte[3][6]=
{
	{texts1,texts2,texts3,texts4,texts5,texts6},
	{texts7,texts8,texts9,texts10,texts11,texts12},
	{texts13,texts14,texts15,texts16,texts17,texts18}
};

char **texte[74]=
{
	text0,text1,text2,text3,text4,text5,text6,text7,text8,text9,
	text10,text11,text12,text13,text14,text15,text16,text17,text18,text19,
	text20,text21,text22,text23,text24,text25,text26,text27,text28,text29,
	text30,text31,text32,text33,text34,text35,text36,text37,text38,text39,
	text40,text41,text42,text43,text44,text45,text46,text47,text48,text49,
	text50,text51,text52,text53,text54,text55,text56,text57,text58,text59,
	text60,text61,text62,text63,text64,text65,text66,text67,text68,
	// Gavin, 206-1-3, 2019-07-25 星期四 09:45:59
	text69,text70,text71,
	// Gavin, Gavin, 2022-02-09 周三 11:56:12
	text72,text73
};

char **mtexte[9]={text0,text2a,text2b,text2,text2c,text3,text4,text5,text6};
char **mtextej[9]={text0,text2a,text2b,text2,text2c,text3,text49,text50,text6};

char *textein[4]={"EIN ","ON  ","ON  ","ON  "};
char *textaus[4]={"AUS ","OFF ","OFF ","OFF "};

U64 com_stack[2400]; //800/8];                        /* A bigger stack for tcp_task */
U64 disp_stack[2600]; //1200   800/8];                        /* A bigger stack for tcp_task */
unsigned char vers[100];
unsigned char NOO[3];
unsigned char NOG[1];
unsigned char NOD[5] = " 3.00";
unsigned char NOF[3] = "OLD";
unsigned char Print_int[1]="0";
unsigned char Print_exit[1]="1";
unsigned char	HardVersion[7] ;
unsigned char	FirstHardVersion[7] = "v3.00.0" ;
char IdNum[14];
USB_OTG_CORE_HANDLE    USB_OTG_dev;

__task void startup (void) 
{
	Init_Device(); 												// 初始化

	GPIO_WriteBit(PORT_A, M_RST, Bit_RESET);
	GPIO_WriteBit(PORT_A, M_RST, Bit_SET);
	
	
	
	readeepbuf((unsigned char*)NOO,ADRNEWEEPROM,3);
	if(NOO[0] == 'O' && NOO[1] == 'L' && NOO[2] == 'D')		//判断是否第一次写入程序
	{
		//readeepbuf((unsigned char *)NOG,ADRVERSION+1,1);
		//if(NOG[0]=='0')																			//只是为了测试版本号问题加入,因为客户第一次写入了程序版本号是0.01,如果不使用上位机进行更改则没办法改成3.00，所以加入这句强制改成3.00
		//	writeepbuf((unsigned char*)NOD,ADRVERSION,5);
		readeepbuf((unsigned char *)NOD,ADRVERSION,5);
		if(strncmp((char *)NOD,(char *)Init_vers,5)!=0)
		{
			strncpy((char *)NOD,(char *)Init_vers,5);
			writeepbuf((unsigned char*)NOD,ADRVERSION,5);
		}
		
		readeepbuf((unsigned char *)vers,ADRVERSION,86);
		druckmode=readinteep(ADRDRUCKMODE);
		
		readeepbuf((unsigned char *)HardVersion,ADRHARDWAREVER,7);			//读硬件版本号
		if(HardVersion[0]!='v')																					//如果读不出'v'说明还没存	
		{
			writeepbuf((unsigned char*)FirstHardVersion,ADRHARDWAREVER,7);//写硬件版本号到eeprom
			readeepbuf((unsigned char *)HardVersion,ADRHARDWAREVER,7);		//读硬件版本号到HardVersion数组
		}
	}
	else																															//第一次写入程序时默认使用vers的原始参数，后续通过上位机进行修改
	{
		writeepbuf((unsigned char*)Init_vers,ADRVERSION,86);						//写入内部参数vers,第一次可以在Version文件更改
		writeepbuf((unsigned char*)FirstHardVersion,ADRHARDWAREVER,7);	//写入硬件版本号
		
		readeepbuf((unsigned char *)vers,ADRVERSION,86);								//读出内部参数
		readeepbuf((unsigned char *)HardVersion,ADRHARDWAREVER,7);			//读出硬件版本号
		writeepbuf((unsigned char*)NOF,ADRNEWEEPROM,3);									//写标志到eeprom,用于判断是否第一次写入程序
	}
	
	
	
	aktLanguage=readinteep(ADRLANGUAGE); 	// 读取语言,德语,英语
	if(aktLanguage>1) 										// 语种变量差错
		aktLanguage=0;
	sp=aktLanguage; 											//语种变量赋值								
	

	insttype=990;
	dmena=vers[45] & 0x0F;
	if(dmena) 
		insttype=991;
	dickena=vers[46] & 0x0F;
	if(dickena) 
		insttype=992;

	// 创建任务-------------------------
	os_tsk_prio_self (100);
	LEDTask=os_tsk_create(LEDRefresh,2);
	CommTask=os_tsk_create_user(Communicate,2,&com_stack,sizeof(com_stack));
	BusyTask=os_tsk_create(BusyComm,2);
	USART3Task=os_tsk_create(UART3Send,2);
	DispTask = os_tsk_create_user(display,2,&disp_stack,sizeof(disp_stack));     /* start task lcd                   */
	
	beep();
	com_initialize();													//初始化串口

	Vinc_Reset();
	
	os_tsk_delete_self();                     /* Terminate Task */
}



/* Main Program */
int main (void) 
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SCB->VTOR = FLASH_BASE | 0xC000;//设置偏移量
	
	RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_OTG_FS,ENABLE);		//?? OTG_FS IO?
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, DISABLE);
	RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_OTG_FS,DISABLE);

	uart_init(115200);																		//串口1初始化,USB虚拟串口需要
	delay_init(84);

	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_CDC_cb,&USR_cb);
	getid(id);
	strncpy(IdNum,(char *)&id[0],14);
	os_sys_init(startup);                        /* Init RTX and start startup */
}

__task void LEDRefresh(void)
{
	while(1) 
	{
		os_dly_wait(1);
	}
}

// 主菜单REST执行函数，查阅测量数据记录-------------------------------------------------
void showresult(int maxanz)
{
	int index,subindex,ch,countflg;

	index=0;
	if(HardOn && !DiamOn && !ThickOn) 
	{
		subindex=0;
	}
	
	if(!HardOn && DiamOn && !ThickOn) 
	{
		subindex=1;
	}
	
	if(!HardOn && !DiamOn && ThickOn) 
	{
		subindex=2;
	}
	
	if(HardOn && DiamOn && !ThickOn) 
	{
		subindex=0;
	}
	
	if(HardOn && !DiamOn && ThickOn) 
	{
		subindex=0;
	}
	
	if(!HardOn && DiamOn && ThickOn) 
	{
		subindex=1;
	}
	
	if(HardOn && DiamOn && ThickOn) 
	{
		subindex=0;
	}

	if(maxanz==0) 
		return;
	
	do 
	{
		if(subindex==1) 
		{
			sprintf(buf,"%02d D",index+1);
			anzout1(buf);
			if(mebuD[index]>99000) 
			{
				anzout2("  NA");
			}
			else 
			{
				if(LaeEinh==0) 														//mm
				{
					sprintf(buf,"%04.0f",mebuD[index]*100);
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 																			//in
				{
					Diameter/=laengfak;
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",(mebuD[index]/laengfak)*1000);
				}	
				anzout2(buf);
			}
		}
		
		if(subindex==0) 
		{
			if(sp==0)
				sprintf(buf,"%02d B",index+1);
			else
				sprintf(buf,"%02d H",index+1);
			anzout1(buf);
			if(mebuH[index]>99000) 
			{
				if(mebuH[index]==99996)
					anzout2("  FL");
				else 
					anzout2("  NA");
			}
			else 
			{
				if(HardEinh==0) 											// N
				{
					sprintf(buf,"%04.0f",mebuH[index]);
					dp1=dp2=dp3=dp4=0;
				}	
				else 																	//kp or Sc
				{
					sprintf(buf,"%04.0f",(mebuH[index]/kraftfak)*10);
					dp1=dp2=dp3=dp4=0;
					dp3=1;
				}	
				anzout2(buf);
			}
		}
		
		if(subindex==2) 
		{
			if(sp==0) 
				sprintf(buf,"%02dDI",index+1);
			else
				sprintf(buf,"%02d T",index+1);

			anzout1(buf);
			if(mebuT[index]>99000) 
			{
				anzout2("  NA");
			}
			else 
			{
				if(LaeEinh==0) 											//mm
				{
					sprintf(buf,"%04.0f",mebuT[index]*100);	
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 																//in
				{
					Diameter/=laengfak;
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",(mebuT[index]/laengfak)*1000);
				}	
				anzout2(buf);
			}
		}
		
		ch=gettast();													//获取按键码

		if(ch==TRU) 		// 右下键
		{
			countflg=0;
			if(HardOn && !DiamOn && !ThickOn) 
			{
				subindex=0;
				countflg=1;
			}
			
			if(!HardOn && DiamOn && !ThickOn) 
			{
				subindex=1;
				countflg=1;
			}
			
			if(!HardOn && !DiamOn && ThickOn) 
			{
				subindex=2;
				countflg=1;
			}
			
			if(HardOn && DiamOn && !ThickOn) 
			{
				if(subindex==0) 
					subindex=1;
				else 
				{
					countflg=1;
					subindex=0;
				}
			}
			
			if(HardOn && !DiamOn && ThickOn) 
			{
				if(subindex==2) 
					subindex=0;
				else 
				{
					countflg=1;
					subindex=2;
				}
			}
			
			if(!HardOn && DiamOn && ThickOn) 
			{
				if(subindex==2) 
					subindex=1;
				else 
				{
					countflg=1;
					subindex=2;
				}
			}
			
			if(HardOn && DiamOn && ThickOn) 
			{
				if(subindex>0) 
					subindex--;
				else 
				{
					countflg=1;
					subindex=2;
				}
			}
			
			if(countflg) 
			{
			 	if(index > 0) 
			 		index--;
			}
		}
		
		if(ch==TRO) 		// 右上键
		{
			countflg=0;
			if(HardOn && !DiamOn && !ThickOn) 
			{
				subindex=0;
				countflg=1;
			}
			
			if(!HardOn && DiamOn && !ThickOn) 
			{
				subindex=1;
				countflg=1;
			}
			
			if(!HardOn && !DiamOn && ThickOn) 
			{
				subindex=2;
				countflg=1;
			}
			
			if(HardOn && DiamOn && !ThickOn) 
			{
				if(subindex==0) 
					subindex=1;
				else 
				{
					countflg=1;
					subindex=0;
				}
			}
			
			if(HardOn && !DiamOn && ThickOn) 
			{
				if(subindex==0) 
					subindex=2;
				else 
				{
					countflg=1;
					subindex=0;
				}
			}
			
			if(!HardOn && DiamOn && ThickOn) 
			{
				if(subindex==1) 
					subindex=2;
				else 
				{
					countflg=1;
					subindex=1;
				}
			}
			
			if(HardOn && DiamOn && ThickOn) 
			{
				if(subindex<2) 
					subindex++;
				else 
				{
					countflg=1;
					subindex=0;
				}
			}
			
			if(countflg) 
			{
			 	if(index < maxanz-1) 
			 		index++;
			}
		}
	}while(ch!=TMO);	// 中上键
}

// 主菜单STAT执行函数，查阅计算结果记录-------------------------------------------------
void showstat(void)
{
	int index,subindex,ch;
	float min,max,mitt,sabw,srel;

	if(HardOn && !DiamOn && !ThickOn) 
	{
		index=0;
	}
	
	if(!HardOn && DiamOn && !ThickOn) 
	{
		index=1;
	}
	
	if(!HardOn && !DiamOn && ThickOn) 
	{
		index=2;
	}
	
	if(HardOn && DiamOn && !ThickOn) 
	{
		index=0;
	}
	
	if(HardOn && !DiamOn && ThickOn) 
	{
		index=0;
	}
	
	if(!HardOn && DiamOn && ThickOn) 
	{
		index=1;
	}
	
	if(HardOn && DiamOn && ThickOn) 
	{
		index=0;
	}
	
	subindex=0;
	calcauswert();									// 根据测试记录计算结果
	switch(index) 
	{
		case 0:
			min=minb;
			max=maxb;
			mitt=mittb;
			sabw=sabwb;
			srel=srelb;
			break;
		case 1:
			min=mind;
			max=maxd;
			mitt=mittd;
			sabw=sabwd;
			srel=sreld;
			break;
		case 2:
			min=mint;
			max=maxt;
			mitt=mittt;
			sabw=sabwt;
			srel=srelt;
			break;
	}

	do 
	{
		if(subindex==0) 										// 显示min
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);
			if(index==0) 
			{
				if(HardEinh==0) 
				{
					dp1=dp2=dp3=dp4=0;
//					dp3=1;
//					sprintf(buf,"%04.0f",(min/*/kraftfak*/)*10);			// Gavin，2021-07-01 星期四 19:06:41
					sprintf(buf,"%04.0f",min);
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp3=1;
					sprintf(buf,"%04.0f",(min/*/kraftfak*/)*10);
				}
			}
			else 
			{					
				if(LaeEinh==0) 
				{
					sprintf(buf,"%04.0f",min*100);
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",(min/*/laengfak*/)*1000);
				}
			}	
			anzout2(buf);
			
/*			if(LaeEinh==0) {
				sprintf(buf,"%04.0f",min*100);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
			}
			else {
				dp1=dp2=dp3=dp4=0;
				dp1=1;							
				if(index==0) {
					sprintf(buf,"%04.0f",(min/kraftfak)*1000);
				}
				else {
					sprintf(buf,"%04.0f",(min/laengfak)*1000);
				}
			}	
			anzout2(buf);*/
		}
		
		if(subindex==1) 										// 显示max
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);
			if(index==0) 
			{
				if(HardEinh==0) 
				{
					dp1=dp2=dp3=dp4=0;
					// dp3=1;																	// Gavin，2021-07-01 星期四 19:08:16
					//sprintf(buf,"%04.0f",(max/*/kraftfak*/)*10); 		// Gavin，2021-07-01 星期四 19:08:16
					sprintf(buf,"%04.0f",max);
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp3=1;
					sprintf(buf,"%04.0f",(max/*/kraftfak*/)*10);
				}
			}
			else 
			{					
				if(LaeEinh==0) 
				{
					sprintf(buf,"%04.0f",max*100);
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",(max/*/laengfak*/)*1000);
				}
			}	
			anzout2(buf);

/*			if(LaeEinh==0) {
				sprintf(buf,"%04.0f",max*100);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
			}
			else {
				dp1=dp2=dp3=dp4=0;
				dp1=1;							
				if(index==0) {
					sprintf(buf,"%04.0f",(max/kraftfak)*1000);
				}
				else {
					sprintf(buf,"%04.0f",(max/laengfak)*1000);
				}
			}	
			anzout2(buf);*/
		}
		
		if(subindex==2) 										// 显示max - min
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);

			if(index==0) 
			{
				if(HardEinh==0) 
				{
					dp1=dp2=dp3=dp4=0;
					// dp3=1;          						 // Gavin,2021-07-01 星期四 19:09:50
					//sprintf(buf,"%04.0f",((max-min)/*/kraftfak*/)*10); 				// Gavin，2021-07-01 星期四 19:09:50
					sprintf(buf,"%04.0f",(max-min));
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp3=1;
					sprintf(buf,"%04.0f",((max-min)/*/kraftfak*/)*10);
				}
			}
			else 
			{					
				if(LaeEinh==0) 
				{
					sprintf(buf,"%04.0f",(max-min)*100);
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",((max-min)/*/laengfak*/)*1000);
				}
			}	
			anzout2(buf);
			
/*			if(LaeEinh==0) {
				sprintf(buf,"%04.0f",(max-min)*100);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
			}
			else {
				dp1=dp2=dp3=dp4=0;
				dp1=1;							
				if(index==0) {
					sprintf(buf,"%04.0f",((max-min)/kraftfak)*1000);
				}
				else {
					sprintf(buf,"%04.0f",((max-min)/laengfak)*1000);
				}
			}	
			anzout2(buf);*/
		}
			
		if(subindex==3) 										// 显示平均值
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);
			if(index==0) 
			{
				if(HardEinh==0) 
				{
					dp1=dp2=dp3=dp4=0;
					dp3=1;
					sprintf(buf,"%04.0f",(mitt/*/kraftfak*/)*10);
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp2=1;
					sprintf(buf,"%04.0f",(mitt/*/kraftfak*/)*100);
				}
			}
			else 
			{					
				if(LaeEinh==0) 
				{
					sprintf(buf,"%04.0f",mitt*100);
					dp1=dp2=dp3=dp4=0;
					dp2=1;
				}
				else 
				{
					dp1=dp2=dp3=dp4=0;
					dp1=1;							
					sprintf(buf,"%04.0f",(mitt/*/laengfak*/)*1000);
				}
			}	
			anzout2(buf);
		}
		
		if(subindex==4) 										// 显示标准偏差
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);
			if(index==0) 
			{
				dp1=dp2=dp3=dp4=0;
				if(HardEinh==0) 
				{
					dp3=1;
					sprintf(buf,"%04.0f",(sabw/*/kraftfak*/)*10);
				}
				else
				{
					dp2=1;
					sprintf(buf,"%04.0f",(sabw/*/kraftfak*/)*100);
				}
			}
			else 
			{
				dp1=dp2=dp3=dp4=0;
				if(LaeEinh==0) 
				{
					dp2=1;							
					sprintf(buf,"%04.0f",(sabw/*/laengfak*/)*100);
				}
				else
				{
					dp1=1;							
					sprintf(buf,"%04.0f",(sabw/*/laengfak*/)*1000);
				}
			}
//      dp1=dp2=dp3=dp4=0;						// Gavin，2021-07-01 星期四 19:22:37
//			dp2=1;
			anzout2(buf);
		}
		
		if(subindex==5) 										// 显示偏差率
		{
			sprintf(buf,stexte[index][subindex][sp]);
			anzout1(buf);
			
			dp1=dp2=dp3=dp4=0;
			//dp2=1;
			//sprintf(buf,"%04.0f",srel*100);
			dp3=1;
			sprintf(buf,"%04.0f",srel*10);
			anzout2(buf);
		}
		
		ch=gettast(); 						// 获取按键码

		if(ch==TRO) 							// 右上键
		{
			if(subindex<5) 
				subindex++;
			else 
			{
				subindex=0;

				if(HardOn && !DiamOn && !ThickOn) 
				{
					index=0;
				}
				
				if(!HardOn && DiamOn && !ThickOn) 
				{
					index=1;
				}
				
				if(!HardOn && !DiamOn && ThickOn) 
				{
					index=2;
				}
				
				if(HardOn && DiamOn && !ThickOn) 
				{
					if(index==0) 
						index=1;
					else 
					{
						index=0;
					}
				}
				
				if(HardOn && !DiamOn && ThickOn) 
				{
					if(index==0) 
						index=2;
					else 
					{
						index=0;
					}
				}
				
				if(!HardOn && DiamOn && ThickOn) 
				{
					if(index==1) 
						index=2;
					else 
					{
						index=1;
					}
				}
				
				if(HardOn && DiamOn && ThickOn) 
				{
					if(index<2) 
						index++;
					else 
					{
						index=0;
					}
				}
				
				switch(index) 
				{
					case 0:
						min=minb;
						max=maxb;
						mitt=mittb;
						sabw=sabwb;
						srel=srelb;
						break;
					case 1:
						min=mind;
						max=maxd;
						mitt=mittd;
						sabw=sabwd;
						srel=sreld;
						break;
					case 2:
						min=mint;
						max=maxt;
						mitt=mittt;
						sabw=sabwt;
						srel=srelt;
						break;
				}
			}
		}
		
		if(ch==TRU) 							// 右下键 
		{
			if(subindex>0) 
				subindex--;
			else 
			{
				subindex=5;
				
				if(HardOn && !DiamOn && !ThickOn) 
				{
					index=0;
				}
				
				if(!HardOn && DiamOn && !ThickOn) 
				{
					index=1;
				}
				
				if(!HardOn && !DiamOn && ThickOn) 
				{
					index=2;
				}
				
				if(HardOn && DiamOn && !ThickOn) 
				{
					if(index==1) 
						index=0;
					else 
					{
						index=1;
					}
				}
				
				if(HardOn && !DiamOn && ThickOn) 
				{
					if(index==2) 
						index=0;
					else 
					{
						index=2;
					}
				}
				
				if(!HardOn && DiamOn && ThickOn) 
				{
					if(index==2) 
						index=1;
					else 
					{
						index=2;
					}
				}
				
				if(HardOn && DiamOn && ThickOn) 
				{
					if(index>0) index--;
					else {
						index=2;
					}
				}
				
				switch(index) 
				{
					case 0:
						min=minb;
						max=maxb;
						mitt=mittb;
						sabw=sabwb;
						srel=srelb;
						break;
					case 1:
						min=mind;
						max=maxd;
						mitt=mittd;
						sabw=sabwd;
						srel=sreld;
						break;
					case 2:
						min=mint;
						max=maxt;
						mitt=mittt;
						sabw=sabwt;
						srel=srelt;
						break;
				}
			}
		}
	}while(ch!=TMO); 						// 中上键
}

//2015.05.23-------------------------------------------------------
extern volatile unsigned char PrintType; 	// 打印机类型:0,台式机;1,微打

// 测量---------------------------------------------------------------
void messung(void)
{
	int i,ch,ch1,vez;
	float dmsave,diamout,thickout,hardout;

	if(!autodiam) 
	{
		if(dickena && ThickOn)
		{
			anzout1(texte[29][sp]); 					// THIC
			sprintf(buf,"%04.0f",dickeing*10);
			dp1=dp2=dp3=dp4=0;
			dp3=1;
			anzout2(buf);
			
			do 
			{
				while((ch=gettast())==0);
				beep();
			
				if(ch==TRO) 										// 右上键
				{
					//if(dickeing<25) 
					if(dickeing < 28) 						// 206-6-1
					{
						dickeing+=0.1f;
						sprintf(buf,"%04.0f",dickeing*10);
						anzout2(buf);
						os_dly_wait(100);
						//while(gettasto()==TRO && dickeing<25) 
						while(gettasto()==TRO && dickeing<28) 		// 206-6-2,右上键
						{
							dickeing+=0.1f;
							sprintf(buf,"%04.0f",dickeing*10);
							anzout2(buf);
							os_dly_wait(20);
						}						
					}
					savefloat(&dickeing,ADRDICKEING);
				}
				
				if(ch==TRU) 										// 右下键
				{
					//if(dickeing>1) 
					if(dickeing > 0.2f) 						// 206-6-3
					{
						dickeing-=0.1f;
						sprintf(buf,"%04.0f",dickeing*10);
						anzout2(buf);
						os_dly_wait(100);
						
						//while(gettasto()==TRU && dickeing>1) 
						while(gettasto()==TRU && dickeing>0.2f) 			// 206-6-4,右下键
						{
							dickeing-=0.1f;
							sprintf(buf,"%04.0f",dickeing*10);
							anzout2(buf);
							os_dly_wait(20);
						}
						savefloat(&dickeing,ADRDICKEING);
					}
				}
			}while(ch!=TMO && ch!=TMU); 			// 中上键&中下键
			if(ch==TMO) return; 							// 中上键
		}

		anzout1(texte[1][sp]); 							// DIAM
		sprintf(buf,"%04.0f",dmeing*10);
		dp1=dp2=dp3=dp4=0;
		dp3=1;
		anzout2(buf);

		do 
		{
			while((ch=gettast())==0);
			beep();
		
			if(ch==TRO) 											// 右上键
			{
				//if(dmeing<25) 206-6-5
				if(dmeing<28)
				{
					dmeing+=0.1f;
					sprintf(buf,"%04.0f",dmeing*10);
					anzout2(buf);
					os_dly_wait(100);
					
					//while(gettasto()==TRO && dmeing<25) 206-6-6
					while(gettasto()==TRO && dmeing<28) 		// 右上键
					{
						dmeing+=0.1f;
						sprintf(buf,"%04.0f",dmeing*10);
						anzout2(buf);
						os_dly_wait(20);
					}						
				}
				savefloat(&dmeing,ADRDMEING);
			}
			
			if(ch==TRU) 											// 右下键
			{
				//if(dmeing>1) 206-6-7
				if(dmeing>0.2f)
				{
					dmeing-=0.1f;
					sprintf(buf,"%04.0f",dmeing*10);
					anzout2(buf);
					os_dly_wait(100);
					
					//while(gettasto()==TRU && dmeing>1) 206-6-8
					while(gettasto()==TRU && dmeing>0.2f) 		// 右下键
					{
						dmeing-=0.1f;
						sprintf(buf,"%04.0f",dmeing*10);
						anzout2(buf);
						os_dly_wait(20);
					}
					savefloat(&dmeing,ADRDMEING);
				}
			}
		}while(ch!=TMO && ch!=TMU); 				// 中上键&中下键
		if(ch==TMO) return; 								// 中上键
	}
	
	MesAbr=0;
	abranz=0;
	
	/*2016-9-22 星期四 15:23:00---------------------
	while((ch=gettasto())!=0);
	dp1=dp2=dp3=dp4=0;
	anzout1(texte[11][sp]); 							// ZERO
	anzout2(texte[12][sp]); 							// JAW
	
	if(!AutoMess) 												// AUME = OFF
	{
		while((ch=gettasto())==0);
		beep();
	}
	*/
	
	while((ch=gettasto())!=0);
	dp1=dp2=dp3=dp4=0;
	
	if(!((!ThickOn)&&(!DiamOn)))
	{
		anzout1(texte[11][sp]); 							// ZERO
		anzout2(texte[12][sp]); 							// JAW
		
		if(!AutoMess) 												// AUME = OFF
		{
			while((ch=gettasto())==0);
			beep();
		}
	}
	
	if(ThickOn || DiamOn) 
	{
		
		ch=blbef(0);
		if(ch==0) 
		{
			anzout1("ERRO");
			anzout2("R   ");
			os_dly_wait(2000);
			return;
		}
	}
	
	if(autodiam) 
	{
		while((ch=gettasto())!=0);
		dp1=dp2=dp3=dp4=0;

		if(dickena && ThickOn) 
		{
			anzout1(texte[55][sp]); 					// AUTO
			anzout2(texte[29][sp]); 					// THIC
			while((ch=gettasto())==0);
			beep();
			if(!GetDick()) 
			{
				anzout1("ERRO");
				anzout2("R   ");
				os_dly_wait(2000);
				return;
			}
		}
		
		while((ch=gettasto())!=0);
		anzout1(texte[55][sp]); 						// AUTO
		anzout2(texte[1][sp]); 							// DIAM
		while((ch=gettasto())==0);
		beep();
		
		if(!GetDiameter()) 
		{
			anzout1("ERRO");
			anzout2("R   ");
			os_dly_wait(2000);
			return;
		}
	}

	for(i=0;i<100;i++) 
	{
		mebuD[i]=99999; 										// 直径
		mebuH[i]=99999; 										// 硬度
		mebuT[i]=99999; 										// 厚度
	}
	
	pruefanzd=pruefanz;	
	pruefanzdi=pruefanz;
	gettimstr(messTime);
	getdatstr(messDate);
	MesAbr=0;
	
	for(i=0;i<pruefanz;i++) 
	{
		if(dickena && ThickOn) 							// 直径关闭,厚度打开
		{
			dmsave=dmeing;
			dmeing=dickeing;
			anzout1(texte[29][sp]); 					// DICK/THIC/THIC/THIC,直接/厚度
			anzout2("    ");
			
			if(!AutoMess) 										// AUME = OFF
			{
				while((ch=gettast())==0);
				if(ch==TMO) 										// 中上键
				{
					dmeing=dmsave;
					MesAbr=1;
					break;
				}
			}
			else 
			{
				vez=MeasurePause*10;
				while(vez-->0) 
				{
					os_dly_wait(100);
				}	
			}
			
			mdbef(1);
			if(MesAbr) break;
			mebuT[i]=Thickness;							//厚度测试数据保存
			dp1=dp2=dp3=dp4=0;
			
			if(Thickness>99000) 
			{
				anzout2("  NA");
			}
			else 
			{
				if(LaeEinh==0) 
				{
					sprintf(buf,"%04.0f",Thickness*100);
					dp2=1;
				}
				else 
				{
					Thickness/=laengfak;
					dp1=1;							
					sprintf(buf,"%04.0f",Thickness*1000);
				}	
				anzout2(buf);
			}
			
			if(!AutoMess) 
			{
				while((ch=gettast())==0);
			}
			else 
			{
				if(MeasurePause==0) 
				{
					os_dly_wait(500);
				}
				else 
				{
					vez=MeasurePause*10;
					while(vez-->0) 
					{
						os_dly_wait(100);
					}	
				}
			}
			
			anzout1(texte[7][sp]); 						// "HARD",硬度
			anzout2("    ");
			dmeing=dmsave;
			
			if(ch==TMO) 											// 中上键
			{
				MesAbr=1;
				break;
			}
		}
		else
		{
			anzout1(texte[7][sp]); 						// "HARD"
			anzout2("    ");
			//dmeing=dmsave;
			
			if(!AutoMess) 										// AUME = OFF
			{
				while((ch=gettast())==0);
				if(ch==TMO) 										// 中上键
				{
					dmeing=dmsave;
					MesAbr=1;
					break;
				}
			}
			else 
			{
				vez=MeasurePause*10;
				while(vez-->0) 
				{
					os_dly_wait(100);
				}	
			}
		}
		
		if(HardOn || DiamOn) 
		{
			if(HardOn) 
			{
				
				mebef();
		 	}
			else 
			{
				mdbef(0);
			}
			
			if(MesAbr) break;
			if(HardOn)
				mebuH[i]=Hardness;									//厚度测试数据保存
			if(dmena && DiamOn)
				mebuD[i]=Diameter;									//直径测试数据保存
			
			if(HardOn) 
			{
				anzout1(texte[7][sp]);
				dp1=dp2=dp3=dp4=0;
				
				if(Hardness>99000) 
				{
					if(Hardness==99996)
						anzout2("  FL");
					else
						anzout2("  NA");
				}
				else 
				{
					if(HardEinh==0) 
					{
						sprintf(buf,"%04.0f",Hardness);
					}	
					else 
					{
						Hardness/=kraftfak;
						Hardness*=10;
						sprintf(buf,"%04.0f",Hardness);
						dp3=1;
					}	
					anzout2(buf);
				}																		
				os_dly_wait(1500);		
			}
			
			dp1=dp2=dp3=dp4=0;
			if(dmena && DiamOn) 
			{
				anzout1(texte[1][sp]);
				if(Diameter>99000) 
				{
					anzout2("  NA");
				}
				else 
				{
					if(LaeEinh==0) 
					{
						sprintf(buf,"%04.0f",Diameter*100);
						dp2=1;
					}
					else 
					{
						Diameter/=laengfak;
						dp1=1;							
						sprintf(buf,"%04.0f",Diameter*1000);
					}	
					anzout2(buf);
				}
				os_dly_wait(1500);		
			}
			
			if(MeasurePause>0) 
			{
				vez=MeasurePause*10;
				while(vez-->0) 
				{
					os_dly_wait(100);
					if(gettasto()==TMO) 					// 中上键
					{
						MesAbr=1;
						break;
					}
				}
			}
			
			if(gettasto()==TMO) 							// 中上键
			{
				MesAbr=1;
				break;
			}
		}
		
		if(DatenAusgabe)										//判断是测试一次后直接通过串口发出测试结果，有一个参数打开这个功能（未确认）,通过DOUT打开
		{
			diamout=Diameter;
			thickout=mebuT[i];
			hardout=Hardness;
			
			if(!dmena || !DiamOn) 
				diamout=99999.0;
			
			if(!dickena || !ThickOn) 
				thickout=99999.0;
			
			if(!HardOn) 
				hardout=99999.0;
			
			sprintf(buf,"%9.2f;%9.2f;%6.0f\r\n",thickout,diamout,hardout);
			SendRS232(buf);
		}
	}
	
	LastMeasAnz=i;
	if(MesAbr) 
		abranz=i;
	
	if(AutoPrint)
	{
		dp1=dp2=dp3=dp4=0;
		anzout1(texte[8][sp]); 							// PRIN
		anzout2(texte[9][sp]); 							// TING
		ch=0;
		
		if(druckmode==0) 										// 打印机模式,0:USB,1:COM
		{
			StartPrinter();
			if(PrintType == 0) 								// 台式打印机
			{
				while((ch1=druckergebnis())!=0 && ch!=TMO) 					// ...&中上键
				{
					if(ch1==2)
					{
						anzout1(texte[63][sp]);			//打印机缺纸
						anzout2(texte[64][sp]);			
					}
					else
					{
						if(ch1==3)
						{
							anzout1(texte[65][sp]);		//打印机报错
							anzout2(texte[66][sp]);
						}
						else
						{
							anzout1(texte[51][sp]);		//打印机未连接
							anzout2(texte[52][sp]);
						}
					}						

					ch=gettast();
					anzout1(texte[8][sp]); 				// PRIN
					anzout2(texte[9][sp]); 				// TING
				}
			}
			else 															// 微型打印机
			{
				druckergebnis32();
			}
			//*/
		}
		else
		{
			druckergebnis32(); 								// 串口打印机
		}
	}	
}

// 显示处理任务，用户菜单
//--------------------------------------
__task void display(void)
{
	int fnkindex,ch,ch1,ch2,i,waicou,waicou1;
	logo();
	LoadFixVal();
//	getid(id);
	waicou=0;
	waicou1=0;
	while((ch=gettasto())!=0);
	
	do 
	{
		anzout1(texte[11][sp]);
		anzout2(texte[12][sp]);
		while((ch=gettasto())==0);
		if(ch==TMU) 							// 中下键
		{
			ch=blbef(0);
			if(ch==0) {
				anzout1("ERRO");
				anzout2("R   ");
				os_dly_wait(2000);
			}
		}
		else 
		{
			if(ch!=TRO) 						// 右上键
			{
				waicou=0;
			}
			else waicou++;
			if(ch!=TLU) 						// 左下键
			{
				waicou1=0;
			}
			else waicou1++;
			ch=0;
		}											 
	}while(ch==0 && waicou<500 && waicou1<500);
	 
	if(waicou<499) 
	{
		JawZeroed=__TRUE;
	}
	
	if(waicou1>=500) 
	{
		blbef(1);						   		//Vorfahren zu Ausrichten
		while((ch=gettasto())!=0);
		while((ch=gettasto())==0);
		backreset();
	}
	fnkindex=0;

//	Ymodem_Receive((u8 *)&buf);
	for(i=0;i<100;i++) 
	{
		mebuD[i]=99999;
		mebuH[i]=99999;
		mebuT[i]=99999;
	}	

	while(1)
	{
		if(fnkindex>=0 && fnkindex<9) 
		{
			if(!terminologie)
				anzout1(mtexte[fnkindex][sp]);
			else
				anzout1(mtextej[fnkindex][sp]);
		}
		else 
			anzout1("IERR");
		
		switch(fnkindex) 
		{
			case 0: 													// NUMB-xxxx
			{
				sprintf(buf,"%04d",pruefanz);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			/*
			case 1:	
				sprintf(buf,"%04.0f",dmeing*10);
				dp1=dp2=dp3=dp4=0;
				dp3=1;
				anzout2(buf);
				break;
			*/
			case 1: 													// REST-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				anzout2("    ");
			}break;				
			case 2: 													// STAT-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				anzout2("    ");
			}break;				
			case 3:														// SPED-xxxx
			{
				sprintf(buf,"%04.0f",MeasSpeed*10);
				dp1=dp2=dp3=dp4=0;
				dp3=1;
				anzout2(buf);
			}break;
			case 4:														// FORC-xxxx
			{
				sprintf(buf,"%04d",Force);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			case 5: 													// PRIN-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				if(AutoPrint)
					anzout2(textein[sp]);
				else
					anzout2(textaus[sp]);	
			}break;				
			case 6: 													// CALI-xxxx, ADJU-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				anzout2("    ");
			}break;				
			case 7: 													// CONT-xxxx, CALI-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				anzout2("    ");
			}break;				
			case 8: 													// SETT-xxxx
			{
				dp1=dp2=dp3=dp4=0;
				anzout2("    ");
			}break;
			default:
				anzout2("FERR");
			break;	
		}
		
		while((ch=gettast())==0);
		beep();
		
		if(ch==TMO) 												// 中上键
		{
			fnkindex=0;
		}
		
		if(ch==TMU) 												// 中下键
		{
			switch(fnkindex) 
			{
				case 0: 												// NUMB
				{
					messung();
				}break;
				case 1: 												// REST
				{
					showresult(LastMeasAnz);
				}break;
				case 2: 												// STAT
				{
					showstat();
				}break;
				case 5: 												// PRIN
				{
					anzout1(texte[8][sp]); 				// PRIN
					anzout2(texte[9][sp]); 				// TING
					ch1=0;
					
					if(druckmode==0) 							// 外置打印机
					{
						StartPrinter(); 						// 打印机启动
						
						if(PrintType == 0) 					// 台式打印机
						{
							while((ch2=druckergebnis())!=0 && ch1!=TMO) 
							{
								if(ch2==2) 												// 打印机缺纸
								{
									anzout1(texte[63][sp]); 				// “NO P”
									anzout2(texte[64][sp]); 				// “APER”
								}
								else 
								{
									if(ch2==3) 											// 打印机报错
									{
										anzout1(texte[65][sp]); 			// “PRI ”
										anzout2(texte[66][sp]); 			// “ERRO”
									}
									else 														// 没有连接打印机
									{
										anzout1(texte[51][sp]); 			// “NO P”
										anzout2(texte[52][sp]); 			// “RINT”
									}
								}
								
								ch1=gettast();
								anzout1(texte[8][sp]); 						// “PRIN”
								anzout2(texte[9][sp]); 						// “TING”
							}
						}
						else 												// 微型打印机
						{
							while(druckergebnis32() && ch1!=TMO) 					// ...中上键
							{
								anzout1(texte[51][sp]);
								anzout2(texte[52][sp]);
								ch1=gettast();
								anzout1(texte[8][sp]); 	// PRIN
								anzout2(texte[9][sp]); 	// TING
							}
						}
					}
					else 													// 内置打印机
					{
						while(druckergebnis32() && ch1!=TMO) 						// ...中上键
						{
							anzout1(texte[51][sp]);
							anzout2(texte[52][sp]);
							ch1=gettast();
							anzout1(texte[8][sp]); 		// PRIN
							anzout2(texte[9][sp]); 		// TING
						}
					}
					
					anzout1(texte[8][sp]); 				// PRIN
					dp1=dp2=dp3=dp4=0;
					if(AutoPrint)
						anzout2(textein[sp]);
					else
						anzout2(textaus[sp]);	
					ch=gettast();
				}break;	
				case 6: 												// CALI,效验
				{
					kalibriere();
				}break;	
				case 7: 												// CONT,检查
				{
					kontrolle();
				}break;	
				case 8: 												// SETT,参数设置
				{
					einstell();
				}break;
				default: break;
			}	
		}
		
		if(ch==TLO) 												// 左上键
		{
			if(fnkindex<8) 
				fnkindex++;
			
			if(vers[50]!='1') 
			{
				if(fnkindex==4) 
					fnkindex=5;
			}	 
		}
		
		if(ch==TLU) 												// 左下键
		{
			if(fnkindex>0) fnkindex--;
			if(vers[50]!='1') {
				if(fnkindex==4) fnkindex=3;
			}	 
		}
		
		if(ch==TRO) 												// 右上键
		{
			switch(fnkindex) 
			{
				case 0: 												// NUMB
				{
					if(pruefanz<100)
						pruefanz++;
					os_dly_wait(100);
					while(gettasto()==TRO && pruefanz<99) 	// 右上键
					{
						pruefanz++;
						sprintf(buf,"%04d",pruefanz);
						anzout2(buf);
						os_dly_wait(20);
					}
					writinteep(pruefanz,ADRPRUEFANZ);
				}break;
/*				case 1:
					if(dmeing<25) {
						dmeing+=0.1;
						sprintf(buf,"%04.0f",dmeing*10);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRO && dmeing<25) {
							dmeing+=0.1;
							sprintf(buf,"%04.0f",dmeing*10);
							anzout2(buf);
							os_dly_wait(20);
						}
						savefloat(&dmeing,ADRDMEING);
					}
					break;		*/
				case 3:
					if(MeasSpeed<3) {
						MeasSpeed+=0.1f;
						sprintf(buf,"%04.0f",MeasSpeed*10);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRO && MeasSpeed<3) // 右上键
						{
							MeasSpeed+=0.1f;
							sprintf(buf,"%04.0f",MeasSpeed*10);
							anzout2(buf);
							os_dly_wait(20);
						}
						savefloat(&MeasSpeed,ADRMEASSPEED);
					}
					break;		
				case 4:
					if(Force<200) {
						Force+=1;
						sprintf(buf,"%04d",Force);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRO && Force<200) 	// 右上键
						{
							Force+=1;
							sprintf(buf,"%04d",Force);
							anzout2(buf);
							os_dly_wait(20);
						}
						writinteep(Force,ADRFORCE);
					}
					break;		
				case 5:
					if(AutoPrint==0) {
						AutoPrint=1;
						anzout2(textein[sp]);
					}	
					else {
						AutoPrint=0;
						anzout2(textaus[sp]);
					}	
					writinteep(AutoPrint,ADRAUTOPRINT);	
								
			}			
		}
		
		if(ch==TRU) 												// 右下键
		{
			switch(fnkindex) 
			{
				case 0:
					if(pruefanz>0)
						pruefanz--;
						os_dly_wait(100);
						while(gettasto()==TRU && pruefanz>1) 	// 右下键
						{
							pruefanz--;
							sprintf(buf,"%04d",pruefanz);
							anzout2(buf);
							os_dly_wait(20);
						}
						writinteep(pruefanz,ADRPRUEFANZ);
					break;
/*				case 1:
					if(dmeing>1) {
						dmeing-=0.1;
						sprintf(buf,"%04.0f",dmeing*10);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRU && dmeing>1) {
							dmeing-=0.1;
							sprintf(buf,"%04.0f",dmeing*10);
							anzout2(buf);
							os_dly_wait(20);
						}
						savefloat(&dmeing,ADRDMEING);
					}
					break;		*/
				case 3:
					if(MeasSpeed>0.5f) {
						MeasSpeed-=0.1f;
						sprintf(buf,"%04.0f",MeasSpeed*10);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRU && MeasSpeed>0.5f) 				// 右下键
						{
							MeasSpeed-=0.1f;
							sprintf(buf,"%04.0f",MeasSpeed*10);
							anzout2(buf);
							os_dly_wait(20);
						}
						savefloat(&MeasSpeed,ADRMEASSPEED);
					}
					break;		
				case 4:
					if(Force>10) {
						Force-=1;
						sprintf(buf,"%04d",Force);
						anzout2(buf);
						os_dly_wait(100);
						while(gettasto()==TRU && Force>10) 		// 右下键
						{
							Force-=1;
							sprintf(buf,"%04d",Force);
							anzout2(buf);
							os_dly_wait(20);
						}
						writinteep(Force,ADRFORCE);
					}
					break;		
				case 5:
					if(AutoPrint==0) {
						AutoPrint=1;
						anzout2(textein[sp]);
					}	
					else {
						AutoPrint=0;
						anzout2(textaus[sp]);
					}	
					writinteep(AutoPrint,ADRAUTOPRINT);	
			}			
		}
	}
}

char *lt1E[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125@","25@@","5@@@","@@@@","@@@@","@@@@"};
char *lt2E[24]={"@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125@","25@@","5@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1ED[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125D","25D@","5D@@","D@@@","@@@@","@@@@"};
char *lt2ED[24]={"@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125D","25D@","5D@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1ETD[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125T","25TD","5TD@","TD@@","D@@@","@@@@"};
char *lt2ETD[24]={"@@@@","@@@E","@@ER","@ERW","ERWE","RWEK","WEKA","EKA@","KA@T","A@TB","@TBH","TBH1","BH12","H125","125T","25TD","5TD@","TD@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};


char *lt1C[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15@","15@@","5@@@","@@@@","@@@@","@@@@","@@@@"};
char *lt2C[24]={"@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15@","15@@","5@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1CD[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15D","15D@","5D@@","D@@@","@@@@","@@@@","@@@@"};
char *lt2CD[24]={"@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15D","15D@","5D@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1CTD[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15T","15TD","5TD@","TD@@","D@@@","@@@@","@@@@"};
char *lt2CTD[24]={"@@@@","@@@C","@@CA","@CAL","CALE","ALEV","LEVA","EVA@","VA@T","A@TH","@THT","THT1","HT15","T15T","15TD","5TD@","TD@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};


char *lt1Q[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","ST@T","T@TH","@THT","THT1","HT12","T125","125@","25@@","5@@@","@@@@","@@@@","@@@@","@@@@"};
char *lt2Q[24]={"@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","ST@T","T@TH","@THT","THT1","HT12","T125","125@","25@@","5@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1QD[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","ST@T","T@TH","@THT","THT1","HT12","T125","125D","25D@","5D@@","D@@@","@@@@","@@@@","@@@@"};
char *lt2QD[24]={"@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","ST@T","T@TH","@THT","THT1","HT12","T125","125@","25D@","5D@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};

char *lt1QTD[24]={"@@@@","@@@@","@@@@","@@@@","@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","VA@T","A@TH","@THT","THT1","HT12","T125","125T","25TD","5TD@","TD@@","D@@@","@@@@","@@@@"};
char *lt2QTD[24]={"@@@@","@@@Q","@@QT","@QTE","QTES","TEST","EST@","ST@T","T@TH","@THT","THT1","HT12","T125","125T","25TD","5TD@","TD@@","D@@@","@@@@","@@@@","@@@@","@@@@","@@@@","@@@@"};



// 初始上电LOGO显示处理函数
//--------------------------------------
void logo(void) 
{
	unsigned char i;
	int verg=24;

	while(gettasto()==0) 
	{
		for(i=0;i<verg;i++) 
		{
			if(gettasto()!=0) 
				break;
			if(vers[23]=='E') 
			{
				if(vers[46]=='1') 
				{
					anzout1(lt1ETD[i]);
					anzout2(lt2ETD[i]);
					verg=24;
				}
				else 
				{
					if(vers[45]=='1') 
					{
						anzout1(lt1ED[i]);
						anzout2(lt2ED[i]);
						verg=23;
					}
					else 
					{
						anzout1(lt1E[i]);
						anzout2(lt2E[i]);
						verg=21;
					}
				}
			}
			
			if(vers[23]=='C') 
			{
				if(vers[46]=='1') 
				{
					anzout1(lt1CTD[i]);
					anzout2(lt2CTD[i]);
					verg=24;
				}
				else 
				{
					if(vers[45]=='1') 
					{
						anzout1(lt1CD[i]);
						anzout2(lt2CD[i]);
						verg=23;
					}
					else 
					{
						anzout1(lt1C[i]);
						anzout2(lt2C[i]);
						verg=21;
					}
				}
			}
			
			if(vers[23]=='Q') 
			{
				if(vers[46]=='1') 
				{
					anzout1(lt1QTD[i]);
					anzout2(lt2QTD[i]);
					verg=24;
				}
				else 
				{
					if(vers[45]=='1') 
					{
						anzout1(lt1QD[i]);
						anzout2(lt2QD[i]);
						verg=23;
					}
					else 
					{
						anzout1(lt1Q[i]);
						anzout2(lt2Q[i]);
						verg=21;
					}
				}
			}
			
			os_dly_wait(150);
		}
	}
	beep();		
	anzout1("@@@@");
	anzout2("@@@@");
}

// 主菜单SETT参数处理函数---------------------------------------------
void einstell(void)
{
	unsigned char eindex0 = 0; 						// 菜单层数指针，Gavin,206-1-7,2019-07-25 星期四 10:20:48
	unsigned char eindex1 = 0; 						// 第一层菜单指针
	unsigned char eindex2 = 0; 						// 第二层菜单指针
	unsigned char ch;
	int i;

	int maxindex;

	unsigned int DatePW = 0; 							// 日期密码
	unsigned int DatePWC = 0; 						// 日期密码效验
	unsigned int PwmgPW = 0; 							// 管理密码
	unsigned int PwmgPWC = 0; 						// 管理密码效验
	unsigned int PassWord = 0; 						// 输入的密码
	
	unsigned char val_ip = 3;  	// 输入密码的位指示,个十百千,0123
		
	RTC_TimeTypeDef RTC_TimeStructure;		// 时间
	RTC_DateTypeDef RTC_DateStructure; 		// 日期
	
	// 检测中下键按住的时间---------------
	i=0;
	while(gettasto()!=0 && i++<10) 
	{
		os_dly_wait(500);
	}
	
	if(i<9) 
		return; 									// 小于设定时间,直接返回
	
	beep(); 										// 大于等于设定时间,进入参数设置

	while(1)
	{
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
		RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
		
		
		if(RTC_DateStructure.RTC_Year<8 || RTC_DateStructure.RTC_Year>30) RTC_DateStructure.RTC_Year=30;
		if(RTC_DateStructure.RTC_Month<1 || RTC_DateStructure.RTC_Month>12) RTC_DateStructure.RTC_Month=1;
		if(RTC_DateStructure.RTC_Date<1 || RTC_DateStructure.RTC_Date>31) RTC_DateStructure.RTC_Date=1;
		if(RTC_TimeStructure.RTC_Hours>23) RTC_TimeStructure.RTC_Hours=1;
		if(RTC_TimeStructure.RTC_Minutes>59) RTC_TimeStructure.RTC_Minutes=0;
		
		
		// 数码管显示处理-------------------
		switch(eindex1)
		{
			case 0: 													// PAUS
			{
				anzout1(texte[16][sp]);
				sprintf(buf,"%04d",MeasurePause);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			case 1:	 													// CPOS
			{
				anzout1(texte[17][sp]);
				sprintf(buf,"%04d",eichpos);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			case 2: 													// UNIH
			{
				anzout1(texte[19][sp]);
				if(HardEinh==0) anzout2("   N");
				if(HardEinh==1) anzout2("  KP");
				if(HardEinh==2) anzout2("  SC");
			}break;	
			case 3: 													// UNID
			{
				anzout1(texte[20][sp]);
				if(LaeEinh==0) anzout2("  MM");
				if(LaeEinh==1) anzout2("INCH");
			}break;	
			case 4: 													// DATE, Gavin, 206-1-2, 2019-07-25 星期四 09:42:43
			{
				if(eindex0 == 0) 								// 根目录
				{
					anzout1(texte[69][sp]); 			// DATE
					
					if(DatePW == 9999)
					{
						anzout2("    ");
					}
					else
					{
						//sprintf(buf, "%04d", PassWord);
						buf[0] = buf[1] = buf[2] = buf[3] = '*';
						buf[4] = 0;
						dp1=dp2=dp3=dp4=0;
						switch(val_ip)
						{
							case 3: buf[0] = 0x30 + PassWord/1000; break;
							case 2: buf[1] = 0x30 + PassWord%1000/100; break;
							case 1: buf[2] = 0x30 + PassWord%1000%100/10; break;
							case 0: buf[3] = 0x30 + PassWord%10; break;
							default: break;
						}
						anzout2(buf);
					}
				}
				else
				if(eindex0 == 1) 								// 一级目录
				{
					switch(eindex2)
					{
						case 0: 											// HOUR
						{
							anzout1(texte[21][sp]);
							sprintf(buf,"%04d",RTC_TimeStructure.RTC_Hours);
							dp1=dp2=dp3=dp4=0;
							anzout2(buf);
						}break;
						case 1: 											// MINU
						{
							anzout1(texte[22][sp]);
							sprintf(buf,"%04d",RTC_TimeStructure.RTC_Minutes);
							dp1=dp2=dp3=dp4=0;
							anzout2(buf);
						}break;
						case 2: 											// DAY
						{
							anzout1(texte[23][sp]);
							sprintf(buf,"%04d",RTC_DateStructure.RTC_Date);
							dp1=dp2=dp3=dp4=0;
							anzout2(buf);
						}break;
						case 3: 											// MONT
						{
							anzout1(texte[24][sp]);
							sprintf(buf,"%04d",RTC_DateStructure.RTC_Month);
							dp1=dp2=dp3=dp4=0;
							anzout2(buf);
						}break;
						case 4: 											// YEAR
						{
							anzout1(texte[25][sp]);
							sprintf(buf,"%04d",RTC_DateStructure.RTC_Year+2000);
							dp1=dp2=dp3=dp4=0;
							anzout2(buf);
						}break;
						default: break;
					}
				}
			}break;
			case 7: 													// THRE, 增加门槛值SSW的设置菜单。Gavin,2022-02-09 周三 09:43:42
			{
				anzout1(texte[72][sp]);
				sprintf(buf,"%04d",Sensibility);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			case 8: 													// CYCL, 增加循环时间ULW的设置菜单。Gavin,2022-02-09 周三 09:43:59
			{
				anzout1(texte[73][sp]);
				sprintf(buf,"%04d",ueberlauf);
				dp1=dp2=dp3=dp4=0;
				anzout2(buf);
			}break;
			case 9: 													// NFCT
			{
				anzout1(texte[28][sp]);
				sprintf(buf,"%04.0f",NwtFak*100);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
				anzout2(buf);
			}break;
			case 10: 													// LANG
			{
				anzout1(texte[30][sp]);
				dp1=dp2=dp3=dp4=0;
				if(aktLanguage==0) anzout2(texte[31][sp]); 					// GERM
				if(aktLanguage==1) anzout2(texte[32][sp]); 					// ENGL
			}break;	
			case 11: 													// VERS
			{
				anzout1(texte[33][sp]);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
				buf[0]=vers[0];
				buf[1]=vers[1];
				buf[2]=vers[3];
				buf[3]=vers[4];
				buf[4]=0;
				anzout2(buf);
			}break;	
			case 12: 													// DATF
			{
				anzout1(texte[34][sp]);
				dp1=dp2=dp3=dp4=0;
				if(DatFormat==0) anzout2(texte[35][sp]); 						// EURO
				if(DatFormat==1) anzout2(texte[36][sp]); 						// USA
			}break;	
			case 13: 													// METH
			{
				anzout1(texte[37][sp]);
				dp1=dp2=dp3=dp4=0;
				if(MeasureMethod==0) anzout2(texte[38][sp]); 				// SPEE
				if(MeasureMethod==1) anzout2(texte[39][sp]); 				// FORC
			}break;	
			case 14: 													// TERM
			{
				anzout1(texte[46][sp]);
				dp1=dp2=dp3=dp4=0;
				if(terminologie==0) anzout2(texte[47][sp]); 				// CALI
				if(terminologie==1) anzout2(texte[48][sp]); 				// ADJU
			}break;	
			case 15: 													// AUTO
			{
				anzout1(texte[55][sp]);
				dp1=dp2=dp3=dp4=0;
				if(autodiam==0) 
					anzout2(textaus[sp]);
				else 
					anzout2(textein[sp]);
			}break;	
			case 16: 													// PRIN
			{
				anzout1(texte[56][sp]);
				dp1=dp2=dp3=dp4=0;
				if(druckmode==0) anzout2(texte[58][sp]); 	// EXT
				if(druckmode==1) anzout2(texte[57][sp]); 	// INT
			}break;	
			case 17: 													// PRLG
			{
				anzout1(texte[59][sp]);
				dp1=dp2=dp3=dp4=0;
				if(PrinterLanguage==0) anzout2(texte[60][sp]); 			// ASCI
				if(PrinterLanguage==1) anzout2(texte[61][sp]); 			// PCL
			}break;	
			case 18: 													// ZECO
			{
				anzout1(texte[62][sp]);
				dp1=dp2=dp3=dp4=0;
				dp3=1;
				sprintf(buf,"%04d",NullKorr);
				anzout2(buf);
			}break;
			case 19: 													// AUME, 2016/10/26 19:40:38
			{
				anzout1(texte[67][sp]);
				dp1=dp2=dp3=dp4=0;
				if(AutoMess==0) anzout2(texte[44][sp]); 						// OFF
				if(AutoMess==1) anzout2(texte[43][sp]); 						// ON
			}break;	
			case 20: 													// DOUT
			{
				anzout1(texte[68][sp]);
				dp1=dp2=dp3=dp4=0;
				if(DatenAusgabe==0) anzout2(texte[44][sp]); 				// OFF
				if(DatenAusgabe==1) anzout2(texte[43][sp]); 				// ON
			}break;	
			case 21: 													// HARD
			{
				anzout1(texte[40][sp]);
				dp1=dp2=dp3=dp4=0;
				if(HardOn==0) anzout2(texte[44][sp]); 		// OFF
				if(HardOn==1) anzout2(texte[43][sp]); 		// ON
			}break;	
			case 22: 													// DIAM
			{
				anzout1(texte[41][sp]);
				dp1=dp2=dp3=dp4=0;
				if(DiamOn==0) anzout2(texte[44][sp]); 		// OFF
				if(DiamOn==1) anzout2(texte[43][sp]); 		// ON
			}break;	
			case 23: 													// THIC
			{
				anzout1(texte[42][sp]);
				dp1=dp2=dp3=dp4=0;
				if(ThickOn==0) anzout2(texte[44][sp]); 		// OFF
				if(ThickOn==1) anzout2(texte[43][sp]); 		// ON
			}break;	
			case 24: 													// PWMG, Gavin, 206-1-4, 2019-07-25 ??? 09:54:20
			{
				if(eindex0 == 0) 								// 根目录
				{
					anzout1(texte[70][sp]);
					
					if(PwmgPW == 9999)
					{
						anzout2("    ");
					}
					else
					{
						//sprintf(buf, "%04d", PassWord);
						buf[0] = buf[1] = buf[2] = buf[3] = '*';
						buf[4] = 0;
						dp1=dp2=dp3=dp4=0;
						switch(val_ip)
						{
							case 3: buf[0] = 0x30 + PassWord/1000; break;
							case 2: buf[1] = 0x30 + PassWord%1000/100; break;
							case 1: buf[2] = 0x30 + PassWord%1000%100/10; break;
							case 0: buf[3] = 0x30 + PassWord%10; break;
							default: break;
						}
						anzout2(buf);
					}
				}
				else
				if(eindex0 == 1) 								// 一级目录
				{
					switch(eindex2)
					{
						case 0: 										// PWST
						{
							anzout1(texte[71][sp]);
							//sprintf(buf,"%04d",PwmgPW);
							buf[0] = buf[1] = buf[2] = buf[3] = '*';
							buf[4] = 0;
							dp1=dp2=dp3=dp4=0;
							switch(val_ip)
							{
								case 3: buf[0] = 0x30 + PwmgPW/1000; break;
								case 2: buf[1] = 0x30 + PwmgPW%1000/100; break;
								case 1: buf[2] = 0x30 + PwmgPW%1000%100/10; break;
								case 0: buf[3] = 0x30 + PwmgPW%10; break;
								default: break;
							}
							anzout2(buf);
						}break;
						case 1: 										// DATE
						{
							anzout1(texte[69][sp]);
							//sprintf(buf,"%04d",DatePW);
							buf[0] = buf[1] = buf[2] = buf[3] = '*';
							buf[4] = 0;
							dp1=dp2=dp3=dp4=0;
							switch(val_ip)
							{
								case 3: buf[0] = 0x30 + DatePW/1000; break;
								case 2: buf[1] = 0x30 + DatePW%1000/100; break;
								case 1: buf[2] = 0x30 + DatePW%1000%100/10; break;
								case 0: buf[3] = 0x30 + DatePW%10; break;
								default: break;
							}
							anzout2(buf);
						}break;
						default: break;
					}
				}
			}break;
			default: break;
		}
		
		// 按键动作处理---------------------
		while((ch=gettast())==0);
		beep();
		
		if(ch==TMO) 												// 中上键
		{
			val_ip = 3;
			
			if(eindex0 != 0) 									// 返回上一层菜单
				eindex0--;
			else
				return;
		}
		
		// 增加中下键动作,Gavin,206-1-6,2019-07-25 星期四 10:04:21
		if(ch==TMU) 												// 中下键
		{
			if(eindex0 == 0) 									// 根目录
			{
				if(eindex1 == 4) 								// DATE
				{
					if((DatePW == 9999)||
						 (PassWord == DatePW))
					{
						eindex0++;
						eindex2 = 0; 								// 下一级菜单指针清零
						
						PassWord = 0;
						val_ip = 3;
					}
					else
					{
						return;
					}
				}
				else
				if(eindex1 == 24) 							// PWMG
				{
					if((PwmgPW == 9999)||
						 (PassWord == PwmgPW))
					{
						if((PwmgPW == 807)&&
							 (DatePW == 807))
						{
							PwmgPW = 9999;
							writinteep(PwmgPW, ADRPWMGPW);
							DatePW = 9999;
							writinteep(DatePW, ADRDATEPW);
						}
						
						eindex0++;
						eindex2 = 0; 								// 下一级菜单指针清零
						
						PassWord = 0;
						val_ip = 3;
					}
					else
					{
						return;
					}
				}
			}
		}
		
		if(ch==TLO) 												// 左上键
		{
			if(eindex0 == 0) 									// 根目录
			{
				//if(JawZeroed) 									// 开机对过零位的正常进入
				//	maxindex = 23; 								// 对过零位的正常SETT目录上限
				//else 														// 开机不用对零位，通过长按右上键进入
					maxindex = 24; 								// 调整上限,Gavin,206-1-5,2019-07-25 星期四 10:00:12
				
				//if(!dickena) maxindex=22; 		// 菜单屏蔽,Gavin,2019-10-07 星期一 10:55:23
				//if(!dmena) maxindex=21;
				
				if(eindex1 < maxindex)
				{
					eindex1++;
					
					if(eindex1 == 4) 							// DATE
					{
						DatePW = readinteep(ADRDATEPW);
						
						// 206-9-1, Gavin, 密码校验,2019-08-12 星期一 15:21:29
						DatePWC = readinteep(ADRDATEPWC);
						DatePWC ^= 0xaaaa;
						if(DatePW != DatePWC)
						{
							DatePW = 9999;
							writinteep(DatePW, ADRDATEPW);
							DatePWC = DatePW;
							DatePWC ^= 0xaaaa;
							writinteep(DatePWC, ADRDATEPWC);
						}
						
						PassWord = 0;
					}
					
					if(eindex1 == 5) 							// 日期设置菜单合并后，需要跳过，Gavin，206-1-8，2019-07-25 星期四 10:33:36
					{
						//eindex1 = 9;
						eindex1 = 7; 								// Gavin, 2022-02-09 周三 11:49:13
					}
					
					if(eindex1 == 13)  						// METH,
					{
						if(vers[50] == '0') 				// 程序写入界面的Constant forc选项，未选择
							eindex1++; 								// 未先择，跳过
					}
					
					if(eindex1 == 18) 						// ZECO
					{
						if(JawZeroed) 							// 启动时对过零进入时跳过，使用长按右上键进入时，可显示ZECO
							eindex1++;
					}
					
					if(eindex1 == 22) 						// DIAM，直径测量?
					{
						if(!dmena) 									// 程序写入界面的Diameter选项，未选择
							eindex1++; 								// 跳过
					}
					
					if(eindex1 == 23) 						// THIC，厚度测量，
					{
						if(!dickena) 								// 程序写入界面的Thickness选项，未选择
							eindex1++; 								// 跳过
					}
					
					if(eindex1 == 24) 						// PWMG
					{
						PwmgPW = readinteep(ADRPWMGPW);
						
						// 206-9-1, Gavin, 密码校验,2019-08-12 星期一 15:21:29
						PwmgPWC = readinteep(ADRPWMGPWC);
						PwmgPWC ^= 0xaaaa;
						if(PwmgPW != PwmgPWC)
						{
							PwmgPW = 9999;
							writinteep(PwmgPW, ADRPWMGPW);
							PwmgPWC = PwmgPW;
							PwmgPWC ^= 0xaaaa;
							writinteep(PwmgPWC, ADRPWMGPWC);
						}
						
						DatePW = readinteep(ADRDATEPW);
						
						// 206-9-1, Gavin, 密码校验,2019-08-12 星期一 15:21:29
						DatePWC = readinteep(ADRDATEPWC);
						DatePWC ^= 0xaaaa;
						if(DatePW != DatePWC)
						{
							DatePW = 9999;
							writinteep(DatePW, ADRDATEPW);
							DatePWC = DatePW;
							DatePWC ^= 0xaaaa;
							writinteep(DatePWC, ADRDATEPWC);
						}
						
						PassWord = 0;
						val_ip = 3;
					}
				}
				else
				{
					// 长按解锁密码控制-------------
					if(eindex1 == 24) 							// PWMG
					{
						i=0;
						while(gettasto()!=0 && i++<20) 
						{
							os_dly_wait(500);
						}
						
						if(i >= 19)
						{
							beep(); 										// 大于等于设定时间，进入参数设置
							
							PwmgPW = 807;
							DatePW = 807;
							PassWord = 0;
							val_ip = 3;
						}
					}
				}
			}
			else
			if(eindex0 == 1) 									// 第一级目录
			{
				if(eindex1 == 4) 								// DATE
				{
					maxindex = 5;
					if(eindex2 < maxindex-1) 
						eindex2++;
				}
				else
				if(eindex1 == 24) 							// PWMG
				{
					val_ip = 3;
					
					maxindex = 2;
					if(eindex2 < maxindex-1) 
						eindex2++;
				}
			}
		}
		
		if(ch==TLU) 												// 左下键
		{
			if(eindex0 == 0) 									// 根目录
			{
				if(eindex1 > 0) 
				{
					eindex1--;
					
					if(eindex1 == 23) 						// THIC，厚度测量
					{
						if(!dickena) 								// 程序写入界面的Thickness选项，未选择
							eindex1--; 								// 跳过
					}
					
					if(eindex1 == 22) 						//DIAM，直径测量，
					{
						if(!dmena) 									// 程序写入界面的Diameter选项，未选择
							eindex1--; 								// 跳过
					}
					
					if(eindex1 == 18) 						// ZECO
					{
						if(JawZeroed) 							// 启动时对过零进入时跳过，使用长按右上键进入时，可显示ZECO
							eindex1--;
					}
					
					if(eindex1 == 13) 						// METH
					{
						if(vers[50] == '0') 				// 程序写入界面的Constant forc选项，未选择
							eindex1--; 								// 跳过
					}
					
					//if(eindex1 == 8) 							//日期设置菜单合并后，需要跳过，Gavin，206-1-9，2019-07-25 星期四 10:33:36
					if(eindex1 == 6) 							//日期设置菜单合并后，需要跳过，Gavin，2022-02-09 周三 11:51:08
					{
						eindex1 = 4;
						
						DatePW = readinteep(ADRDATEPW);
						PassWord = 0;
						val_ip = 3;
					}
				}
			}
			else
			if(eindex0 == 1) 									// 一级目录
			{
				if(eindex1 == 4) 								// DATE
				{
					if(eindex2 > 0) 
						eindex2--;
				}
				else
				if(eindex1 == 24) 							// PWMG
				{
					val_ip = 3;
					
					if(eindex2 > 0) 
						eindex2--;
				}
			}
		}
		
		if(ch==TRO) 												// 右上键
		{
			switch(eindex1)
			{
				case 0: 												// PAUS
				{
					if(MeasurePause<10)
						MeasurePause++;
					sprintf(buf,"%04d",MeasurePause);
					anzout2(buf);
					writinteep(MeasurePause,ADRMEASUREPAUSE);
				}break;
				case 1: 												// CPOS
				{
					if(eichpos<20) 
					{
						eichpos+=1;
						sprintf(buf,"%04d",eichpos);
						anzout2(buf);
						writinteep(eichpos,ADREICHPOS);
					}
				}break;
				case 2: 												// UNIH
				{
					if(HardEinh<2) 
					{
						HardEinh++;
						if(HardEinh==0) anzout2("   N");
						if(HardEinh==1) anzout2("  KP");
						if(HardEinh==2) anzout2("  SC");
						writinteep(HardEinh,ADRHEINH);
						setkraftfak();
					}				
				}break;
				case 3: 												// UNID
				{
					if(LaeEinh<1) 
					{
						LaeEinh++;
						if(LaeEinh==0) anzout2("  MM");
						if(LaeEinh==1) anzout2("INCH");
						writinteep(LaeEinh,ADRLEINH);
						setlaengfak();
					}				
				}break;
				case 4: 												// DATE
				{
					if(eindex0 == 0) 							// 根目录
					{
						if(DatePW != 9999)
						{
							switch(val_ip)
							{
								case 3:
								{
									if(PassWord/1000 == 9)
										PassWord -= 9000;
									else
										PassWord += 1000;
								}break;
								case 2:
								{
									if(PassWord%1000/100 == 9)
										PassWord -= 900;
									else
										PassWord += 100;
								}break;
								case 1:
								{
									if(PassWord%1000%100/10 == 9)
										PassWord -= 90;
									else
										PassWord += 10;
								}break;
								case 0:
								{
									if(PassWord%10 == 9)
										PassWord -= 9;
									else
										PassWord += 1;
								}break;
								default: break;
							}
						}
					}
					else
					if(eindex0 == 1) 							// 一级目录
					{
						switch(eindex2)
						{
							case 0: 									// HOUR
							{
								if(RTC_TimeStructure.RTC_Hours<24)
									RTC_TimeStructure.RTC_Hours++;
								sprintf(buf,"%04d",RTC_TimeStructure.RTC_Hours);
								RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
								anzout2(buf);
								
							}break;
							case 1: 									// MINU
							{
								if(RTC_TimeStructure.RTC_Minutes<59)
									RTC_TimeStructure.RTC_Minutes++;
								sprintf(buf,"%04d",RTC_TimeStructure.RTC_Minutes);
								anzout2(buf);
								RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
							}break;
							case 2: 									// DAY
							{
								if(RTC_DateStructure.RTC_Date<31)
									RTC_DateStructure.RTC_Date++;
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Date);
								anzout2(buf);

								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);

							}break;
							case 3: 									// MONT
							{
								if(RTC_DateStructure.RTC_Month<12)
									RTC_DateStructure.RTC_Month++;
								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Month);
								anzout2(buf);
								
							}break;
							case 4: 									// YEAR
							{
								if(RTC_DateStructure.RTC_Year<30)
									RTC_DateStructure.RTC_Year++;
								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Year+2000);
								anzout2(buf);
							}break;
							default: break;
						}
					}
				}break;
				/*case 4: 												// HOUR
				{
					if(tim.hours<24)
						tim.hours++;
					sprintf(buf,"%04d",tim.hours);
					anzout2(buf);
					RTC_SetTime(tim);
				}break;
				case 5: 												// MINU
				{
					if(tim.minutes<59)
						tim.minutes++;
					sprintf(buf,"%04d",tim.minutes);
					anzout2(buf);
					RTC_SetTime(tim);
				}break;
				case 6: 												// DAY
				{
					if(dat.day<31)
						dat.day++;
					sprintf(buf,"%04d",dat.day);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;
				case 7: 												// MONT
				{
					if(dat.month<12)
						dat.month++;
					sprintf(buf,"%04d",dat.month);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;
				case 8: 												// YEAR
				{
					if(dat.year<30)
						dat.year++;
					sprintf(buf,"%04d",dat.year+2000);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;*/
				case 7: 												// THRE, 增加门槛SSW菜单，Gavin，2022-02-09 周三 09:53:04
				{
					if(Sensibility<50)
					{
						Sensibility++;
						
						sprintf(buf,"%04d",Sensibility);
						anzout2(buf);
						writinteep(Sensibility,ADRSENSIBILITY);
					}
				}break;
				case 8: 												// CYCL，增加循环时间ULW菜单，Gavin，2022-02-09 周三 09:53:11
				{
					if(ueberlauf<120)
					{
						ueberlauf++;
						
						sprintf(buf,"%04d",ueberlauf);
						anzout2(buf);
						writinteep(ueberlauf,ADRUEBERLAUF);
					}
				}break;
				case 10: 												// LANG
				{
					if(aktLanguage<1) 
					{
						aktLanguage++;
						anzout2(texte[32][sp]); 							// ENGL
						sp=aktLanguage;
						writinteep(aktLanguage,ADRLANGUAGE);
					}
				}break;
				case 12: 												// DATF
				{
					if(DatFormat<1) 
					{
						DatFormat++;
						anzout2(texte[36][sp]); 							// USA
						writinteep(DatFormat,ADRDATFORMAT);
					}
				}break;
				case 13: 												// METH
				{
					
					if(MeasureMethod<1) 
					{
						MeasureMethod++;
						anzout2(texte[39][sp]); 							// SPEE
						writinteep(MeasureMethod,ADRMEASUREMETHOD);
					}
				}break;
				case 14: 												// TERM
				{
					if(terminologie<1) 
					{
						terminologie++;
						anzout2(texte[48][sp]); 							// ADJU
						writinteep(terminologie,ADRTERMINOLOGIE);
					}
				}break;
				case 15: 												// AUTO
				{
					if(autodiam<1) 
					{
						autodiam++; 								// ??AUME
						anzout2(textein[sp]);
						writinteep(autodiam,ADRAUTODIAM);
					}
				}break;
				case 16: 												// PRIN
				{
					if(druckmode<1) 
					{
						druckmode++;
						anzout2(texte[57][sp]); 							// INT
						writinteep(druckmode,ADRDRUCKMODE);
						//writeepbuf((unsigned char*)Print_int,ADRVERSION+47,1);
						//vers[47] = 0x31;
					}
				}break;
				case 17: 												// PRLG
				{
					if(PrinterLanguage<1) 
					{
						PrinterLanguage++;
						anzout2(texte[61][sp]); 							// PCL
						writinteep(PrinterLanguage,ADRPRINTERLANGUAGE);
					}
				}break;
				case 18: 												// ZECO
				{
					if(NullKorr<25) 
					{
						NullKorr++;
						sprintf(buf,"%04d",NullKorr);
						anzout2(buf);
						writinteep(NullKorr,ADRNULLKORR);
					}
				}break;
				case 19: 												// AUME
				{
					if(AutoMess<1) 
					{
						AutoMess++;
						anzout2(texte[43][sp]); 							//   ON
						writinteep(AutoMess,ADRAUTOMESS);
						//autodiam=0; 								// ??AUTO,2016/11/2 14:55:43
						writinteep(autodiam,ADRAUTODIAM);
					}
				}break;
				case 20: 												// DOUT
				{
					if(DatenAusgabe<1) 
					{
						DatenAusgabe++;
						anzout2(texte[43][sp]); 							//   ON
						writinteep(DatenAusgabe,ADRDATENAUSGABE);
					}
				}break;
				case 21: 												// HARD
				{
					if(HardOn<1) 
					{
						HardOn++;
						anzout2(texte[43][sp]); 							//   ON
						writinteep(HardOn,ADRHARDON);
					}
				}break;
				case 22: 												// DIAM
				{
					if(DiamOn<1) 
					{
						DiamOn++;
						anzout2(texte[43][sp]); 							//   ON
						writinteep(DiamOn,ADRDIAMON);
					}
				}break;
				case 23: 												// THIC
				{
					if(ThickOn<1) 
					{
						ThickOn++;
						anzout2(texte[43][sp]); 							//   ON
						writinteep(ThickOn,ADRTHICKON);
					}
				}break;
				case 24: 												// PWMG
				{
					if(eindex0 == 0) 							// 根目录
					{
						if(PwmgPW != 9999)
						{
							switch(val_ip)
							{
								case 3:
								{
									if(PassWord/1000 == 9)
										PassWord -= 9000;
									else
										PassWord += 1000;
								}break;
								case 2:
								{
									if(PassWord%1000/100 == 9)
										PassWord -= 900;
									else
										PassWord += 100;
								}break;
								case 1:
								{
									if(PassWord%1000%100/10 == 9)
										PassWord -= 90;
									else
										PassWord += 10;
								}break;
								case 0:
								{
									if(PassWord%10 == 9)
										PassWord -= 9;
									else
										PassWord += 1;
								}break;
								default: break;
							}
						}
					}
					else
					if(eindex0 == 1) 							// 一级目录
					{
						switch(eindex2)
						{
							case 0: 									// PWST
							{
								switch(val_ip)
								{
									case 3:
									{
										if(PwmgPW/1000 == 9)
											PwmgPW -= 9000;
										else
											PwmgPW += 1000;
									}break;
									case 2:
									{
										if(PwmgPW%1000/100 == 9)
											PwmgPW -= 900;
										else
											PwmgPW += 100;
									}break;
									case 1:
									{
										if(PwmgPW%1000%100/10 == 9)
											PwmgPW -= 90;
										else
											PwmgPW += 10;
									}break;
									case 0:
									{
										if(PwmgPW%10 == 9)
											PwmgPW -= 9;
										else
											PwmgPW += 1;
									}break;
									default: break;
								}
								
								writinteep(PwmgPW,ADRPWMGPW);
								
								// 206-9-1, Gavin, 保存检验,2019-08-12 星期一 16:10:02
								PwmgPWC = PwmgPW;
								PwmgPWC ^= 0xaaaa;
								writinteep(PwmgPWC,ADRPWMGPWC);
							}break;
							case 1: 									// DATE
							{
								switch(val_ip)
								{
									case 3:
									{
										if(DatePW/1000 == 9)
											DatePW -= 9000;
										else
											DatePW += 1000;
									}break;
									case 2:
									{
										if(DatePW%1000/100 == 9)
											DatePW -= 900;
										else
											DatePW += 100;
									}break;
									case 1:
									{
										if(DatePW%1000%100/10 == 9)
											DatePW -= 90;
										else
											DatePW += 10;
									}break;
									case 0:
									{
										if(DatePW%10 == 9)
											DatePW -= 9;
										else
											DatePW += 1;
									}break;
									default: break;
								}
								
								writinteep(DatePW,ADRDATEPW);
								
								// 206-9-1, Gavin, 保存检验,2019-08-12 星期一 16:10:02
								DatePWC = DatePW;
								DatePWC ^= 0xaaaa;
								writinteep(DatePWC,ADRDATEPWC);
							}break;
							default: break;
						}
					}
				}break;
				default: break;
			}		
		}
		
		if(ch==TRU) 												// 右下键
		{
			switch(eindex1) 
			{
				case 0: 												// PAUS
				{
					if(MeasurePause>0)
						MeasurePause--;
					
					sprintf(buf,"%04d",MeasurePause);
					anzout2(buf);
					writinteep(MeasurePause,ADRMEASUREPAUSE);
				}break;
				case 1: 												// CPOS
				{
					if(eichpos>0) 
					{
						eichpos-=1;
						sprintf(buf,"%04d",eichpos);
						anzout2(buf);
						writinteep(eichpos,ADREICHPOS);
					}
				}break;		
				case 2: 												// UNIH
				{
					if(HardEinh>0) 
					{
						HardEinh--;
						if(HardEinh==0) anzout2("   N");
						if(HardEinh==1) anzout2("  KP");
						if(HardEinh==2) anzout2("  SC");
						writinteep(HardEinh,ADRHEINH);
						setkraftfak();
					}				
				}break;
				case 3: 												// UNID
				{
					if(LaeEinh>0) 
					{
						LaeEinh--;
						if(LaeEinh==0) anzout2("  MM");
						if(LaeEinh==1) anzout2("INCH");
						writinteep(LaeEinh,ADRLEINH);
						setlaengfak();
					}				
				}break;
				case 4: 												// DATE
				{
					if(eindex0 == 0) 							// 根目录
					{
						if(DatePW != 9999)
						{
							if(val_ip > 0)
								val_ip--;
							else
								val_ip = 3;
						}
					}
					else
					if(eindex0 == 1) 							// 一级目录
					{
						switch(eindex2)
						{
							case 0: 									// HOUR
							{
								if(RTC_TimeStructure.RTC_Hours>0)
									RTC_TimeStructure.RTC_Hours--;
								
								sprintf(buf,"%04d",RTC_TimeStructure.RTC_Hours);
								anzout2(buf);
								RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
							}break;
							case 1: 									// MINU
							{
								if(RTC_TimeStructure.RTC_Minutes>0)
									RTC_TimeStructure.RTC_Minutes--;
								
								sprintf(buf,"%04d",RTC_TimeStructure.RTC_Minutes);
								anzout2(buf);
								RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
							}break;
							case 2: 									// DAY
							{
								if(RTC_DateStructure.RTC_Date>1)
									RTC_DateStructure.RTC_Date--;
								
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Date);
								anzout2(buf);
								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
							}break;
							case 3: 									// MONT
							{
								if(RTC_DateStructure.RTC_Month>1)
									RTC_DateStructure.RTC_Month--;
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Month);
								anzout2(buf);
								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
							}break;
							case 4: 									// YEAR
							{
								if(RTC_DateStructure.RTC_Year>8)
									RTC_DateStructure.RTC_Year--;
								
								sprintf(buf,"%04d",RTC_DateStructure.RTC_Year+2000);
								anzout2(buf);
								RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
							}break;
							default: break;
						}
					}
				}break;
				/*case 4: 												// HOUR
				{
					if(tim.hours>0)
						tim.hours--;
					
					sprintf(buf,"%04d",tim.hours);
					anzout2(buf);
					RTC_SetTime(tim);
				}break;
				case 5: 												// MINU
				{
					if(tim.minutes>0)
						tim.minutes--;
					
					sprintf(buf,"%04d",tim.minutes);
					anzout2(buf);
					RTC_SetTime(tim);
				}break;
				case 6: 												// DAY
				{
					if(dat.day>1)
						dat.day--;
					
					sprintf(buf,"%04d",dat.day);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;
				case 7: 												// MONT
				{
					if(dat.month>1)
						dat.month--;
					
					sprintf(buf,"%04d",dat.month);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;
				case 8: 												// YEAR
				{
					if(dat.year>8)
						dat.year--;
					
					sprintf(buf,"%04d",dat.year+2000);
					anzout2(buf);
					RTC_SetDate(dat);
				}break;*/
				case 7: 												// THRE, 增加门槛SSW菜单，Gavin，2022-02-09 周三 09:52:36
				{
					if(Sensibility>3)
					{
						Sensibility--;
						
						sprintf(buf,"%04d",Sensibility);
						anzout2(buf);
						writinteep(Sensibility,ADRSENSIBILITY);
					}
				}break;
				case 8: 												// CYCL，增加循环时间ULW菜单，Gavin，2022-02-09 周三 09:52:44
				{
					if(ueberlauf>10)
					{
						ueberlauf--;
						
						sprintf(buf,"%04d",ueberlauf);
						anzout2(buf);
						writinteep(ueberlauf,ADRUEBERLAUF);
					}
				}break;
				case 10: 												// LANG
				{
					if(aktLanguage>0)
					{
						aktLanguage--;
						anzout1(texte[30][sp]); 							// LANG
						anzout2(texte[31][sp]); 							// GERM
						sp=aktLanguage;
						writinteep(aktLanguage,ADRLANGUAGE);
					}
				}break;
				case 12: 												// DATF
				{
					if(DatFormat>0) 
					{
						DatFormat--;
						anzout2(texte[35][sp]); 							// EURO
						writinteep(DatFormat,ADRDATFORMAT);
					}
				}break;
				case 13: 												// METH
				{
					if(MeasureMethod>0) 
					{
						MeasureMethod--;
						anzout2(texte[38][sp]); 							// SPEE
						writinteep(MeasureMethod,ADRMEASUREMETHOD);
					}
				}break;
				case 14: 												// TERM
				{
					if(terminologie>0) 
					{
						terminologie--;
						anzout2(texte[47][sp]); 							// CALI
						writinteep(terminologie,ADRTERMINOLOGIE);
					}
				}break;
				case 15: 												// AUTO
				{
					if(autodiam>0) 
					{
						autodiam--;
						anzout2(textaus[sp]);
						writinteep(autodiam,ADRAUTODIAM);
					}
				}break;
				case 16: 												// PRIN
				{
					if(druckmode>0) 
					{
						druckmode--;
						anzout2(texte[58][sp]); 							// EXT
						//writeepbuf((unsigned char*)Print_exit,ADRVERSION+47,1);
						//vers[47] = 0x30;
						writinteep(druckmode,ADRDRUCKMODE);
						
					}
				}break;
				case 17: 												// PRLG
				{
					if(PrinterLanguage>0) 
					{
						PrinterLanguage--;
						anzout2(texte[60][sp]); 							// ASCI
						writinteep(PrinterLanguage,ADRPRINTERLANGUAGE);
					}
				}break;
				case 18: 												// ZECO
				{
					if(NullKorr > -25) 
					{
						NullKorr--;
						sprintf(buf,"%04d",NullKorr);
						anzout2(buf);
						writinteep(NullKorr,ADRNULLKORR);
					}
				}break;
				case 19: 												// AUME
				{
					if(AutoMess>0) 
					{
						AutoMess--;
						anzout2(texte[44][sp]); 							//  OFF
						writinteep(AutoMess,ADRAUTOMESS);
					}
				}break;
				case 20: 												// DOUT
				{
					if(DatenAusgabe>0) 
					{
						DatenAusgabe--;
						anzout2(texte[44][sp]); 							//  OFF
						writinteep(DatenAusgabe,ADRDATENAUSGABE);
					}
				}break;
				case 21: 												// HARD
				{
					if(HardOn>0) 
					{
						HardOn--;
						anzout2(texte[44][sp]); 							//  OFF
						writinteep(HardOn,ADRHARDON);
					}
				}break;
				case 22: 												// DIAM
				{
					if(DiamOn>0) 
					{
						DiamOn--;
						anzout2(texte[44][sp]); 							//  OFF
						writinteep(DiamOn,ADRDIAMON);
					}
				}break;
				case 23: 												// THIC
				{
					if(ThickOn>0) 
					{
						ThickOn--;
						anzout2(texte[44][sp]); 							//  OFF
						writinteep(ThickOn,ADRTHICKON);
					}
				}break;
				case 24: 												// PWMG
				{
					if(eindex0 == 0) 							// 根目录
					{
						if(PwmgPW != 9999)
						{
							if(val_ip > 0)
								val_ip--;
							else
								val_ip = 3;
						}
					}
					else
					if(eindex0 == 1) 							// 一级目录
					{
						if(val_ip > 0)
							val_ip--;
						else
							val_ip = 3;
					}
				}break;
				default: break;
			}		
		}										
	}
}	

// 主菜单CALI校验执行函数----------------------------------------------
void kalibriere(void)
{
	unsigned char calpoi1=0; 							// 0/1: 长度操作/硬度操作
	unsigned char ch; 										// 获取按键码的临时变量
	int printok; 													// 打印报告操作的返回值，0/2/3/x：正常/缺纸/报错/未连接
	unsigned int methsave; 								// Gavin，2022-02-09 周三 14:29:51
	
	methsave = MeasureMethod; 						// Gavin, 2022-02-09 周三 14:22:36
	MeasureMethod = 0; 										// 校验检查强制恒速
	
	bkal = 0; 														// 硬度标志复位，2016/11/21 11:18:08
	dkal = 0; 														// 直径标志复位，2016/11/21 11:18:20
	savefloat(&NwtFak,ADRONWT); 					// 保存当前牛顿系数
	
	// 长度及硬度校验测量 ------------------------------------
	while(1) 
	{
		if(dmena) 													// 长度测量允许
		{
			do 
			{
				if(calpoi1==0)
					anzout1(texte[1][sp]); 				// 直径,DIAM
				else
					anzout1(texte[7][sp]); 				// 硬度,HARD
				anzout2("    ");
				while((ch=gettast())==0);
				if(ch==TLO || ch==TLU) 					// 左上键OR左下键
				{
					if(calpoi1==0) 
						calpoi1=1;
					else 
						calpoi1=0;
				}	
			} while(ch!=TMU && ch!=TMO); 			// 中下键&中上键
		}
		else 																// 长度测量禁止
		{
			do
			{
				anzout1(texte[7][sp]); 					// 硬度,HARD
				while((ch=gettast())==0);
				calpoi1=1;
			}while(ch!=TMU && ch!=TMO); 			// 中下键&中上键
		}
		
		if(ch==TMO) 												// 中上键
		{
			anzout2("    ");
			break;
		}
		
		if(calpoi1==0) 
			kalidurchm(0); 										// 直径校验
		else 
			kalibruch(0); 										// 硬度校验
	}
	
	// 打印报告 ----------------------------------------------
	dp1=dp2=dp3=dp4=0;
	anzout1(texte[8][sp]); 								// PRIN
	anzout2(texte[9][sp]); 								// TING
	ch=0;

	do 
	{
		// 打印校验报告 ----------------------------------------
		if(druckmode==0) 										// EXT，打印模式
		{
			do 
			{
				anzout1(texte[8][sp]); 					// PRIN
				anzout2(texte[9][sp]); 					// TING
				
				StartPrinter();
				if(PrintType == 0) 				// 台式打印机
					printok = druckkp(); 		// 校验报告
				else 											// 微型打印机
					printok = druckkp32();
				
				if(printok) 
				{
					if(printok==2) 
					{
						anzout1(texte[63][sp]); 							// NO P
						anzout2(texte[64][sp]); 							// APER
					}
					else 
					{
						if(printok==3) 
						{
							anzout1(texte[65][sp]); 						// PRI
							anzout2(texte[66][sp]); 						// ERRO
						}
						else 
						{
							anzout1(texte[51][sp]); 						// NO P
							anzout2(texte[52][sp]); 						// RINT
						}
					}
					ch=gettast();
				}
			}while(ch!=TMO && printok); 			// 中上键...
		}
		else 																// 内部打印机 
		{
			do 
			{
				anzout1(texte[8][sp]); 					// PRIN
				anzout2(texte[9][sp]); 					// TING
				printok=druckkp32();
				if(printok) 
				{
					anzout1(texte[51][sp]); 			// NO P
					anzout2(texte[52][sp]); 			// RINT
					ch=gettast();
				}
			}while(ch!=TMO && printok); 			// 中上键...
		}
		
		os_dly_wait(500);
		ch=0;
		// 打印检查报告 ----------------------------------------
		if(!printok) 
		{
			if(druckmode==0) 									// 外部打印机
			{
				do 
				{
					anzout1(texte[8][sp]); 				// PRIN
					anzout2(texte[9][sp]); 				// TING
					
					StartPrinter();
					if(PrintType == 0) 							// 台式打印机
						printok = druckuep(); 				// 检查报告
					else
						printok = druckuep32(); 			// 微型打印机
					
					if(printok) 
					{
						if(printok==2) 
						{
							anzout1(texte[63][sp]); 						// NO P
							anzout2(texte[64][sp]); 						// APER
						}
						else 
						{
							if(printok==3) 
							{
								anzout1(texte[65][sp]); 					// PRI
								anzout2(texte[66][sp]); 					// ERRO
							}
							else 
							{
								anzout1(texte[51][sp]); 					// NO P
								anzout2(texte[52][sp]); 					// RINT
							}
						}
						ch=gettast();
					}
				}while(ch!=TMO && printok); 		// 中上键...
			}
			else  														// 内部打印机
			{
				anzout1(texte[8][sp]); 					// PRIN
				anzout2(texte[9][sp]); 					// TING
				druckuep32();
			}
		}
		ch=0;
		anzout1(texte[53][sp]); 						// REPE
		anzout2(texte[54][sp]); 						// AT
		ch=gettast();
	}while(ch!=TMO); 											// 中上键
	bueb=dueb=0; 													// 硬度和直径，校验完成标志清零
	savefloat(&NwtFak,ADRONWT);
	
	MeasureMethod = methsave; 						// 恢复模式，Gavin，2022-02-09 周三 14:23:52
}

// mode:0,效验;1,检查
void kalidurchm(unsigned char mode) 		// 直径或厚度，2016-11-10 星期四 17:23:45
{
	unsigned char ch;
	DTIM dat;
	float dmsave;

	dmsave=dmeing;
	// 对零 ----------------------------------------
	do 
	{
		anzout1(texte[11][sp]); 						// ZERO
		anzout2(texte[12][sp]); 						// JAW
		while((ch=gettast())==0);
		if(ch==TMO) return; 								// 中上键
	}while(ch!=TMU); 											// 中下键
	
	// 长度校验 ------------------------------------
	blbef(0);
	if(mode==0) 													// 校验
	{
		do 
		{
			anzout1(texte[10][sp]); 					// BLCK
			anzout2("  10");
			while((ch=gettast())==0);
			if(ch==TMO) return; 							// 中上键
		}while(ch!=TMU); 										// 中下键
		
		eichmas=10.0;
		savefloat(&eichmas,ADRLASDIAM);
		blebef();
	}
	
	// 长度检查，5mm -------------------------------
	do 
	{
		anzout1(texte[13][sp]); 						// CON1
		anzout2("  5");
		while((ch=gettast())==0);
		if(ch==TMO) return; 								// 中上键
	}while(ch!=TMU); 											// 中下键
	
	// 长度标准量输出LED
	dmeing=5.0;
	solld[0]=5.0;
	
	// 长度检查量输出LED
	mebef();
	if(Diameter>90) 
		strcpy(buf,"  NA");
	else
		sprintf(buf,"%04.0f",Diameter*100);
	istd[0]=Diameter;
	dp1=dp2=dp3=dp4=0;
	dp2=1;
	anzout2(buf);
	
	// 按键等待，继续/退出
	do 
	{
		while((ch=gettast())==0);
		if(ch==TMO) 							// 中上键
		{
			dmeing=dmsave;
		 	return;
		}
	}while(ch!=TMU); 						// 中下键
	
	// 长度检查，10mm ------------------------------
	do 
	{
		anzout1(texte[14][sp]); 						// CON2
		anzout2("  10");
		while((ch=gettast())==0);
		if(ch==TMO) 		// 中上键
		{
			dmeing=dmsave;
		 	return;
		}
	}while(ch!=TMU); 	// 中下键
	
	// 长度标准量输出LED
	dmeing=10.0;
	solld[1]=10.0;
	
	// 长度检查量输出LED
	mebef();
	if(Diameter>90) 
		strcpy(buf,"  NA");
	else
		sprintf(buf,"%04.0f",Diameter*100);
	istd[1]=Diameter;
	dp1=dp2=dp3=dp4=0;
	dp2=1;
	anzout2(buf);
	
	// 按键等待，继续/退出
	do 
	{
		while((ch=gettast())==0);
		if(ch==TMO) 		// 中上键
		{
			dmeing=dmsave;
		 	return;
		}
	}while(ch!=TMU); 	// 中下键
	
	// 长度检查，15mm ------------------------------
	do 
	{
		anzout1(texte[15][sp]); 						// CON3
		anzout2("  15");
		while((ch=gettast())==0);
		if(ch==TMO) 		// 中上键
		{
			dmeing=dmsave;
		 	return;
		}
	}while(ch!=TMU); 	// 中下键
	
	// 长度标准输出LED
	dmeing=15.0;
	solld[2]=15.0;
	
	// 检查长度输出LED
	mebef();
	if(Diameter>90) 
		strcpy(buf,"  NA");
	else
		sprintf(buf,"%04.0f",Diameter*100);
	istd[2]=Diameter;
	dp1=dp2=dp3=dp4=0;
	dp2=1;
	anzout2(buf);
	
	// 按键等待，继续/退出
	do 
	{
		while((ch=gettast())==0);
		if(ch==TMO) 		// 中上键
		{
			dmeing=dmsave;
		 	return;
		}
	}while(ch!=TMU); 	// 中下键
	
	// 数码管显示锁定 ------------------------------
	//TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);				// 禁止定时器3更新中断
	
	//GPIO_WriteBit(PORT_E, PD0, Bit_SET);
	//GPIO_WriteBit(PORT_E, PD1, Bit_SET);
	//GPIO_WriteBit(PORT_E, PD2, Bit_SET);
	//GPIO_WriteBit(PORT_E, PD3, Bit_SET);
	
	// 测试结果保存 --------------------------------
	if(mode==0) 																		// 校验+检查
	{
		LoadDate(&dat,ADRJUSDDAT);
		WriteDate(&dat,ADRJUSDDATO);	
		AKC_RTCGetOrgDate(&dat);
		WriteDate(&dat,ADRJUSDDAT);
		LoadDate(&dat,ADRKALDDAT);
		WriteDate(&dat,ADRKALDDATO);	
		AKC_RTCGetOrgDate(&dat);
		WriteDate(&dat,ADRKALDDAT);
		dkal=1; 														// 直径或厚度校验模式Caliberation
	}
	else 																						// 不校验，只检查 
	{
		LoadDate(&dat,ADRKALDDAT);
		WriteDate(&dat,ADRKALDDATO);	
		AKC_RTCGetOrgDate(&dat);
		WriteDate(&dat,ADRKALDDAT);
		dkal=0; 														// 2016-11-10 星期四 16:49:56; 直径或厚度检查模式Check
	}
	
	// 数码管显示解锁 ------------------------------
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);				// 开启定时器3更新中断
	
	dmeing=dmsave;
	dp1=dp2=dp3=dp4=0;
	dueb=1; 															// 直径，校验或者检查完成 
}

// mode：0，校验；1，检查
void kalibruch(unsigned char mode) 			// 硬度,2016-11-10 星期四 17:23:50
{
	unsigned char ch;
	char hibu[20];
	float adf;
	int hil;
	float kalwgt=30;
	float checkwgt1=30,checkwgt2=20,checkwgt3=10;
	DTIM dat;
	
	if(!StartKalibrator()) 								// 判断U盘模式或砝码模式
	{
		kalimode=0; 												// 砝码模式
		if(mode==0) 												// 校验
		{
			do 
			{
				anzout1(texte[17][sp]); 									// CPOS
				sprintf(buf,"%2.0fKG",kalwgt);
				anzout2(buf);
				while((ch=gettast())==0);
				if(ch==TRO && kalwgt<50) 				// 右上键
					kalwgt++;
				if(ch==TRU && kalwgt>1) 				// 右下键
					kalwgt--;
				if(ch==TMO) 										// 中上键
					return;
			}while(ch!=TMU); 									// 中下键
			
			hil=readinteep(ADRLBKAL);
			writinteep(hil,ADRLBKALO);
			writinteep((int)(kalwgt * 9.81f),ADRLBKAL);	
			eipbef();
			
			do 
			{
				anzout1(texte[18][sp]); 									// WEIT
				while(gettasto()!=0);
				while((ch=gettasto())==0)
				{
					adf=GetDoseAvrgN(1,100);
					adf /= NwtFak;
					adf/=9.81f;
					os_dly_wait(200);
					sprintf(buf,"%04.0f",adf*10);
					dp3=1;
					anzout2(buf);			
				}
				beep();
				if(ch==TRO || ch==TRU || ch==TLU || ch==TLO) // 右上键OR右下键OR左下键OR左上键
				{
					tbhnull();
				}
			}while(ch!=TMU && ch!=TMO); 			// 中下键&中上键
			writinteep((int)(adf * 9.81f),ADRLBIKAL);	
	
			if(ch!=TMO) 						// 中上键
			{ 
				NwtFak=NwtFak * (adf/kalwgt) ;
				savefloat(&NwtFak,ADRNWTFAK);
				anzout1(texte[28][sp]); 									// NFCT
				sprintf(buf,"%04.0f",NwtFak*100.0f);
				dp1=dp2=dp3=dp4=0;
				dp2=1;
				anzout2(buf);
				os_dly_wait(1000);
			}
		}
		
		// 检查-----------------------------
		dp1=dp2=dp3=dp4=0;
		do 
		{
			anzout1(texte[13][sp]); 					// “CON1”
			sprintf(buf,"%2.0fKG",checkwgt1);
			anzout2(buf); 										// 显示检查量
			
			while((ch=gettasto())==0); 				// 等待按键
			beep();
			
			if(ch==TRO && checkwgt1<50) 			// 右上键
				checkwgt1++;
			
			if(ch==TRU && checkwgt1>1) 				// 右下键
				checkwgt1--;
			
			if(ch==TMO) 											// 中上键
				return;
			
		}while(ch!=TMU); 										// 中下键
		
		savefloat(&checkwgt1,ADRCHECKWGT1); // 保存检查量
		while((ch=gettasto())!=0); 					// 等待按键
		
		do 
		{
			while(gettasto()!=0);
			while((ch=gettasto())==0)
			{
				adf=GetDoseAvrgN(1,100);
				adf /= NwtFak;
				adf/=9.81f;
				sprintf(buf,"%04.0f",adf*10);
				dp3=1; 															// 2015.05.22 增加小数点
				anzout2(buf);			
				os_dly_wait(50);
			}
			beep();
			if(ch==TMO) return; 		// 中上键
			if(ch==TRO || ch==TRU || ch==TLU || ch==TLO) // 右上键OR右下键OR左下键OR左上键
			{
				tbhnull();
			}
		}while(ch!=TMU); 										// 中下键
		while((ch=gettasto())!=0);
	
		sollb[0]=checkwgt1;
		istb[0]=adf;
		
		dp1=dp2=dp3=dp4=0; 									// 2015.05.22 增加小数点消隐
		do 
		{
			anzout1(texte[14][sp]); 										// CON2
			sprintf(buf,"%2.0fKG",checkwgt2);
			anzout2(buf);
			while((ch=gettasto())==0);
			beep();
			if(ch==TRO && checkwgt2<50) 			// 右上键
				checkwgt2++;
			if(ch==TRU && checkwgt2>1) 				// 右下键
				checkwgt2--;
			if(ch==TMO) 											// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		
		savefloat(&checkwgt2,ADRCHECKWGT2);
		while((ch=gettasto())!=0);
	
		do 
		{
			while(gettasto()!=0);
			while((ch=gettasto())==0)
			{
				adf=GetDoseAvrgN(1,100);
				adf /= NwtFak;
				adf/=9.81f;
				sprintf(buf,"%04.0f",adf*10);
				dp3=1; 															// 2015.05.22 增加小数点
				anzout2(buf);			
				os_dly_wait(50);
			}
			beep();
			if(ch==TRO || ch==TRU || ch==TLU || ch==TLO) // 右上键OR右下键OR左下键OR左上键
			{
				tbhnull();
			}
			if(ch==TMO) 						// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		while((ch=gettasto())!=0);
	
		sollb[1]=checkwgt2;
		istb[1]=adf;
	
		dp1=dp2=dp3=dp4=0; 									// 2015.05.22 增加小数点消隐
		do 
		{
			anzout1(texte[15][sp]); 										// CON3
			sprintf(buf,"%2.0fKG",checkwgt3);
			anzout2(buf);
			while((ch=gettasto())==0);
			beep();
			if(ch==TRO && checkwgt3<50) 			// 右上键
				checkwgt3++;
			if(ch==TRU && checkwgt3>1) 				// 右下键
				checkwgt3--;
			if(ch==TMO) 											// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		
		savefloat(&checkwgt3,ADRCHECKWGT3);
		while((ch=gettasto())!=0);
	
		do 
		{
			while(gettasto()!=0);
			while((ch=gettasto())==0)
			{
				adf=GetDoseAvrgN(1,100);
				adf /= NwtFak;
				adf/=9.81f;
				sprintf(buf,"%04.0f",adf*10);
				dp3=1;
				anzout2(buf);			
				os_dly_wait(50);
			}
			beep();
			if(ch==TRO || ch==TRU || ch==TLU || ch==TLO) // 右上键OR右下键OR左下键OR左上键
			{
				tbhnull();
			}
			if(ch==TMO) 						// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		while((ch=gettasto())!=0);
		sollb[2]=checkwgt3;
		istb[2]=adf;
 	}
	else 																	// U盘模式
	{
		kalwgt=300;
		kalimode=1; 												// U盘模式
		if(mode==0) 												// 调整
		{
			do 
			{
				anzout1(texte[45][sp]); 									// CELL
				sprintf(buf,"%3.0fN",kalwgt);
				anzout2(buf);
				while((ch=gettast())==0);
				if(ch==TRO && kalwgt<400) 			// 右上键
					kalwgt++;
				if(ch==TRU && kalwgt>10) 				// 右下键
					kalwgt--;
				if(ch==TMO) 										// 中上键
					return;
			}while(ch!=TMU); 									// 中下键
			tbhnull();
			beep();
			adf=GetKaliVal(&kalwgt);
			hil=readinteep(ADRLBKAL);
			writinteep(hil,ADRLBKALO);
			hil=(int)kalwgt;
			writinteep(hil,ADRLBKAL);	

			hil = (int)kalwgt;
			sprintf(buf,"%04.0f",(float)hil);
			anzout1(buf);
			
			hil = (int)adf;
			sprintf(buf,"%04.0f",(float)hil);
			anzout2(buf);
			
			os_dly_wait(3000);
			hil=(int)adf;
			writinteep(hil,ADRLBIKAL);	
	
			if(ch!=TMO) 						// 中上键
			{ 
				dp1=dp2=dp3=dp4=0;
				dp10=dp20=dp30=dp40=0;
				NwtFak=NwtFak * (kalwgt/adf) ;
				savefloat(&NwtFak,ADRNWTFAK);
				anzout1(texte[28][sp]); 									// NFCT
				sprintf(buf,"%04.0f",NwtFak*100.0f);
				dp2=1;
				anzout2(buf);
				os_dly_wait(2500);
			}
		}
		
		// 检查程序-------------------------
		checkwgt1=300;
		checkwgt2=200;
		checkwgt3=100;
		dp1=dp2=dp3=dp4=0;
		do 
		{
			anzout1(texte[13][sp]); 					// CON1
			sprintf(buf,"%3.0fN",checkwgt1);
			anzout2(buf);
			while((ch=gettasto())==0);
			beep();
			if(ch==TRO && checkwgt1<400) 			// 右上键
				checkwgt1++;
			if(ch==TRU && checkwgt1>10) 			// 右下键
				checkwgt1--;
			if(ch==TMO) 											// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		adf=GetKaliVal(&checkwgt1);
		checkwgt1=round0(checkwgt1);
		savefloat(&checkwgt1,ADRCHECKWGT1);
		
		// Gavin, 206-8-2, 2019-07-26 ??? 15:58:39
		//sprintf(buf,"%04.0f",adf*10);
		//dp30=1;
		sprintf(buf,"%04.0f",adf); 			// U盘工具获取的测量值,2019-07-27 星期六 09:58:38
		anzout2(buf);			
		
		sprintf(buf,"%04.0f",checkwgt1); 		// 设备自身获取的测量值,2019-07-27 星期六 09:58:44
		anzout1(buf);			
		beep();
		os_dly_wait(2500);
		sollb[0]=checkwgt1;
		istb[0]=adf;

		dp1=dp2=dp3=dp4=0;
		dp10=dp20=dp30=dp40=0;
		do 
		{
			anzout1(texte[14][sp]); 					// CON2
			sprintf(buf,"%3.0fN",checkwgt2);
			anzout2(buf);
			while((ch=gettasto())==0);
			beep();
			if(ch==TRO && checkwgt2<400) 			// 右上键
				checkwgt2++;
			if(ch==TRU && checkwgt2>10) 			// 右下键
				checkwgt2--;
			if(ch==TMO) 											// 中上键
				return;
		}while(ch!=TMU); 										// 中下键

		adf=GetKaliVal(&checkwgt2);
		checkwgt2=round0(checkwgt2);
		savefloat(&checkwgt2,ADRCHECKWGT2);
		
		// Gavin, 206-8-3, 2019-07-26 星期五 15:58:39
		//dp3=1;
		//sprintf(buf,"%04.0f",adf*10);
		sprintf(buf,"%04.0f",adf); 			// U盘工具获取的测量值,2019-07-27 星期六 09:59:03
		anzout2(buf);
		sprintf(buf,"%04.0f",checkwgt2); 		// 设备自身获取的测量值,2019-07-27 星期六 09:59:08
		anzout1(buf);			
		beep();
		os_dly_wait(2500);
		sollb[1]=checkwgt2;
		istb[1]=adf;

		dp1=dp2=dp3=dp4=0;
		dp10=dp20=dp30=dp40=0;
		do 
		{
			anzout1(texte[15][sp]); 					// CON3
			sprintf(buf,"%3.0fN",checkwgt3);
			anzout2(buf);
			while((ch=gettasto())==0);
			beep();
			if(ch==TRO && checkwgt3<400) 			// 右上键
				checkwgt3++;
			if(ch==TRU && checkwgt3>10) 			// 右下键
				checkwgt3--;
			if(ch==TMO) 											// 中上键
				return;
		}while(ch!=TMU); 										// 中下键
		savefloat(&checkwgt3,ADRCHECKWGT3);

		adf=GetKaliVal(&checkwgt3);
		checkwgt3=round0(checkwgt3);
		
		// Gavin, 206-8-4, 2019-07-26 星期五 15:58:39
		//dp3=1;
		//sprintf(buf,"%04.0f",adf*10);
		sprintf(buf,"%04.0f",adf); 					// U盘工具获取的测量值,2019-07-27 星期六 09:59:36
		anzout2(buf);			
		sprintf(buf,"%04.0f",checkwgt3); 		// 设备自身获取的测量值,2019-07-27 星期六 09:59:41
		anzout1(buf);			
		beep();
		os_dly_wait(2500);
		sollb[2]=checkwgt3;
		istb[2]=adf;

	}
	
	dp1=dp2=dp3=dp4=0;
	dp10=dp20=dp30=dp40=0;
	
	if(kalimode==0) 											// 砝码模式
	{
		anzout1(texte[26][sp]); 						// DISM
		anzout2(texte[27][sp]); 						// OUNT
		gettast();	
		backreset();
	}
	
	anzout2("    ");
	
	// 调整和检查的时间及工具类别的信息保存
	// 校验/检查模式，0：校验+检查；1：只做检查，不校验
	if(mode==0) 													// 校验/检查模式，0：校验+检查；
	{
		LoadDate(&dat,ADRJUSBDAT); 					// 读出上一次硬度校验日期
		WriteDate(&dat,ADRJUSBDATO); 				// 写入本次校验的上一次校验日期
		AKC_RTCGetOrgDate(&dat); 						// 读出当前日期
		WriteDate(&dat,ADRJUSBDAT); 				// 写入本次校验的日期
		LoadDate(&dat,ADRKALBDAT); 					// 读出上一次硬度检查日期
		WriteDate(&dat,ADRKALBDATO); 				// 写入本次校验的上一次检查日期
		AKC_RTCGetOrgDate(&dat); 						// 读出当前日期
		WriteDate(&dat,ADRKALBDAT); 				// 写入本次检查日期
		bkal=1; 														// 硬度校验模式
		
		//调整过程使用的工具信息保存(砝码/U盘)-----------------
		hil=readinteep(ADRKALIMODE); 				// 读出当前值
		writinteep(hil,ADRLASKMOD); 				// 保存为上一次的值
		writinteep(kalimode,ADRKALIMODE); 	// 改写并保存当前值
		
		if(kalimode) 												// U盘模式，保存ID号
		{
			LoadKalID(hibu,ADRKALIVERS); 			// 读
			WriteKalID(hibu,ADRKALIVERSO); 		// 写
			WriteKalID(kalivers,ADRKALIVERS); // 改
			LoadDosID(hibu,ADRDOSNR); 				// 读
			WriteDosID(hibu,ADRDOSNRO); 			// 写
			WriteDosID(dosnr,ADRDOSNR);	 			// 改	
		}
	}
	else 																	// 1：只做检查
	{
		writinteep(kalimode,ADRKALIMODE);		// 保存kailimode,0x01是U盘模式,0x00是砝码模式 2021/10/14
		LoadDate(&dat,ADRKALBDAT); 					// 读
		WriteDate(&dat,ADRKALBDATO); 				// 写	
		AKC_RTCGetOrgDate(&dat); 						// 获取实时日期
		WriteDate(&dat,ADRKALBDAT); 				// 改
		bkal=0; 														// 2016-11-10 星期四 17:21:10,硬度检查模式
	}
	
	// Gavin,2019-07-26 星期五 14:22:19
	// 修改为，如果只是检查模式，不做调整，则不记录使用的工具是U盘还是砝码，
	// 也不记录U盘的ID号；
	// 只有检测到执行了调整程序才记录相关信息
	/*
	hil=readinteep(ADRKALIMODE);
	writinteep(hil,ADRLASKMOD);
	writinteep(kalimode,ADRKALIMODE);
	
	if(kalimode) 													// U盘模式
	{
		LoadKalID(hibu,ADRKALIVERS);
		WriteKalID(hibu,ADRKALIVERSO);
		WriteKalID(kalivers,ADRKALIVERS);
		LoadDosID(hibu,ADRDOSNR);
		WriteDosID(hibu,ADRDOSNRO);
		WriteDosID(dosnr,ADRDOSNR);		
	}
	*/

	dp1=dp2=dp3=dp4=0;
	bueb=1; 															// 硬度，校验或者检查完成
}

// 主菜单CONT检查执行函数---------------------------------------------
void kontrolle(void)
{
	unsigned char calpoi1=0,ch,ch1;
	unsigned int methsave; 								// Gavin，2022-02-09 周三 14:25:52
	
	methsave = MeasureMethod; 						// Gavin, 2022-02-09 周三 14:25:35
	MeasureMethod = 0; 										// 校验检查强制恒速
	
	bkal = 0; 														// 硬度标志复位,2016/11/21 11:17:37
	dkal = 0; 														// 直径标志复位,2016/11/21 11:17:50
	
	do 
	{
		do 
		{
			if(dmena) 												// 直径测试允许
			{
				if(calpoi1==0)
					anzout1(texte[1][sp]); 				// 直径,DIAM
				else
					anzout1(texte[7][sp]); 				// 硬度,HARD
				anzout2("    ");
				while((ch=gettast())==0);
				
				if(ch==TLO || ch==TLU) 					// 左上键或左下键
				{
					if(calpoi1==0) 
						calpoi1=1;
					else 
						calpoi1=0;
				}	
			}
			else 
			{
				anzout1(texte[7][sp]); 					// 硬度,HARD
				calpoi1=1;
				while((ch=gettast())==0);
			}
		} while(ch!=TMU && ch!=TMO); 				// 不是中下键，也不是中上键
		
		if(ch==TMU) 												// 中下键
		{
			if(calpoi1==0) 
				kalidurchm(1); 									// 直径检查
			else 
				kalibruch(1); 									// 硬度检查
		}
	}while(ch!=TMO); 											// 中上键
	
	dp1=dp2=dp3=dp4=0;
	anzout1(texte[8][sp]); 								// PRIN
	anzout2(texte[9][sp]); 								// TING
	do 
	{
		ch=0;
		if(druckmode==0) 										// EXT 模式
		{
			StartPrinter();
			if(PrintType == 0) 								// 台式打印机
			{		
				while((ch1=druckuep())!=0 && ch!=TMO) 		// 检查报告，中上键
				{
					if(ch1==2) 
					{
						anzout1(texte[63][sp]); 		// NO P
						anzout2(texte[64][sp]); 		// APER
					}
					else 
					{
						if(ch1==3) 
						{
							anzout1(texte[65][sp]); 	// PRI 
							anzout2(texte[66][sp]); 	// ERRO
						}
						else 
						{
							anzout1(texte[51][sp]); 	// NO P
							anzout2(texte[52][sp]); 	// RINT
						}
					}
					ch=gettast();
				}
			}
			else 															// 微型打印机
			{
				druckuep32();
			}
		}
		else 																// INT
		{
			druckuep32();
		}
		ch=0;
		anzout1(texte[53][sp]); 						// REPE
		anzout2(texte[54][sp]); 						// AT
		ch=gettast();
	}while(ch!=TMO); 											// 中上键
	
	bueb=dueb=0; 													// 硬度和直径，检查完成标志清零
	anzout2("    ");
	
	MeasureMethod = methsave; 						// 恢复模式，Gavin，2022-02-09 周三 14:26:18
}

void showID(void)
{
	unsigned int i;

//	getid(id);
	dp1=dp2=dp3=dp4=dp10=dp20=dp30=dp40=0;
	while(gettasto()!=0)
		os_tsk_pass();
	while(gettasto()==0) {
		buf[0]='I';
		buf[1]='D';
		buf[2]=id[0];
		buf[3]=id[1];
		buf[4]=0;
		anzout1(buf);
		buf[0]=id[2];
		buf[1]=id[3];
		buf[2]=id[4];
		buf[3]=id[5];
		anzout2(buf);
		i=0;
		while(gettasto()==0 && i++<100)
			os_dly_wait(20);
		buf[0]=id[6];
		buf[1]=id[7];
		buf[2]=id[8];
		buf[3]=id[9];
		anzout1(buf);
		buf[0]=id[10];
		buf[1]=id[11];
		buf[2]=id[12];
		buf[3]=id[13];
		anzout2(buf);
		i=0;
		while(gettasto()==0 && i++<100)
			os_dly_wait(20);
	}
	while(gettasto()!=0)
		os_tsk_pass();
}

