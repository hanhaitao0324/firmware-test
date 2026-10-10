/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    system.c
 *      Purpose: system 
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"   							// Device header
#include "eeprom.h"
#include "kbd.h"
#include "led.h"
#include "stdio.h"
#include "beeper.h"
#include "system.h"
#include "utils.h"
#include "string.h"
#include "mcp3201.h"
#include "eeprom.h"
#include "io.h"
#include "time.h"
#include "timer.h"
#include "rtc.h"

const Platform_Data_s SYSTEM_Default = 
{
	"000000",            //serial number of the device
////	DEVICE_VDT1,         //device type
////	PASSWORD,            //settings password
	LANGUAGE_MODE_GER,	 //default language
//	0,					//NOLICENSE,

//	"000000",		//MESSTIME,
//	0,					//MESSZEITL,
//	10,					//MESSZEITH,
//	0,					//TMODE,
//	380,				//FULLDAYS,
//	3,					//PERIODS,
//	0,					//AKTPERIOD,
//	0,					//PRINTERFRAMES,
//	0,					//AUTOPRINT,
//	0,					//PENETRATION,
////	0,					//DAYSLEFT,
////	0.0,				//DATEOFFSET,
////	0,					//LASTDAY,
////	{1,1,15,0,0,0},					//LASTDATE, 01.01.2015,00:00:00
	150,					//default 150mm Exsiccator
  0,            //Penetration_inverse 0=no,1=yes
 {0,0,0,0,0,0,0},  //place for adding new values
	0                    //CRC32
};

Platform_Data_s *Platform_Data = &SYSTEM.Platform_Data;	
MCP3201_Data_s	*ADC_Data 		 = &SYSTEM.MCP_3201_Data;
SYSTEM_Data_s	SYSTEM;


static int fnkindex;
static int ch;
static int speedup	= 0;
static int MaxVac		= MAXVACUM;

int tmpVacuum,aktPress	=	0;
int AutoPrint,tMode			=	0;	// 时间模式，0：分钟 1：小时
int Vacuum,RedVacuum,messZeitL,messZeitH,ablZeitH,ablZeitL,Penetration;
int mepresure[10],metimh[10],metiml[10];
unsigned char dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
unsigned int PrinterPort,PrinterPaper,PrinterLanguage,PrinterFrames;
unsigned int DatFormat;
uint8_t Abbruch 				= CHANCEL_NO;
char PresReduced 				= 0;

extern char id[];
extern char buf[];
extern float VacM, VacB;

bool FirstReached	=	false;
bool MessFlg			=	false;	// 测量是否正在进行
bool CountFree		=	false;
int intervall;
int count;
char ParaLock;	// 参数锁定

void logo(void);
void LoadFixVal(void);
void einstell(void);
void kalibriere(void);
void kontrolle(void);
void messung(void);
void showID(void);
void pumpe_entl_start(void);
	
extern osThreadId DispTask;                        /* assigned task id of task:  DispTask */
extern void druckergebnis(void);
extern void AKC_RTCGetDate(DTIM *tim);
extern void AKC_RTCSetDate(DTIM tim) ;
extern void StartLicManagement(void);
extern void TestLicense(void);
extern void ShowDaysLeft(void);
extern unsigned int GetDaysLeft(void);

static char *text0[4]={"ZEIT","TIME","TIME","TIME"};
static char *text1[4]={"VAC ","VAC ","VAC ","VAC "};
static char *text3[4]={"DRUK","PRT ","PRT ","PRT "};
static char *text4[4]={"KALI","CALI","CALI","CALI"};
static char *text5[4]={"KONT","CONT","CONT","CONT"};
static char *text6[4]={"EINS","SETT","SETT","SETT"};

static char *text7[4]={"DRUC","PRIN","PRIN","PRIN"};
static char *text8[4]={"EN  ","TING","TING","TING"};

static char *text9[4]={"STUN","HOUR","HOOR","HOUR"};
static char *text10[4]={"MINU","MINU","MINU","MINU"};
static char *text11[4]={"TAG ","DAY ","DAY ","DAY "};
static char *text12[4]={"MONA","MONT","MONT","MONT"};
static char *text13[4]={"JAHR","YEAR","YEAR","YEAR"};

static char *text14[4]={"ID  ","ID  ","ID  ","ID  "};
static char *text15[4]={"ZMOD","TMOD","TMOD","TMOD"};
static char *text16[4]={"SPRA","LANG","LANG","LANG"};
static char *text17[4]={"LIZS","LICS","LICS","LICS"};
static char *text18[4]={"DEUT","GERM","GERM","GERM"};
static char *text19[4]={"ENGL","ENGL","ENGL","ENGL"};
static char *text20[4]={"VERS","VERS","VERS","VERS"};
static char *text21[4]={"PENE","PENE","PENE","PENE"};
static char *text22[4]={"EXSI","EXSI","EXSI","EXSI"};
static char *text23[4]={"PINV","PINV","PINV","PINV"};
static char *text24[4]={"DEV ","DEV ","DEV ","DEV "};
static char *text25[4]={"SNUM","SNUM","SNUM","SNUM"};

						//	 0	   1	 2	   3	 4	   5	 6	   7	 8	    9	  10	 11	    12	   13	  14
static char **texte[25]={text0,text1,text3,text4,text5,text6,text7,text8,text9,text10,text11,text12,text13,text14,text15,
	 					//  15	   16	  17	 18	 	19	   20	  21	 22	    23	   24
						text16,text17,text18,text19,text20,text21,text22,text23,text24,text25};
static char **mtexte[6]={text0,text1,text3,text4,text5,text6};

static char *textein[4]={"EIN ","ON  ","ON  ","ON  "};
static char *textaus[4]={"AUS ","OFF ","OFF ","OFF "};

static char *textminu[4]={"MINU","MINU","MINU","MINU"};
static char *textstun[4]={"STUN","HOUR","HOUR","HOUR"};

static char *lt1[18]={"    ","    ","    ","    ","    ","   E","  ER"," ERW","ERWE","RWEK","WEKA","EKA ","KA V","A VD"," VDT","VDTS","DTS ","TS  "};
static char *lt2[18]={"    ","   E","  ER"," ERW","ERWE","RWEK","WEKA","EKA ","KA V","A VD"," VDT","VDTS","DTS ","TS  ","S   ","    ","    ","    "};

extern const char *month_names[];
 
/*******************************************************************************
* Function Name  : VDT_System_init
* Description    : Initialize the VDT 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void VDT_System_init(void){
  LoadFixVal();
	fnkindex=0;
}
/*******************************************************************************
* Function Name  : StartBootloader
* Description    : Initialize the FW update, reboot to bootloader 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StartBootloader(void){
	unsigned int * ptr;
		
	ptr =(unsigned int *)RAM_KEY_ADDR;
	*ptr = BOOTLOADER_KEY1;
	ptr++;
	*ptr = BOOTLOADER_KEY2;
	
	__disable_irq();

	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(1);
	IWDG_ReloadCounter();
	IWDG_Enable();
	while(1);
}
/*******************************************************************************
* Function Name  : LoadFixVal
* Description    : loads eeprom values at startup
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/	
void LoadFixVal(void){
  uint8_t i;
  
	if(readeeprom(ADRSIGNATUR)!=SIGNATUR) {
		InitEEPROM(0);
	}

	Platform_Data->Language = readeeprom(ADR_SPRACHE);
	if(Platform_Data->Language > LANGUAGE_MODE_LAST-1) Platform_Data->Language = LANGUAGE_MODE_GER;

	messZeitL=readinteep(ADRMESSZEITL);
	if(messZeitL>59) messZeitL=0;
	messZeitH=readinteep(ADRMESSZEITH);
	if(messZeitH>99) messZeitH=0;
	Vacuum=readinteep(ADRVACUUM);
	if(Vacuum>MaxVac) Vacuum = MAXVACUM;
	Penetration=readinteep(ADRPENETRATION);
	if(Penetration>10) Penetration=0;
	tMode=readinteep(ADRTMODE);
	if(tMode<0 || tMode>1) tMode=0;
	loadfloat(&VacM, ADRVACM);
	if(VacM<0.25 || VacM>0.55) VacM=0.4;
	loadfloat(&VacB,ADRVACB);
	if(VacB<-500.0 || VacB>0.0) VacB=-230.0;
	Platform_Data->Exisccator_size = readinteep(ADREXSICCATOR_SIZE);					// Exsiccator
	
	// Gavin，2023/11/3 周五 11:32:30 --------------------------------------------------------------------
	//if(Platform_Data->Exisccator_size > 250){ Vacuum = MAXVACUM_EXISCCATOR; MaxVac = MAXVACUM_EXISCCATOR;}
	if(Platform_Data->Exisccator_size > 250) 				// 容器尺寸大于250mm
	{
		MaxVac = MAXVACUM_EXISCCATOR; 								// 压力上限由900修改为700
		if(Vacuum > MaxVac) 													// 上一次测试压力值大于上限
			Vacuum = MaxVac; 														// 修改为上限
	}
	// End, 2023/11/3 周五 11:33:41
	
	Platform_Data->Penetration_inverse = readinteep(ADRPENETRATION_INVERSE);
	if(Platform_Data->Penetration_inverse > 1){Platform_Data->Penetration_inverse = 0;}

  for (i=0;i<SERIAL_NUMBER_LEN;i++){
    Platform_Data->SerialNumber[i] = readeeprom(ADRSERNUM+i);               //read Serial Number
  }
	getid(id);																																//loads the chip id into global variable
	AutoPrint=readeeprom(ADRAUTOPRINT);
	if(AutoPrint>1) AutoPrint=0;
	ParaLock = readeeprom(ADRLOCKPARA);
	if(ParaLock > 1) ParaLock = 0;
}
/*******************************************************************************
* Function Name  : Display_Handler
* Description    : the menu
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Display_Handler(void)
{
	if(fnkindex>=0 && fnkindex<6) 
	{
		dp10=dp20=dp30=dp40=0;
		if(fnkindex==0 && tMode==1) dp40=1;
		LED_Print(LED_DISPLAY_LEFT, 0, mtexte[fnkindex][Platform_Data->Language]);
	}
	else
		LED_Print(LED_DISPLAY_LEFT, 0,"IERR");

	switch(fnkindex) 
	{
		case 0:
			sprintf(buf,"%02d%02d",messZeitH,messZeitL);
			dp1=dp2=dp3=dp4=0;
			dp2=1;
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		break;
		case 1:	
			sprintf(buf,"%04d",Vacuum);
			dp1=dp2=dp3=dp4=0;
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		break;
		case 2:
			dp1=dp2=dp3=dp4=0;
			dp10=dp20=dp30=dp40=0;
			if(AutoPrint)
				LED_Print(LED_DISPLAY_RIGHT, 0,textein[Platform_Data->Language]);
			else
				LED_Print(LED_DISPLAY_RIGHT, 0,textaus[Platform_Data->Language]);
		break;				
		case 3:
			dp1=dp2=dp3=dp4=0;
			LED_Print(LED_DISPLAY_RIGHT, 0,"    ");			
			break;				
		case 4:
			dp10=dp20=dp30=dp40=0;
			dp1=dp2=dp3=dp4=0;
			LED_Print(LED_DISPLAY_RIGHT, 0,"    ");			
		break;				
		case 5:
			dp10=dp20=dp30=dp40=0;
			dp1=dp2=dp3=dp4=0;
			LED_Print(LED_DISPLAY_RIGHT, 0,"    ");			
		break;				
		default:
			LED_Print(LED_DISPLAY_RIGHT, 0,"FERR");			
		break;	
	}

	while((ch=gettast())==0);
	BEEPER_PlayTones(tones_1khz);

	if(fnkindex == 0)
	{
		count=0;
		osDelay(20);

		while(gettasto() == KBD_CODE_MENU_DOWN_KBD_CODE_VALUE_DOWN && count++<16) 
		{
			osDelay(500);
		}

		if(count<15) 
			;
		else
		{
			ch = 0;
			
			if(ParaLock == 0)
			{
				ParaLock = 1;
				BEEPER_PlayTones(tones_lock);
			}
			else
			{
				ParaLock = 0;
				BEEPER_PlayTones(tones_unlock);
			}
			writeeeprom(ParaLock, ADRLOCKPARA);
		}
	}

	if(ch==KBD_CODE_STOP)
		fnkindex=0;
	if(ch==KBD_CODE_START) 
	{
		switch(fnkindex) 
		{
			case 0:
				messung();
			break;
			case 1:
			break;
			case 2:
				if(Platform_Data->Language==0) 
				{
					LED_Print(LED_DISPLAY_LEFT, 0,"DRUC");
					LED_Print(LED_DISPLAY_RIGHT, 0,"KEN");						
				}
				else 
				{
					LED_Print(LED_DISPLAY_LEFT, 0,"PRIN");
					LED_Print(LED_DISPLAY_RIGHT, 0,"TING");
				}
				druckergebnis();	// 打印报告
				if(Platform_Data->Language==0)
					LED_Print(LED_DISPLAY_LEFT, 0,"DRUK");
				else
					LED_Print(LED_DISPLAY_LEFT, 0,"PRIN");
				dp1=dp2=dp3=dp4=0;
				if(AutoPrint)
					LED_Print(LED_DISPLAY_RIGHT, 0,textein[Platform_Data->Language]);
				else
					LED_Print(LED_DISPLAY_RIGHT, 0,textaus[Platform_Data->Language]);
				ch=gettast();
			break;	
			case 3:
				kalibriere();	// 校正
			break;	
			case 4:
				kontrolle();	// 测试压力
			break;	
			case 5:
				einstell();		// 设置菜单
			break;	
		}	
	}
	if(ch==KBD_CODE_MENU_UP) 
	{
		if(ParaLock == 0)
		{
			if(fnkindex<5) 
				fnkindex++;
		}
	}
	if(ch==KBD_CODE_MENU_DOWN) 
	{
		if(fnkindex>0) fnkindex--;
	}
	if(ch==KBD_CODE_VALUE_UP) 
	{
		if(ParaLock == 0)
		{
			switch(fnkindex) 
			{
				case 0:
					speedup=0;
					if(messZeitH<100)
						messZeitL++;
					if(messZeitL>59) 
					{
						messZeitL=0;
						if(messZeitH<99) messZeitH++;
					}	 
					sprintf(buf,"%02d%02d",messZeitH,messZeitL);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
					osDelay(100);
					while(gettasto()==KBD_CODE_VALUE_UP && messZeitH<99) 
					{
						if(speedup++<20)
							messZeitL++;
						else
							messZeitL+=10;
						if(messZeitL>59) 
						{
							messZeitL=0;
							if(messZeitH<99) messZeitH++;
						}	 
						sprintf(buf,"%02d%02d",messZeitH,messZeitL);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
						if(speedup++<20)
							osDelay(20);
						else
							osDelay(1);
					}
					writinteep(messZeitL,ADRMESSZEITL);
					writinteep(messZeitH,ADRMESSZEITH);
				break;
				case 1:
					if(Vacuum<MaxVac) 
					{
						Vacuum+=1;
						sprintf(buf,"%04d",Vacuum);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
						osDelay(100);
						while(gettasto()==KBD_CODE_VALUE_UP && Vacuum<MaxVac) 
						{
							if(speedup++<20)
								Vacuum+=1;
							else
								Vacuum+=10;
							if(Vacuum>MaxVac) Vacuum=MaxVac;
							sprintf(buf,"%04d",Vacuum);
							LED_Print(LED_DISPLAY_RIGHT, 0,buf);
							osDelay(20);
						}
						writinteep(Vacuum,ADRVACUUM);
					}
				break;		
				case 2:
					if(AutoPrint==0) 
					{
						AutoPrint=1;
						LED_Print(LED_DISPLAY_RIGHT, 0,textein[Platform_Data->Language]);
					}	
					else 
					{
						AutoPrint=0;
						LED_Print(LED_DISPLAY_RIGHT, 0,textaus[Platform_Data->Language]);
					}	
					writeeeprom(AutoPrint,ADRAUTOPRINT);
				break;
			}
		}
	}										
	if(ch==KBD_CODE_VALUE_DOWN) 
	{
		if(ParaLock == 0)
		{
			switch(fnkindex) 
			{
				case 0:
					speedup=0;		
					if(messZeitH >= 0 && messZeitL >=0 )
					{
						if(messZeitH==0)
						{
							if(messZeitL>10) messZeitL--;
						}
						else 
						{	
							messZeitL--;
						}
						if(messZeitL<0) 
						{
							messZeitL=59;
							if(messZeitH>0) messZeitH--;
						}
						sprintf(buf,"%02d%02d",messZeitH,messZeitL);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
						osDelay(100);
						while(gettasto()==KBD_CODE_VALUE_DOWN && messZeitH>=0) 
						{
							if(messZeitH>0) 
							{// hier die minimale Zeit auf 10 sec. begrenzen
								if(speedup++<20)
									messZeitL--;
								else
									messZeitL-=10;
							}
							else
							{
								if(speedup++<20)
								{
									if(messZeitL>10) messZeitL--;
								}
								else
								{
									if(messZeitL>11 && ((messZeitL-10)>10))
									{
										messZeitL-=10;
									}
									else
									{
										if(messZeitL>10)
											messZeitL--;
									}				
								}
							}								
							if(messZeitL<0) 
							{
								messZeitL=59;
								if(messZeitH>0) messZeitH--;
							}
							sprintf(buf,"%02d%02d",messZeitH,messZeitL);
							LED_Print(LED_DISPLAY_RIGHT, 0,buf);
							osDelay(20);
						}
						writinteep(messZeitH,ADRMESSZEITH);
						writinteep(messZeitL,ADRMESSZEITL);
					}
				break;
				case 1:
					speedup=0;
					if(Vacuum>1) 
					{
						Vacuum-=1;
						sprintf(buf,"%04d",Vacuum);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
						osDelay(100);
						while(gettasto()==KBD_CODE_VALUE_DOWN && Vacuum>1) 
						{
							if(speedup++<20 || Vacuum<10)
								Vacuum-=1;
							else
								Vacuum-=10;
							sprintf(buf,"%04d",Vacuum);
							LED_Print(LED_DISPLAY_RIGHT, 0,buf);
							osDelay(20);
						}
						writinteep(Vacuum,ADRVACUUM);
					}
				break;		
				case 2:
					if(AutoPrint==0) 
					{
						AutoPrint=1;
						LED_Print(LED_DISPLAY_RIGHT, 0,textein[Platform_Data->Language]);
					}	
					else 
					{
						AutoPrint=0;
						LED_Print(LED_DISPLAY_RIGHT, 0,textaus[Platform_Data->Language]);
					}	
					writeeeprom(AutoPrint,ADRAUTOPRINT);
				break;
			}	
		}		
	}
}
/*******************************************************************************
* Function Name  : logo
* Description    : displays at startup the logo as scrolltext
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void logo(void) {
	unsigned char i;

	dp1=dp2=dp3=dp4=dp10=dp20=dp30=dp40=0;
	while(gettasto()==KBD_CODE_NONE) {
		for(i=0;i<18;i++) {
			if(gettasto()!=KBD_CODE_NONE) break;
					LED_Print(LED_DISPLAY_LEFT, 0,lt1[i]);
					LED_Print(LED_DISPLAY_RIGHT, 0,lt2[i]);
			osDelay(150);
		}

	}
	BEEPER_PlayTones(tones_1khz);		
	LED_Print(LED_DISPLAY_LEFT, 0,"    ");
	LED_Print(LED_DISPLAY_RIGHT, 0,"    ");
	osDelay(150);
}

/*******************************************************************************
* Function Name  : einstell
* Description    : menu for adjusting settings
* Input          : None
* Output         : None
* Return         : None
  *******************************************************************************/
void einstell(void)
{
	unsigned char einstell_menu_index=0,ch;
	int i;
	DTIM dtim;
	unsigned char time_change_flag=0; 		// 时间修改标志

	i=0;
	while(gettasto()!=KBD_CODE_NONE && i++<10) 
	{
		osDelay(500);
	}
	if(i<9) return;
	BEEPER_PlayTones(tones_1khz);

	//AKC_RTCGetDate(&dtim);
	
	while(1)
	{
		switch(einstell_menu_index) 
		{
			case 0:
			{
				dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_LEFT, 0, texte[14][Platform_Data->Language]);
				
				if(tMode==0) 
				{
					LED_Print(LED_DISPLAY_RIGHT, 0,textminu[Platform_Data->Language]);
				}
				else 
				{
					LED_Print(LED_DISPLAY_RIGHT, 0,textstun[Platform_Data->Language]);			
				}
			}break;			
			case 1:
      {
      	dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_LEFT, 0, texte[20][Platform_Data->Language]);
				
				sprintf(buf,"%4d",Penetration);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0, buf);
			}break;
			case 2:
      {
      	dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_LEFT, 0, texte[22][Platform_Data->Language]);
				
        if(Platform_Data->Penetration_inverse==0)
        {
          LED_Print(LED_DISPLAY_RIGHT, 0,textaus[Platform_Data->Language]);
        }
        else
        {
          LED_Print(LED_DISPLAY_RIGHT, 0,textein[Platform_Data->Language]);
        }
        dp1=dp2=dp3=dp4=0;
			}break;
			case 3:
			{
				dp10=dp20=dp30=dp40=0;	
        LED_Print(LED_DISPLAY_LEFT, 0, texte[15][Platform_Data->Language]);
        
				if(Platform_Data->Language==0) 
				{
          LED_Print(LED_DISPLAY_RIGHT, 0,texte[17][Platform_Data->Language]);
				}
				else 
				{
					LED_Print(LED_DISPLAY_RIGHT, 0,texte[18][Platform_Data->Language]);			
				}
			}break;			
			case 4:
			{
				dp10=dp20=dp30=dp40=0;	
        LED_Print(LED_DISPLAY_LEFT, 0, texte[13][Platform_Data->Language]);
				LED_Print(LED_DISPLAY_RIGHT, 0,"    ");
			}break;
			case 5:
      {
      	dp10=dp20=dp30=dp40=0;	
        LED_Print(LED_DISPLAY_LEFT, 0, texte[8][Platform_Data->Language]);
				sprintf(buf,"%4d",dtim.Hour);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			}break;
			case 6:
			{
				dp10=dp20=dp30=dp40=0;
        LED_Print(LED_DISPLAY_LEFT, 0, texte[9][Platform_Data->Language]);
				sprintf(buf,"%4d",dtim.Minute);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			}break;
			case 7:
			{
				dp10=dp20=dp30=dp40=0;	
        LED_Print(LED_DISPLAY_LEFT, 0, texte[10][Platform_Data->Language]);
				sprintf(buf,"%4d",dtim.Day);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			}break;
			case 8:
			{
				dp10=dp20=dp30=dp40=0;	
        LED_Print(LED_DISPLAY_LEFT, 0, texte[11][Platform_Data->Language]);
				sprintf(buf,"%4s",month_names[dtim.Month-1]);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			}break;
			case 9:
			{
				dp10=dp20=dp30=dp40=0;
        LED_Print(LED_DISPLAY_LEFT, 0, texte[12][Platform_Data->Language]);
				sprintf(buf,"20%2d",dtim.Year);
				dp1=dp2=dp3=dp4=0;
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			}break;
			case 10:
			{
				dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_LEFT, 0, texte[19][Platform_Data->Language]);
				dp1=dp3=1;
				sprintf(buf,"%s", DEVICE_VERSION);
				LED_Print(LED_DISPLAY_RIGHT, 0, buf);
			}break;	
			case 11:
			{
				dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_LEFT, 0, texte[21][Platform_Data->Language]);
				sprintf(buf,"%4d",Platform_Data->Exisccator_size);
				dp1=dp2=dp3=dp4=dp10=dp20=dp30=dp40=0;
				LED_Print(LED_DISPLAY_RIGHT, 0, buf);
			}break;			
			case 12:      
      {
      	dp10=dp20=dp30=dp40=0;
        dp1=dp2=dp3=dp4=0;
        LED_Print(LED_DISPLAY_LEFT, 0, texte[23][Platform_Data->Language]);
        LED_Print(LED_DISPLAY_RIGHT, 0, texte[24][Platform_Data->Language]);
      
        if(gettasto()!=KBD_CODE_START) break;
        ENGINEERING_DevSNum();

        LED_Print(LED_DISPLAY_LEFT, 0, texte[23][Platform_Data->Language]);
        LED_Print(LED_DISPLAY_RIGHT, 0, texte[24][Platform_Data->Language]);
      }break;
    }
    
    while((ch=gettast())==0);//有按键按下跳出循环
		BEEPER_PlayTones(tones_1khz);	
		
		if(ch==KBD_CODE_STOP) 
		{
			//Einstelldaten ins eeprom schreiben
			if((einstell_menu_index > 4)&&(einstell_menu_index < 10)&&(time_change_flag == 1))//从时间段中直接返回上级菜单，根据标志位判断是否修改内部时钟
			{
				AKC_RTCSetDate(dtim);									// write Date & Time (STM RTC Counter)	
				time_change_flag = 0;
			}
			
			writeeeprom(Platform_Data->Language, ADR_SPRACHE);
			writinteep(tMode,ADRTMODE);
			writinteep(Penetration,ADRPENETRATION);
			writinteep(Platform_Data->Exisccator_size, ADREXSICCATOR_SIZE);// Beh鋖ter Gr鲞e speichern
     	writinteep(Platform_Data->Penetration_inverse, ADRPENETRATION_INVERSE);//Invertierung der Penetration Phase speichern
			return;
		}
		
		if(ch==KBD_CODE_MENU_UP) 
		{
			if(einstell_menu_index<12) 
				einstell_menu_index++;
			
			if(einstell_menu_index == 5)//进入时间段，需要读取内部时钟
			{
				AKC_RTCGetDate(&dtim);
			}
			
			if((einstell_menu_index == 10)&&(time_change_flag == 1))//离开时间段，需要根据标志位来判断是否修改内部时钟
			{
				AKC_RTCSetDate(dtim);
				time_change_flag = 0;
			}
		}
		
		if(ch==KBD_CODE_MENU_DOWN) 
		{
			if(einstell_menu_index>0) 
				einstell_menu_index--;
			
			if(einstell_menu_index == 9)//进入时间段，需要读取内部时钟
			{
				AKC_RTCGetDate(&dtim);
			}
			
			if((einstell_menu_index == 4)&&(time_change_flag == 1))//离开时间段，需要根据标志位来判断是否修改内部时钟
			{
				AKC_RTCSetDate(dtim);
				time_change_flag = 0;
			}			
		}
		
		if(ch==KBD_CODE_VALUE_UP) 				// Taste Hoch
		{
			switch(einstell_menu_index) 
			{
				case 0:
				{
					if(tMode==0) 
						tMode=1;
					else 
						tMode=0;
				}break;
				case 1:
				{
					if(Penetration<10) 
					{
						Penetration++;
						sprintf(buf,"%4d",Penetration);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
					}
				}break;
				case 2:
				{
					Platform_Data->Penetration_inverse=1;
				}break;
				case 3:
				{
					if(Platform_Data->Language==LANGUAGE_MODE_GER) 
						Platform_Data->Language=LANGUAGE_MODE_ENG;
					else 
						Platform_Data->Language=LANGUAGE_MODE_GER;       
				}break;
				case 4:
				{
					showID();
				}break;
				case 5:
				{
					if(dtim.Hour<24)
						dtim.Hour++;
					
					sprintf(buf,"%4d",dtim.Hour);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 6:
				{
					if(dtim.Minute<59)
						dtim.Minute++;
					
					sprintf(buf,"%4d",dtim.Minute);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 7:
				{
					if(dtim.Day<31)
						dtim.Day++;
					
					sprintf(buf,"%4d",dtim.Day);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 8:
				{
					if(dtim.Month<12)
						dtim.Month++;
					
					sprintf(buf,"%4d",dtim.Month);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 9:
				{
					if(dtim.Year<50) 											 // 2050
						dtim.Year++;
					
					sprintf(buf,"20%2d",dtim.Year);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 11:
				{
					if(Platform_Data->Exisccator_size<300) // 300mm max
						Platform_Data->Exisccator_size+=50;
					
					sprintf(buf,"%4d",Platform_Data->Exisccator_size);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
					
					if(Platform_Data->Exisccator_size > 250) 
						MaxVac = MAXVACUM_EXISCCATOR;
					
					if(Vacuum > MaxVac) 
						Vacuum = MAXVACUM_EXISCCATOR;
				}break;
				case 12:
        break;
			}		
			
			if((einstell_menu_index > 4)&&(einstell_menu_index < 10))//若时间段有修改，则将标志位置位
			{
				time_change_flag = 1;
			}
		}
											
		if(ch==KBD_CODE_VALUE_DOWN) 				// Taste runter
		{
			switch(einstell_menu_index) 
			{
				case 0:
				{
					if(tMode==0) 
						tMode=1;
					else 
						tMode=0;
				}break;
				case 1:
				{
					if(Penetration>0) 
					{
						Penetration--;
						sprintf(buf,"%4d",Penetration);
						LED_Print(LED_DISPLAY_RIGHT, 0,buf);
					}
				}break;
				case 2:
				{
					Platform_Data->Penetration_inverse=0;
				}break;
				case 3:
				{
					if(Platform_Data->Language==LANGUAGE_MODE_GER) 
						Platform_Data->Language=LANGUAGE_MODE_ENG;
					else 
						Platform_Data->Language=LANGUAGE_MODE_GER;
				}break;
				case 4:
				{
					showID();
				}break;
				case 5:
				{
					if(dtim.Hour>0)
						dtim.Hour--;
					
					sprintf(buf,"%4d",dtim.Hour);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 6:
				{
					if(dtim.Minute>0)
						dtim.Minute--;
					
					sprintf(buf,"%4d",dtim.Minute);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;		  
				case 7:
				{
					if(dtim.Day>1)
						dtim.Day--;
					
					sprintf(buf,"%4d",dtim.Day);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 8:
				{
					if(dtim.Month>1)
						dtim.Month--;
					
					sprintf(buf,"%4d",dtim.Month);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 9:
				{
					if(dtim.Year>12)
						dtim.Year--;
					
					sprintf(buf,"20%2d",dtim.Year);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				}break;
				case 11:
				{
					if(Platform_Data->Exisccator_size>150) // 150mm (minimum)
						Platform_Data->Exisccator_size-=50;
					
					sprintf(buf,"%4d",Platform_Data->Exisccator_size);
					LED_Print(LED_DISPLAY_RIGHT, 0,buf);
					if(Platform_Data->Exisccator_size <= 250) 
						MaxVac = MAXVACUM;
				}break;		
				case 12:
        break;          
			}
			
			if((einstell_menu_index > 4)&&(einstell_menu_index < 10))//若时间段有修改，则将标志位置位
			{
				time_change_flag = 1;
			}
		}										
	}
}

/*******************************************************************************
* Function Name  : kalibriere
* Description    : menu for kalibrating the pressure sensor
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void kalibriere(void){
	int soll,soll1,ist,ist1,i;
	unsigned int ch,press;
	i=0;
	
	while((ch=gettasto())!=KBD_CODE_NONE);
	while((ch=gettasto())==KBD_CODE_NONE && i++<10) {
		osDelay(500);
	}
	if(i>=10) return;
	if(ch!=KBD_CODE_MENU_DOWN_KBD_CODE_VALUE_UP) return;
		BEEPER_PlayTones(tones_1khz);
	
	IO_SetVentil(Bit_SET);//bel黤ten schliessen
#ifdef DCPUMP
//	IO_SetDcPump(Bit_SET);// Pumpe von system trennen (bypass)
#else
	IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
	IO_SetAcPump(Bit_SET);//pumpe einschalten
	dp1=dp2=dp3=dp4=0;

	while(gettasto()!=KBD_CODE_NONE);
	while((ch=gettasto())!=KBD_CODE_START) {
		if(Platform_Data->Language==LANGUAGE_MODE_ENG) {
			LED_Print(LED_DISPLAY_LEFT, 0, "DRUC");
		}
		else {
			LED_Print(LED_DISPLAY_LEFT, 0, "PRES");
		}
		sprintf(buf,"%04d",(press = GetVacAvrg()));
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		if(press>201) {
#ifdef DCPUMP
//			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
			IO_SetAcPump(Bit_RESET);
		}
		if(press<199 && !GPIO_ReadInputDataBit(PORT_A, ACPUMP)) {
			IO_SetDcPump(Bit_SET);// Pumpe von system trennen (bypass)
//#ifndef DCPUMP
			osDelay(400);
//#endif
			IO_SetAcPump(Bit_SET);//pumpe einschalten
//#ifndef DCPUMP
			osDelay(400);
			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
//#endif
		}
		osDelay(300);
	}
	soll=press;
	dp4=1;
	LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		BEEPER_PlayTones(tones_1khz);	
	while(gettasto()!=KBD_CODE_NONE);
	while((ch=gettasto())!=KBD_CODE_START) {	//erste kalibrierung best鋞igen
		if(ch==KBD_CODE_VALUE_UP) {
			soll++;
			sprintf(buf,"%04d",soll);
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			osDelay(100);
		}
		if(ch==KBD_CODE_VALUE_DOWN) {
			soll--;
			sprintf(buf,"%04d",soll);
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			osDelay(100);
		}
	}
	ist = getadavrg();
	dp4=0;
	LED_Print(LED_DISPLAY_RIGHT, 0,buf);
	while(gettasto()!=KBD_CODE_NONE);
	while((ch=gettasto())!=KBD_CODE_START) {
		if(Platform_Data->Language==LANGUAGE_MODE_ENG) {
			LED_Print(LED_DISPLAY_LEFT, 0, "DRUC");
		}
		else {
			LED_Print(LED_DISPLAY_LEFT, 0, "PRES");
		}
		sprintf(buf,"%04d",(press = GetVacAvrg()));
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		if(press>701) {
#ifdef DCPUMP
//		IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
		IO_SetAcPump(Bit_RESET);
		}
		if(press<699 && !GPIO_ReadInputDataBit(PORT_A, ACPUMP)) {
			IO_SetDcPump(Bit_SET);// Pumpe von system trennen (bypass)
//#ifndef DCPUMP
			osDelay(400);
//#endif
			IO_SetAcPump(Bit_SET);//pumpe einschalten
//#ifndef DCPUMP
			osDelay(400);
			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
//#endif
		}
		osDelay(300);
	}
	soll1=press;
	dp4=1;
	LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		BEEPER_PlayTones(tones_1khz);	
	while(gettasto()!=KBD_CODE_NONE);
	while((ch=gettasto())!=KBD_CODE_START) {
		if(ch==KBD_CODE_VALUE_UP) {
			soll1++;
			sprintf(buf,"%04d",soll1);
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			osDelay(100);
		}
		if(ch==KBD_CODE_VALUE_DOWN) {
			soll1--;
			sprintf(buf,"%04d",soll1);
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			osDelay(100);
		}
	}
	ist1 = getadavrg();
	while(gettasto()!=KBD_CODE_NONE);
	IO_SetVentil(Bit_RESET); // bel黤ten ein
	IO_SetDcPump(Bit_RESET);// Pumpe ist im system
	IO_SetAcPump(Bit_RESET);

	VacM=soll1-soll;
	VacM /= (float)(ist1-ist);
	VacB=(float)soll - (float)((float)ist*VacM);
	if(VacM<0.25 || VacM>0.55) 
		VacM=0.4;
	if(VacB<-500.0 || VacB>0.0) 
		VacB=-230.0;
	savefloat(&VacM,ADRVACM);
	savefloat(&VacB,ADRVACB);
}

/*******************************************************************************
* Function Name  : Kontrolle
* Description    : Dauer einschalten und Druckanzeige
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void kontrolle(void){
#ifdef DCPUMP
//	IO_SetDcPump(Bit_SET);// Pumpe von system trennen (bypass)
#else
	IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
	IO_SetAcPump(Bit_SET);//pumpe einschalten
	IO_SetVentil(Bit_SET);//bel黤ten schliessen

	while(gettasto()!=KBD_CODE_NONE);
	do {
		if(Platform_Data->Language==0) {
			LED_Print(LED_DISPLAY_LEFT, 0, "DRUC");
		}
		else {
			LED_Print(LED_DISPLAY_LEFT, 0, "PRES");
		}
		sprintf(buf,"%04d",GetVacAvrg());
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		osDelay(100);
	}while(gettasto()==KBD_CODE_NONE);
	IO_SetDcPump(Bit_RESET);// Pumpe ist im system
	IO_SetAcPump(Bit_RESET);// pumpe ausschalten
	IO_SetVentil(Bit_RESET); // bel黤ten ein
	while(gettasto()!=KBD_CODE_NONE);
}
/*******************************************************************************
* Function Name  : showID
* Description    : Display the MC Chip ID on the LED Display
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void showID(void){
	unsigned int i;

	getid(id);
	dp1=dp2=dp3=dp4=dp10=dp20=dp30=dp40=0;
	while(gettasto()!=KBD_CODE_NONE)
		osThreadYield();
	while(gettasto()==KBD_CODE_NONE) {
		buf[0]='I';
		buf[1]='D';
		buf[2]=id[0];
		buf[3]=id[1];
		LED_Print(LED_DISPLAY_LEFT, 0, buf);
		buf[0]=id[2];
		buf[1]=id[3];
		buf[2]=id[4];
		buf[3]=id[5];
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		i=0;
		osDelay(150);
		while(gettasto()==KBD_CODE_NONE && i++<300)
		osThreadYield();			
		BEEPER_PlayTones(tones_1khz);	
		buf[0]=id[6];
		buf[1]=id[7];
		buf[2]=id[8];
		buf[3]=id[9];
		LED_Print(LED_DISPLAY_LEFT, 0, buf);
		buf[0]=id[10];
		buf[1]=id[11];
		buf[2]=id[12];
		buf[3]=id[13];
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		i=0;
		osDelay(150);

		while(gettasto()==KBD_CODE_NONE && i++<300)
		osThreadYield();			
		BEEPER_PlayTones(tones_1khz);	
		buf[0]=id[14];
		buf[1]=id[15];
		buf[2]=id[16];
		buf[3]=id[17];
		LED_Print(LED_DISPLAY_LEFT, 0, buf);
		buf[0]=id[18];
		buf[1]=id[19];
		buf[2]=id[20];
		buf[3]=id[21];
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		i=0;
		osDelay(150);
    
		while(gettasto()==KBD_CODE_NONE && i++<300)
		osThreadYield();			
		BEEPER_PlayTones(tones_1khz);	
		buf[0]=id[22];
		buf[1]=id[23];
		buf[2]=' ';
		buf[3]=' ';
		LED_Print(LED_DISPLAY_LEFT, 0, buf);
		buf[0]=' ';
		buf[1]=' ';
		buf[2]=' ';
		buf[3]=' ';
		LED_Print(LED_DISPLAY_RIGHT, 0,buf);
		i=0;
		osDelay(150);

		while(gettasto()==KBD_CODE_NONE && i++<300)
		osThreadYield();			
		BEEPER_PlayTones(tones_1khz);	
		osThreadYield();
	}
	while(gettasto()!=KBD_CODE_NONE)
		osThreadYield();
}
/*******************************************************************************
* Function Name  : messung
* Description    : menu Messung, stellt den zuvor eingestellten Unterdruck f黵 eine eingestellte Zeit ein
* Input          : None
* Output         : None
* Return         : None
* anzverzoegerung = die update geschwindigkeit der Anzeige w鋜end der Messung und w鋜end dem ersten Druckaufbau
*******************************************************************************/
void messung(void)
{
	unsigned int press,intverg,index=0,anzverg=0,anzverzoegerung=5;
  	unsigned int cnt_pene_end=0;                                        //counter for pene end pieper
  	bool stopSaving=false;	// 只有测得10个数据点才保存
	FirstReached=false;
	PresReduced=0;	// 压力是否达到设定值,加压超时
	tmpVacuum=Vacuum;
	MessFlg=true;
	
	for(press=0;press<10;press++) {
		mepresure[press]=0;
		metimh[press]=0;
		metiml[press]=0;
	}		
	Abbruch=false;	// 测试是否被用户中止
	ablZeitH=messZeitH;		// 分钟
	ablZeitL=messZeitL;		// 秒
  
//entfernt eventuell startet pumpe hier auch nicht bei 60Hz  
//15.11.2016	IO_SetVentil(Bit_SET);//bel黤ten schliessen
#ifdef DCPUMP
//	IO_SetDcPump(Bit_SET);// Pumpe von system trennen (bypass)
#else
  pumpe_entl_start();
//15.11.2016	IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
//15.11.2016	IO_SetAcPump(Bit_SET);//pumpe einschalten
	dp1=dp2=dp3=dp4=0;
	dp10=dp20=dp30=dp40=0;
	dp20=1;
	if(tMode==1) dp40=1;
	while(gettasto()!=KBD_CODE_NONE);
	CountFree=0;
	intverg=(ablZeitH * 60) + ablZeitL;
	intverg/=10;
	if(intverg>2)
		intervall=intverg-2;
	else
	{
		if(ablZeitH==0 && ablZeitL ==10)
			intervall=0;//intverg;
		else
			intervall=intverg;
	}
	
	do {
		sprintf(buf,"%02d%02d",ablZeitH,ablZeitL);
		LED_Print(LED_DISPLAY_LEFT, 0, buf);
		press=GetVacAvrg();
		sprintf(buf,"%04d", press);

		if(anzverg>0) {
			anzverg--;
		}	else {	
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			anzverg = anzverzoegerung; 											// Wert f黵 Anzeigeverz鰃erung 黚ernehmen
		}

		if(PresReduced != 0)
		{
			Abbruch=CHANCEL_PRESS_TIMEOUT;
			IO_SetDcPump(Bit_RESET); 	// PA1-PUMP
			IO_SetAcPump(Bit_RESET);	// PA0-RELAY
			IO_SetVentil(Bit_RESET); 	// PC5-VENTIL
			BEEPER_PlayTones(tones_error);

			if(PresReduced == 1)
			{
				dp1=1;
				sprintf(buf,"TOUT");
			}
			else
			{
				if(Platform_Data->Language == 0)
					sprintf(buf,"ABBR");
				else
					sprintf(buf,"FAIL");
			}
			LED_Print(LED_DISPLAY_RIGHT, 1,buf);
			osDelay(3000);
			while(gettasto()!=KBD_CODE_START)
			{
				osDelay(10);
			}
			BEEPER_PlayTones(tones_1khz);//按键音
			goto end;
		}
		else 
		{
			dp1=dp2=dp3=dp4=0;
		}

		aktPress=press;
    
		if(press>tmpVacuum+3) 
		{														// Druck erreicht, Pumpe abschalten
			FirstReached=true;

#ifdef DCPUMP
//			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
#endif
			IO_SetAcPump(Bit_RESET);
		}
		if(press<tmpVacuum-3 && !GPIO_ReadInputDataBit(PORT_A, ACPUMP)) 
		{ // Druck zu niedrig, Pumpe einschalten
			anzverzoegerung=10;
			pumpe_entl_start(); //Pumpe entl黤tet starten
			anzverg = anzverzoegerung;	// 2025-11-6 11:11 HHT

    		if(press<tmpVacuum+3 && !GPIO_ReadInputDataBit(PORT_A, ACPUMP)) 
			{ // Druck zu hoch, Pumpe ausschalten
        
  #ifdef DCPUMP
  //			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
  #else
        IO_SetAcPump(Bit_RESET);	
  #endif
      		}
		}
		if(press<tmpVacuum-5) 
		{						// wenn untere Tolleranz erreicht Zeit stoppen
			CountFree=0;										// Zeit anhalten
		}	
		else 
		{
			if(FirstReached) CountFree=1;										// Zeit weiter z鋒len wenn der Druck einmal erreicht wurde und in tolleranz ist
		}

		if(intervall==0 && FirstReached && CountFree && (!stopSaving) ) 
		{	// wenn Speicher intervall Z鋒ler 0, wenn Druck einmal erreicht, wenn Zeit nicht l鋟ft
			intervall=intverg;
			mepresure[index]=press;												 	// messwert speichern
			metimh[index]=ablZeitH;
			metiml[index]=ablZeitL;
			if(index<9)
			{
				index++;
			} else
			{
				stopSaving = true;
			}
		}
    
		osDelay(50);
	}while(gettasto()!=KBD_CODE_STOP && (ablZeitH>0 || ablZeitL>0 || (!stopSaving))); //wait untill the last measurement is done
 
	MessFlg=false;
	if(ablZeitH>0 || ablZeitL>0) 
	{ 			// pr黤en ob abbruch durch user
		Abbruch=CHANCEL_USER;
		BEEPER_PlayTones(tones_error);
	}
	else 
	{
		if(Penetration>0) 
		{
			IO_SetDcPump(Bit_RESET);// Pumpe ist im system
			IO_SetAcPump(Bit_RESET);
			if(!Platform_Data->Penetration_inverse)
			{// Penetration mit Vacuum ja / nein
				IO_SetVentil(Bit_RESET); // bel黤ten ein
			}
			dp1=dp2=dp3=dp4=0;
			dp10=dp20=dp30=dp40=0;
			dp2=1;
			CountFree=1;										// Zeit starten
			ablZeitH=Penetration;
			ablZeitL=0;
			LED_Print(LED_DISPLAY_LEFT, 0, texte[20][Platform_Data->Language]);
			do {
				sprintf(buf,"%02d%02d",ablZeitH,ablZeitL);
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
				osDelay(20);
			}while(gettasto()!=KBD_CODE_STOP && (ablZeitH>0 || ablZeitL>0));
			CountFree=0;										// Zeit anhalten
			
			if(ablZeitH>0 || ablZeitL>0) 
			{	// pr黤en ob abbruch durch user
				Abbruch=CHANCEL_USER;
				BEEPER_PlayTones(tones_error);
				goto end;
			}	
			if(ablZeitH==0 && ablZeitL==0) 
			{ // Penetration (Nachlaufzeit in Minuten) abgelaufen
				sprintf(buf,"%02d%02d",ablZeitH,ablZeitL);
				LED_Print(LED_DISPLAY_RIGHT, 0,buf);
       			cnt_pene_end = 3;               //3 times beep
				do {
					BEEPER_PlayTones(tones_1khz);	
					osDelay(500);
          			cnt_pene_end--;
				}while(cnt_pene_end>0 && gettasto()==KBD_CODE_NONE);
			}
		}
	}
end:	
	CountFree=0;						 // bug in <V2.02, Zeit anhalten
	IO_SetDcPump(Bit_RESET); // Pumpe ist im system
	IO_SetAcPump(Bit_RESET);
	IO_SetVentil(Bit_RESET); // bel黤ten ein

	dp1=dp2=dp3=dp4=0;
	dp10=dp20=dp30=dp40=0;

	// 显示OK
	if(Abbruch == 0 && PresReduced == 0)
	{
		LED_Print(LED_DISPLAY_LEFT, 0, "    "); 
		LED_Print(LED_DISPLAY_RIGHT, 0,"OK");
		while(gettasto()!=KBD_CODE_START);
		BEEPER_PlayTones(tones_1khz);//按键音
	}

	if(AutoPrint) 
	{
		if(Platform_Data->Language==0) 
		{
			LED_Print(LED_DISPLAY_LEFT, 0, "DRUC");
			LED_Print(LED_DISPLAY_RIGHT, 0,"KEN");
		}
		else 
		{
			LED_Print(LED_DISPLAY_LEFT, 0, "PRIN");
			LED_Print(LED_DISPLAY_RIGHT, 0,"TING");			
		}
		druckergebnis();
	}
	while(gettasto()!=KBD_CODE_NONE);
}
/*******************************************************************************
* Function Name  : pumpe_entl_start
* Description    : Pumpe entl黤tet starten
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void pumpe_entl_start(void){
	IO_SetDcPump(Bit_SET); // Pumpe von system trennen (bypass), Zusatzventil zur Pumpenentlastung auf DC Pumpen Motorausgang
	IO_SetVentil(Bit_RESET); // bel黤ten ein
	osDelay(700);
	IO_SetAcPump(Bit_SET);	//pumpe einschalten
	IO_SetVentil(Bit_SET);  //bel黤ten schliessen
	osDelay(700);
	IO_SetDcPump(Bit_RESET);// Pumpe ist im system, Zusatzventil zur Pumpenentlastung auf DC Pumpen Motorausgang
}
	
/*******************************************************************************
* Function Name  : getid
* Description    : get the chip ID from STM32f1
* Input          : char *buf
* Output         : char *buf
* Return         : None
*******************************************************************************/
void getid(char *buf){
    sprintf(buf, "%08X%08X%08X",
    STM32_UUID[0],      //4bytes
    STM32_UUID[1],      //4bytes
    STM32_UUID[2]);	    //4bytes
}

//				osDelay(200);																									 
//			if(Platform_Data->Exisccator_size == 150){				// Gr鲞e von Exiskkator
//				if		 (tmpVacuum>=700) osDelay(800);						// Pumpe je nach ben鰐igter Menge einschalten
//				else if(tmpVacuum>=600) osDelay(600);
//				else if(tmpVacuum>=500) osDelay(400);
//				else if(tmpVacuum>=400) osDelay(300);
//				else if(tmpVacuum>=300) osDelay(200);
//				else if(tmpVacuum>=200) osDelay(100);
//			}else if(Platform_Data->Exisccator_size == 200){
//				if		 (tmpVacuum>=850) {
//					osDelay(2200);}
//				else if(tmpVacuum>=700) osDelay(800);
//				else if(tmpVacuum>=600) osDelay(600);
//				else if(tmpVacuum>=500) osDelay(450);
//				else if(tmpVacuum>=400) osDelay(350);
//				else if(tmpVacuum>=300) osDelay(250);
//				else if(tmpVacuum>=200) osDelay(150);
//			}else if(Platform_Data->Exisccator_size == 250){
//				if		 (tmpVacuum>=700) osDelay(800);
//				else if(tmpVacuum>=600) osDelay(600);
//				else if(tmpVacuum>=500) osDelay(500);
//				else if(tmpVacuum>=400) osDelay(400);
//				else if(tmpVacuum>=300) osDelay(300);
//				else if(tmpVacuum>=200) osDelay(200);
//			}else if(Platform_Data->Exisccator_size == 300){
//				if		 (tmpVacuum>=700) osDelay(800);
//				else if(tmpVacuum>=600) osDelay(600);
//				else if(tmpVacuum>=500) osDelay(550);
//				else if(tmpVacuum>=400) osDelay(450);
//				else if(tmpVacuum>=300) osDelay(350);
//				else if(tmpVacuum>=200) osDelay(250);
//			}else{
//				if		 (tmpVacuum>=700) osDelay(800);								// Pumpe je nach ben鰐igter Menge einschalten
//				else if(tmpVacuum>=600) osDelay(600);
//				else if(tmpVacuum>=500) osDelay(400);
//				else if(tmpVacuum>=400) osDelay(300);
//				else if(tmpVacuum>=300) osDelay(200);
//				else if(tmpVacuum>=200) osDelay(100);
//			}			
//	
/*******************************************************************************
* Function Name  : ENGINEERING_DevSNum
* Description    : set device serial number
* Input          : void
* Output         : 
* Return         : bool
*******************************************************************************/
bool ENGINEERING_DevSNum(void)
{
  char title[]="N-  ";
  char tStr[SERIAL_NUMBER_LEN];
  uint8_t i;

  for (i=0;i<SERIAL_NUMBER_LEN;i++){
    tStr[i]=readeeprom(ADRSERNUM+i); 
  }
        
  if (EDIT_Advanced(title, tStr, SERIAL_NUMBER_LEN, false))//return is true weil nur Stop Taste genutzt wird
  {
//    for (i=0;i<SERIAL_NUMBER_LEN;i++)
//      writeeeprom(tStr[i],ADRSERNUM+i); 
      BEEPER_PlayTones(tones_1khz);
//  } else
//  {  
//    BEEPER_PlayTones(tones_error);
  }  
  return true;
}
/*******************************************************************************
* Function Name  : EDIT_Advanced
* Description    : advanced edit (serial number and password)
* Input          : char *title, char *tStr, uint8_t len, bool sound
* Output         :
* Return         : uint8_t
*******************************************************************************/
uint8_t EDIT_Advanced(char *title, char *tStr, uint8_t len, bool sound)
{
  uint8_t key;
  uint8_t initPos, curPos, stPos;
  
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  stPos = LED_NUM_DIGITS*2 - len;
  ledBlinkPos = stPos;
  curPos=0;
  osDelay(200);  
  for (;;){
    //print title
    memcpy(&title[2], tStr, initPos);
    LED_Print(LED_DISPLAY_LEFT,false,title);
    //print value
    LED_Print(LED_DISPLAY_RIGHT,false,&tStr[initPos]);

    key=gettasto();
		osDelay(120);
    switch(key)
    {
      case KBD_CODE_STOP:           
        if (sound) BEEPER_PlayTones(tones_1khz);
        ledBlinkPosYes=false;
        return true;
//      case KBD_CODE_VALUE_DOWN:
//        tStr[curPos]--;
//        break;
//      case KBD_CODE_VALUE_UP:
//        tStr[curPos]++;
//        break;
//      case KBD_CODE_MENU_DOWN:
//        if (!curPos) curPos=len-1;
//          else curPos--;
//        ledBlinkPos=curPos+stPos;
//        break;
//      case KBD_CODE_MENU_UP:
//        curPos++;
//        if (curPos==len) curPos=0;
//          ledBlinkPos=curPos+stPos;
//        break;      
      default:
        break;
    }       
    if (tStr[curPos]>0x39)
      tStr[curPos]=0x30;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x39;
  }
}

