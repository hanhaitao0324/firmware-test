/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    LicManagement.c
 *      Purpose: Lic
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "system.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eeprom.h"
#include "led.h"
#include "beeper.h"
#include "timedate.h"
#include "kbd.h"
#include "rtc.h"

char *texttage[4]={"TAGE","DAYS","DAYS","DAYS"};
unsigned int DaysLeft,FullDays,Periods,AktPeriod;
float dateoffset;
int LastDay,NoLicense;

extern unsigned char dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
extern char buf[1500],id[14];

//void AKC_RTCSetOrgDate(DTIM dtim);
//void AKC_RTCGetOrgDate(DTIM *dtim);
long FCode(char *ID,int art);
//double JulDat(DTIM dtim);
//DTIM GregDat(double JulDat);

extern void RTC_SetTime(RTC_TIME *Time);
extern void RTC_GetTime(RTC_TIME *Time);
extern void RTC_SetDate(RTC_DATE *Date);
extern void RTC_GetDate(RTC_DATE *Date);

void StartLicManagement(void){
	DTIM dtim;
	DTIM orgtim;

	LoadDate(&dtim, ADRLASTDATE);
	//AKC_RTCGetOrgDate(&orgtim);
  RTC_GetDateTime(&orgtim);
	if (orgtim.Year<dtim.Year || orgtim.Month>12 || orgtim.Day>31) 
  {
		orgtim.Year=dtim.Year;
		orgtim.Month=dtim.Month;
		orgtim.Day=dtim.Day;
		orgtim.Hour=dtim.Hour;
		orgtim.Minute=dtim.Minute;
		orgtim.Second=dtim.Second;
		//AKC_RTCSetOrgDate(orgtim);
    RTC_SetDateTime(&orgtim);
	}		
	DaysLeft=readinteep(ADRDAYSLEFT);

	FullDays=readinteep(ADRFULLDAYS);
	if(FullDays>1000) 
    FullDays=380;
	Periods=readinteep(ADRPERIODS);
	if(Periods>10) 
    Periods=3;
	NoLicense=readinteep(ADRNOLICENSE);
	if(NoLicense>1) 
    NoLicense=0;
}

void TestLicense(void){
	unsigned int cod=0,ch,intind=0;
	unsigned int stelle[6];

	if(DaysLeft>FullDays) {
		DaysLeft=0;
		writinteep(DaysLeft,ADRDAYSLEFT);
	}
	if(DaysLeft==0 && !NoLicense) {
		for(ch=0;ch<6;ch++)
			stelle[ch]=0;
		dp10=dp20=dp30=dp40=dp1=dp2=dp3=dp4=0;
		dp30=1;	
		while(DaysLeft==0) {
			buf[0]=(AktPeriod + 1) | 0x30;
			buf[1]='L';
			buf[2]=stelle[0] | 0x30;
			buf[3]=stelle[1] | 0x30;
			LED_Print(LED_DISPLAY_LEFT, 0,buf);

			buf[0]=stelle[2] | 0x30;
			buf[1]=stelle[3] | 0x30;
			buf[2]=stelle[4] | 0x30;
			buf[3]=stelle[5] | 0x30;
			LED_Print(LED_DISPLAY_RIGHT, 0,buf);
			ch=gettast();
			if(ch==KBD_CODE_MENU_UP || ch==KBD_CODE_MENU_DOWN) showID();
			if(ch==KBD_CODE_START) {
				if(intind++>4) intind=0;
				dp10=dp20=dp30=dp40=dp1=dp2=dp3=dp4=0;
				switch(intind) {
					case 0:
						dp30=1;
						break;
					case 1:
						dp40=1;
						break;
					case 2:
						dp1=1;
						break;
					case 3:
						dp2=1;
						break;
					case 4:
						dp3=1;
						break;
					case 5:
						dp4=1;
						break;
				}
			}
			if(ch==KBD_CODE_STOP) {
				cod=stelle[0]*100000 + stelle[1]*10000 + stelle[2]*1000 + stelle[3]*100 + stelle[4]*10 + stelle[5];
				if(cod==FCode(id,1)) {
					if(AktPeriod<Periods) {
						DaysLeft=FullDays;
						writinteep(DaysLeft,ADRDAYSLEFT);
						AktPeriod++;
						writinteep(AktPeriod,ADRAKTPERIOD);
					}
				}
				if(cod==FCode(id,3)) {
					NoLicense=1;
					writinteep(NoLicense,ADRNOLICENSE);
					DaysLeft=FullDays;
					writinteep(DaysLeft,ADRDAYSLEFT);
				}
			}		 
			if(ch==KBD_CODE_VALUE_UP) {
				stelle[intind]++;
				if(stelle[intind]>9) stelle[intind]=0;
			}
			if(ch==KBD_CODE_VALUE_DOWN) {
				stelle[intind]--;
				if(stelle[intind]>9) stelle[intind]=9;
			}
		}
	}
}

void InitLicManagement(void){
	DTIM dtim;

	RTC_Set_DefaultTimeDate(&dtim);
	
	dateoffset=0.0;
	savefloat(&dateoffset,ADRDATEOFFSET);
	DaysLeft=0;
	writinteep(DaysLeft,ADRDAYSLEFT);
	LastDay=dtim.Day;
	writinteep(LastDay,ADRLASTDAY);
	WriteDate(&dtim,ADRLASTDATE);
	AktPeriod=0;
	writinteep(AktPeriod,ADRAKTPERIOD);
	FullDays=380;
	writinteep(FullDays,ADRFULLDAYS);
	Periods=3;
	writinteep(Periods,ADRPERIODS);
	NoLicense=0;
	writinteep(NoLicense,ADRNOLICENSE);
}

unsigned int GetDaysLeft(void){
	return(DaysLeft);
}


void ShowDaysLeft(void){
	unsigned int i;

	if(NoLicense) return;
	LED_Print(LED_DISPLAY_LEFT, 0, texttage[Platform_Data->Language]);
	sprintf(buf,"%03d",DaysLeft);
	LED_Print(LED_DISPLAY_RIGHT, 0, buf);
	if(DaysLeft>21) {
		osDelay(2000);
	}
	else {
		for(i=0;i<5;i++) {
			BEEPER_PlayTones(tones_1khz);	
			osDelay(380);
		}
}
}
 
/*void AKC_RTCSetOrgDate(DTIM dtim){
	RTC_DATE orgdat;
	RTC_TIME orgtim;
	
	orgdat.year			= dtim.Year;
	orgdat.month		= dtim.Month;
	orgdat.day			= dtim.Day;
	orgtim.hours		= dtim.Hour;
	orgtim.minutes	= dtim.Minute;
	orgtim.seconds	= dtim.Second;

	RTC_SetDate(&orgdat);
	RTC_SetTime(&orgtim);
}*/

/*void AKC_RTCGetOrgDate(DTIM *dtim){
	RTC_DATE orgdat;
	RTC_TIME orgtim;

	RTC_GetDate(&orgdat);
	RTC_GetTime(&orgtim);

	dtim->Year	 = orgdat.year;
	dtim->Month	 = orgdat.month;
	dtim->Day		 = orgdat.day;
	dtim->Hour	 = orgtim.hours;
	dtim->Minute = orgtim.minutes;
	dtim->Second = orgtim.seconds;
}*/

void AKC_RTCSetDate(DTIM tim){
	//RTC_TIME time;
	//DTIM orgtim;
	//RTC_DATE orgdat;
//	float addjul;
	
	//AKC_RTCGetOrgDate(&orgtim);
//	addjul=JulDat(tim) - JulDat(orgtim);
//	savefloat(&addjul, ADRDATEOFFSET);

	//orgdat.year  = tim.Year;
	//orgdat.month = tim.Month;
	//orgdat.day 	 = tim.Day;

	//time.hours		= tim.Hour;
	//time.minutes	=	tim.Minute;
	//time.seconds	= tim.Second;
	//time.milliseconds = 0;
	
	//RTC_SetDate(&orgdat);
	//osDelay(500);
	//RTC_SetTime(&time);
  RTC_SetDateTime(&tim);
}

void AKC_RTCGetDate(DTIM *tim)
{
	//DTIM orgtim;
	//RTC_TIME time;
	//float orgjul,addjul;
	
	//AKC_RTCGetOrgDate(&orgtim);
  //AKC_RTCGetOrgDate(tim);
//	loadfloat(&addjul,ADRDATEOFFSET);
	/*orgjul=JulDat(orgtim) + addjul;
	*tim=GregDat(orgjul);
	RTC_GetTime(&time);
	tim->Hour		= time.hours;
	tim->Minute	=	time.minutes;
	tim->Second	= time.seconds;*/
  RTC_GetDateTime(tim);
}

void LicManager_Handler(void)
{
	DTIM dtim;

	while(1) {
		osSignalWait(0x1,0xFFFF);		//todo ?

		AKC_RTCGetDate(&dtim);
		WriteDate(&dtim,ADRLASTDATE);
		if(dtim.Day!=LastDay) {
			DaysLeft--;
			writinteep(DaysLeft,ADRDAYSLEFT);
		  	LastDay=dtim.Day;
			writinteep(LastDay,ADRLASTDAY);
			TestLicense();
		}

	}
}

long FCode(char *ID,int art) {
			int qs,i,k1,k2,k3,k4;
			long ur;
			
				k1=7;		//Verlängern ohne Periodenverlängerung
				k2=0;		//Code + (-Periode^3)*131
				k3=3;
				k4=5;

			if(art==2) {	//Verlängern mit Periodenverlängerung
				k1=3;
				k2=6;
				k3=0;
				k4=1;
			}
			if(art==3) {	//Frei für immer
				k1=1;
				k2=3;
				k3=7;
				k4=4;
			}
			if(art==4) {	//Frei für imerr mit ERWEKA Kennung
				k1=2;
				k2=6;
				k3=7;
				k4=2;
			}

			qs=0;

			for(i=0;i<strlen(ID);i++)
				qs += (ID[i] & 0x0F);
			ur=atol(ID);

			if((ID[strlen(ID)-k1] & 0x0F)!=0 || (ID[strlen(ID)-k2] & 0x0F)!=0) 
				ur /= (qs* ((ID[strlen(ID)-k1] & 0x0F) + (ID[strlen(ID)-k2] & 0x0F))) ;
			else
				ur /= 13;


			if((ID[strlen(ID)-k3] & 0x0F)!=0)
				ur *= (ID[strlen(ID)-k3] & 0x0F);
			else
				ur *= 3;


			if((ID[strlen(ID)-k4] & 0x0F)!=0)
				ur /= (ID[strlen(ID)-k4] & 0x0F);
			else
				ur /= 7;


			while(ur>999999) ur=ur/3;
			i=3;
			while(ur<99999 && i<strlen(ID)) {
				if(ID[strlen(ID)-i] != 0x30) {
					ur*=(ID[strlen(ID)-i] & 0x0F);
				}
				i++;
			}
			if(art==1) {
				ur +=(long)(pow(((double)AktPeriod * -1.0),3.0) * 137.0);
			}
			return(ur);
}



/*double JulDat(DTIM dtim)
{
	double IntMonth,IntYear,IntDay;
	double jul,c;
	


	if(dtim.Month<3){
		IntMonth=dtim.Month + 12;
		IntYear=dtim.Year-1;
	}
	else {
		IntMonth=dtim.Month;
		IntYear=dtim.Year;
	}
	IntDay = dtim.Day;
	IntDay += (double)(dtim.Hour / 24.0);
	IntDay += (double)(dtim.Minute / 1440.0);
	IntDay += (double)(dtim.Second / 86400.0);

	c=2 - floor(IntYear/100) + floor(IntYear/400);

	jul=floor(1461.0 * (IntYear + 4716.0) /4.0) + floor(153.0*(IntMonth+1.0)/5.0) + IntDay + c - 1524.5;
	return(jul);
}*/

/*DTIM GregDat(double JulDat)
{
	double p,n,s1,s2,s3,s4,q,i,e,day;
	DTIM dtim;

	p=floor(JulDat + 0.5);
	s1=p+68569.0;
	n=floor(4.0 * s1/146097.0);
	s2=s1 - floor((146097.0 * n + 3.0)/4.0);
	i=floor(4000.0 * (s2 + 1.0)/1461001.0);
	s3= s2 - floor(1461.0*i/4.0) + 31.0;
	q=floor(80.0 * s3 / 2447.0);
	e=s3-floor(2447.0*q/80);
	s4=floor(q/11);

	dtim.Month=q+ 2 - 12 * s4;
	dtim.Year=100*(n-49) + i + s4;
	day=e + JulDat - p + 0.5;
	dtim.Day=floor(day);
	day-=dtim.Day;
	dtim.Hour=floor(day*24.0);
	day-=(double)(dtim.Hour/24.0);
	dtim.Minute=floor(day*1440.0);
	day-=(double)(dtim.Minute/1440.0);
	dtim.Second=floor(day*86400.0);

	return(dtim);
}*/	
