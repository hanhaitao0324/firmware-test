#include "tbh125.h"

extern unsigned int sp;
extern unsigned char dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
extern char buf[],id[];


char *texttage[4]={"TAGE","DAYS","DAYS","DAYS"};


unsigned int DaysLeft,FullDays,Periods,AktPeriod;
double dateoffset;
int LastDay,NoLicense;
int ManufactCode;

/*void StartLicManagement(void)
{
	DTIM dtim;
	DTIM orgtim;

	LoadDate(&dtim,ADRLASTDATE);
	AKC_RTCGetOrgDate(&orgtim);
	if(orgtim.year<dtim.year || orgtim.month>12 || orgtim.day>31) {
		orgtim.year=dtim.year;
		orgtim.month=dtim.month;
		orgtim.day=dtim.day;
		orgtim.hour=dtim.hour;
		orgtim.min=dtim.min;
		orgtim.sec=dtim.sec;
		AKC_RTCSetOrgDate(orgtim);
	}		
	DaysLeft=readinteep(ADRDAYSLEFT);

	FullDays=readinteep(ADRFULLDAYS);
	if(FullDays>1000) FullDays=380;
	Periods=readinteep(ADRPERIODS);
	if(Periods>10) Periods=3;
	ManufactCode=readinteep(ADRMANUFACT);
	if(ManufactCode>1) ManufactCode=0;
	NoLicense=readinteep(ADRNOLICENSE);
	if(NoLicense>1) NoLicense=0;
}

void TestLicense(void)
{
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
			anzout1(buf);

			buf[0]=stelle[2] | 0x30;
			buf[1]=stelle[3] | 0x30;
			buf[2]=stelle[4] | 0x30;
			buf[3]=stelle[5] | 0x30;
			anzout2(buf);
				   
//			while(gettasto()!=0);
//			while((ch=gettasto())==0);
			ch=gettast();
			if(ch==TLO || ch==TLU) showID();
			if(ch==TMU) {
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
			if(ch==TMO) {
				cod=stelle[0]*100000 + stelle[1]*10000 + stelle[2]*1000 + stelle[3]*100 + stelle[4]*10 + stelle[5];
				if(cod==FCode(id,1)) {
					if(AktPeriod<Periods) {
						DaysLeft=FullDays;
						writinteep(DaysLeft,ADRDAYSLEFT);
						AktPeriod++;
						writinteep(AktPeriod,ADRAKTPERIOD);
						ManufactCode=0;
						writinteep(ManufactCode,ADRMANUFACT);
					}
				}
				if(cod==FCode(id,3)) {
					ManufactCode=1;
					writinteep(ManufactCode,ADRMANUFACT);
					NoLicense=1;
					writinteep(NoLicense,ADRNOLICENSE);
					DaysLeft=FullDays;
					writinteep(DaysLeft,ADRDAYSLEFT);
				}
			}		 
			if(ch==TRO) {
				stelle[intind]++;
				if(stelle[intind]>9) stelle[intind]=0;
			}
			if(ch==TRU) {
				stelle[intind]--;
				if(stelle[intind]>9) stelle[intind]=9;
			}
		}
	}
}*/

/*void InitLicManagement(void)
{
	DTIM dtim;
	double addjul;	

	dtim.year=2009;
	dtim.month=1;
	dtim.day=1;
	dtim.hour=0;
	dtim.min=0;
	dtim.sec=0;

	AKC_RTCSetOrgDate(dtim);

	addjul=0.0;
	savedouble(&addjul,ADRDATEOFFSET);
	DaysLeft=0;
	writinteep(DaysLeft,ADRDAYSLEFT);
	LastDay=dtim.day;
	writinteep(LastDay,ADRLASTDAY);
	WriteDate(&dtim,ADRLASTDATE);
	AktPeriod=0;
	writinteep(AktPeriod,ADRAKTPERIOD);
	FullDays=380;
	writinteep(FullDays,ADRFULLDAYS);
	Periods=3;
	writinteep(Periods,ADRPERIODS);
	ManufactCode=0;
	writinteep(ManufactCode,ADRMANUFACT);
	NoLicense=0;
	writinteep(NoLicense,ADRNOLICENSE);

}*/

/*unsigned int GetDaysLeft(void)
{
	return(DaysLeft);
}


void ShowDaysLeft(void)
{
	unsigned int i;

	if(NoLicense) return;
	anzout1(texttage[sp]);
	sprintf(buf,"%03d",DaysLeft);
	anzout2(buf);
	if(DaysLeft>21) {
		os_dly_wait(2000);
	}
	else {
		for(i=0;i<5;i++) {
			beep();
			os_dly_wait(380);
		}
}
}*/

void AKC_RTCSetOrgDate(DTIM dtim) 
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_DateStructure.RTC_Year=dtim.year-2000;
	RTC_DateStructure.RTC_Month=dtim.month;
	RTC_DateStructure.RTC_Date=dtim.day;
	RTC_TimeStructure.RTC_Hours=dtim.hour;
	RTC_TimeStructure.RTC_Minutes=dtim.min;
	RTC_TimeStructure.RTC_Seconds=dtim.sec;

	RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
}

void AKC_RTCGetOrgDate(DTIM *dtim)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;

	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);

	dtim->year=RTC_DateStructure.RTC_Year+2000;
	dtim->month=RTC_DateStructure.RTC_Month;
	dtim->day=RTC_DateStructure.RTC_Date;
	dtim->hour=RTC_TimeStructure.RTC_Hours;
	dtim->min=RTC_TimeStructure.RTC_Minutes;
	dtim->sec=RTC_TimeStructure.RTC_Seconds;	
}

/*void AKC_RTCSetDate(DTIM tim) 
{
	DTIM orgtim;
	double addjul;
	
	AKC_RTCGetOrgDate(&orgtim);
	addjul=JulDat(tim)-JulDat(orgtim);
	savedouble(&addjul,ADRDATEOFFSET);
}

void AKC_RTCGetDate(DTIM *tim)
{
	DTIM orgtim;
	double orgjul,addjul;
	
	AKC_RTCGetOrgDate(&orgtim);
	loaddouble(&addjul,ADRDATEOFFSET);
	orgjul=JulDat(orgtim) + addjul;
	*tim=GregDat(orgjul);
		
}*/



/*__task void LicManager(void)
{
	DTIM dtim;

	while(1) {
		os_evt_wait_or(0x0007,0xFFFF);

		AKC_RTCGetDate(&dtim);
		WriteDate(&dtim,ADRLASTDATE);
		if(dtim.day!=LastDay) {
			DaysLeft--;
			writinteep(DaysLeft,ADRDAYSLEFT);
		  	LastDay=dtim.day;
			writinteep(LastDay,ADRLASTDAY);
			TestLicense();
		}

	}
}*/

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
	


	if(dtim.month<3){
		IntMonth=dtim.month + 12;
		IntYear=dtim.year-1;
	}
	else {
		IntMonth=dtim.month;
		IntYear=dtim.year;
	}
	IntDay = dtim.day;
	IntDay += (double)(dtim.hour / 24.0);
	IntDay += (double)(dtim.min / 1440.0);
	IntDay += (double)(dtim.sec / 86400.0);

	c=2 - floor(IntYear/100) + floor(IntYear/400);

	jul=floor(1461.0 * (IntYear + 4716.0) /4.0) + floor(153.0*(IntMonth+1.0)/5.0) + IntDay + c - 1524.5;
	return(jul);
}

DTIM GregDat(double JulDat)
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

	dtim.month=q+ 2 - 12 * s4;
	dtim.year=100*(n-49) + i + s4;
	day=e + JulDat - p + 0.5;
	dtim.day=floor(day);
	day-=dtim.day;
	dtim.hour=floor(day*24.0);
	day-=(double)(dtim.hour/24.0);
	dtim.min=floor(day*1440.0);
	day-=(double)(dtim.min/1440.0);
	dtim.sec=floor(day*86400.0);

	return(dtim);
}*/			


		
