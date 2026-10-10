#include "tbh125.h"
#include "usbd_cdc_vcp.h"
void usb_getchar (void);

extern unsigned int blpos,blepos,eichpos,insttype,MaxNachzieh;
extern char befbuf[];
extern float mmfak,Diameter,Hardness;
extern float dmeing,dicke,eichmas,vorbrems,NwtFak,dickeing;
extern long counth,countd,countt,countw; 
extern unsigned int MeasureMethod,HardOn,DiamOn,ThickOn,AutoMess,DatenAusgabe;
extern float aktgew;
extern char	usb_RX_transition[200];
float gewicht;
char buf[150],buf1[50];
volatile char id[20];
BOOL RemoteFlg=__FALSE;
BOOL BusyFlg=__FALSE;
extern int Sensibility,ForceLimit;
extern unsigned int ueberlauf; 					// Gavin, 2022-02-09 周三 10:12:54
extern float MeasSpeed,oblbreit;
extern unsigned char vers[];
extern int YModemAktiv,Force;

extern OS_TID CommTask; 
extern OS_TID BusyTask; 

extern BOOL USB_Comm;

char message [] =   "Hello, here I am!\r\n";

unsigned int eepadr;

extern unsigned char	HardVersion[7] ;
extern char IdNum[14];
char Version_buf[100];		//处理收发关于Version的缓存
char HardVersion_buf[7]; 	//处理电路版本号缓存

BOOL SetFlg = 0;					//设置标志为0表示不进行设置
extern u8 SaveVers ;

__task void BusyComm(void)
{
	int val;
	U16 events;
	char befbuf[100],rsbuf[100],rspoint=0;
	
	while(1) {
		os_evt_wait_or(0x0003,0xFFFF);
		events=os_evt_get();
		do {
			if(events==0x0002){
				USB_Comm=__TRUE;
//				val=usb_getchar();
			}
			else {
				USB_Comm=__FALSE;
				val=com_getchar2();
			}
			if(val!=0x0A && val!=-1) {
				rsbuf[rspoint]=val;
				if(rsbuf[rspoint]==0x0D) {
					rsbuf[rspoint]=0;
					strcpy(befbuf,rsbuf);
					rspoint=0;
				}
				else { 
        			if(rspoint<50)	rspoint++;
				}
			}
		}while(val!=-1);
		putstr("ST 09\r");
	}
} 
		
 	
__task void Communicate(void)
{
	int i,j;
	int val;
	float hil;
	U16 events;
	char befbuf[100],rsbuf[50],rspoint=0,buf1[50],buf[50];
	DTIM dat;
	
	while(1) 
	{
		
		BusyFlg=__FALSE;
		os_evt_wait_or(0x0003,0xFFFF);
				
		events=os_evt_get();
		
		do 
		{
			
			if(events==0x0002)
			{
				USB_Comm=__TRUE;
				memset(usb_RX_transition,0,200);	
				usb_getchar();										//usb字符类型转换 u8转成char
				strcpy(befbuf,usb_RX_transition);
				break;
			}
			else 
			{
				USB_Comm=__FALSE;
				val=com_getchar2();
			}
			
			if(val!=0x0A && val!=-1) 
			{
				rsbuf[rspoint]=val;
				if(rsbuf[rspoint]==0x0D) 
				{
					rsbuf[rspoint]=0;
					strcpy(befbuf,rsbuf);
					rspoint=0;
				}
				else 
				{ 
        	if(rspoint<50)	rspoint++;
				}
			}
		}while(val!=-1);
		
		BusyFlg=__TRUE;
		if(strncmp(befbuf,"STYPE ",6)==0) 						// 设置仪器型号
		{	
			strncpy(buf,&befbuf[6],3);
			buf[3]=0;
			writeeeprom(buf[0],ADRVERSION+10);
			if(buf[1] == 'D')
			{
				writeeeprom(buf[1],ADRVERSION+11);
				writeeeprom(0x20,ADRVERSION+12);
				vers[10] = buf[0];
				vers[11] = buf[1];
				vers[12] = 0x20;
			}else if(buf[1] == 'T')
			{
				writeeeprom(buf[1],ADRVERSION+11);
				writeeeprom(buf[2],ADRVERSION+12);
				vers[10] = buf[0];
				vers[11] = buf[1];
				vers[12] = buf[2];
			}else if(buf[1] == 0)
			{
				writeeeprom(0x20,ADRVERSION+11);
				writeeeprom(0x20,ADRVERSION+12);
				vers[10] = buf[0];
				vers[11] = 0x20;
				vers[12] = 0x20;
			}
			putstr("Set OK\r\n");
			continue;
		}
		if(strcmp(befbuf,"GTYPE")==0) 								// 获取仪器型号
		{
			strncpy(buf1,(char *)&vers[10],3);
			buf1[3]=0;
			if(buf1[1] == 'D')
				putstr("GTYPE:TBH125D\r\n");
			if(buf1[1] == 'T')
				putstr("GTYPE:TBH125TD\r\n");
			if(buf1[1] == 0X20)
				putstr("GTYPE:TBH125\r\n");
			continue;
		}
		if(strncmp(befbuf,"SRANGE ",7)==0) 						// 设置电机大小
		{
			strncpy(buf,&befbuf[7],1);
			if(buf[0] == '3')
			{
				vers[49] = '0';
				writeeeprom(0X30,ADRVERSION+49);
			}else
			{
				vers[49] = '1';
				writeeeprom(0x31,ADRVERSION+49);
			}
			putstr("Set OK\r\n");
			continue;
		}
		if(strcmp(befbuf,"GRANGE")==0) 								// 获取电机大小
		{ 
			if(vers[49]=='0')	
				putstr("GRANGE:Small Motor\r\n");
			else
				putstr("GRANGE:Big Motor\r\n");
			continue;
		}
		if(strcmp(befbuf,"HQ")==0) 										// 参数发送给上位机标志
		{ 
			
			strncpy(Version_buf,(char *)&vers[0],86);
			strncpy(HardVersion_buf,(char *)&HardVersion[0],7);
			
			putstr(Version_buf);														//86位内部参数
			putstr(HardVersion_buf);												//7位电路板硬件版本号
			putstr(IdNum);																	//14位ID号
			continue;
		}
		if(strcmp(befbuf,"SET")==0) 									// 参数设置开始标志
		{ 
			SetFlg = 1;																			//设置标志为1表示上位机对单片机发送了设置指令
			continue;	
		}
		if(strcmp(befbuf,"END")==0) 									// 参数设置结束标志标志
		{ 
			SetFlg = 0;																			//标志清零
			SaveVers = 0;																		 
			writeepbuf((unsigned char*)vers,ADRVERSION,86); //把vers参数数组写入EEPROM
			continue;	
		}
		if(strcmp(befbuf,"ST")==0) 
		{              /*Abfrage Messmethode*/
			putstr("ST 00\r");
			continue;
		}
		if(strcmp(befbuf,"HVS")==0) 
		{
			strncpy(buf1,(char *)&vers[0],5);
			buf1[5]=0;
			sprintf(buf,"HVS: %.6s\r",buf1);
			putstr(buf);
			continue;
		}
		/*if(strcmp(befbuf,"GSVER")==0) 								// 获取软件版本号，Gavin，2021-09-13 星期一 15:22:55
		{
			readeepbuf((unsigned char *)buf1,ADRVERSION,5);
			//strncpy(buf1,(char *)&vers[0],5);
			buf1[5]=0;
			sprintf(buf,"GSVER: %.6s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SSVER ",6)==0) 						// 设置软件版本号
		{
			strncpy(buf1,&befbuf[6],5);
			writeepbuf((unsigned char *)buf1,ADRVERSION,5);
			buf1[5]=0;
			sprintf(buf,"SSVER: %.6s\r",buf1);
			putstr(buf);
			readeepbuf((unsigned char *)vers,ADRVERSION,86);								//读出内部参数
			continue;
		}*/
		if(strcmp(befbuf,"BL")==0) 
		{
			putstr("BL\r");
			blbef(0);
			continue;
		}
		if(strcmp(befbuf,"BLE")==0) 
		{
			putstr("BLE\r");
			blebef();
			continue;
		}
		if(strcmp(befbuf,"MD")==0) 
		{
			putstr("MD\r");
			mdbef(0);
			continue;
		}
		if(strcmp(befbuf,"ME")==0) 
		{
			putstr("ME\r");
			
			mebef();
			continue;
		}
		if(strcmp(befbuf,"HMM")==0) 
		{
			sprintf(buf,"HMM: %4.2f\r",mmfak);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HD")==0) 
		{
			sprintf(buf,"HD: %4.2f\r",Diameter);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HB")==0) 
		{
			sprintf(buf,"HB: %4.2f\r",Hardness);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HBB")==0) 
		{
			sprintf(buf,"HBB: %d %d\r",blpos,blepos);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HM" )==0 || strcmp(befbuf,"HMKAL")==0) 
		{
			hil=GetDoseAvrgN(1,1000);
			hil/=NwtFak;
			sprintf(buf,"HM: %5.1f\r",hil);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"BK")==0) 
		{
			tbhnull();
			putstr("BK\r");
			continue;
		}
		if(strcmp(befbuf,"BR")==0) 
		{
			backreset();
			putstr("BR\r");
			if(vers[49]==0x31)      //#ifdef GRMOT
				ForceLimit=FORCELIMITMAXGMOT;
			else
				ForceLimit=FORCELIMITMAXKMOT;
			continue;
		}
		if(strcmp(befbuf,"HKL")==0) 
		{
			sprintf(buf,"HKL: %3d\r",ForceLimit);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SKL ",4)==0) 
		{
			ForceLimit=atof(&befbuf[4]);
			if(vers[49]==0x31 && ForceLimit>FORCELIMITMAXGMOT) 
			{
				ForceLimit=FORCELIMITMAXGMOT;
			}
			if(vers[49]==0x30 && ForceLimit>FORCELIMITMAXKMOT) 
			{
				ForceLimit=FORCELIMITMAXKMOT;
			}
			sprintf(buf,"SKL: %3d\r",ForceLimit);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SDM ",4)==0) 
		{
			dmeing=atof(&befbuf[4]);
			sprintf(buf,"SDM: %4.1f\r",dmeing);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"GI")==0) 
		{
//			getid(id);
			sprintf(buf,"GI: %014s\r",id);
			putstr(buf);
			continue;																								 
		}
		if(strcmp(befbuf,"VINCPROG")==0) 
		{
			Ymodem_Receive((u8 *)buf);
			Vinc_Reset();
			sprintf(buf,"VINCPROG\r");
			putstr(buf);
			continue;
		}			
		if(strcmp(befbuf,"VINCRESET")==0) 
		{
			putstr("VINCRESET\r");
			Vinc_Reset();
			putstr("VINCRESET\r");
			continue;
		}
		if(strncmp(befbuf,"VS ",3)==0) 
		{
			Vinc_Send(&befbuf[3]);
			os_dly_wait(100);
			Vinc_GetBuf(buf);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"VINCEXT")==0) 
		{
			Vinc_Extended();
			putstr("VINCEXT\r");
			continue;
		}
		if(strncmp(befbuf,"VINCPORT ",9)==0) 
		{
			sprintf(buf,"VINCPORT: %02X\r",Vinc_Query(befbuf[9] & 0x0F));
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"VINCVERS",9)==0) 
		{
			Vinc_Vers(buf1);
			sprintf(buf,"VINCVERS: %s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"VINCDIR",9)==0) 
		{
			Vinc_Dir(buf1);
			sprintf(buf,"VINCDIR: %s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"VINCTEST",8)==0) 
		{
			i=Vinc_TestAlive();
			sprintf(buf,"VINCTEST: %d\r",i);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"VINCQD")==0) 
		{
			for(j=0;j<16;j++) 
			{
				Vinc_QD(j,buf1);
				sprintf(buf,"VINCQD: %02X",buf1[0]);
				putstr(buf);
				for(i=1;i<20;i++)
				{
					sprintf(buf," %02X",buf1[i]);				
					putstr(buf);
				}
				putstr("\r");
				os_dly_wait(25);
			}
			continue;
		}
		if(strncmp(befbuf,"GCOD ",5)==0) 
		{
			i=atoi(&befbuf[5]);
			sprintf(buf,"GCOD: %6lu\r",FCode((char *)id,i));
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"HINIVAL ",8)==0) 
		{
			i=atoi(&befbuf[8]);
			sprintf(buf,"HINIVAL: %d\r",GetIniVal(i));
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"HINI ",5)==0) 
		{
			i=atoi(&befbuf[5]);
			sprintf(buf,"HINI: %d\r",GetIni(i));
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SEM ",4)==0) 
		{               /*Eingabe Eichmass Durchmesser*/
			strncpy(buf,&befbuf[4],5);
			buf[5]=0;
			eichmas=atof(buf);
			savefloat(&eichmas,ADREICHMAS);
			eichmas=0;
			loadfloat(&eichmas,ADREICHMAS);
			sprintf(buf,"SEM: %5.2f\r",eichmas);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HEM")==0) 
		{ 
			sprintf(buf,"HEM: %5.2f\r",eichmas);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"MEKA")==0) 
		{ 
			putstr("MEKA\r");
			mekabef();
			continue;
		}
		if(strncmp(befbuf,"SVB ",4)==0) 
		{               /*Eingabe Vorbremsweg*/
			vorbrems=atof(&befbuf[4]);
			sprintf(buf,"SVB: %5.2f\r",vorbrems);
			savefloat(&vorbrems,ADRVORBREMS);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HVB")==0) 
		{               /*Abfrage Newton Faktor*/
			sprintf(buf,"HVB: %5.2f\r",vorbrems);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SNF ",4)==0) 
		{               /*Eingabe Newtonfaktor*/
			strncpy(buf,&befbuf[4],5);
			buf[5]=0;
			NwtFak=atof(buf);
			savefloat(&NwtFak,ADRNWTFAK);
			sprintf(buf,"SNF: %5.2f\r",NwtFak);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SNF ",4)==0) 
		{               /*Eingabe Newtonfaktor*/
			strncpy(buf,&befbuf[4],5);
			buf[5]=0;
			hil=atof(buf);
			savefloat(&hil,ADRONWT);
			sprintf(buf,"SNOF: %5.2f\r",hil);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HNF")==0) 
		{               /*Abfrage Newton Faktor*/
			sprintf(buf,"HNF: %5.2f\r",NwtFak);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HNOF")==0) 
		{               /*Abfrage Newton Faktor*/
			loadfloat(&hil,ADRONWT);
			sprintf(buf,"HNOF: %5.2f\r",hil);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HMS")==0) 				// Abfrage Kraftanstieg
		{
			sprintf(buf,"HMS: %4.1f\r",MeasSpeed);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SMS ",4)==0) 		// Eingabe Newtonfaktor
		{
			strncpy(buf,&befbuf[4],5);
			buf[5]=0;
			MeasSpeed=atof(buf);
			savefloat(&NwtFak,ADRMEASSPEED);
			sprintf(buf,"SMS: %4.1f\r",MeasSpeed);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SSW ",4)==0) 		// Eingabe Schwelle，// 门槛值的串口修改
		{
			strncpy(buf,&befbuf[4],3);
			buf[3]=0;
			Sensibility=atoi(buf);
			writinteep(Sensibility,ADRSENSIBILITY);
			Sensibility=0;
			Sensibility=readinteep(ADRSENSIBILITY);
			sprintf(buf,"SSW: %3d\r",Sensibility);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HSS")==0) 				// Abfrage Schwelle
		{
			sprintf(buf,"HSS: %3d\r",Sensibility);
			putstr(buf);
			continue;
		}
		// Gavin，2022-02-09 周三 10:22:46--------------------
		if(strncmp(befbuf,"ULW ",4)==0) 		// Eingabe ueberlauf, 循环时间的串口修改
		{
			strncpy(buf,&befbuf[4],3);
			buf[3]=0;
			ueberlauf=atoi(buf); 							// 字符串转成整型数
			if(ueberlauf < 10) ueberlauf = 10;
			if(ueberlauf > 120) ueberlauf = 120;
			writinteep(ueberlauf,ADRUEBERLAUF);
			ueberlauf=0;
			ueberlauf=readinteep(ADRUEBERLAUF);
			sprintf(buf,"ULW: %3d\r",ueberlauf);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HUL")==0) 				// Abfrage ueberlauf
		{
			sprintf(buf,"HUL: %3d\r",ueberlauf);
			putstr(buf);
			continue;
		}
		
		if(strncmp(befbuf,"SEP ",4)==0) 		// Eingabe Eichposition
		{
			eichpos=atoi(&befbuf[4]);
			writinteep(eichpos,ADREICHPOS);
			eichpos=0;
			eichpos=readinteep(ADREICHPOS);
			sprintf(buf,"SEP: %3d\r",eichpos);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HEP")==0) 				// Abfrage Eichpos
		{
			sprintf(buf,"HEP: %3d\r",eichpos);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"EIP")==0) 				// Eichposition anfahren
		{
			putstr("EIP\r");
			eipbef();
			continue;
		}
		if(strcmp(befbuf,"HED")==0) 				// Holen Eichdatum Durchmesser
		{
			LoadDate(&dat,ADRKALDDAT);
			sprintf(buf,"HED: %02d.%02d.%4d\r",dat.day,dat.month,dat.year);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HEB")==0) 				// Holen Eichdatum Durchmesser
		{
			LoadDate(&dat,ADRKALBDAT);
			sprintf(buf,"HEB: %02d.%02d.%4d\r",dat.day,dat.month,dat.year);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SED ",4)==0) 		// Holen Eichdatum Durchmesser
		{
			strncpy(buf1,&befbuf[4],2);
			buf1[2]=0;
			dat.day=atoi(buf1);
			strncpy(buf1,&befbuf[7],2);
			buf1[2]=0;
			dat.month=atoi(buf1);
			strncpy(buf1,&befbuf[10],4);
			buf1[4]=0;
			dat.year=atoi(buf1);

			WriteDate(&dat,ADRKALDDAT);
			putstr("SED\r");
			continue;
		}
		if(strncmp(befbuf,"SEB ",4)==0) 		// Holen Eichdatum Durchmesser
		{
			strncpy(buf1,&befbuf[4],2);
			buf1[2]=0;
			dat.day=atoi(buf1);
			strncpy(buf1,&befbuf[7],2);
			buf1[2]=0;
			dat.month=atoi(buf1);
			strncpy(buf1,&befbuf[10],4);
			buf1[4]=0;
			dat.year=atoi(buf1);
			WriteDate(&dat,ADRKALBDAT);
			putstr("SEG\r");
			continue;
		}
		if(strcmp(befbuf,"HSNR")==0) 				// 获取序列号
		{
			strncpy(buf1,(char *)&vers[58],6);
			buf1[6]=0;
			sprintf(buf,"HSNR: %6s\r",buf1);
			continue;
		}
		if(strncmp(befbuf,"SSNR ",5)==0) 		// 设置序列号
		{	
			strncpy(buf,&befbuf[5],6);
			buf[6]=0;
			for(i=0;i<6;i++)
			{
				vers[58+i] =  buf[i];
				writeeeprom(buf[i],ADRVERSION+58+i);
			}
			putstr("Set OK");
			continue;
		}
		if(strcmp(befbuf,"GT")==0) 					// Ger則etyp senden
		{
			sprintf(buf,"GT %3d\r",insttype);
			putstr(buf);
			continue;
		}

		if(strcmp(befbuf,"HCOUD")==0) 
		{
			sprintf(buf,"HCOUD: %u\r",countd);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HCOUH")==0) 
		{
			sprintf(buf,"HCOUH: %u\r",counth);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"RCOUD")==0) 
		{	
			countd=0;
			writlongeep(countd,ADRCOUNTD);
			putstr(befbuf);
			continue;
		}
		if(strcmp(befbuf,"RCOUH")==0) 
		{
			counth=0;
			writlongeep(counth,ADRCOUNTH);
			putstr(befbuf);
			continue;
		}
		if(strcmp(befbuf,"EP")==0) 					// EPROM Version senden
		{
			for(i=0;i<5;i++)
				buf1[i]=vers[i];
			buf1[5]=0;
			sprintf(buf,"EPROM %4s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"EPSONDER")==0) 		// EPROM Version senden
		{
			for(i=0;i<3;i++)
				buf1[i]=vers[i+6];
			buf1[3]=0;
			sprintf(buf,"EPSONDER %s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"EPDATUM")==0) 		// EPROM Version senden
		{
			for(i=0;i<8;i++)
				buf1[i]=vers[i+14];
			buf1[8]=0;
			sprintf(buf,"EPDATUM %s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HUPDDAT")==0) 		// Update Datum senden
		{
			for(i=0;i<8;i++)
				buf1[i]=vers[i+25];
			buf1[8]=0;
			sprintf(buf,"HUPDDAT %s\r",buf1);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HUPDID")==0) 			// Update ID senden
		{
			putstr("HUPDID: 0001234567\r");
			continue;
		}
		if(strncmp(befbuf,"SL ",3)==0) 			// Setzen LoopId
		{
			RemoteFlg=__TRUE;
			strcat(befbuf,"\r");
			putstr(befbuf);
			continue;
		}
		if(strncmp(befbuf,"GA ",3)==0) 			// Setzen LoopId
		{
			RemoteFlg=__TRUE;
			strcat(befbuf,"\r");
			putstr(befbuf);
			continue;
		}
		if(strcmp(befbuf,"ENDPC")==0) 			// Update ID senden
		{
			RemoteFlg=__FALSE;
			putstr("ENDPC\r");
			continue;
		}
		if(strcmp(befbuf,"HIN")==0) 				// Inis abfragen
		{
			if(GetIni(1))
			{
				putstr("HIN: 1\r");
			}
			else 
			{
				putstr("HIN: 0\r");
			}
			continue;
		}
		if(strcmp(befbuf,"HKAN")==0) 				// Abfrage Kraftanstieg
		{
			sprintf(buf,"HKAN: %3d\r",Force);
			putstr(buf);
			continue;
		}
		if(strncmp(befbuf,"SKAN ",5)==0) 		// Abfrage Kraftanstieg
		{
			strncpy(buf,&befbuf[5],3);
			buf[3]=0;
			Force=atoi(buf);
			writinteep(Force,ADRFORCE);
			Force=0;
			Force=readinteep(ADRFORCE);
			sprintf(buf,"SKAN: %3d\r",Force);
/*				hil1=(double)kraftanst;
			hil1*=KKORR;
			ffspeed=(unsigned int)(65536.0-(((ffak/hil1)*921600.0)*2));
			ffadd=(65536-ffspeed)/10;
			hil1*=nwtfak;
			hil1/=10;
			kraftverg=(unsigned int)hil1;*/
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"SMET0")==0) 			// Setze Messmethode 0
		{
			strcat(befbuf,"\r");
			MeasureMethod=0;
			writinteep(MeasureMethod,ADRMEASUREMETHOD);
			putstr(befbuf);
			continue;
		}
		if(strcmp(befbuf,"SMET1")==0) 			// Setze Messmethode 1
		{
			
			strcat(befbuf,"\r");
			MeasureMethod=1;
			writinteep(MeasureMethod,ADRMEASUREMETHOD);
			putstr(befbuf);
			continue;
		}
		if(strcmp(befbuf,"HMET")==0) 				// Abfrage Messmethode
		{
			if(MeasureMethod==0)
				putstr("HMET: 0\r");
			else
				putstr("HMET: 1\r");
			continue;
		}
		if(strcmp(befbuf,"HWA")==0) 				// Abfrage Wandler direkt
		{
			sprintf(buf,"HWA: %4d\r",getadDose());
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HLA")==0) 				// Abfrage Laengenfaktor
		{
			sprintf(buf,"HLA: %4u\r",blpos);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"HMMF")==0) 
		{
			sprintf(buf,"HMMF: %6.1f\r",mmfak);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"SB115200")==0) 
		{
			putstr("SB115200\r");
			//com_baudrate (0,115200); 				// 115200 at 96MHz
			SET_Baudrate(USART2,115200);
			continue;
		}
		if(strcmp(befbuf,"HMMF")==0) 
		{
			sprintf(buf,"HMMF: %6.1f\r",mmfak);
			putstr(buf);
			continue;
		}
		if(strcmp(befbuf,"PRINTTEST")==0) 
		{
			if(!StartPrinter()) 
			{
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				StopPrinter();
			}
		 	continue;
		}
		memset(befbuf,0,100);
		putstr("NA\r");
	}
}




/*----------------------------------------------------------------------------
  checks the serial state and initiates notification
 *---------------------------------------------------------------------------*/
void VCOM_CheckSerialState (void) 
{
/*  U16 serialState;

  serialState = CDC_GetSerialState();
  if (serialState != CDC_SerialState) {
     CDC_SerialState = serialState;
     CDC_NotificationIn();              
  }*/
}

