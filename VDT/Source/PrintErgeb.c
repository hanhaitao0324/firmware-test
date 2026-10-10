#include "osObjects.h"                      // RTOS object definitions
#include "system.h"
#include "vinculum.h"
#include "utils.h"
#include <string.h>
#include "stdio.h"

extern char id[25],buf[150];
char messDate[11];
char messTime[6];

extern unsigned int PrinterPort,PrinterPaper,PrinterLanguage,PrinterFrames;
extern int Vacuum,messZeitL,messZeitH,ablZeitH,ablZeitL,tMode,Penetration;
extern int mepresure[10],metimh[10],metiml[10];
extern Platform_Data_s *Platform_Data;

int seite;
extern int zeile,altLanguage;
char framedruck;
char typstr[20];
extern bool Abbruch;
extern bool PresReduced;
extern bool MartelMode;
extern int tmpVacuum;
void druckergebnisMart(void);
void druckergebnisNorm(void);

unsigned char prLang;
const unsigned char prTab[4]={1,0,2,3};

char const text1[4][90]={   "Unterdruck",
                            "Vacuum",
                            "Vacuum",
                            "Vacuum"};
char const text2[4][90]={   "Umgebungsdruck",
                            "Ambient pressure",
                            "Ambient pressure",
                            "Ambient pressure"};

char const textp1[4][90]={ "       \xB3           %24s MESSPROTOKOLL          Seite: %2d  \xB3\n\r\n\r\n\r",
                           "       \xB3           %24s TEST PROTOCOL          Page : %2d  \xB3\n\r\n\r\n\r",
                           "       \xB3             %24s PROTOCOLE DE MESURE    Page : %2d\xB3\n\r\n\r\n\r",
                           "       \xB3       %24s PROTOCOLO DE MEDICIONES    Pagina : %2d\xB3\n\r\n\r\n\r"};

char const textp2[4][90]={ "       \xB3ID: %24s          Datum: %s Zeit: %s    \xB3\n\r",
                           "       \xB3ID: %24s           Date: %s Time: %s    \xB3\n\r",
                           "       \xB3ID: %24s           Date: %s Time: %s    \xB3\n\r",
                           "       \xB3ID: %24s           Date: %s Time: %s    \xB3\n\r"};

char const textp2a[4][90]={ "       \xB3Seriennummer: %s\xB3\n\r",
						  "       \xB3Serialnumber: %s\xB3\n\r",
						  "       \xB3Serialnumber: %s\xB3\n\r",
						  "       \xB3Serialnumber: %s\xB3\n\r"};	

char const textp3[4][90]={"       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02d h %02d min\r\n",
													"       \xB3Nominal   :  %3d mbar\r\n       \xB3Testtime  :  %02d h %02d min\r\n",
													"       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02d h %02d min\r\n",
													"       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02d h %02d min\r\n"};	

char const textp3a[4][90]={"       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02d min %02d sek\r\n",
													 "       \xB3Nominal   :  %3d mbar\r\n       \xB3Testtime  :  %02d min %02d sec\r\n",
													 "       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02bdh %02bdmin\r\n",
													 "       \xB3Sollwert  :  %3d mbar\r\n       \xB3Testzeit  :  %02bdh %02bdmin\r\n"};	

char const textp4[4][90]={"       \xB3Messwert Nr         Druck       Zeit                                   \xB3\n\r",
                          "       \xB3Measure Value No    Pressure     Time                                  \xB3\n\r",
                          "       \xB3Measure Value No    Pressure     Time                                  \xB3\n\r",
                          "       \xB3Measure Value No    Pressure     Time                                  \xB3\n\r"};

char const textp20[4][90]={    "       \xB3Unterschrift:...........................................               \xB3\n\r",
                              "       \xB3Signature:..............................................               \xB3\n\r",
                              "       \xB3Signature:..............................................               \xB3\n\r",
                              "       \263Firma:..................................................               \xB3\n\r"};

char const textp23[4][90]={    "       \xB3Penetrationszeit: %3d Minuten                                          \xB3\n\r",
                               "       \xB3Penetration time: %3d minutes                                          \xB3\n\r",
                               "       \xB3Penetration time: %3d minutes                                          \xB3\n\r",
                               "       \xB3Penetration time: %3d minutes                                          \xB3\n\r"};


char const textp21[4][90]={   "       \xB3          MESSUNG ABGEBROCHEN DURCH USER  !!                          \xB3\n\r",
                              "       \xB3              MEASUREMENT ABORTED BY USER !!                          \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r"};

char const textp22[4][90]={   "       \xB3    Der eingestellte Druck wurde nicht erreicht        !!             \xB3\n\r",
                              "       \xB3         the adjusted pressure was  not reached        !!             \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r"};
/*
char const textp22[4][90]={   "       \xB3          Druck wurde automatisch reduziert auf %3d hPa!!             \xB3\n\r",
                              "       \xB3          Pressure was automatically reduced to %3d hPA!!             \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r"};
*/                              
															
char const textp24[4][90]={"        Exsikkator:  %3d mm\n\r",
													 "        Exsiccator:  %3d mm\n\r",
													 "        Exsiccator:  %3d mm\n\r",
													 "        Exsiccator:  %3d mm\n\r"};	

char const textp25[4][90]={   "        Penetration: %s\n\r",
                              "        Penetration: %s\n\r",
                              "        Penetration: %s\n\r",
                              "        Penetration: %s\n\r"};

char const textsp1[4][90]={ "%s\r\n     MESSPROTOKOLL      \n\r\r\n",
                            "%24s\r\n     TEST PROTOCOL\n\r\r\n",
                            "%24s\r\nPROTOC.DE MESURE Page:%2d\n\r\n\r",
                            "%24s\r\nPROTOC.DE MEDIC. Pag.: %2d\xB3\n\r\n\r"};

char const textsp2[4][90]={ "ID:%20s\r\nDatum:%s %s\r\n\r\n",
						    "ID: %20s\r\nDate:%s %s\r\n\r\n",
						    "ID: %20s\r\nDate:%s Time:%s\n\r",
						    "ID: %20s\r\nDate:%s Time:%s\n\r"};	

char const textsp2a[4][90]={ "Seriennummer: %s\n\r",
						  "Serialnumber: %s\n\r",
						  "Serialnumber: %s\n\r",
						  "Serialnumber: %s\n\r"};	

char const textsp3[4][90]={ "Sollwert  :  %3d mbar\r\nTestzeit  :  %02d h %02d min\r\n\r\n",
													  "Nominal   :  %3d mbar\r\nTesttime  :  %02d h %02d min\r\n\r\n",
													  "Sollwert  :  %3d mbar\r\nTestzeit  :  %02d h %02d min\r\n",
													  "Sollwert  :  %3d mbar\r\nTestzeit  :  %02d h %02d min\r\n"};	

char const textsp3a[4][90]={"Sollwert  :  %3d mbar\r\nTestzeit  :  %02d min %02d sek\r\n\r\n",
														"Nominal   :  %3d mbar\r\nTesttime  :  %02d min %02d sec\r\n\r\n",
														"Sollwert  :  %3d mbar\r\nTestzeit  :  %02bdh %02bdmin\r\n",
														"Sollwert  :  %3d mbar\r\nTestzeit  :  %02bdh %02bdmin\r\n"};	

char const textsp4[4][90]={" Nr   Druck    Zeit\r\n",
                           " No  Pressure  Time\r\n",
                          "       \xB3Mesure Value No    Pressure     Time                                   \xB3\n\r",
                          "       \xB3Mesure Value No    Pressure     Time                                   \xB3\n\r"};

char const textsp20[4][90]={  "Unterschrift:...........\r\n",
                              "Signature:..............\r\n",
                              "       \xB3Signature:..............................................               \xB3\n\r",
                              "       \263Firma:..................................................               \xB3\n\r"};

char const textsp23[4][90]={  "Penetrationszeit:%3d min\n\r",
                              "Penetration time:%3d min\n\r",
                               "       \xB3Penetration time: %3d minutes                                          \xB3\n\r",
                               "       \xB3Penetration time: %3d minutes                                          \xB3\n\r"};


char const textsp21[4][90]={   "ABBRUCH DURCH USER!!\n\r",
                               "ABORTED BY USER!!\n\r",
                              "       \xB3              MEASUREMENT ABORTED BY USER !!                          \xB3\n\r",
                              "       \xB3              MEASUREMENT ABORTED BY USER !!                          \xB3\n\r"};

char const textsp22[4][90]={  "DRUCK NICHT ERREICHT!!\n\r",
                              "PRESSURE NOT REACHED!!\n\r",
                              "       \xB3                      PRESSURE NOT REACHED !!                         \xB3\n\r",
                              "       \xB3                      PRESSURE NOT REACHED !!                         \xB3\n\r"};
/*
char const textsp22[4][90]={  "Druck wurde automatisch\r\nreduziert auf %3d hPa!!\n\r",
                              "Pressure was automatically\r\nreduced to %3d hPA!!\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r",
                              "       \xB3                      MEASUREMENT ABORTED !!                          \xB3\n\r"};
*/
char const textsp24[4][90]={"Exsikkator:  %3d mm\n\r",
														"Exsiccator:  %3d mm\n\r",
														"Exsiccator:  %3d mm\n\r",
														"Exsiccator:  %3d mm\n\r"};

char const textsp25[4][90]={   "Penetration: %s\n\r",
                               "Penetration: %s\n\r",
                               "Penetration: %s\n\r",
                               "Penetration: %s\n\r"};

void druckergebnis(void)
{
    osDelay(500);
		if(!StartPrinter())return;
    osDelay(500);
    if(MartelMode)
       druckergebnisMart();
    else    
       druckergebnisNorm();
	StopPrinter();
}
                              
                              

void druckergebnisMart(void){
	char type[25];
	int i;

	seite=1;
	zeile=0;
	PrinterFrames=0;

	gettimstr(messTime);
	getdatstr(messDate);

	sprintf(buf,"\x1B\x21\x02\r\n"); // kleinen font einstellen
	SendPrinter(buf);

	sprintf(type,"ERWEKA VDTS %c.%c%c.%c  ",DEVICE_VERSION[0], DEVICE_VERSION[1], DEVICE_VERSION[2], DEVICE_VERSION[3]);
	sprintf(buf,textsp1[Platform_Data->Language],type);
	SendPrinter(buf);
	sprintf(buf,textsp2[Platform_Data->Language], id, messDate,messTime);
	SendPrinter(buf);
	sprintf(buf,textsp2a[Platform_Data->Language], Platform_Data->SerialNumber);
	SendPrinter(buf);
	sprintf(buf,textsp24[Platform_Data->Language],Platform_Data->Exisccator_size); // Exisccator_size
	SendPrinter(buf);
	if(Penetration>0) {  
    if(Platform_Data->Penetration_inverse){                   // Penetration inverse?
      sprintf(buf,textsp25[Platform_Data->Language], text1[Platform_Data->Language]); // 1= inverse
    }else{
      sprintf(buf,textsp25[Platform_Data->Language], text2[Platform_Data->Language]); // 0=None inverse
    }
    SendPrinter(buf);
  }
  if(tMode==1)
		sprintf(buf,textsp3[Platform_Data->Language],Vacuum,messZeitH,messZeitL);
	else
		sprintf(buf,textsp3a[Platform_Data->Language],Vacuum,messZeitH,messZeitL);
	SendPrinter(buf);
	SendPrinterCC(textsp4[Platform_Data->Language]);
	for(i=0;i<10;i++) {
		sprintf(buf," %2d   %3d    %2d:%02d\r\n",i+1,mepresure[i],metimh[i],metiml[i]);
		SendPrinter(buf);
	}
	if(Abbruch==CHANCEL_USER) 
		SendPrinterCC(textsp21[Platform_Data->Language]);
	if(PresReduced==true) {
		sprintf(buf,textsp22[Platform_Data->Language],tmpVacuum);
		SendPrinter(buf);
	}
	if(Penetration>0) {
		sprintf(buf,textsp23[Platform_Data->Language],Penetration);
		SendPrinter(buf);
	}			

    SendPrinter("\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
	SendPrinterCC(textsp20[Platform_Data->Language]);
    SendPrinter("\r\n\r\n\r\n\r\n");
}

void druckergebnisNorm(void)
{

	char type[25];
	int i;

	seite=1;
	zeile=0;
	PrinterFrames=0;

	gettimstr(messTime);
	getdatstr(messDate);

	//SendPrinter("       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n\r");
	// DA-×óÉÏ½Ç£¬C4-ÉÏÏÂ±ßÏß£¬BF-ÓÒÉÏ½Ç
	SendPrinter("       \xDA"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xBF\n\r");
	
	sprintf(type,"ERWEKA VDTS %c.%c%c.%c  ",DEVICE_VERSION[0], DEVICE_VERSION[1], DEVICE_VERSION[2], DEVICE_VERSION[3]);
	sprintf(buf,textp1[Platform_Data->Language],type,seite);
	SendPrinter(buf);
	sprintf(buf, textp2[Platform_Data->Language], id, messDate, messTime);
	SendPrinter(buf);
	sprintf(buf, textp2a[Platform_Data->Language], Platform_Data->SerialNumber);
	SendPrinter(buf);
	// B3-ÓÒ±ßÏß
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n\r");
	// C3-×ó±ßÏß¼ÓÖĞºáÏß£¬B4-ÓÒ±ßÏß¼ÓÖĞºáÏß
	SendPrinter("       \xC3"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xB4\n\r");
	
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	sprintf(buf,textp24[Platform_Data->Language],Platform_Data->Exisccator_size); 													// Exisccator_size
	SendPrinter(buf);
  if(Penetration>0) {
    if(Platform_Data->Penetration_inverse){                   // Penetration inverse?
      sprintf(buf,textp25[Platform_Data->Language], text1[Platform_Data->Language]); // 1= inverse
    }else{
      sprintf(buf,textp25[Platform_Data->Language], text2[Platform_Data->Language]); // 0=None inverse
    }  
    SendPrinter(buf);
  }
  if(tMode==1)
		sprintf(buf,textp3[Platform_Data->Language],Vacuum,messZeitH,messZeitL);
	else
		sprintf(buf,textp3a[Platform_Data->Language],Vacuum,messZeitH,messZeitL);
	SendPrinter(buf);
	//SendPrinter("       ?                                                                      ³\n\r");
  SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
  //SendPrinter("       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n\r");
	SendPrinter("       \xC3"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xB4\n\r");
	
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	SendPrinterCC(textp4[Platform_Data->Language]);
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	for(i=0;i<10;i++)
	{
		//sprintf(buf,"       ?   %2d              %3d       %2d:%02d                                    ³\n\r",i+1,mepresure[i],metimh[i],metiml[i]);
		sprintf(buf,"       \xB3   %2d              %3d       %2d:%02d                                    \xB3\n\r",i+1,mepresure[i],metimh[i],metiml[i]);
		SendPrinter(buf);
	}
	
	if(Abbruch==CHANCEL_USER) 
		SendPrinterCC(textp21[Platform_Data->Language]);
	else
	{
		//SendPrinter("       ?                                                                      ³\n\r");
		SendPrinter("       \xB3"
								"                                                                      "
								"\xB3\n\r");
	}
	
	if(PresReduced==true)
	{
		sprintf(buf,textp22[Platform_Data->Language],tmpVacuum);
		SendPrinter(buf);
	}
	else
	{
		//SendPrinter("       ?                                                                      ³\n\r");
		SendPrinter("       \xB3"
								"                                                                      "
								"\xB3\n\r");
	}
	
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	if(Penetration>0)
	{
		sprintf(buf,textp23[Platform_Data->Language],Penetration);
		SendPrinter(buf);
		//SendPrinter("       ?                                                                      ³\n\r");
		SendPrinter("       \xB3"
								"                                                                      "
								"\xB3\n\r");
	}
	
	//SendPrinter("       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n\r");
	SendPrinter("       \xC3"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xB4\n\r");
	
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	//SendPrinter("       ?                                                                      ³\n\r");
	SendPrinter("       \xB3"
							"                                                                      "
							"\xB3\n\r");
	
	SendPrinterCC(textp20[Platform_Data->Language]);
	//SendPrinter("       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\r");
	// C0-×óÏÂ½Ç£¬C4-ÉÏÏÂ±ßÏß£¬D9-ÓÒÏÂ½Ç
	SendPrinter("       \xC0"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xC4\xC4\xC4\xC4\xC4\xC4\xC4"
							"\xD9\n\r");
}

