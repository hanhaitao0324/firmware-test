#include "tbh125.h"


extern unsigned int GewEinh,LaeEinh,HardEinh,aktLanguage,PrinterPaper;
extern unsigned int MeasureMethod,LastStammdate,ThickOn,DiamOn,HardOn,AutoMess,DatenAusgabe;

extern char id[],buf[];
float minb,mind,mint,maxb,maxd,maxt,mittb,mittd,mittt;
float ming,mint,maxt,oblbreit;
extern const unsigned char vers[];

int korrval=0;
char messDate[11];
char messTime[6];

extern unsigned int pruefanz,pruefanzd,pruefanzdi;
extern volatile unsigned int abranz; 
unsigned int auswanz,auswanzd,auswanzt; 

float sabwb,sabwd,srelb,sreld,sabwt,srelt;
extern float MeasSpeed,laengfak,kraftfak;
extern unsigned int Force;
int seite;
extern int zeile;
DTIM kalbdat,kalddat;
extern int Sensibility;
extern int ForceLimit;
extern int terminologie;


char toldruck,framedruck;
char typstr[20],kalbbuf[20],kaldbuf[20];
extern float mebuT[100],mebuD[100],mebuH[100];
extern int dmena,dickena;

unsigned char prLang;
const unsigned char prTab[4]={0,1,2,3};



char const text230[4][90] = 
{
	"       \263                 %14s MESSPROTOKOLL Vs %5s      Seite: %2d  \263\n\r",
	"       \263                %14s TEST PROTOCOL Vs %5s       Page : %2d  \263\n\r",
	"       \263              %14s PROTOCOLE DE MESURE Vs %5s     Page : %2d\263\n\r",
	"       \263        %14s PROTOCOLO DE MEDICIONES Vs %5s     Pagina : %2d\263\n\r"
};


char const text232[4][80] = 
{
	"       \263Identnummer : %15s             Geschwindigkeit: %3.1f mm/sec  \263\n\r",
	"       \263ID-Number   : %15s                       Speed: %3.1f mm/sec  \263\n\r",
	"       \263ID-No.      : %15s                     Vitesse: %3.1f mm/sec  \263\n\r",
	"       \263Nr.ID       : %15s                   Velocidad: %3.1f mm/sec  \263\n\r"
};

char const text232a[4][80] = 
{
	"       \263Identnummer : %15s                Kraftanstieg: %3d N/sec   \263\n\r",
	"       \263ID-Number : %15s             Increase of force: %3d N/sec   \263\n\r",
	"       \263ID-No.: %15s                Augmentation force: %3d N/sec   \263\n\r",
	"       \263Nr.ID: %15s                  Aumento de fuerza: %3d N/sec   \263\n\r"
};

// 2021-06-07 星期一 16:46:29, Gavin，第一行由英文修改为德文（Serial number）
char const text232b[4][90] = 
{
	"       \263Seriennummer:  %s  Kraftlimit: %3d N         Schwelle: %3d N       \263\n\r",
	"       \263Serial number: %s  Force limit: %3d N       Threshold: %3d N       \263\n\r",
	"       \263Serial number: %s  Force limit: %3d N     Sensibility: %3d N       \263\n\r",
	"       \263Serial number: %s  Force limit: %3d N     Sensivility: %3d N       \263\n\r"
};

char const text233[4][90] = 
{
	"       \263Kontrolle Bruch: %10s Durchmesser: %10s                    \263\n\r",
	"       \263Check Hardness: %10s Diameter: %10s                        \263\n\r",
	"       \263Calibrage durete: %10s Diametre: %10s                      \263\n\r",
	"       \263Calibracion de la dureza: %10s Diametro: %10s              \263\n\r"};

char const text233nt[4][90] = 
{
	"       \263Kalibrierung Bruch: %10s Durchmesser: %10s                 \263\n\r",
	"       \263Calibration Hardness: %10s Diameter: %10s                  \263\n\r",
	"       \263Calibrage durete: %10s Diametre: %10s                      \263\n\r",
	"       \263Calibracion de la dureza: %10s Diametro: %10s              \263\n\r"
};

char const text233o[4][90] = 
{
	"       \263Kontrolle Bruch: %10s                                            \263\n\r",
	"       \263Check Hardness: %10s                                             \263\n\r",
	"       \263Calibrage durete: %10s                                           \263\n\r",
	"       \263Calibracion de la dureza: %10s                                   \263\n\r"
};

char const text233ont[4][90] = 
{
	"       \263Kalibrierung Bruch: %10s                                         \263\n\r",
	"       \263Calibration Hardness: %10s                                       \263\n\r",
	"       \263Calibrage durete: %10s                                           \263\n\r",
	"       \263Calibracion de la dureza: %10s                                   \263\n\r"
};


char const toxt234[4][80] = 
{
	"       \263Datum: %10s    Zeit: %5s                                       \263\n\r",
	"       \263Date: %10s    Time: %5s                                        \263\n\r",
	"       \263Date: %10s    Heure: %5s                                       \263\n\r",
	"       \263Fecha: %10s   Hora: %5s                                        \263\n\r"
};

char const text2371[4][90] = 
{
	"       \263Anzahl Pr\201flinge:  \263                \263      %3d       \263                 \263\n\r",
		 "       \263Number of samples: \263                \263      %3d       \263                 \263\n\r",
		 "       \263Nombre comprimes:  \263                \263      %3d       \263                 \263\n\r",
		 "       \263Numero de muestras:\263                \263      %3d       \263                 \263\n\r"
};

char const text2371a[4][90] = 
{
	"       \263Anzahl Pr\201flinge:  \263                \263                \263      %3d        \263\n\r",
		 "       \263Number of samples: \263                \263                \263      %3d        \263\n\r",
		 "       \263Nombre comprimes:  \263                \263                \263      %3d        \263\n\r",
		 "       \263Numero de muestras:\263                \263                \263      %3d        \263\n\r"
};

char const text2372[4][90] = 
{
	"       \263Anzahl Pr\201flinge:  \263                \263      %3d       \263      %3d        \263\n\r",
		 "       \263Number of samples: \263                \263      %3d       \263      %3d        \263\n\r",
		 "       \263Nombre comprimes:  \263                \263      %3d       \263      %3d        \263\n\r",
		 "       \263Numero de muestras:\263                \263      %3d       \263      %3d        \263\n\r"
};

char const text2373[4][90] = 
{
	"       \263Anzahl Pr\201flinge:  \263      %3d       \263      %3d       \263      %3d        \263\n\r",
		 "       \263Number of samples: \263      %3d       \263      %3d       \263      %3d        \263\n\r",
		 "       \263Nombre comprimes:  \263      %3d       \263      %3d       \263      %3d        \263\n\r",
		 "       \263Numero de muestras:\263      %3d       \263      %3d       \263      %3d        \263\n\r" 
};

char const text237a[4][90]={   "       \263Korrekturwert   : %3d                                                  \263\n\r",
             		          "       \263Correction value : %3d                                                 \263\n\r",
		                      "       \263Valeur correction: %3d                                                 \263\n\r",
		                      "       \263Valor de correccion: %3d                                               \263\n\r"};

char const toxt247[4][90] = 
{
	"       \263                   Messung durch Benutzer abgebrochen !                \263\n\r",
	"       \263                     Measuring interrupted by user !                   \263\n\r",
	"       \263                   Mesure interrompue par utilisateur !                \263\n\r",
	"       \263                   Medicion interrumpida por usuario !                 \263\n\r"
};

char const text238b[4][90] =
{
	"       \263                   \263      Dicke     \263  Bruchfestig.  \263  Durchmesser    \263\n\r",
	"       \263                   \263    Thickness   \263    Hardness    \263    Diameter     \263\n\r",
	"       \263                   \263     Epaisseur  \263     Durete     \263    Diametre     \263\n\r",
	"       \263                   \263     Grosor     \263     Dureza     \263    Diametro     \263\n\r"
};

char const text238[4][90] = 
{
	"       \263                   \263                \263  Bruchfestig.  \263  Durchmesser    \263\n\r",
	"       \263                   \263                \263    Hardness    \263    Diameter     \263\n\r",
	"       \263                   \263                \263     Durete     \263    Diametre     \263\n\r",
	"       \263                   \263                \263     Dureza     \263    Diametro     \263\n\r"
};

char const text238c[4][90] = 
{
	"       \263                   \263                \263           .    \263  Durchmesser    \263\n\r",
	"       \263                   \263                \263                \263    Diameter     \263\n\r",
	"       \263                   \263                \263                \263    Diametre     \263\n\r",
	"       \263                   \263                \263                \263    Diametro     \263\n\r"
};

char const text238o[4][90] = 
{
	"       \263                   \263                \263  Bruchfestig.  \263                 \263\n\r",
	"       \263                   \263                \263    Hardness    \263                 \263\n\r",
	"       \263                   \263                \263     Durete     \263                 \263\n\r",
	"       \263                   \263                \263     Dureza     \263                 \263\n\r"};


char const text247[4][90] = 
{
	"       \263                         E I N Z E L W E R T E                         \263\n\r",
	"       \263                    M E A S U R I N G   V A L U E S                    \263\n\r",
	"       \263              V A L E U R S  I N D I V I D U E L L E S                 \263\n\r",
	"       \263                 V A L O R E S  D E  M E D I C I O N                   \263\n\r"
};

char const text248o[4][40] = 
{
	"       \263Anz.Ausw:          \263",
	"       \263No. stat:          \263",
	"       \263No. stat:          \263",
	"       \263Nr. evaluacion:    \263"
};

char const text249o[40]={      "       \263X min:             \263"};

char const text250o[40]={      "       \263X max:             \263"};


char const text251o[40]={      "       \263X max-min:         \263"};

char const text253o[4][40] = 
{
	"       \263X mittelw:         \263",
	"       \263X average:         \263",
	"       \263X moyenne:         \263",
	"       \263X media  :         \263"
};


char const text254o[40]={      "       \263X S:               \263"};

char const text255o[40]={      "       \263X rel:             \263"};


char const toxt256[4][90] = 
{
	"       \263                         A U S W E R T U N G                           \263\n\r",
	"       \263                         S T A T I S T I C S                           \263\n\r",
	"       \263                       S T A T I S T I Q U E S                         \263\n\r",
	"       \263                         E V A L U A C I O N                           \263\n\r"
};

char const text256a[90]={   "       \263                   \263   %7s  \263   %7s  \263   %7s  \263   %7s  \263\n\r"};
char const text257[4][90]={    "       \263Unterschrift:...........................................               \263\n\r\f",
                              "       \263Signature:..............................................               \263\n\r\f",
                              "       \263Signature:..............................................               \263\n\r\f",
                              "       \263Firma:..................................................               \263\n\r\f"};

char const text257a[4][90] = 
{
	"       \263Unterschrift:...........................................               \263\n\r",
	"       \263Signature:..............................................               \263\n\r",
	"       \263Signature:..............................................               \263\n\r",
	"       \263Firma:..................................................               \263\n\r"
};


// 微型打印机报告 --------------------------------------------------------------
char const vtext230[4][90] = 
{
	"  ERWEKA TBH125 MESSPROTOKOLL   \r\n",
	"  ERWEKA TBH125 TEST PROTOCOL   \n\r",
	"  ERWEKA TBH125 PROTO.DE MESURE \n\r",
	"  ERWEKA TBH125 PROTO.DE MEDIC. \n\r"
};

char const vtext230c[4][90] = 
{
	"  CALEVA THT15 MESSPROTOKOLL   \r\n",
	"  CALEVA THT15 TEST PROTOCOL   \n\r",
	"  CALEVA THT15 PROTO.DE MESURE \n\r",
	"  CALEVA THT15 PROTO.DE MEDIC. \n\r"
};

char const vtext230q[4][90] = 
{
	"  QTEST THT125 MESSPROTOKOLL   \r\n",
	"  QTEST THT125 TEST PROTOCOL   \n\r",
	"  QTEST THT125 PROTO.DE MESURE \n\r",
	"  QTEST THT125 PROTO.DE MEDIC. \n\r"
};

char const vtext232[4][80] = 
{
	"Identnummer:%15s\r\nSerial number: %s\r\nGeschwindigkeit: %3.1f mm/sec\n\r",
	"ID-Number  :%15s\r\nSerial number: %s\r\nSpeed: %3.1f mm/sec  \n\r",
	"ID-No.     :%15s\r\nSerial number: %s\r\nVitesse: %3.1f mm/sec\n\r",
	"Nr.ID      :%15s\r\nSerial number: %s\r\nVelocidad: %3.1f mm/sec\n\r"
};

char const vtext232a[4][80] = 
{
	"Identnummer:%15s\r\nSerial number: %s\r\nKraftanstieg: %3d N/sec\n\r",
	"ID-Number: %15s\\r\nSerial number: %s\r\nIncrease of force: %3d N/sec\n\r",
	"ID-No.: %15s\r\nSerial number: %s\r\nAugmentation force: %3d N/sec\n\r",
	"Nr.ID: %15s\r\nSerial number: %s\r\nAumento de fuerza: %3d N/sec\n\r"
};

char const vtext232b[4][90] = 
{
	"Kraftlimit: %3d N\r\nSchwelle: %3d N\n\r",
	"Force limit: %3d N\r\nThreshold:   %3d N\n\r",
	"Force limit: %3d N\r\nSensibility: %3d N\n\r",
	"Force limit: %3d N\r\nSensivility: %3d N\n\r"
};

char const vtext233[4][90] = 
{
	"Kontrolle Bruch: %10s\r\nDurchmesser: %10s\n\r",
	"Check Hardness: %10s\r\nDiameter: %10s \n\r",
	"Calibrage durete: %10s\r\nDiametre: %10s\n\r",
	"Calibracion dureza: %10s\r\nDiametro: %10s\n\r"
};

char const vtext233nt[4][90] = 
{
	"Kalibrierung Bruch: %10s\r\nDurchmesser: %10s\n\r",
	"Calibration Hardness: %10s\r\nDiameter: %10s \n\r",
	"Calibrage durete: %10s\r\nDiametre: %10s\n\r",
	"Calibracion dureza: %10s\r\nDiametro: %10s\n\r"
};

char const vtext233o[4][90] = 
{
	"Kontrolle Bruch: %10s\n\r",
	"Check Hardness: %10s\n\r",
	"Calibrage durete: %10s\n\r",
	"Calibracion de la dureza: %10s\n\r"
};

char const vtext233ont[4][90] = 
{
	"Kalibrierung Bruch: %10s\n\r",
	"Calibration Hardness: %10s\n\r",
	"Calibrage durete: %10s\n\r",
	"Calibracion de la dureza: %10s\n\r"
};

char const vtoxt234[4][80] = 
{
	"Datum: %10s Zeit: %5s\n\r",
	"Date: %10s  Time: %5s\n\r",
	"Date: %10s    Heure: %5s\n\r",
	"Fecha: %10s   Hora: %5s\n\r"
};

char const vtext237a[4][90] = {
																"Korrekturwert   : %3d\n\r",
																"Correction value : %3d\n\r",
																"Valeur correction: %3d\n\r",
																"Valor de correccion: %3d\n\r"
															};

char const vtoxt247[4][90] = 
{
	"      Messung abgebrochen!      \n\r",
	"     Measuring interrupted !    \n\r",
	"      Mesure interrompue !      \n\r",
	"     Medicion interrumpida !    \n\r"
};

char const vtext2371[4][90] = 
{
	"Anzahl            %3d\n\r",
	"Number            %3d\n\r",
	"Nombre            %3d\n\r",
	"Numero            %3d\n\r"
};

char const vtext2371a[4][90] = 
{
	"Anzahl                    %3d   \n\r",
	"Number                    %3d   \n\r",
	"Nombre                    %3d   \n\r",
	"Numero:                   %3d   \n\r"
};

char const vtext2372[4][90] = 
{
	"Anzahl            %3d     %3d    \n\r",
	"Number            %3d     %3d    \n\r",
	"Nombre            %3d     %3d    \n\r",
	"Numero            %3d     %3d    \n\r"
};

char const vtext2373[4][90] = 
{
	"Anzahl    %3d     %3d     %3d   \n\r",
	"Number    %3d     %3d     %3d   \n\r",
	"Nombre    %3d     %3d     %3d   \n\r",
	"Numero    %3d     %3d     %3d   \n\r"
};

char const vtext238b[4][90] = 
{
	"         Dicke   Bruch  Durchm. \n\r",
	"      Thickness  Hardn. Diameter\n\r",
	"      Epaisseur  Durete Diametre\n\r",
	"        Grosor   Dureza Diametro\n\r"
};

char const vtext238[4][90] = 
{
	"                 Bruch  Durchm. \n\r",
	"                 Hardn. Diameter\n\r",
	"                 Durete Diametre\n\r",
	"                 Dureza Diametro\n\r"
};

char const vtext238c[4][90] = 
{
	"                        Durchm. \n\r",
	"                        Diameter\n\r",
	"                        Diametre\n\r",
	"                        Diametro\n\r"
};

char const vtext238o[4][90] = 
{
	"                 Bruch\n\r",
	"                Hardness\n\r",
	"                 Durete\n\r",
	"                 Dureza\n\r"
};

char const vtext247[4][90] = 
{
	"      E I N Z E L W E R T E     \n\r",
	"M E A S U R I N G   V A L U E S \n\r",
	"V A L E U R S  I N D I V I D U .\n\r",
	"V A L O R E S   M E D I C I O N \n\r"
};

char const vtext248o[4][40] = 
{
	"Anz.Ausw ",
	"No. stat ",
	"No. stat ",
	"Nr. eval "
};

char const vtext249o[40] = {    "X min   "};

char const vtext250o[40] = {    "X max   "};

char const vtext251o[40] = {    "Xmax-min"};

char const vtext253o[4][40] = 
{
	"Xmittelw",
	"Xaverage",
	"Xmoyenne",
	"Xmedia  "
};

char const vtext254o[40] = {    "X S:    "};

char const vtext255o[40] = {    "X rel:  "};

char const vtoxt256[4][90] = 
{
	"       A U S W E R T U N G      \n\r",
	"       S T A T I S T I C S      \n\r",
	"     S T A T I S T I Q U E S    \n\r",
	"       E V A L U A C I O N      \n\r"
};

char const vtext257a[4][90] = 
{
	"Unterschrift:...................\n\r",
	"Signature:......................\n\r",
	"Signature:......................\n\r",
	"Firma:..........................\n\r"
};

int druckergebnis(void) 					// USB台式，测量报告
{
	int i,ch420temp,maxzeile;
	unsigned char verganz;
	char vs[10],snr[8];
	static char buf[120];
	float fdat; 								// Gavin, 2021-07-08 星期四 9:11:44
	
	prLang=prTab[aktLanguage];

	seite=1;
	zeile=0;
	if(PrinterPaper==0) 
		maxzeile=58;
	else 
		maxzeile=48;
	
	if(dmena) 
	{
		LoadDate(&kalddat,ADRKALDDAT);
		getDTIMdatstr(kalddat,kaldbuf);
	}
	LoadDate(&kalbdat,ADRKALBDAT);
	getDTIMdatstr(kalbdat,kalbbuf);
	
	// 计算的测试结果，min - max - 差值 - 平均值 - 标准变差 - 偏差率 -------------
	calcauswert();

	if((i=StartPrinter())!=0)
		return(i);
	
	for(i=0;i<5;i++) vs[i]=vers[i];
	vs[5]=0;
	
	for(i=0;i<6;i++) 
	{
		snr[i]=vers[58+i];
		snr[6]=0;
	}
	
	//SendPrinter("       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n\r");
	SendPrinter("       \xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xbf\n\r");
	
	if(vers[23]=='E') 
	{
		sprintf(buf,text230[prLang],"ERWEKA TBH125",vs,seite);
	}
	if(vers[23]=='C') 
	{
		sprintf(buf,text230[prLang],"CALEVA THT15",vs,seite);
	}
	if(vers[23]=='Q') 
	{
		sprintf(buf,text230[prLang],"QTEST THT125",vs,seite);
	}
	SendPrinter(buf);
	
	if(MeasureMethod==0)
		sprintf(buf,text232[prLang],id,MeasSpeed);
	else
		sprintf(buf,text232a[prLang],id,Force);
	SendPrinter(buf);
	
	// 206-7-3，Gavin，2019-07-26 星期五 14:59:20
	//sprintf(buf,text232b[prLang],ForceLimit,Sensibility);
	sprintf(buf,text232b[prLang],snr,ForceLimit,Sensibility);
	SendPrinter(buf);

	if(dmena) 
	{
		if(terminologie)
			sprintf(buf,text233nt[prLang],kalbbuf,kaldbuf);
		else
			sprintf(buf,text233[prLang],kalbbuf,kaldbuf);
	}
	else
	{	
		if(terminologie)
			sprintf(buf,text233ont[prLang],kalbbuf);
		else
			sprintf(buf,text233o[prLang],kalbbuf);
	}
	SendPrinter(buf);

	SendPrinter("       \263                                                                       \263\n\r");
	sprintf(buf,toxt234[prLang],messDate,messTime);
	SendPrinter(buf);
	SendPrinter("       \263                                                                       \263\n\r");
//	sprintf(buf,text237a[prLang],korrval);
//	SendPrinter(buf);

  //SendPrinter("       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄ´\n\r");
	SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	if(dickena && ThickOn) 
	{
		sprintf(buf,text2373[prLang],pruefanzdi,pruefanz,pruefanzd);
	}
	else 
	{
		if(dmena && DiamOn) 
		{
			if(HardOn)
				sprintf(buf,text2372[prLang],pruefanz,pruefanzd);
			else
				sprintf(buf,text2371a[prLang],pruefanzd);
		}
		else 
		{
			sprintf(buf,text2371[prLang],pruefanz);
		}
	}
	SendPrinter(buf);
	
	//                  |-------------------------------------------------------------------------------|-------------------------------------------------------------------|-------------------------------------------------------------------|-----------------------------------------------------------------------|
  SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	
	// M E A S U R I N G   V A L U E S--------------------------------------------
	SendPrinterCC(text247[prLang]);
  SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	
	if(dickena && ThickOn) 
	{
		SendPrinterCC(text238b[prLang]);
	}
	else 
	{
		if(dmena && DiamOn) 
		{
			if(HardOn)
				SendPrinterCC(text238[prLang]);
			else
				SendPrinterCC(text238c[prLang]);
		}
		else
			SendPrinterCC(text238o[prLang]);
	}
	
	if(abranz==0) 
	{
		verganz=pruefanz;
		if(dmena && DiamOn) 
		{
			if(pruefanzd>verganz) 
				verganz=pruefanzd;
		}
	}
	else 
		verganz=abranz;
	
	// 测试记录，序号 - 厚度 - 硬度 - 直径 ---------------------------------------
	for(i=0;i<verganz;i++)
	{
		sprintf(buf,"       \263                %3d\263",i+1);
		SendPrinter(buf);

		//SendPrinter("            \263"); 		// 删除一列，Gavin，2021-09-08 星期三 11:34:49

		if(dickena && ThickOn) 							// 厚度测量记录
		{
			if(mebuT[i]<90000.0f) 
			{
				if(LaeEinh==0) 				// 公制单位，毫米
				{
					fdat = round2(mebuT[i]/laengfak);
					sprintf(buf,"   %6.2f  %2s   \263",fdat,"mm");
				}
				else 									// 英制单位，英寸
				{
					fdat = round3(mebuT[i]/laengfak);
					sprintf(buf,"   %6.3f  %2s   \263",fdat,"In");
				}
			}
			else 
			{
				if(mebuT[i]!=99999.0f)
					sprintf(buf,"     NA    %2s   \263","  ");
				else
					sprintf(buf,"           %2s   \263","  ");
			}
			
			SendPrinter(buf);
		}
		else 
		{
			SendPrinter("                \263");
		}
		
		if(HardOn) 													// 硬度测量记录
		{
			if(mebuH[i]<90000.0f) 
			{
				if(HardEinh==0)
				{
					fdat = round0(mebuH[i]/kraftfak);
					sprintf(buf,"    %6.0f %2s   \263",fdat," N");
				}
				else
				if(HardEinh==1)
				{
					fdat = round1(mebuH[i]/kraftfak);
					sprintf(buf,"    %6.1f %2s   \263",fdat,"kp");
				}
				else
				if(HardEinh==2)
				{
					fdat = round1(mebuH[i]/kraftfak);
					sprintf(buf,"    %6.1f %2s   \263",fdat,"Sc");
				}
			}
			else 
			{
				if(mebuH[i]!=99999.0f) 
				{
					if(mebuH[i]==99996.0f)
						sprintf(buf,"     FL   %2s    \263","  ");
					else
						sprintf(buf,"     NA   %2s    \263","  ");
				}
				else 
				{
					sprintf(buf,"                \263");
				}
			}
			
			SendPrinter(buf);
		}
		else 
		{
			SendPrinter("                \263");
		}

		if(dmena && DiamOn) 								// 直径测量记录
		{
			if(mebuD[i]<90000.0f) 
			{
				if(LaeEinh==0)
				{
					fdat = round2(mebuD[i]/laengfak);
					sprintf(buf,"    %6.2f  %2s   \263",fdat,"mm");
				}
				else 
				{
					fdat = round3(mebuD[i]/laengfak);
					sprintf(buf,"    %6.3f  %2s   \263",fdat,"In");
				}
			}
			else 
			{
				if(mebuD[i]!=99999.0f) 
				{
					sprintf(buf,"      NA    %2s   \263","  ");
				}
				else 
				{
					sprintf(buf,"                 \263");
				}
			}
			
			SendPrinter(buf);
		}
		else 
		{
			SendPrinter("                 \263");
		}
		sprintf(buf,"\n\r");
		SendPrinter(buf);
		
		if(zeile>maxzeile) 
		{
			zeile=0;
			seite++;
			printheader(ch420temp); 					// 打印表头，Gavin，2019-11-20 星期三 10:09:07
		}
	}
	
  if(abranz!=0) 
	{
		SendPrinterCC(toxt247[prLang]);
	}
	
	if(zeile > maxzeile-17) 
	{
		zeile=0;
		seite++;
		printheader(ch420temp);
	}
  
	//                  |-------------------------------------------------------------------------------|-------------------------------------------------------------------|-------------------------------------------------------------------|-----------------------------------------------------------------------|
  SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	SendPrinterCC(toxt256[prLang]); 			// S T A T I S T I C S
  SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	
	// No. stat:------------------------------------------------------------------
	sprintf(buf,text248o[prLang]);
	SendPrinter(buf);

	if(pruefanz>0) 												// 厚度 + 硬度
	{
		if(dickena && ThickOn) 
		{ 
			if(HardOn)
				sprintf(buf,"      %3d       \263      %3d       \263",auswanzt,auswanz);
			else
				sprintf(buf,"      %3d       \263                \263",auswanzt);
		}
		else 
		{
			if(HardOn)
				sprintf(buf,"                \263      %3d       \263",auswanz);
			else
				sprintf(buf,"                \263                \263");
		}
	}
	else 
	{
		sprintf(buf,"                \263                \263");
	}		
	SendPrinter(buf);
	
	// 测试 2021-07-15 星期四 11:17:16------------------------------
	//dmena = 1;
	//DiamOn = 1;
	//pruefanzd = 10;
	
	if(dmena && DiamOn && pruefanzd>0) 		// 直径
	{
		sprintf(buf,"      %3d        \263",auswanzd);
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X min:---------------------------------------------------------------------
	sprintf(buf,text249o);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 厚度
	{
		if(LaeEinh==0) 
		{
			fdat = round2(mint);
			sprintf(buf,"    %5.2f %2s    \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(mint);
			sprintf(buf,"    %5.3f %2s    \263",fdat,"In");
		}
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);

	if(pruefanz>0 && HardOn) 							// 硬度
	{
		if(HardEinh==0) 
		{
			fdat = round0(minb);
			sprintf(buf,"    %5.0f %2s    \263",fdat," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				fdat = round1(minb);
				sprintf(buf,"    %5.1f %2s    \263",fdat,"kp");
			}
			else 
			{
				fdat = round1(minb);
				sprintf(buf,"    %5.1f %2s    \263",fdat,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 直径
	{
		if(LaeEinh==0) 
		{
			fdat = round2(mind);
			sprintf(buf,"    %5.2f %2s     \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(mind);
			sprintf(buf,"    %5.3f %2s     \263",fdat,"In");
		}
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X max:---------------------------------------------------------------------
	sprintf(buf,text250o);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 厚度
	{
		if(LaeEinh==0) 
		{
			fdat = round2(maxt);
			sprintf(buf,"    %5.2f %2s    \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(maxt);
			sprintf(buf,"    %5.3f %2s    \263",fdat,"In");
		}
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(pruefanz>0 && HardOn) 							// 硬度
	{
		if(HardEinh==0) 
		{
			fdat = round0(maxb);
			sprintf(buf,"    %5.0f %2s    \263",fdat," N");
		}	
		else 
		{
			if(HardEinh==1) 
			{
				fdat = round1(maxb);
				sprintf(buf,"    %5.1f %2s    \263",fdat,"kp");
			}
			else 
			{
				fdat = round1(maxb);
				sprintf(buf,"    %5.1f %2s    \263",fdat,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 直径
	{
		if(LaeEinh==0) 
		{
			fdat = round2(maxd);
			sprintf(buf,"    %5.2f %2s     \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(maxd);
			sprintf(buf,"    %5.3f %2s     \263",fdat,"In");
		}
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");

	// X max-min:-----------------------------------------------------------------
	sprintf(buf,text251o);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 厚度
	{
		if(LaeEinh==0) 
		{
			fdat = round2(round2(maxt)-round2(mint));
			sprintf(buf,"    %5.2f %2s    \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(round3(maxt)-round3(mint));
			sprintf(buf,"    %5.3f %2s    \263",fdat,"In");
		}
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(pruefanz>0 && HardOn) 							// 硬度
	{
		if(HardEinh==0) 
		{
			fdat = round0(round0(maxb)- round0(minb));
			sprintf(buf,"    %5.0f %2s    \263",fdat," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				fdat = round1(round1(maxb)-round1(minb));
				sprintf(buf,"    %5.1f %2s    \263",fdat,"kp");
			}
			else 
			{
				fdat = round1(round1(maxb)-round1(minb));
				sprintf(buf,"    %5.1f %2s    \263",fdat,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 直径
	{
		if(LaeEinh==0) 
		{
			fdat = round2(round2(maxd)-round2(mind));
			sprintf(buf,"    %5.2f %2s     \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(round3(maxd)-round3(mind));
			sprintf(buf,"    %5.3f %2s     \263",fdat,"In");
		}
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X average:-----------------------------------------------------------------
	sprintf(buf,text253o[prLang]);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 打印厚度
	{
		if(LaeEinh==0) 
		{
			fdat = round2(mittt);
			sprintf(buf,"    %5.2f %2s    \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(mittt);
			sprintf(buf,"    %5.3f %2s    \263",fdat,"In");
		}
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(pruefanz>0 && HardOn) 							// 打印硬度
	{
		if(HardEinh==0) 
		{
			fdat = round1(mittb);
			sprintf(buf,"   %6.1f %2s    \263",fdat," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				fdat = round2(mittb);
				sprintf(buf,"   %6.2f %2s    \263",fdat,"kp");
			}
			else 
			{
				fdat = round2(mittb);
				sprintf(buf,"   %6.2f %2s    \263",fdat,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 打印直径
	{
		if(LaeEinh==0) 
		{
			fdat = round2(mittd);
			sprintf(buf,"    %5.2f %2s     \263",fdat,"mm");
		}
		else 
		{
			fdat = round3(mittd);
			sprintf(buf,"    %5.3f %2s     \263",fdat,"In");
		}
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X S:-----------------------------------------------------------------------
	sprintf(buf,text254o);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 打印厚度
	{
		if(LaeEinh==0) 
		{
			fdat = round2(sabwt);
			sprintf(buf,"    %5.2f %2s    \263",fdat,"mm");
		}
		else
		{
			fdat = round3(sabwt);
			sprintf(buf,"    %5.3f %2s    \263",fdat,"In");
		}
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(pruefanz>0 && HardOn) 							// 打印硬度
	{ 
		if(HardEinh==0) 
		{
			fdat = round1(sabwb);
			sprintf(buf,"    %5.1f %2s    \263",fdat," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				fdat = round2(sabwb);
				sprintf(buf,"    %5.2f %2s    \263",fdat,"kp");
			}
			else 
			{
				fdat = round2(sabwb);
				sprintf(buf,"    %5.2f %2s    \263",fdat,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 打印直径
	{
		if(LaeEinh==0) 
		{
			fdat = round2(sabwd);
			sprintf(buf,"    %5.2f %2s     \263",fdat,"mm");
		}
		else
		{
			fdat = round3(sabwd);
			sprintf(buf,"    %5.3f %2s     \263",fdat,"In");
		}
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\n\r");
	
	// X rel:---------------------------------------------------------------------
	sprintf(buf,text255o);
	SendPrinter(buf);
	
	if(dickena && ThickOn) 								// 打印厚度
	{
		fdat = round1(srelt);
		sprintf(buf,"    %5.1f %%     \263",fdat);
	}
	else
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(pruefanz>0 && HardOn) 							// 打印硬度
	{
		fdat = round1(srelb);
		sprintf(buf,"    %5.1f %%     \263",fdat);
	}
	else 
	{
		sprintf(buf,"                \263");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 		// 打印直径
	{
		fdat = round1(sreld);
		sprintf(buf,"    %5.1f %%      \263",fdat);
	}
	else 
	{
		sprintf(buf,"                 \263");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
  //                  |-------------------------------------------------------------------------------|-------------------------------------------------------------------|-------------------------------------------------------------------|-----------------------------------------------------------------------|
  SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	SendPrinter("       \xb3                                                                       \xb3\n\r");
	SendPrinter("       \xb3                                                                       \xb3\n\r");
//	SendPrinter("       ³                                                                       ³\n\r");
//	SendPrinter("       ³                                                                       ³\n\r");
	SendPrinter("       \xb3                                                                       \xb3\n\r");
	SendPrinter("       \xb3                                                                       \xb3\n\r");
	
	// Signature:-----------------------------------------------------------------
	SendPrinterCC(text257a[prLang]);
#ifndef NOFF
	SendPrinter("       \xc0\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xd9\n\r\f");
#else
	SendPrinter("       \xc0\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xd9\n\r");
#endif	
	StopPrinter();
	return(0);
}

int druckergebnis32(void) 				// 微型打印（USB，COM），测量报告
{
	int i;
	unsigned char verganz;
	char snr[8]; 													// Gavin, 206-7-4, 2019-07-26 星期五 15:20:19
	static char buf[120];


	prLang=prTab[aktLanguage];

	if(dmena) 
	{
		LoadDate(&kalddat,ADRKALDDAT);
		getDTIMdatstr(kalddat,kaldbuf);
	}
	
	LoadDate(&kalbdat,ADRKALBDAT);
	getDTIMdatstr(kalbdat,kalbbuf);

	calcauswert();

	if(StartPrinter())return(1);
	
	for(i=0;i<6;i++) 											// 206-7-5, Gavin, 2019-07-26 星期五 15:21:25
	{
		snr[i]=vers[58+i];
		snr[6]=0;
	}
	
	if(vers[23]=='E') 
	{
		SendPrinterCC(vtext230[prLang]);
	}
	if(vers[23]=='C') 
	{
		SendPrinterCC(vtext230c[prLang]);
	}
	if(vers[23]=='Q') 
	{
		SendPrinterCC(vtext230q[prLang]);
	}
	
	if(MeasureMethod==0)
		sprintf(buf,vtext232[prLang],id,snr,MeasSpeed);
	else
		sprintf(buf,vtext232a[prLang],id,snr,Force);
	SendPrinter(buf);

	sprintf(buf,vtext232b[prLang],ForceLimit,Sensibility);
	SendPrinter(buf);

	if(dmena)
	{
		if(terminologie)
			sprintf(buf,vtext233nt[prLang],kalbbuf,kaldbuf);
		else
			sprintf(buf,vtext233[prLang],kalbbuf,kaldbuf);
	}
	else 
	{	
		if(terminologie)
			sprintf(buf,vtext233ont[prLang],kalbbuf);
		else
			sprintf(buf,vtext233o[prLang],kalbbuf);
	}
	SendPrinter(buf);
	
	// Date: 				Time: 				
	//-----------------------------------
	SendPrinter("\n\r");
	sprintf(buf,vtoxt234[prLang],messDate,messTime);
	SendPrinter(buf);
	SendPrinter("\n\r");
	
	// Number
	if(dickena && ThickOn) 
	{
		sprintf(buf,vtext2373[prLang],pruefanzdi,pruefanz,pruefanzd);
	}
	else 
	{
		if(dmena && DiamOn) 
		{
			if(HardOn)
				sprintf(buf,vtext2372[prLang],pruefanz,pruefanzd);
			else
				sprintf(buf,vtext2371a[prLang],pruefanzd);
		}
		else 
		{
			sprintf(buf,vtext2371[prLang],pruefanz);
		}
	}
	SendPrinter(buf);
	SendPrinter("\n\r");
	
	// M E A S U R I N G   V A L U E S
	SendPrinterCC(vtext247[prLang]);
	SendPrinter("\n\r");
	
	// Thickness  Hardn. Diameter
	if(dickena && ThickOn) 
	{
		SendPrinterCC(vtext238b[prLang]);
	}
	else 
	{
		if(dmena && DiamOn) 
		{
			if(HardOn)
				SendPrinterCC(vtext238[prLang]);
			else
				SendPrinterCC(vtext238c[prLang]);
		}
		else
			SendPrinterCC(vtext238o[prLang]);
	}
	if(abranz==0) 
	{
		verganz=pruefanz;
		if(dmena && DiamOn) 
		{
			if(pruefanzd>verganz) verganz=pruefanzd;
		}
	}
	else 
		verganz=abranz;

	for(i=0;i<verganz;i++) 
	{
		sprintf(buf,"%3d    ",i+1);
		SendPrinter(buf);

		if(dickena && ThickOn) 
		{
			if(mebuT[i]<90000.0f) 
			{
				if(LaeEinh==0)
				{
					sprintf(buf,"%5.2f%2s ",mebuT[i]/laengfak,"mm");
				}
				else 
				{
					sprintf(buf,"%5.3f%2s ",mebuT[i]/laengfak,"In");
				}
			}
			else 
			{
				if(mebuT[i]!=99999.0f) 
				{
					sprintf(buf,"   NA   ");
				}
				else 
				{
					sprintf(buf,"        ");
				}
			}
			SendPrinter(buf);
		}
		else 
		{
			SendPrinter("        ");
		}
		
		if(HardOn) 
		{
			if(mebuH[i]<90000.0f) 
			{
				if(HardEinh==0)
					sprintf(buf,"%5.0f%2s ",mebuH[i]/kraftfak," N");
				if(HardEinh==1)
					sprintf(buf,"%5.1f%2s ",mebuH[i]/kraftfak,"kp");
				if(HardEinh==2)
					sprintf(buf,"%5.1f%2s ",mebuH[i]/kraftfak,"Sc");
			}
			else 
			{
				if(mebuH[i]!=99999.0f) 
				{
					if(mebuH[i]==99996.0f)
						sprintf(buf,"   FL   ");
					else
						sprintf(buf,"   NA   ");
				}
				else 
				{
					sprintf(buf,"        ");
				}
			}
			SendPrinter(buf);
		}
		else 
		{
			SendPrinter("        ");
		}

		if(dmena && DiamOn) 
		{
				if(mebuD[i]<90000.0f) 
				{
					if(LaeEinh==0)
					{
						sprintf(buf,"%5.2f%2s ",mebuD[i]/laengfak,"mm");
					}
					else 
					{
						sprintf(buf,"%5.3f%2s ",mebuD[i]/laengfak,"In");
					}
				}
				else 
				{
					if(mebuD[i]!=99999.0f) 
					{
						sprintf(buf,"   NA   ");
					}
					else 
					{
						sprintf(buf,"        ");
					}
				}
				SendPrinter(buf);
		}
		else 
		{
			SendPrinter("        ");
		}
		sprintf(buf,"\n\r");
		SendPrinter(buf);
	}
	
	// 
	if(abranz!=0) 
	{
		SendPrinterCC(vtoxt247[prLang]);
	}
	
	// S T A T I S T I C S
	SendPrinter("\n\r");
	SendPrinterCC(vtoxt256[prLang]);
  SendPrinter("\n\r");
	
	// No. stat
	sprintf(buf,vtext248o[prLang]);
	SendPrinter(buf);

	if(pruefanz>0)
	{
		if(dickena && ThickOn) 
		{ 
			if(HardOn)
				sprintf(buf," %3d     %3d   ",auswanzt,auswanz);
			else
				sprintf(buf," %3d           ",auswanzt);
		}
		else 
		{
			if(HardOn)
				sprintf(buf,"         %3d   ",auswanz);
		}
	}
	SendPrinter(buf);

	if(dmena && DiamOn && pruefanzd>0) 
	{
		sprintf(buf," %3d ",auswanzd);
	}
	else 
	{
		sprintf(buf,"     ");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");

	// X min
	sprintf(buf,vtext249o);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	
	if(dickena && ThickOn) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",mint,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",mint/*/laengfak*/,"In");
		}
	}
	else
		sprintf(buf,"        ");
	
	SendPrinter(buf);

	if(pruefanz>0 && HardOn) 
	{
		if(HardEinh==0) 
		{
			sprintf(buf,"%5.0f%2s ",minb," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				sprintf(buf,"%5.1f%2s ",minb/*/kraftfak*/,"kp");
			}
			else 
			{
				sprintf(buf,"%5.1f%2s ",minb/*/kraftfak*/,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}
	SendPrinter(buf);
	
	if(dmena && DiamOn && pruefanzd>0) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",mind,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",mind/*/laengfak*/,"In");
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X max
	sprintf(buf,vtext250o);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	
	if(dickena && ThickOn) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",maxt,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",maxt/*/laengfak*/,"In");
		}
	}
	else
		sprintf(buf,"        ");
	
	SendPrinter(buf);
	if(pruefanz>0 && HardOn) 
	{
		if(HardEinh==0) 
		{
			sprintf(buf,"%5.0f%2s ",maxb," N");
		}	
		else 
		{
			if(HardEinh==1) 
			{
				sprintf(buf,"%5.1f%2s ",maxb/*/kraftfak*/,"kp");
			}
			else 
			{
				sprintf(buf,"%5.1f%2s ",maxb/*/kraftfak*/,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}
	SendPrinter(buf);
	if(dmena && DiamOn && pruefanzd>0) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",maxd,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",maxd/*/laengfak*/,"In");
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// Xmax-min
	sprintf(buf,vtext251o);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	

	if(dickena && ThickOn) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",maxt-mint,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",(maxt-mint)/*/laengfak*/,"In");
		}
	}
	else
		sprintf(buf,"        ");
	
	SendPrinter(buf);
	if(pruefanz>0 && HardOn) 
	{
		if(HardEinh==0) 
		{
			sprintf(buf,"%5.0f%2s ",maxb-minb," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				sprintf(buf,"%5.1f%2s ",(maxb-minb)/*/kraftfak*/,"kp");
			}
			else 
			{
				sprintf(buf,"%5.1f%2s ",(maxb-minb)/*/kraftfak*/,"Sc");
			}
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}	
	SendPrinter(buf);
	if(dmena && DiamOn && pruefanzd>0) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",maxd-mind,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",(maxd-mind)/*/laengfak*/,"In");
		}
	}
	else 
	{
		sprintf(buf,"        ");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// Xaverage
	sprintf(buf,vtext253o[prLang]);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	

	if(dickena && ThickOn)
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",mittt,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",mittt/*/laengfak*/,"In");
		}
	}
	else
		sprintf(buf,"       ");

	SendPrinter(buf);
	if(pruefanz>0 && HardOn) 
	{
		if(HardEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",mittb," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				sprintf(buf,"%5.2f%2s ",mittb/*/kraftfak*/,"kp");
			}
			else 
			{
				sprintf(buf,"%5.2f%2s ",mittb/*/kraftfak*/,"Sc");
			}
		}
		SendPrinter(buf);
	}
	else 
	{
		sprintf(buf,"        ");
		SendPrinter(buf);
	}
	
	if(dmena && DiamOn && pruefanzd>0) 
	{
		if(LaeEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",mittd,"mm");
		}
		else 
		{
			sprintf(buf,"%5.3f%2s ",mittd/*/laengfak*/,"In");
		}
	}
	else 
	{
		sprintf(buf,"       ");
	}
	SendPrinter(buf);
	SendPrinter("\r\n");
	
	// X S:
	sprintf(buf,vtext254o);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	

	if(dickena && ThickOn) 
	{
		sprintf(buf,"%5.2f%2s ",sabwt/*/laengfak*/,"mm");
	}
	else
		sprintf(buf,"        ");
	
	SendPrinter(buf);
	if(pruefanz>0 && HardOn)
	{ 
		if(HardEinh==0) 
		{
			sprintf(buf,"%5.2f%2s ",sabwb/*/kraftfak*/," N");
		}
		else 
		{
			if(HardEinh==1) 
			{
				sprintf(buf,"%5.2f%2s ",sabwb/*/kraftfak*/,"kp");
			}
			else 
			{
				sprintf(buf,"%5.2f%2s ",sabwb/*/kraftfak*/,"Sc");
			}
		}
		SendPrinter(buf);
	}
	else 
	{
		sprintf(buf,"        ");
		SendPrinter(buf);
	}

	if(dmena && DiamOn && pruefanzd>0) 
	{
		sprintf(buf,"%5.2f%2s ",sabwd/*/laengfak*/,"mm");
	}
	else 
	{
		sprintf(buf,"       ");
	}
	SendPrinter(buf);
	SendPrinter("\n\r");
	
	// X rel:
	sprintf(buf,vtext255o);
	SendPrinter(buf);
	sprintf(buf," ");
	SendPrinter(buf);  	

	if(dickena && ThickOn)
	{
		sprintf(buf,"%5.2f %% ",srelt);
	}
	else
		sprintf(buf,"        ");
	
	SendPrinter(buf);
	if(pruefanz>0 && HardOn) 
	{
		sprintf(buf,"%5.2f %% ",srelb);
		SendPrinter(buf);
	}
	else 
	{
		sprintf(buf,"        ");
		SendPrinter(buf);
	}

	if(dmena && DiamOn && pruefanzd>0) 
	{
		sprintf(buf,"%5.2f %% \n\r",sreld);
	}
	else 
	{
		sprintf(buf,"       ");
	}
	SendPrinter(buf);
	
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	
	// Signature:......................
	SendPrinterCC(vtext257a[prLang]);
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	StopPrinter();
	return(0);
}

void printheader(unsigned char ch420temp) 				// 换页时，打印表头
{
	int i;
	char vs[10],snr[8];
	
#ifndef NOFF
	sprintf(buf,"\f");
	SendPrinter(buf);
#endif
	
	for(i=0;i<5;i++) vs[i]=vers[i];
	vs[5]=0;
	
	//SendPrinter("       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n\r");
	SendPrinter("       \xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xbf\n\r");
	
	if(vers[23]=='E') 
	{
		sprintf(buf,text230[prLang],"ERWEKA TBH125",vs,seite);
	}
	
	if(vers[23]=='C') 
	{
		sprintf(buf,text230[prLang],"CALEVA THT15",vs,seite);
	}
	
	if(vers[23]=='Q') 
	{
		sprintf(buf,text230[prLang],"QTEST THT125",vs,seite);
	}
	
	SendPrinter(buf);
	
	// 打印"ID-Number", Gavin, 2019-11-20 星期三 10:13:33-------------------------
	if(MeasureMethod==0)
		sprintf(buf,text232[prLang],id,MeasSpeed);
	else
		sprintf(buf,text232a[prLang],id,Force);
	
	SendPrinter(buf);
	
	// 打印“Serial number”，Gavin，2019-11-20 星期三 10:14:41---------------------
	//sprintf(buf,text232b[prLang],ForceLimit,Sensibility);
	// 增加打印序列号的参数，v2.08，Gavin，2019-11-20 星期三 10:23:23-------------
	for(i=0;i<6;i++) 
	{
		snr[i]=vers[58+i];
		snr[6]=0;
	}
	
	sprintf(buf,text232b[prLang],snr,ForceLimit,Sensibility);
	SendPrinter(buf);
	
	if(dmena)
	{
		if(terminologie)
			sprintf(buf,text233nt[prLang],kalbbuf,kaldbuf);
		else
			sprintf(buf,text233[prLang],kalbbuf,kaldbuf);
	}
	else 
	{
		if(terminologie)
			sprintf(buf,text233ont[prLang],kalbbuf);
		else
			sprintf(buf,text233o[prLang],kalbbuf);
	}
	
	SendPrinter(buf);
	
	SendPrinter("       \263                                                                       \263\n\r");
	sprintf(buf,toxt234[prLang],messDate,messTime);
	SendPrinter(buf);
	SendPrinter("       \263                                                                       \263\n\r");
	
	SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	if(dickena && ThickOn) 
	{
		sprintf(buf,text2373[prLang],pruefanzdi,pruefanz,pruefanzd);
	}
	else 
	{
		if(dmena && DiamOn)
			sprintf(buf,text2372[prLang],pruefanz,pruefanzd);
		else
			sprintf(buf,text2371[prLang],pruefanz);
	}
	
	SendPrinter(buf);
	
	//SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
	//SendPrinter("       ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄ´\n\r");
	//SendPrinter("       \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n\r");
}

