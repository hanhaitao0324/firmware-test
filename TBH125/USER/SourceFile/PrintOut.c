#include "TBH125.h"

void printkpText(int);
void druckuepText(int);

// 测试
extern unsigned char prLang;
extern const unsigned char prTab[4];
extern char eidatb[11],eidatd[11];
//extern char jusdatb[11],jusdatd[11];
extern unsigned int LaeEinh,HardEinh,aktLanguage;
extern char id[],buf[150];
char kalivers[20],dosnr[10];
char kaliverso[20],dosnro[10];
extern float eichmas,NwtFak;
extern int dmena,dickena,waagena;
extern float minb,mind,mint,maxb,maxt,maxd,mittd,mittb,mittt,srelb,srelt,sreld;
extern unsigned int pruefanz,pruefanzd,pruefanzt,auswanz,auswanzd,auswanzt,abranz;
extern float sabwb,sabwd,sabwt;
extern float mebuD[100],mebuH[100],mebuT[100];
extern const unsigned char vers[];
extern unsigned int DatFormat;
extern float laengfak,kraftfak;

float sollb[3],istb[3],solld[3],istd[3],lasbnwt,lasdiam;

char eidatb[11],eidatd[11];
// bkal=0/1,硬度操作=检查/校验+检查；dkal=0/1,长度操作=检查/检验+检查；
char bkal,dkal; 												// Gavin, 2021-10-13 星期三 12:51:11
char eidatdo[11],eidatbo[11];
// bueb=0/1,硬度校验检查=未完成/完成；dueb=0/1,长度校验检查=未完成/完成；
char bueb=0,dueb=0; 										// Gavin, 2021-10-13 星期三 12:51:22
char jusdatdo[11],jusdatbo[11];
char jusdatd[11],jusdatb[11];
int laskmod,kalimode;
int lasbgew,lasbist,terminologie,autodiam;
float sollwert,iistwert;



char const textkp1a[4][85]=
{
	"\n\n                  ERWEKA TBH125 \232BERPR\232FUNG                       Seite: 1\n\r",
	"\n\n                    ERWEKA TBH125 CHECK                           Page : 1\n\r",
	"\n\n                    ERWEKA TBH125 CALIBRAGE                       Page : 1\n\r",
	"\n\n                  ERWEKA TBH125 CALIBRACION                      Pagina: 1\n\r"
};

char const textkp1a1[4][85]=
{
	"\n\n                  ERWEKA TBH125 KALIBRIERUNG                      Seite: 1\n\r",
	"\n\n                  ERWEKA TBH125 CALIBRATION                       Page : 1\n\r",
	"\n\n                    ERWEKA TBH125 CALIBRAGE                       Page : 1\n\r",
	"\n\n                   ERWEKA TBH125 CALIBRACION                     Pagina: 1\n\r"
};

char const textkp1a2[4][85]=
{
	"\n\n                   ERWEKA TBH125 JUSTIERUNG                       Seite: 1\n\r",
	"\n\n                   ERWEKA TBH125 ADJUSTMENT                       Page : 1\n\r",
	"\n\n                    ERWEKA TBH125 AJUSTAGE                        Page : 1\n\r",
	"\n\n                     ERWEKA TBH125 AJUSTE                        Pagina: 1\n\r"
};

char const textkp1ac[4][85]=
{
	"\n\n                  CALEVA THT15  \232BERPR\232FUNG                       Seite: 1\n\r",
	"\n\n                    CALEVA THT15  CHECK                           Page : 1\n\r",
	"\n\n                    CALEVA THT15  CALIBRAGE                       Page : 1\n\r",
	"\n\n                  CALEVA THT15  CALIBRACION                      Pagina: 1\n\r"};

char const textkp1a1c[4][85]=
{
	"\n\n                  CALEVA THT15  KALIBRIERUNG                      Seite: 1\n\r",
	"\n\n                  CALEVA THT15  CALIBRATION                       Page : 1\n\r",
	"\n\n                    CALEVA THT15  CALIBRAGE                       Page : 1\n\r",
	"\n\n                   CALEVA THT15  CALIBRACION                     Pagina: 1\n\r"
};

char const textkp1a2c[4][85]=
{
	"\n\n                   CALEVA THT15  JUSTIERUNG                       Seite: 1\n\r",
	"\n\n                   CALEVA THT15  ADJUSTMENT                       Page : 1\n\r",
	"\n\n                    CALEVA THT15  AJUSTAGE                        Page : 1\n\r",
	"\n\n                     CALEVA THT15  AJUSTE                        Pagina: 1\n\r"
};

char const textkp1aq[4][85]=
{
	"\n\n                  QTEST THT125  \232BERPR\232FUNG                       Seite: 1\n\r",
	"\n\n                    QTEST THT125  CHECK                           Page : 1\n\r",
	"\n\n                    QTEST THT125  CALIBRAGE                       Page : 1\n\r",
	"\n\n                  QTEST THT125  CALIBRACION                      Pagina: 1\n\r"
};

char const textkp1a1q[4][85]=
{
	"\n\n                  QTEST THT125  KALIBRIERUNG                      Seite: 1\n\r",
	"\n\n                  QTEST THT125  CALIBRATION                       Page : 1\n\r",
	"\n\n                    QTEST THT125  CALIBRAGE                       Page : 1\n\r",
	"\n\n                   QTEST THT125  CALIBRACION                     Pagina: 1\n\r"
};

char const textkp1a2q[4][85]=
{
	"\n\n                   QTEST THT125  JUSTIERUNG                       Seite: 1\n\r",
	"\n\n                   QTEST THT125  ADJUSTMENT                       Page : 1\n\r",
	"\n\n                    QTEST THT125  AJUSTAGE                        Page : 1\n\r",
	"\n\n                     QTEST THT125  AJUSTE                        Pagina: 1\n\r"
};

char const textkp2[4][48]=
{
	"       Identnummer: %15s    Seriennummer: %s\n\r",
	"       ID-Number: %15s    Serial number: %s\n\r",
	"       Numero ID: %15s    Serial number: %s\n\r",
	"       Numero ID: %15s    Serial number: %s\n\r"
};

char const textkp3[4][45]=
{
	"       Datum: %-8s    Zeit: %-5s\n\n\r",
	"       Date: %-8s    Time: %-5s\n\n\r",
	"       Date: %-8s    Heure: %-5s\n\n\r",
	"       Fecha: %-8s    Hora: %-5s\n\n\r"
};

char const textkp6[4][30]=
{
	"\n\n       Kalibrierung NEU\n\r",
	"\n\r       Calibration NEW\n\r",
	"\n\r       NOUVEAU Calibrage \n\r",
	"\n\r       Calibracion NUEVOS\n\r"
};

char const textkp61[4][30]=
{
	"\n\n       Justierung NEU\n\r",
	"\n\r       Adjustment NEW\n\r",
	"\n\r       NOUVEAU Ajustage \n\r",
	"\n\r       Ajuste NUEVOS\n\r"
};

char const textkp6a[4][30]=
{
	"\n\r       Kontrolle NEU\n\r",
	"\n\r       Check NEW\n\r",
	"\n\r       Controle NOUVEAU \n\r",
	"\n\r       Control NUEVOS\n\r"
};

char const textkp8a[4][75]=
{
	"       Kontrollkraft:         Soll: %-3.0f N Ist: %-3.0f N    ", 						// ‘-’含义为左对齐，右边填充空格
	"       Control force        Nominal: %-3.0f N Actual: %-3.0f N    ",
	"       Poids de controle    Nominal: %-3.0f N Actuel: %-3.0f N    ",
	"       Peso de comprobacion Nominal: %-3.0f N Real  : %-3.0f N    "
};

char const textkp8a1[4][75]=
{
	"       Kontrollgewicht:   Soll: %-4.1f kg Ist: %-4.1f kg    ",
	"       Control weight   Nominal: %-4.1f kg Actual: %-4.1f kg    ",
	"       Poids de controle    Nominal: %-4.1f kg Actuel: %-4.1f kg    ",
	"       Peso de control      Nominal: %-4.1f kg Real  : %-4.1f kg    "
};


char const textkp12[4][75]=
{
	"       Kontrollmass:      Soll: %-5.2fmm  Ist: %-5.2fmm    ",
	"       Control gauge:       Nominal: %-5.2fmm Actual: %-5.2fmm    ",
	"       Mesure de controle:  Nominal: %-5.2fmm Actuel: %-5.2fmm    ",
	"       Medida de control:   Nominal: %-5.2fmm Real: %-5.2fmm    "
};

char const textkp12Er[4][75]=
{
	"       Kontrollmass:      Soll: %-5.2fmm  Ist:   NA    \n\r",
	"       Control gauge:       Nominal: %-5.2fmm Actual:   NA    \n\r",
	"       Mesure de controle:  Nominal: %-5.2fmm Actuel:   NA    \n\r",
	"       Medida de control:   Nominal: %-5.2fmm Real:   NA    \n\r"
};

char const textkp14[4][80]=
{
	"       letztes Kalibrierdatum: %s  letzte Kalibrierung mit: %3d N\n\r",
	"       last calibration date:%s      last calibration with: %3d N\n\r",
	"       derniere date de calibr.:%s derniere calibrat. avec: %3d N\n\r",
	"       Ultima fecha de calibracion: %s  ultima calibr. con: %3d N\n\r"
};

char const textkp14j[4][80]=
{
	"       letztes Justierdatum: %s  letzte Justierung mit: %3d N\n\r",
	"       last adjustment date:%s      last adjustment with: %3d N\n\r",
	"       derniere date de ajustage:%s derniere ajustage avec: %3d N\n\r",
	"       Ultima fecha de ajuste: %s  ultima ajuste con: %3d N\n\r"
};

// 2016-8-3 星期三 14:48:09-----------------------------------------------------
/*
char const textkp15[4][90]=
{
	"                               Istwert: %3dN  Newtonfaktor: %4.2f\n\r",
	"                               Actual : %3dN  Newtonfactor: %4.2f\n\r",
	"                               Actuel : %3dN  facteur Nwt.: %4.2f\n\r",
	"                                 Real : %3dN factor Newton: %4.2f\n\r"
};
*/
char const textkp15[4][90]=
{
	"                                              Newtonfaktor: %4.2f\n\r",
	"                                              Newtonfactor: %4.2f\n\r",
	"                                              facteur Nwt.: %4.2f\n\r",
	"                                             factor Newton: %4.2f\n\r"
};

char const textkp14a[4][80]= 
{
	"       letztes Kontrolldatum: %s\n\r",
	"       last control date:%s  \n\r",
	"       derniere date de controle:%s \n\r",
	"       Ultima fecha de control: %s  \n\r"
};


char const textkp14a12[4][80]=
{
	"       letztes Justierdatum:   %s\n\r",
	"       last adjustment date:  %s  \n\r",
	"       derniere date de adjuste:%s \n\r",
	"       Ultima fecha de ajuste: %s  \n\r"
};

char const textkp14a1[4][80]=
{
	"       letztes Kalibrierdatum: %s\n\r",
	"       last calibration date: %s  \n\r",
	"       derniere date de calibration.:%s \n\r",
	"       Ultima fecha de calibracion: %s  \n\r"
};

char const textkp14a11[4][80]=
{
	"       letzte \232berpr\201fung:     %s\n\n\r",
	"       last instrument check: %s  \n\r",
	"       derniere date de ccontrole:%s \n\r",
	"       Ultima fecha de ccontrol: %s  \n\r"
};


char const textkp141[4][80]=
{
	"       letztes Kalibrierdatum: %s  letzte Kalibrierung mit: %4.1f kg\n\r",
	"       last calibration date:%s      last calibration with: %4.1f kg\n\r",
	"       derniere date de calibr.:%s  derniere calibrat.avec: %4.1f kg\n\r",
	"       Ultima fecha de calibracion: %s  ultima calibr. con: %4.1f kg\n\r"
};

char const textkp141j[4][80]=
{
	"       letztes Justierdatum: %s  letzte Justierung mit: %4.1f kg\n\r",
	"       last adjustment date:%s      last adjustment with: %4.1f kg\n\r",
	"       derniere date de ajustage:%s  derniere ajustage avec: %4.1f kg\n\r",
	"       Ultima fecha de ajuste: %s  ultima ajuste con: %4.1f kg\n\r"
};

// 2016-8-3 星期三 14:49:31-----------------------------------------------------
/*
char const textkp151[4][90]=
{
	"                            Istwert: %4.1fkg  Newtonfaktor: %4.2f\n\r",
	"                            Actual : %4.1fkg  Newtonfactor: %4.2f\n\r",
	"                            Actuel : %4.1fkg  facteur Nwt.: %4.2f\n\r",
	"                              Real : %4.1fkg factor Newton: %4.2f\n\r"
};
*/

char const textkp151[4][90]=
{
	"                                              Newtonfaktor: %4.2f\n\r",
	"                                              Newtonfactor: %4.2f\n\r",
	"                                              facteur Nwt.: %4.2f\n\r",
	"                                             factor Newton: %4.2f\n\r"
};

char const textkp15a[4][60]=
{
	"       mit Kalibrator Nr.: %s Dose Nr.: %s\n\r", 
	"       with Calibrator No.: %s Gauge No.: %s \n\r",
	"       avev Calibrateur No.: %s Dose Nr.: %s \n\r",
	"       con nr. de Calibrador: %s Nr. Celda : %s\n\r"
};

char const textkp16[4][90]=
{
	"       Kalibrierkraft Sollwert: %3.0fN Ist: %3.0fN\n\r",
	"       Calibrationforce Nominal: %3.0f N Act.: %3.0fN\n\r",
	"       Poids de calibr. Nominal: %3.0f N Act.: %3.0fN\n\r",
	"       Peso de calibracion     : %3.0f N Real: %3.0fN\n\r"
};

char const textkp16j[4][90]=
{
	"       Justierkraft Sollwert: %3.0fN Ist: %3.0fN\n\r",
	"       Adjustmentforce Nominal: %3.0f N Act.: %3.0fN\n\r",
	"       Poids de ajustage Nominal: %3.0f N Act.: %3.0fN\n\r",
	"       Peso de ajuste           : %3.0f N Real: %3.0fN\n\r"
};

char const textkp161[4][90]=
{
	"       Kalibriergewicht Soll: %4.1fkg Istwert: %4.1fkg\n\r",
	"       Calibrationweight Nominal: %4.1fkg Actual:%4.1fkg\n\r",
	"       Poids de calibr.  Nominal: %4.1fkg Actuel:%4.1fkg\n\r",
	"       Peso de calibracion      : %4.1fkg Real  :%4.1fkg\n\r"
};

char const textkp161j[4][90]=
{
	"       Justiergewicht Soll: %4.1fkg Istwert: %4.1fkg\n\r",
	"       Adjustmentweight Nominal: %4.1fkg Actual:%4.1fkg\n\r",
	"       Poids de ajustage Nominal: %4.1fkg Actuel:%4.1fkg\n\r",
	"       Peso de ajuste           : %4.1fkg Real  :%4.1fkg\n\r"
};

char const textkp163[4][90]=
{
	"       neuer Newtonfaktor: %4.2f\n\r",
	"       Newtonfactor new: %4.2f\n\r",
	"       nouveau facteur Newton: %4.2f\n\r",
	"       factor Newton nuevo: %4.2f\n\r"
};

char const textkp13[4][70]=
{
	"\n                             BRUCHFESTIGKEIT\n\r",
	"\n                                HARDNESS\n\r",
	"\n                            TESTEUR DE DURETE\n\r",
	"\n                                  DUREZA\n\r"
};

char const textkp17[4][60]=
{
	"\n                               DURCHMESSER\n\r",
	"\n                                DIAMETER\n\r",
	"\n                                DIAMETRE\n\r",
	"\n                                DIAMETRO\n\r"
};

char const textkp17b[4][60]=
{
	"\n                            DURCHMESSER/DICKE\n\r",
	"\n                           DIAMETER/THICKNESS\n\r",
	"\n                           DIAMETRE/EPAISSEUR\n\r",
	"\n                            DIAMETRO/ESPESOR\n\r"
};

char const textkp18[4][80]=
{
	"       letztes Kalibrierdatum: %s letzte Kalibrierung mit: %5.2fmm\n\r",
	"       last calibration date   : %s last calibration with: %5.2fmm\n\r",
	"       derniere date de calibr.: %s dern. calibrage avec : %5.2fmm\n\r",
	"       ultima fecha de calibr. : %s ultima calibracion con:%5.2fmm\n\r"
};

char const textkp18j[4][80]=
{
	"       letztes Justierdatum: %s letzte Justierung mit: %5.2fmm\n\r",
	"       last adjustment date   : %s last adjustment with: %5.2fmm\n\r",
	"       derniere date de ajustage: %s dern. ajusteage avec : %5.2fmm\n\r",
	"       ultima fecha de ajuste : %s ultima ajuste con:%5.2fmm\n\r"
};

char const textkp19[4][60]=
{
	"       Kalibriermass:             Soll: %5.2fmm\n\r",
	"       Calibration gauge:         Nominal: %5.2fmm\n\r",
	"       Mesure de calibrage:       Nominal: %5.2fmm\n\r",
	"       Medida de calibracion:     Nominal: %5.2fmm\n\r"
};

char const textkp19j[4][60]=
{
	"       Justiermass:               Soll: %5.2fmm\n\r",
	"       Adjustment gauge:        Nominal: %5.2fmm\n\r",
	"       Mesure de ajustage:      Nominal: %5.2fmm\n\r",
	"       Medida de ajuste:        Nominal: %5.2fmm\n\r"
};

char const textkp19a[4][60]=
{
	"       Kalibriergewicht: internes Gewicht\n\r",
	"       Calibration weight:         internal weight\n\r", 
	"       Mesure de calibrage:       Nominla: 5.00 mm\n\r",
	"       Medida de calibracion:     Nominal: 5.00 mm\n\r"
};

char const textkp50[4][40]=
{
	"       Kalibrierdatum: %-8s\n\r",
	"       Calibration date: %-8s\n\r",
	"       Date de calibration: %-8s\n\r",
	"       Fecha de calibracion: %-8s\n\r"
};

char const textkp50j[4][40]=
{
	"       Justierdatum: %-8s\n\r",
	"       Adjustment date: %-8s\n\r",
	"       Date de ajustage: %-8s\n\r",
	"       Fecha de ajuste: %-8s\n\r"
};

char const textkp51[4][90]=
{
	"\n\n\n\n\n       Unterschrift:.........................................................\n\r",
	"\n\n\n\n\n       Signature:............................................................\n\r",
	"\n\n\n\n\n       Signature:............................................................\n\r",
	"\n\n\n\n\n       Firma:................................................................\n\r"
};

char const textkp50a[4][80]=
{
	"       Kontrolldatum: %s\n\r",
	"       Control date:%s  \n\r",
	"       Date de control.:%s \n\r",
	"       Fecha de control: %s  \n\r"
};

char const vtextkp1a[4][85]=
{
	"\n\n ERWEKA TBH125 \232BERPR\232FUNG \n\r",
	"\n\n    ERWEKA TBH125 CHECK    \n\r",
	"\n\n   ERWEKA TBH125 CALIBRAGE \n\r",
	"\n\n ERWEKA TBH125 CALIBRACION \n\r"
};

char const vtextkp1a1[4][85]=
{
	"\n\nERWEKA TBH125 KALIBRIERUNG \n\r",
	"\n\n ERWEKA TBH125 CALIBRATION  \n\r",
	"\n\n ERWEKA TBH125 CALIBRAGE    \n\r",
	"\n\n ERWEKA TBH125 CALIBRACION  \n\r"
};

char const vtextkp1a2[4][85]=
{
	"\n\n ERWEKA TBH125 JUSTIERUNG  \n\r",
	"\n\n  ERWEKA TBH125 ADJUSTMENT  \n\r",
	"\n\n   ERWEKA TBH125 AJUSTAGE   \n\r",
	"\n\n    ERWEKA TBH125 AJUSTE    \n\r"
};

char const vtextkp1ac[4][85]=
{
	"\n\n CALEVA THT15 \232BERPR\232FUNG \n\r",
	"\n\n    CALEVA THT15 CHECK    \n\r",
	"\n\n   CALEVA THT15 CALIBRAGE \n\r",
	"\n\n CALEVA THT15 CALIBRACION \n\r"
};

char const vtextkp1a1c[4][85]=
{
	"\n\nCALEVA THT15 KALIBRIERUNG \n\r",
	"\n\n CALEVA THT15 CALIBRATION  \n\r",
	"\n\n CALEVA THT15 CALIBRAGE    \n\r",
	"\n\n CALEVA THT15 CALIBRACION  \n\r"
};

char const vtextkp1a2c[4][85]=
{
	"\n\n QTEST THT125 JUSTIERUNG  \n\r",
	"\n\n  QTEST THT125 ADJUSTMENT  \n\r",
	"\n\n   QTEST THT125 AJUSTAGE   \n\r",
	"\n\n    QTEST THT125 AJUSTE    \n\r"
};

char const vtextkp1aq[4][85]=
{
	"\n\n QTEST THT125 \232BERPR\232FUNG \n\r",
	"\n\n    QTEST THT125 CHECK    \n\r",
	"\n\n   QTEST THT125 CALIBRAGE \n\r",
	"\n\n QTEST THT125 CALIBRACION \n\r"
};

char const vtextkp1a1q[4][85]=
{
	"\n\nQTEST THT125 KALIBRIERUNG \n\r",
	"\n\n QTEST THT125 CALIBRATION  \n\r",
	"\n\n QTEST THT125 CALIBRAGE    \n\r",
	"\n\n QTEST THT125 CALIBRACION  \n\r"
};

char const vtextkp1a2q[4][85]=
{
	"\n\n QTEST THT125 JUSTIERUNG  \n\r",
	"\n\n  QTEST THT125 ADJUSTMENT  \n\r",
	"\n\n   QTEST THT125 AJUSTAGE   \n\r",
	"\n\n    QTEST THT125 AJUSTE    \n\r"
};


char const vtextkp2[4][48]=
{
	"Identnummer:%15s\r\nSeriennummer: %s\n\r",
	"ID-Number: %15s\r\nSerial number: %s\n\r",
	"Numero ID: %15s\r\nSerial number: %s\n\r",
	"Numero ID: %15s\t\nSerial number: %s\n\r"
};

char const vtextkp3[4][45]=
{
	"Datum: %-8s Zeit: %-5s\n\n\r",
	"Date: %-8s Time: %-5s\n\n\r",
	"Date: %-8s Heure: %-5s\n\n\r",
	"Fecha: %-8s Hora: %-5s\n\n\r"
};

char const vtextkp6[4][30]=
{
	"\n\n       Kalibrierung NEU\n\r",
	"\n\r       Calibration NEW\n\r",
	"\n\r       NOUVEAU Calibrage \n\r",
	"\n\r       Calibracion NUEVOS\n\r"
};

char const vtextkp61[4][30]=
{
	"\n\n       Justierung NEU\n\r",
	"\n\r       Adjustment NEW\n\r",
	"\n\r       NOUVEAU Ajustage \n\r",
	"\n\r       Ajuste NUEVOS\n\r"
};

char const vtextkp6a[4][35]=
{
	"\n\r       Kontrolle NEU\n\r",
	"\n\r       Check NEW\n\r",
	"\n\r       Controle NOUVEAU \n\r",
	"\n\r       Control NUEVOS\n\r"
};

char const vtextkp8a[4][75]=
{
	"Kontrollkraft\r\nSoll: %-3.0f N Ist: %-3.0f N",
	"Control force\r\nNominal: %-3.0f N Actual: %-3.0f N    ",
	"Poids de controle\r\nNominal: %-3.0f N Actuel: %-3.0f N    ",
	"Peso de comprobacion\r\nNominal: %-3.0f N Real  : %-3.0f N    "
};

char const vtextkp8a1[4][75]=
{
	"Kontrollgewicht\r\nSoll: %-4.1f kg Ist: %-4.1f kg    ",
	"Control weight\r\nNominal: %-4.1f kg Actual: %-4.1f kg    ",
	"Poids de controle\r\nNominal: %-4.1f kg Actuel: %-4.1f kg    ",
	"Peso de control\r\nNominal: %-4.1f kg Real  : %-4.1f kg    "
};


char const vtextkp12[4][75]=
{
	"Kontrollmass\r\nSoll: %-5.2fmm  Ist: %-5.2fmm    ",
	//2017/2/6 9:59:15--------------------
//"Control gauge\r\nNominal: %-5.2fmm Actual: %-5.2fmm    ",
	"Control gauge\r\nNominal: %-5.2fmm Actual: %-5.2fmm",
	
	"Mesure de controle\r\nNominal: %-5.2fmm Actuel: %-5.2fmm    ",
	"Medida de control\r\nNominal: %-5.2fmm Real: %-5.2fmm    "
};

char const vtextkp12Er[4][75]=
{
	"Kontrollmass\r\nSoll: %-5.2fmm  Ist:   NA    \n\r",
	//2017/2/6 10:00:44-------------------
//"Control gauge\r\nNominal: %-5.2fmm Actual:   NA    \n\r",
	"Control gauge\r\nNominal: %-5.2fmm Actual:   NA\n\r",
	
	"Mesure de controle\r\nNominal: %-5.2fmm Actuel:   NA    \n\r",
	"Medida de control\r\nNominal: %-5.2fmm Real:   NA    \n\r"
};

char const vtextkp14[4][80]=
{
	"letztes Kalibrierdat.:%s\r\nletzte Kalibrierung mit: %3d N\n\n\r",
	"last calibration date:%s\r\nlast calibration with: %3d N\n\n\r",
	"derniere date calibr.:%s\r\nderniere calibrat. avec: %3d N\n\n\r",
	"Ultima fecha calibra.:%s\r\nultima calibr. con: %3d N\n\n\r"
};

char const vtextkp14j[4][80]=
{
	"letztes Justierdatum:%s\r\nletzte Kalibrierung mit: %3d N\n\n\r",
	"last adjustment date:%s\r\nlast calibration with: %3d N\n\n\r",
	"derniere date ajustage:%s\r\nderniere calibrat. avec: %3d N\n\n\r",
	"Ultima fecha ajuste:%s\r\nultima calibr. con: %3d N\n\n\r"
};

// 2016-8-3 星期三 14:51:24-------------------------------------------
/*
char const vtextkp15[4][90]=
{
	"Istwert:%3dN\r\nNewtonfaktor:%4.2f\n\r",
	"Actual :%3dN\r\nNewtonfactor:%4.2f\n\r",
	"Actuel :%3dN\r\nfacteur Nwt.:%4.2f\n\r",
	"Real : %3dN\r\n factor Newton: %4.2f\n\r"
};
*/
char const vtextkp15[4][90]=
{
	"Newtonfaktor:%4.2f\n\r",
	"Newtonfactor:%4.2f\n\r",
	"facteur Nwt.:%4.2f\n\r",
	" factor Newton: %4.2f\n\r"
};

char const vtextkp14a[4][80]= 
{
	"letztes Kontrolldatum:%s\n\r",
	"last control date:%s\n\r",
	"derniere date de controle:%s\n\r",
	"Ultima fecha de control:%s\n\r"
};


char const vtextkp14a12[4][80]=
{
	"letztes Justierdatum:%s\n\r",
	"last adjustment date:%s\n\r",
	"derniere date de adjuste:%s \n\r",
	"Ultima fecha de ajuste: %s  \n\r"
};

char const vtextkp14a1[4][80]=
{
	"letztes Kalibrierdat.:%s\n\r",
	"last calibration date:%s\n\r",
	"derniere date de calib%s\n\r",
	"Ultima fecha de calib:%s\n\r"
};

char const vtextkp14a11[4][80]=
{
	"letzte \232berpr\201fung:%s\n\n\r",
	"last instrument check:%s\n\r",
	"derniere date de contr.:%s\n\r",
	"Ultima fecha de control:%s\n\r"
};

char const vtextkp141[4][80]=
{
	"letztes Kalibrierdat.:%s\r\nletzte Kalibrierung mit:%4.1f kg\n\r",
	"last calibration date:%s\r\nlast calibration with: %4.1f kg\n\r",
	"derniere date calibr:%s\r\nderniere calibrat.avec: %4.1f kg\n\r",
	"Ultima fecha calib.: %s\r\nultima calibr. con: %4.1f kg\n\r"
};

char const vtextkp141j[4][80]=
{
	"letztes Justierdatum:%s\r\nletzte Justierung mit:%4.1f kg\n\r",
	"last adjustment date:%s\r\nlast adjustment with: %4.1f kg\n\r",
	"derniere date de ajustage:%s\r\nderniere ajustage avec: %4.1f kg\n\r",
	"Ultima fecha de ajuste: %s\r\nultima ajuste con: %4.1f kg\n\r"
};

// 2016-8-3 星期三 14:53:07-------------------------------------------
/*
char const vtextkp151[4][90]=
{
	"Istwert: %4.1fkg\r\nNewtonfaktor: %4.2f\n\r",
	"Actual : %4.1fkg\r\nNewtonfactor: %4.2f\n\r",
	"Actuel : %4.1fkg\r\nfacteur Nwt.: %4.2f\n\r",
	"Real : %4.1fkg\r\nfactor Newton: %4.2f\n\r"
};
*/
char const vtextkp151[4][90]=
{
	"Newtonfaktor: %4.2f\n\r",
	"Newtonfactor: %4.2f\n\r",
	"facteur Nwt.: %4.2f\n\r",
	"factor Newton: %4.2f\n\r"
};

// 2017/1/13 14:12:34
char const vtextkp15a[4][60]=
{
	"mit Kalibrator Nr.: \r\n%s\r\nDose Nr.: %s\n\r", 
	"with Calibrator No.: \r\n%s\r\nGauge No.: %s \n\r",
	"avev Calibrateur No.: \r\n%s\r\n Dose Nr.: %s \n\r",
	"con nr. de Calibrador: \r\n%s\r\n Nr. Celda : %s\n\r"
};


char const vtextkp16[4][90]=
{
	"Kalibrierkraft\r\nSollwert: %3.0fN Ist: %3.0fN\n\r",
	"Calibrationforce\r\nNominal: %3.0f N Act.: %3.0fN\n\r",
	"Poids de calibr.\r\n Nominal: %3.0f N Act.: %3.0fN\n\r",
	"Peso de calibracion\r\n     : %3.0f N Real: %3.0fN\n\r"
};

char const vtextkp16j[4][90]=
{
	"Justierkraft\r\nSollwert: %3.0fN Ist: %3.0fN\n\r",
	"Adjustmentforce\r\n Nominal: %3.0f N Act.: %3.0fN\n\r",
	"Poids de ajustage\r\nNominal: %3.0f N Act.: %3.0fN\n\r",
	"Peso de ajuste\r\n      : %3.0f N Real: %3.0fN\n\r"
};

char const vtextkp161[4][90]=
{
	"Kalibriergewicht\r\nSoll: %4.1fkg Istwert: %4.1fkg\n\r",
	"Calibrationweight\r\nNominal: %4.1fkg Actual:%4.1fkg\n\r",
	"Poids de calibr.\r\nNominal: %4.1fkg Actuel:%4.1fkg\n\r",
	"Peso de calibracion      : %4.1fkg Real  :%4.1fkg\n\r"
};

char const vtextkp161j[4][90]=
{
	"Justiergewicht Soll: %4.1fkg Istwert: %4.1fkg\n\r",
	"Adjustmentweight\r\nNominal: %4.1fkg Actual:%4.1fkg\n\r",
	"Poids de ajustage\r\nNominal: %4.1fkg Actuel:%4.1fkg\n\r",
	"Peso de ajuste\r\n       : %4.1fkg Real  :%4.1fkg\n\r"
};

char const vtextkp163[4][90]=
{
	"neuer Newtonfaktor: %4.2f\n\r",
	"Newtonfactor new: %4.2f\n\r",
	"nouveau facteur Newton: %4.2f\n\r",
	"factor Newton nuevo: %4.2f\n\r"
};

char const vtextkp13[4][70]=
{
	"\n         BRUCHFESTIGKEIT\n\r",
	"\n             HARDNESS\n\r",
	"\n         TESTEUR DE DURETE\n\r",
	"\n             DUREZA\n\r"
};

char const vtextkp17[4][60]=
{
	"\n          DURCHMESSER\n\r",
	"\n             DIAMETER\n\r",
	"\n            DIAMETRE\n\r",
	"\n             DIAMETRO\n\r"
};

char const vtextkp17b[4][60]=
{
	"\n      DURCHMESSER/DICKE\n\r",
	"\n        DIAMETER/THICKNESS\n\r",
	"\n        DIAMETRE/EPAISSEUR\n\r",
	"\n        DIAMETRO/ESPESOR\n\r"
};

char const vtextkp18[4][80]=
{
	"letztes Kalibrierdat.:%s\r\nletzte Kalibrierung mit: %5.2fmm\n\r",
	"last calibration date:%s\r\nlast calibration with: %5.2fmm\n\r",
	"derniere date calibr.:%s\r\ndern. calibrage avec : %5.2fmm\n\r",
	"ultima fecha calibra.:%s\r\nultima calibracion con:%5.2fmm\n\r"
};

char const vtextkp18j[4][80]=
{
	"letztes Justierdatum:%s\r\nletzte Justierung mit: %5.2fmm\n\r",
	"last adjustment date:%s\r\nlast adjustment with: %5.2fmm\n\r",
	"derniere date ajust.:%s\r\ndern. ajusteage avec : %5.2fmm\n\r",
	"ultima fecha ajuste: %s\r\nultima ajuste con:%5.2fmm\n\r"
};

char const vtextkp19[4][60]=
{
	"Kalibriermass: Soll: %5.2fmm\n\r",
	
	//2017/2/6 9:44:41------------------------
//"Calibration gauge: Nominal:%5.2fmm\n\r",
	"Calibr. gauge: Nominal:%5.2fmm\n\r",
	
	"Mesure calibrage:Nominal:%5.2fmm\n\r",
	"Medida calibracion:Nominal:%5.2fmm\n\r"
};

char const vtextkp19j[4][60]=
{
	"Justiermass: Soll: %5.2fmm\n\r",
	"Adjustment gauge: Nominal: %5.2fmm\n\r",
	"Mesure de ajustage: Nominal: %5.2fmm\n\r",
	"Medida de ajuste: Nominal: %5.2fmm\n\r"
};

char const vtextkp50[4][40]=
{
	"Kalibrierdatum: %-8s\n\r",
	"Calibration date: %-8s\n\r",
	"Date de calibration: %-8s\n\r",
	"Fecha de calibracion: %-8s\n\r"
};

char const vtextkp50j[4][40]=
{
	"Justierdatum: %-8s\n\r",
	"Adjustment date: %-8s\n\r",
	"Date de ajustage: %-8s\n\r",
	"Fecha de ajuste: %-8s\n\r"
};

char const vtextkp51[4][90]=
{
	"\n\n\n\n\nUnterschrift:...................\n\n\n\n\n\r",
	"\n\n\n\n\nSignature:......................\n\n\n\n\n\r",
	"\n\n\n\n\nSignature:......................\n\n\n\n\n\r",
	"\n\n\n\n\nFirma:..........................\n\n\n\n\n\r"
};

char const vtextkp50a[4][80]=
{
	"Kontrolldatum: %s\n\r",
	"Control date:%s  \n\r",
	"Date de control.:%s \n\r",
	"Fecha de control: %s  \n\r"
};

// 台式打印机
//----------------------------------------------------------------
int druckkp(void) 											// 调整，校验报告
{
	char aktdat[10],akttim[10],snr[8];
	static char buf[120];
	int i;
	float hil,hil1;
	DTIM dat;

	prLang=prTab[aktLanguage];

	getdatstr(aktdat);
	gettimstr(akttim);
	if(DatFormat==0) 																// 日期格式
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 加载上次校验日期
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 加载本次校验日期
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
	}
	else 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year); 			// 上一次硬度校正日期
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year); 			// 上一次直径或厚度校正日期
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
	}
	
	// 加载硬度校验U盘工具ID------------------------
	LoadKalID(kaliverso,ADRKALIVERSO); 							// 上次
	LoadKalID(kalivers,ADRKALIVERS); 								// 本次
	LoadDosID(dosnro,ADRDOSNRO); 										// 上次
	LoadDosID(dosnr,ADRDOSNR); 											// 本次

	if(bkal) 																				// 硬度校验，调整+检查
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew =readinteep(ADRLBKALO);
		lasbist =readinteep(ADRLBIKALO);
		sollwert=readinteep(ADRLBKAL);
		iistwert=readinteep(ADRLBIKAL);
		
		laskmod=readinteep(ADRLASKMOD);
		kalimode=readinteep(ADRKALIMODE);
	}
	else 																						// 本次只做检查
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKAL);
		lasbist=readinteep(ADRLBIKAL);
		
		laskmod=readinteep(ADRLASKMOD); 							// 2017/1/13 13:43:10
		kalimode=readinteep(ADRKALIMODE);
	}
	loadfloat(&lasdiam,ADRLASDIAM);

	if((i=StartPrinter())!=0) return(i);
	
	PrintBold();
	if(terminologie)
	{
		if(vers[23]=='E') 
		{
			SendPrinterCC(textkp1a2[prLang]); 					// ERWEKA TBH125 ADJUSTMENT
		}
		if(vers[23]=='C') 
		{
			SendPrinterCC(textkp1a2c[prLang]);
		}
		if(vers[23]=='Q') 
		{
			SendPrinterCC(textkp1a2q[prLang]);
		}
	}
	else 
	{
		if(vers[23]=='E') 
		{
			SendPrinterCC(textkp1a1[prLang]); 					// ERWEKA TBH125 CALIBRATION
		}
		if(vers[23]=='C') 
		{
			SendPrinterCC(textkp1a1c[prLang]);
		}
		if(vers[23]=='Q') 
		{
			SendPrinterCC(textkp1a1q[prLang]);
		}
	}
	
	PrintNorm();
	for(i=0;i<6;i++) 
	{
		snr[i]=vers[58+i];
		snr[6]=0;
	};

	sprintf(buf,textkp2[prLang],id,snr);
	SendPrinter(buf);
	sprintf(buf,textkp3[prLang],aktdat,akttim);
	SendPrinter(buf);
	SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");

	PrintBold();
	SendPrinterCC(textkp13[prLang]); 			// 打印硬度标题，HARDNESS
	PrintNorm();
	PrintCondensed();
	
	// 上一次硬度校正信息---------------------
	if(bueb) 															// 本次硬度校验完成，调整+检查
	{
		if(laskmod==1) 											// 上一次硬度校验模式，U盘
		{
			// 上一次硬度校验日期和数据
			if(!terminologie) 								// CALI
			{
				sprintf(buf,textkp14[prLang],jusdatbo,lasbgew); 		// last calibration date:%s      last calibration with: %3d N\n\r
			}
			else 															// ADJU
			{
				sprintf(buf,textkp14j[prLang],jusdatbo,lasbgew); 		// last adjustment date:%s      last adjustment with: %3d N\n\r
			}
			SendPrinter(buf);
			
			// 上一次硬度校验参数
			sprintf(buf,textkp15[prLang],lasbnwt); 								//
			SendPrinter(buf);
			
			// 上一次硬度校验U盘编码
			sprintf(buf,textkp15a[prLang],kaliverso,dosnro); 			// 
			SendPrinter(buf);
		}
		else 																// 砝码
		{
			hil=(float)lasbgew;
			hil/=9.81f;
			
			// 上一次硬度校验日期和数据
			if(!terminologie)
			{
				sprintf(buf,textkp141[prLang],jusdatbo,hil);
			}
			else
			{
				sprintf(buf,textkp141j[prLang],jusdatbo,hil);
			}
			SendPrinter(buf);
			
			// 上一次硬度校验参数
			sprintf(buf,textkp151[prLang],lasbnwt);
			SendPrinter(buf);
		}
	}
	else 																	// 未完成
	{
		if(kalimode==1) 										// U盘
		{
			// 上一次硬度校验日期和数据
			if(!terminologie) 								// CALI
			{
				sprintf(buf,textkp14[prLang],jusdatb,lasbgew); 		// last calibration date:%s      last calibration with: %3d N\n\r
			}
			else 															// ADJU
			{
				sprintf(buf,textkp14j[prLang],jusdatb,lasbgew); 		// last adjustment date:%s      last adjustment with: %3d N\n\r
			}
			SendPrinter(buf);
			
			// 上一次硬度校验参数
			sprintf(buf,textkp15[prLang],lasbnwt); 								//
			SendPrinter(buf);
			
			// 上一次硬度校验U盘编码
			sprintf(buf,textkp15a[prLang],kalivers,dosnr);
			SendPrinter(buf);
		}
		else 																// 砝码
		{
			hil=(float)lasbgew;
			hil/=9.81f;
			
			if(!terminologie)
			{
				sprintf(buf,textkp141[prLang],jusdatb,hil);
			}
			else
			{
				sprintf(buf,textkp141j[prLang],jusdatb,hil);
			}
			SendPrinter(buf);
			
			// 上一次硬度校验参数
			sprintf(buf,textkp151[prLang],lasbnwt);
			SendPrinter(buf);
		}
	}
	
	/*
	if(kalimode==1)
		//sprintf(buf,textkp15[prLang],lasbist,lasbnwt);
		sprintf(buf,textkp15[prLang],lasbnwt); 									// 2016-8-3 星期三 14:55:47
	else 
	{
		hil=(float)lasbist;
		hil/=9.81;
		if(bkal)
			//sprintf(buf,textkp151[prLang],hil,lasbnwt);
			sprintf(buf,textkp151[prLang],lasbnwt); 							// 2016-8-3 星期三 14:57:45
		else
			//sprintf(buf,textkp151[prLang],hil,NwtFak);
			sprintf(buf,textkp151[prLang],NwtFak); 								// 2016-8-3 星期三 14:58:48
	}
	SendPrinter(buf);
	
	if(laskmod) 																							// 上一次检测模式
	{
		sprintf(buf,textkp15a[prLang],kaliverso,dosnro);
		SendPrinter(buf);
	}
	*/
	
	PrintNorm();
	//if(bkal) 																			// 感觉标志用错了；2016/11/14 20:56:46
	if(bueb) 																				// 1，硬度校验完成；0，未完成
	{
		PrintBold();
		if(terminologie)
			SendPrinterCC(textkp61[prLang]); 						// Adjustment NEW
		else
			SendPrinterCC(textkp6[prLang]); 						// Calibration NEW
		
		PrintNorm();
		if (kalimode==1) 										// 硬度校正模式，0：砝码校正模式；1：自动校正模式（U盘）
		{
			sprintf(buf,textkp15a[prLang],kalivers,dosnr);
			SendPrinter(buf);
		}	
	
		sprintf(buf,textkp50[prLang],eidatb);
		SendPrinter(buf);
		
		if(kalimode==1) 
		{
			if(!terminologie)
				sprintf(buf,textkp16[prLang],sollwert,iistwert);
			else
				sprintf(buf,textkp16j[prLang],sollwert,iistwert);

			SendPrinter(buf);
			sprintf(buf,textkp163[prLang],NwtFak);
		}
		else 																// 砝码
		{
			hil=(float)sollwert;
			hil1=(float)iistwert;
			hil/=9.81f;
			hil1/=9.81f;
			if(!terminologie)
				sprintf(buf,textkp161[prLang],hil,hil1);
			else
				sprintf(buf,textkp161j[prLang],hil,hil1);

			SendPrinter(buf);
			sprintf(buf,textkp163[prLang],NwtFak);
		}
		
		SendPrinter(buf);
	}
	
	SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");
	
	// 直径/厚度校正
	//----------------------------------------------
	if(dmena) 																			// 检测直径允许，设备参数
	{
		PrintBold();
		
		if(dickena) 																	// 选择标致，
			SendPrinterCC(textkp17b[prLang]); 					// 打印标题，DIAMETER/THICKNESS
		else
			SendPrinterCC(textkp17[prLang]); 						// 打印标题，DIAMETER
		
		PrintNorm();
		PrintCondensed();
		
		if(dueb) 																			// 1，直径校验完成；0，未完成
		{
			if(!terminologie)
				sprintf(buf,textkp18[prLang],jusdatdo,lasdiam); // "       last calibration date   : %s last calibration with: %5.2fmm\n\r"
			else
				sprintf(buf,textkp18j[prLang],jusdatdo,lasdiam); // "       last adjustment date   : %s last adjustment with: %5.2fmm\n\r"
			
			SendPrinter(buf);
		}
		else
		{
			//SendPrinterCC("       DIAM CALI NOFINISH!\n\r"); 
			if(!terminologie)
				sprintf(buf,textkp18[prLang],jusdatd,lasdiam); // "       last calibration date   : %s last calibration with: %5.2fmm\n\r"
			else
				sprintf(buf,textkp18j[prLang],jusdatd,lasdiam); // "       last adjustment date   : %s last adjustment with: %5.2fmm\n\r"
			
			SendPrinter(buf);
		}
		
		//SendPrinter(buf);
		PrintNorm();
		//if(dkal) 																			// 感觉标志用错，2016/11/14 20:58:49
		if(dueb) 																			// 1，直径校验完成；0，未完成
		{
			PrintBold();
			
			if(terminologie)
				SendPrinterCC(textkp61[prLang]); 					// Adjustment NEW
			else
				SendPrinterCC(textkp6[prLang]); 					// Calibration NEW
			
			PrintNorm();
			sprintf(buf,textkp50[prLang],jusdatd); 			// Calibration date: %-8s\n\r
			SendPrinter(buf);
			
			if(!terminologie)
				sprintf(buf,textkp19[prLang],eichmas); 		// Calibration gauge:         Nominal: %5.2fmm\n\r
			else
				sprintf(buf,textkp19j[prLang],eichmas); 	// Adjustment gauge:        Nominal: %5.2fmm\n\r
			
			SendPrinter(buf);
			lasdiam=eichmas;
			savefloat(&lasdiam,ADRLASDIAM);
		}
		
		SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");
	}
	
	SendPrinterCC(textkp51[prLang]); 			// Signature:......
	StopPrinter();
	return(0);
}

int druckuep(void) 											// 检查报告
{
	char aktdat[10],akttim[10],snr[8];
	static char buf[120];
	int i;
	float hil;
	DTIM dat;
	
	int i_sollb, i_istb; 									// 整型硬度临时变量，2019-07-27 星期六 10:52:19
	
	int i_solld = 0; 											// 整型临时标准量, 170603-101
	int i_istd = 0; 											// 整型临时检测量, 170603-101
	int i_hil = 0; 												// 整型临时误差量, 170603-101
	
	prLang=prTab[aktLanguage];

	getdatstr(aktdat);
	gettimstr(akttim);
	
	if(DatFormat==0) 											// 日期格式
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 上次日期
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 本次日期
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
	}
	else 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
	}
	
	// 硬度检测U盘工具ID------------------
	LoadKalID(kaliverso,ADRKALIVERSO); 		// 上次
	LoadKalID(kalivers,ADRKALIVERS); 			// 本次
	LoadDosID(dosnro,ADRDOSNRO);
	LoadDosID(dosnr,ADRDOSNR);

	/*if(bkal) 																				// 硬度校验，调整+检查，2021-09-07 星期二 16:30:55
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew =readinteep(ADRLBKALO);
		lasbist =readinteep(ADRLBIKALO);
		sollwert=readinteep(ADRLBKAL);
		iistwert=readinteep(ADRLBIKAL);
		
		laskmod=readinteep(ADRLASKMOD);
		kalimode=readinteep(ADRKALIMODE);
	}
	else 																						// 本次只做检查
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKAL);
		lasbist=readinteep(ADRLBIKAL);
		
		laskmod=readinteep(ADRLASKMOD); 							// 2017/1/13 13:43:10
		kalimode=readinteep(ADRKALIMODE);
	}
	loadfloat(&lasdiam,ADRLASDIAM);*/
	
	//读取kalimode，判断是U盘还是砝码模式
	kalimode=readinteep(ADRKALIMODE);
	
	// 启动打印机 ----------------------------------
	if((i=StartPrinter())!=0) return(i);

	// 打印报告标题 --------------------------------
	PrintBold();
	if(terminologie)
	{
		if(vers[23]=='E') 
		{
			SendPrinterCC(textkp1a1[prLang]); 				// ERWEKA TBH125 CALIBRATION
		}
		if(vers[23]=='C') 
		{
			SendPrinterCC(textkp1a1c[prLang]);
		}
		if(vers[23]=='Q') 
		{
			SendPrinterCC(textkp1a1q[prLang]);
		}
	}
	else 
	{
		if(vers[23]=='E') 
		{
			SendPrinterCC(textkp1a[prLang]); 					// ERWEKA TBH125 CHECK
		}
		if(vers[23]=='C') 
		{
			SendPrinterCC(textkp1ac[prLang]);
		}
		if(vers[23]=='Q') 
		{
			SendPrinterCC(textkp1aq[prLang]);
		}
	}
	PrintNorm();

	for(i=0;i<6;i++) 
	{
		snr[i]=vers[58+i];
		snr[6]=0;
	};


	sprintf(buf,textkp2[prLang],id,snr);
	SendPrinter(buf);
	sprintf(buf,textkp3[prLang],aktdat,akttim);
	SendPrinter(buf);
	SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");

	PrintBold(); 																		// 加黑字体
	SendPrinterCC(textkp13[prLang]); 								// HARDNESS，硬度检测
	
	PrintNorm(); 																		// 标准字体
	// bkal：1，硬度校验模式；0，检查模式
	if(bkal) 																				// 硬度校验模式
	{
		if(bueb) 																			// 1，硬度检查完成；0，未完成
		{
			if(terminologie==0) 												// CALI
			{
				sprintf(buf,textkp14a1[prLang],jusdatbo); 	// last calibration date: %s  \n\r
				SendPrinter(buf);
				sprintf(buf,textkp14a11[prLang],eidatbo); 	// last instrument check: %s  \n\r
				SendPrinter(buf);
			}
			else 																				// ADJU
			{
				sprintf(buf,textkp14a12[prLang],jusdatbo); 	// last adjustment date:  %s  \n\r
				SendPrinter(buf);
				sprintf(buf,textkp14a1[prLang],eidatbo); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
			}
			
			// 206-4-1，Gavin，2019-07-30 星期二 14:11:37
			/*
			if(kalimode==1)  														// 硬度校正模式，0：砝码；1：U盘
			{
				sprintf(buf,textkp15a[prLang],kaliverso,dosnro);
				SendPrinter(buf);
			}
			*/
			SendPrinter("\r\n");
		}
		else 																					// 硬度校验未完成
		{
			if(terminologie==0) 												// CALI
			{
				sprintf(buf,textkp14a1[prLang],jusdatb); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
				sprintf(buf,textkp14a11[prLang],eidatb); 		// last instrument check: %s  \n\r
				SendPrinter(buf);
			}
			else 																				// ADJU
			{
				sprintf(buf,textkp14a12[prLang],jusdatb); 	// last adjustment date:  %s  \n\r
				SendPrinter(buf);
				sprintf(buf,textkp14a1[prLang],eidatb); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
			}
			
			if(kalimode==1)  														// 硬度校正模式，0：砝码；1：U盘
			{
				sprintf(buf,textkp15a[prLang],kalivers,dosnr);
				SendPrinter(buf);
			}
			
			SendPrinter("\r\n");
		}
	}
	else 																						// 硬度检查模式
	{
		if(bueb) 																			// 1，硬度检查完成；0，未完成
		{
			if(terminologie==0) 												// CALI
			{
				sprintf(buf,textkp14a1[prLang],jusdatb); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
				
				sprintf(buf,textkp14a11[prLang],eidatbo); 	// last instrument check: %s  \n\r
				SendPrinter(buf);
			}
			else 																				// ADJU
			{
				sprintf(buf,textkp14a12[prLang],jusdatb); 	// last adjustment date:  %s  \n\r
				SendPrinter(buf);
				
				sprintf(buf,textkp14a1[prLang],eidatbo); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
			}
			
			// 检查报告不再打印U盘ID号，Gavin，2019-07-26 星期五 14:40:02
			/*
			if(kalimode==1)  														// 硬度校正模式，0：砝码；1：U盘
			{
				sprintf(buf,textkp15a[prLang],kaliverso,dosnro);
				SendPrinter(buf);
			}
			*/
			SendPrinter("\r\n");
		}
		else 																					// 硬度检查未完成
		{
			if(terminologie==0) 												// CALI
			{
				sprintf(buf,textkp14a1[prLang],jusdatb); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
				
				sprintf(buf,textkp14a11[prLang],eidatb); 		// last instrument check: %s  \n\r
				SendPrinter(buf);
			}
			else 																				// ADJU
			{
				sprintf(buf,textkp14a12[prLang],jusdatb); 	// last adjustment date:  %s  \n\r
				SendPrinter(buf);
				sprintf(buf,textkp14a1[prLang],eidatb); 		// last calibration date: %s  \n\r
				SendPrinter(buf);
			}
			
			// 检查报告不再打印U盘ID号，Gavin，2019-07-26 星期五 14:40:02
			/*
			if(kalimode==1)  														// 硬度校正模式，0：砝码；1：U盘
			{
				sprintf(buf,textkp15a[prLang],kalivers,dosnr);
				SendPrinter(buf);
			}
			*/
			SendPrinter("\r\n");
		}
	}
	
	if(bueb) 																				// 1，硬度检查完成；0，未完成
	{
		PrintBold();
		if(terminologie) 															// 校验
		{
			SendPrinterCC(textkp6[prLang]); 						// Calibration NEW
			PrintNorm();
			sprintf(buf,textkp50[prLang],eidatb); 			// Calibration date: %-8s\n\r
			SendPrinter(buf);
		}
		else 																					// 检查
		{
			SendPrinterCC(textkp6a[prLang]); 						// Check NEW
			PrintNorm();
			
			// 206-3-1,2019-07-26 星期五 11:30:12
			if (kalimode==1) 					// 硬度校正模式，0：砝码校正模式；1：自动校正模式（U盘）
			{
				sprintf(buf,textkp15a[prLang],kalivers,dosnr);
				SendPrinter(buf);
			}	
			
			sprintf(buf,textkp50a[prLang],eidatb); 			// Control date:%s  \n\r
			SendPrinter(buf);
		}

		for(i=0;i<3;i++) 
		{
			// 206-5，Gavin，2019-07-27 星期六 10:29:53
			/*
			if(sollb[i]>istb[i]) 
				hil=sollb[i]-istb[i];
			else 
				hil=istb[i]-sollb[i];
			*/
			
			if(kalimode) 											// U盘模式
			{
				// 206-5, Gavin, 2019-07-27 星期六 11:12:42
				//i_sollb = sollb[i];
				//i_istb = istb[i];
				i_sollb = sollb[i]+0.5f; 				// +0.5后取整，模拟四舍五入
				i_istb = istb[i]+0.5f; 					// v2.08，Gavin，2019-11-07 星期四 18:18:38
				
				if(i_sollb > i_istb)
					hil = i_sollb - i_istb;
				else
					hil = i_istb - i_sollb;
				
				hil=round0(hil);
				sprintf(buf,textkp8a[prLang],sollb[i],istb[i]);
				SendPrinter(buf);
				if(hil>1.0001f)
				{
					PrintBold();
					SendPrinter("Out of range!\n\r");
					PrintNorm();
				}
				else 
				{
					SendPrinter("OK\n\r");
				}
			}
			else 															// 砝码模式
			{
				// 206-5, Gavin, 2019-07-27 星期六 11:12:42
				if(sollb[i]>istb[i]) 
					hil=sollb[i]-istb[i];
				else 
					hil=istb[i]-sollb[i];
				
				sprintf(buf,textkp8a1[prLang],sollb[i],istb[i]);
				SendPrinter(buf);
				hil=round1(hil);
				if(hil>0.1001f) 
				{
					PrintBold();
					SendPrinter("Out of range!\n\r");
					PrintNorm();
				}
				else 
				{
					SendPrinter("OK\n\r");
				}
			}
		}
	}
	SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");
	
	// 直径/厚度检查报告Check
	//----------------------------------------------
	if(dmena) 																			// 直径检查允许
	{
		PrintBold();
		if(dickena)
			SendPrinterCC(textkp17b[prLang]); 					// 打印标题，DIAMETER/THICKNESS
		else
			SendPrinterCC(textkp17[prLang]); 						// 打印标题，DIAMETER
		
		PrintNorm();
		// dkal：1，直径校验模式；0，检查模式
		if(dkal) 																			// 直径校验模式
		{
			// dueb: 1，直径检查完成；0，未完成
			if(dueb) 																		// 1，直径检查完成；
			{
				if(terminologie==0) 											// CALI
				{
					sprintf(buf,textkp14a1[prLang],jusdatdo); 				// last calibration date: %s  \n\r
					SendPrinter(buf);
					sprintf(buf,textkp14a11[prLang],eidatdo); 				// last instrument check: %s  \n\r
					SendPrinter(buf);
				}
				else 																			// ADJU
				{
					sprintf(buf,textkp14a12[prLang],jusdatdo);
					SendPrinter(buf);
					sprintf(buf,textkp14a1[prLang],eidatdo);
					SendPrinter(buf);
				}
			}
			else 																				// 直径校验未完成
			{
				if(terminologie==0) 											// CALI
				{
					sprintf(buf,textkp14a1[prLang],jusdatd); 				// last calibration date: %s  \n\r
					SendPrinter(buf);
					sprintf(buf,textkp14a11[prLang],eidatd); 				// last instrument check: %s  \n\r
					SendPrinter(buf);
				}
				else 																			// ADJU
				{
					sprintf(buf,textkp14a12[prLang],jusdatd);
					SendPrinter(buf);
					sprintf(buf,textkp14a1[prLang],eidatd);
					SendPrinter(buf);
				}
			}
		}
		else 																					// 直径检查模式
		{
			// dueb: 1，直径检查完成；0，未完成
			if(dueb) 																		// 1，直径检查完成；
			{
				if(terminologie==0) 											// CALI
				{
					sprintf(buf,textkp14a1[prLang],jusdatd); 				// last calibration date: %s  \n\r
					SendPrinter(buf);
					sprintf(buf,textkp14a11[prLang],eidatdo); 				// last instrument check: %s  \n\r
					SendPrinter(buf);
				}
				else 																			// ADJU
				{
					sprintf(buf,textkp14a12[prLang],jusdatd);
					SendPrinter(buf);
					sprintf(buf,textkp14a1[prLang],eidatdo);
					SendPrinter(buf);
				}
			}
			else 																				// 直径检查未完成
			{
				if(terminologie==0) 											// CALI
				{
					sprintf(buf,textkp14a1[prLang],jusdatd); 				// last calibration date: %s  \n\r
					SendPrinter(buf);
					sprintf(buf,textkp14a11[prLang],eidatd); 				// last instrument check: %s  \n\r
					SendPrinter(buf);
				}
				else 																			// ADJU
				{
					sprintf(buf,textkp14a12[prLang],jusdatd);
					SendPrinter(buf);
					sprintf(buf,textkp14a1[prLang],eidatd);
					SendPrinter(buf);
				}
			}
		}
		SendPrinter("\r\n");
		
		if(dueb) 																			// 1，直径检查完成；0，未完成
		{
			PrintBold();
			if(terminologie) 														// 校验
			{
				SendPrinterCC(textkp6[prLang]); 					// Calibration NEW
				PrintNorm();
				sprintf(buf,textkp50[prLang],eidatd); 		// Calibration date: %-8s\n\r
				SendPrinter(buf);
			}
			else 																				// 检查
			{
				SendPrinterCC(textkp6a[prLang]); 					// Check NEW
				PrintNorm();
				sprintf(buf,textkp50a[prLang],eidatd); 		// Control date:%s  \n\r
				SendPrinter(buf);
			}
			
			for(i=0;i<3;i++)
			{
				// 170603-102, 自编四舍五入
				i_solld = solld[i]*1000;
				i_solld = (i_solld+5)/10;
				solld[i] = (float)(i_solld)/100;
				
				i_istd = istd[i]*1000;
				i_istd = (i_istd+5)/10;
				istd[i] = (float)(i_istd)/100;
				
				if(i_solld>i_istd) 
					i_hil = i_solld - i_istd; 				// 标准量大于检测量
				else 
					i_hil = i_istd - i_solld; 				// 标准量小于等于检测量
				// 170603-102, End
				
				if(istd[i]<90000) 
				{
					sprintf(buf,textkp12[prLang],solld[i],istd[i]);
					SendPrinter(buf);
					
					// 170603-103
					//if(hil > 0.05)
					if(i_hil > 5) 								// 与5（0.05）比较
					{
						PrintBold(); 								// 加黑字体
						SendPrinter("Out of range!\n\r");
						PrintNorm(); 								// 标准字体
					}
					else 
					{				
						SendPrinter("OK\n\r");
					}
				}
				else 
				{
					sprintf(buf,textkp12Er[prLang],solld[i]);
					SendPrinter(buf);
				}
			}
		}
		SendPrinter("       \xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\n\r");
	}
	SendPrinterCC(textkp51[prLang]); 								// Signature:......
	StopPrinter();
	return(0);
}
// End 台式打印机
//-------------------------------------------------------------------

// 微型打印机
//-------------------------------------------------------------------
int druckkp32(void) 										// 校验报告
{
	char aktdat[10],akttim[10];  //,snr[8];
	//static char buf[120];
	int i;
	//float hil,hil1;
	//int lasbgew,lasbist;
	DTIM dat;
	prLang=prTab[aktLanguage];

	getdatstr(aktdat);
	gettimstr(akttim);
	if(DatFormat==0) 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 上次日期
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 本次日期
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
	}
	else 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
	}
	
	// 硬度校验U盘工具ID------------------
	LoadKalID(kaliverso,ADRKALIVERSO); 		// 上次
	LoadKalID(kalivers,ADRKALIVERS); 			// 本次
	LoadDosID(dosnro,ADRDOSNRO);
	LoadDosID(dosnr,ADRDOSNR);

	if(bkal) 															// 执行硬度校验
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKALO);
		lasbist=readinteep(ADRLBIKALO);
		sollwert=readinteep(ADRLBKAL);
		iistwert=readinteep(ADRLBIKAL);
		laskmod=readinteep(ADRLASKMOD);
		kalimode=readinteep(ADRKALIMODE);
	}
	else 																	// 未执行硬度校验
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKAL);
		lasbist=readinteep(ADRLBIKAL);
		
		//kalimode=readinteep(ADRKALIMODE); 					// 2016/12/8 9:33:35
		
		laskmod=readinteep(ADRLASKMOD); 							// 2017/1/13 13:44:25
		kalimode=readinteep(ADRKALIMODE);
	}
	
	loadfloat(&lasdiam,ADRLASDIAM);
	
	if(StartPrinter()) 
		return(1);
/*#ifdef CHINAPRINTER
	for(i=20;i>=0;i--) {
		printkpText(i);
	}
#else*/
	for(i=0;i<21;i++) 
	{
		printkpText(i);
	}
//#endif

	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	
	if(dkal) 
	{
		lasdiam=eichmas;
		savefloat(&lasdiam,ADRLASDIAM);
	}
	
	StopPrinter();
	return(0);
}

void printkpText(int txtnr)
{
	char aktdat[10],akttim[10],snr[8];
	static char buf[120];
	int i;
	float hil,hil1;

	switch(txtnr) 
	{
		case 0:
		{
			PrintBold();
			if(terminologie)
			{
				if(vers[23]=='E') 
				{
					SendPrinterCC(vtextkp1a2[prLang]); 			// ERWEKA TBH125 ADJUSTMENT(调整)
				}
				if(vers[23]=='C') 
				{
					SendPrinterCC(vtextkp1a2c[prLang]); 		// QTEST THT125 ADJUSTMENT
				}
				if(vers[23]=='Q') 
				{
					SendPrinterCC(vtextkp1a2q[prLang]); 		// QTEST THT125 ADJUSTMENT
				}
			}
			else 
			{
				if(vers[23]=='E') 
				{
					SendPrinterCC(vtextkp1a1[prLang]); 			// ERWEKA TBH125 CALIBRATION(校准)
				}
				if(vers[23]=='C') 
				{
					SendPrinterCC(vtextkp1a1c[prLang]); 		// CALEVA THT15 CALIBRATION
				}
				if(vers[23]=='Q') 
				{
					SendPrinterCC(vtextkp1a1q[prLang]); 		// QTEST THT125 CALIBRATION
				}
			}
			
			PrintNorm();
		}break;
		case 1:
		{
			for(i=0;i<6;i++) 
			{
				snr[i]=vers[58+i];
				snr[6]=0;
			}

			sprintf(buf,vtextkp2[prLang],id,snr); 								// Identnummer
			SendPrinter(buf);
		}break;
		case 2:
		{
			getdatstr(aktdat);
			gettimstr(akttim);
			sprintf(buf,vtextkp3[prLang],aktdat,akttim); 					// Datum
			SendPrinter(buf);
		}break;
		case 3:
		{
			SendPrinter("\n\r");
		}break;
		case 4:
		{
			PrintBold();
			SendPrinterCC(vtextkp13[prLang]); 										// BRUCHFESTIGKEIT
			PrintNorm();
		}break;
		case 5:
		{
			PrintCondensed();
			
			// 上一次硬度校正信息---------------------
			if(bueb) 													// 本次硬度校验完成
			{
				if(laskmod==1) 									// 上一次硬度校验模式，U盘
				{
					// 上一次硬度校验日期和数据
					if(!terminologie) 						// CALI
					{
						sprintf(buf,vtextkp14[prLang],jusdatbo,lasbgew); 	// "last calibration date:%s\r\nlast calibration with: %3d N\n\n\r"
					}
					else 													// ADJU
					{
						sprintf(buf,vtextkp14j[prLang],jusdatbo,lasbgew); // "last adjustment date:%s\r\nlast calibration with: %3d N\n\n\r",
					}
				}
				else 														// 砝码校验
				{
					hil=(float)lasbgew;
					hil/=9.81f;
					
					if(!terminologie) 						// CALI
					{
						sprintf(buf,vtextkp141[prLang],jusdatbo,hil);
					}
					else 													// ADJU
					{
						sprintf(buf,vtextkp141j[prLang],jusdatbo,hil);
					}
				}
			}
			else 															// 本次硬度校验未完成
			{
				if(kalimode==1) 								// 上一次硬度校验模式，U盘
				{
					// 上一次硬度校验日期和数据
					if(!terminologie) 						// CALI
					{
						sprintf(buf,vtextkp14[prLang],jusdatb,lasbgew); 	// "last calibration date:%s\r\nlast calibration with: %3d N\n\n\r"
					}
					else 													// ADJU
					{
						sprintf(buf,vtextkp14j[prLang],jusdatb,lasbgew); // "last adjustment date:%s\r\nlast calibration with: %3d N\n\n\r",
					}
				}
				else 														// 砝码校验
				{
					hil=(float)lasbgew;
					hil/=9.81f;
					
					if(!terminologie) 						// CALI
					{
						sprintf(buf,vtextkp141[prLang],jusdatb,hil);
					}
					else 													// ADJU
					{
						sprintf(buf,vtextkp141j[prLang],jusdatb,hil);
					}
				}
			}
			
			/*
			//if (kalimode==1) 
			if (laskmod==1) 																			// 自动校正模式（U盘）
			{
				if(!terminologie)
					sprintf(buf,vtextkp14[prLang],jusdatbo,lasbgew); 	// "last calibration date:%s\r\nlast calibration with: %3d N\n\n\r"
				else
					sprintf(buf,vtextkp14j[prLang],jusdatbo,lasbgew); // "last adjustment date:%s\r\nlast calibration with: %3d N\n\n\r",
			}
			else 
			{
				hil=(float)lasbgew;
				hil/=9.81;
				if(!terminologie)
					sprintf(buf,vtextkp141[prLang],jusdatbo,hil);
				else
					sprintf(buf,vtextkp141j[prLang],jusdatbo,hil);
			}
			*/
			
			SendPrinter(buf);
		}break;
		case 6: 														// 上一次硬度校验参数
		{
			/*
			//if(kalimode==1) 								// 2016/11/17 18:03:32
			if(laskmod==1) 										// U盘
			{
				//sprintf(buf,vtextkp15[prLang],lasbist,lasbnwt);
				sprintf(buf,vtextkp15[prLang],lasbnwt); 						// 2016-8-3 星期三 15:00:13
			}
			else 															// 砝码
			{
				hil=(float)lasbist;
				hil/=9.81;
				if(bkal) 																						// 硬度校验
					//sprintf(buf,vtextkp151[prLang],hil,lasbnwt);
					sprintf(buf,vtextkp151[prLang],lasbnwt); 					// 2016-8-3 星期三 15:01:10
				else 																								// 非硬度校验
					//sprintf(buf,vtextkp151[prLang],hil,NwtFak);
					sprintf(buf,vtextkp151[prLang],NwtFak); 					// 2016-8-3 星期三 15:01:45
			}
			*/
			
			/*
			if(laskmod==1) 										// U盘
			{
				sprintf(buf,vtextkp15[prLang],lasbnwt); 						// 校验参数
			}
			else 															// 砝码
			{
				sprintf(buf,vtextkp151[prLang],lasbnwt); 						// 校验参数
			}
			*/
			
			// 上一次硬度校正信息---------------------
			if(bueb) 													// 本次硬度校验完成
			{
				if(laskmod==1) 									// 上一次硬度校验模式，U盘
				{
					sprintf(buf,vtextkp15[prLang],lasbnwt); 						// 校验参数
				}
				else 														// 砝码
				{
					sprintf(buf,vtextkp151[prLang],lasbnwt); 						// 校验参数
				}
			}
			else
			{
				if(kalimode==1) 								// 上一次硬度校验模式，U盘
				{
					sprintf(buf,vtextkp15[prLang],lasbnwt); 						// 校验参数
				}
				else 														// 砝码
				{
					sprintf(buf,vtextkp151[prLang],lasbnwt); 						// 校验参数
				}
			}
			
			SendPrinter(buf);
		}break;
	  case 7:
		{
			if(bueb) 													// 本次硬度校验完成
			{
				if(laskmod) 																					// 上一次检测模式，U盘
				{
					sprintf(buf,vtextkp15a[prLang],kaliverso,dosnro); 	// "with Calibrator No.: %s\r\nGauge No.: %s \n\r"
					SendPrinter(buf);
				}
			}
			else 															// 未完成
			{
				if(kalimode) 																					// 上一次检测模式，U盘
				{
					sprintf(buf,vtextkp15a[prLang],kalivers,dosnr); 		// "with Calibrator No.: %s\r\nGauge No.: %s \n\r"
					SendPrinter(buf);
				}
			}
			
			PrintNorm();
		}break;
		case 8:
		{
			//if(bkal) 																		// 执行硬度校验
			if(bueb) 																		// 1，硬度校验完成；0，未完成
			{
				PrintBold(); 															// 粗体字形
				if(terminologie)
					SendPrinterCC(vtextkp61[prLang]); 			// "\n\r       Adjustment NEW\n\r"
				else
					SendPrinterCC(vtextkp6[prLang]); 				// "\n\r       Calibration NEW\n\r"
				PrintNorm();
			}
		}break;
		case 9:
		{
			//if(bkal) 																		// 执行硬度校验
			if(bueb) 																		// 1，硬度校验完成；0，未完成
			{
				if (kalimode==1) 													// 当前硬度校验模式是U盘
				{
					sprintf(buf,vtextkp15a[prLang],kalivers,dosnr); 	// "with Calibrator No.: %s\r\nGauge No.: %s \n\r"
					SendPrinter(buf);
				}	
			}
		}break;
		case 10:
		{
			if(bueb) 																		// 1，硬度校验完成；0，未完成
			{
				sprintf(buf,vtextkp50[prLang],eidatb); 		// "Calibration date: %-8s\n\r"
				SendPrinter(buf);
			}
		}break;
		case 11:
		{
			if(bueb) 																		// 1，硬度校验完成；0，未完成
			{
				if(kalimode==1) 														// 当前硬度校验模式是U盘
				{
					if(!terminologie)
						sprintf(buf,vtextkp16[prLang],sollwert,iistwert); 					// "Calibrationforce\r\nNominal: %3.0f N Act.: %3.0fN\n\r"
					else
						sprintf(buf,vtextkp16j[prLang],sollwert,iistwert); 					// "Adjustmentforce\r\n Nominal: %3.0f N Act.: %3.0fN\n\r"
	
					SendPrinter(buf);
				}
				else 																				// 砝码
				{
					hil=(float)sollwert;
					hil1=(float)iistwert;
					hil/=9.81f;
					hil1/=9.81f;
					
					if(!terminologie)
						sprintf(buf,vtextkp161[prLang],hil,hil1); 									// "Calibrationforce\r\nNominal: %3.0f N Act.: %3.0fN\n\r"
					else
						sprintf(buf,vtextkp161j[prLang],hil,hil1); 									// "Adjustmentforce\r\n Nominal: %3.0f N Act.: %3.0fN\n\r"
	
					SendPrinter(buf);
				}
			}
		}break;
		case 12:
		{
			if(bueb) 																		// 1，硬度校验完成；0，未完成
			{
				if(kalimode==1)
					sprintf(buf,vtextkp163[prLang],NwtFak); 						// "Newtonfactor new: %4.2f\n\r"
				else
					sprintf(buf,vtextkp163[prLang],NwtFak); 						// "Newtonfactor new: %4.2f\n\r"
				
				SendPrinter(buf);
			}
		}break;
		case 13:
		{
			SendPrinter("\n\r");
		}break;
		case 14: 														// 直径校验报告
		{
			if(dmena) 												// 直径允许
			{
				PrintBold();
				if(dickena)
					SendPrinterCC(vtextkp17b[prLang]); 			// "\n        DIAMETER/THICKNESS\n\r"
				else
					SendPrinterCC(vtextkp17[prLang]); 			// "\n             DIAMETER\n\r"
				PrintNorm();
			}
		}break;
		case 15:
		{
			if(dmena) 												// 直径允许
			{
				PrintCondensed();
				
				if(dueb) 																			// 1，直径校验完成；0，未完成
				{
					if(!terminologie)
						sprintf(buf,vtextkp18[prLang],jusdatdo,lasdiam); 		// "last calibration date:%s\r\nlast calibration with: %5.2fmm\n\r"
					else
						sprintf(buf,vtextkp18j[prLang],jusdatdo,lasdiam); 	// "last adjustment date:%s\r\nlast adjustment with: %5.2fmm\n\r"
				}
				else
				{
					if(!terminologie)
						sprintf(buf,vtextkp18[prLang],jusdatd,lasdiam); 		// "last calibration date:%s\r\nlast calibration with: %5.2fmm\n\r"
					else
						sprintf(buf,vtextkp18j[prLang],jusdatd,lasdiam); 	// "last adjustment date:%s\r\nlast adjustment with: %5.2fmm\n\r"
				}
				
				SendPrinter(buf);
	
				PrintNorm();
			}
		}break;
		case 16:
		{
			if(dmena) 												// 直径允许
			{
				//if(dkal) 
				if(dueb) 																			// 1，直径校验完成；0，未完成
				{
					PrintBold();
					if(terminologie)
						SendPrinterCC(vtextkp61[prLang]); 			// Adjustment NEW
					else
						SendPrinterCC(vtextkp6[prLang]); 				// Calibration NEW
					PrintNorm();
				}
			}
		}break;
		case 17:
		{
			if(dmena) 												// 直径允许
			{
				//if(dkal) 
				if(dueb) 																			// 1，直径校验完成；0，未完成
				{
					sprintf(buf,vtextkp50[prLang],eidatd); 		// Calibration date: %-8s\n\r
					SendPrinter(buf);
				}
			}
		}break;
		case 18:
		{
			if(dmena) 												// 直径允许
			{
				//if(dkal) 
				if(dueb) 																			// 1，直径校验完成；0，未完成
				{
					if(!terminologie)
						sprintf(buf,vtextkp19[prLang],eichmas); 					// Calibration gauge: Nominal:%5.2fmm\n\r
					else
						sprintf(buf,vtextkp19j[prLang],eichmas); 					// Adjustment gauge: Nominal: %5.2fmm\n\r
					SendPrinter(buf);
				}
			}
		}break;
		case 19:
		{
			SendPrinter("\n\r");
		}break;
		case 20:
		{
			SendPrinterCC(vtextkp51[prLang]); 					// Signature:......................
		}break;
		default: break;
	}
}

int druckuep32(void) 															// 检查报告
{
	char aktdat[10],akttim[10];  //,snr[8];
	//static char buf[120];
	int i;
//	float hil;
	DTIM dat;

	prLang=prTab[aktLanguage];

	getdatstr(aktdat);
	gettimstr(akttim);

	if(DatFormat==0) 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 上次日期
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d.%02d.%d",dat.day,dat.month,dat.year); 			// 本次日期
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d.%02d.%d",dat.day,dat.month,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d.%02d.%d",dat.day,dat.month,dat.year);
	}
	else 
	{
		LoadDate(&dat,ADRJUSBDATO);
		sprintf(jusdatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSBDAT);
		sprintf(jusdatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDATO);
		sprintf(eidatbo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALBDAT);
		sprintf(eidatb,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDATO);
		sprintf(jusdatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRJUSDDAT);
		sprintf(jusdatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDATO);
		sprintf(eidatdo,"%02d/%02d/%d",dat.month,dat.day,dat.year);
		LoadDate(&dat,ADRKALDDAT);
		sprintf(eidatd,"%02d/%02d/%d",dat.month,dat.day,dat.year);
	}
	
	// 硬度校验U盘工具ID------------------
	LoadKalID(kaliverso,ADRKALIVERSO); 		// 上次
	LoadKalID(kalivers,ADRKALIVERS); 			// 本次
	LoadDosID(dosnro,ADRDOSNRO);
	LoadDosID(dosnr,ADRDOSNR);
	
	/*// Gavin，2021-09-07 星期二 17:10:25
	if(bkal) 															// 执行硬度校验
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKALO);
		lasbist=readinteep(ADRLBIKALO);
		sollwert=readinteep(ADRLBKAL);
		iistwert=readinteep(ADRLBIKAL);
		laskmod=readinteep(ADRLASKMOD);
		kalimode=readinteep(ADRKALIMODE);
	}
	else 																	// 未执行硬度校验
	{
		loadfloat(&lasbnwt,ADRONWT);
		lasbgew=readinteep(ADRLBKAL);
		lasbist=readinteep(ADRLBIKAL);
		
		//kalimode=readinteep(ADRKALIMODE); 					// 2016/12/8 9:33:35
		
		laskmod=readinteep(ADRLASKMOD); 							// 2017/1/13 13:44:25
		kalimode=readinteep(ADRKALIMODE);
	}
	
	loadfloat(&lasdiam,ADRLASDIAM); 			// Gavin，2021-09-07 星期二 17:10:43
	*/
	kalimode=readinteep(ADRKALIMODE);
	
	if(StartPrinter()) 
		return(1);
/*#ifdef CHINAPRINTER
	for(i=27;i>=0;i--) {
		druckuepText(i);
	}
#else*/
	for(i=0;i<28;i++) 
	{
		druckuepText(i);
	}
//#endif
	SendPrinter("\r\n");
	SendPrinter("\r\n");
	SendPrinter("\r\n");

	StopPrinter();
	return(0);
}

void druckuepText(int textNr)
{
	char aktdat[10],akttim[10],snr[8];
	static char buf[120];
	int i;
	static float hil;
//	DTIM dat;
	
	static int i_sollb, i_istb; 					// 整型硬度临时变量，2019-07-27 星期六 10:52:19
	
	// 170603-104, 变量定义
	static int i_solld = 0; 							// 整型临时标准量
	static int i_istd = 0; 								// 整型临时检测量
	static int i_hil = 0; 								// 整型临时误差量
	
	switch(textNr) 
	{
		case 0:
		{
 			PrintBold();
			if(terminologie)
			{
				if(vers[23]=='E') 
				{
					SendPrinterCC(vtextkp1a1[prLang]); 						// ERWEKA TBH125 CALIBRATION(校准)
				}
				
				if(vers[23]=='C') 
				{
					SendPrinterCC(vtextkp1a1c[prLang]);
				}
				
				if(vers[23]=='Q') 
				{
					SendPrinterCC(vtextkp1a1q[prLang]);
				}
			}
			else 
			{
				if(vers[23]=='E') 
				{
					SendPrinterCC(vtextkp1a[prLang]); 						// ERWEKA TBH125 CHECK(检查)
				}
				
				if(vers[23]=='C') 
				{
					SendPrinterCC(vtextkp1ac[prLang]);
				}
				
				if(vers[23]=='Q') 
				{
					SendPrinterCC(vtextkp1aq[prLang]);
				}
			}
			PrintNorm();
		}break;
		case 1:
		{
			for(i=0;i<6;i++) 
			{
				snr[i]=vers[58+i];
				snr[6]=0;
			}
			 
			sprintf(buf,vtextkp2[prLang],id,snr);
			SendPrinter(buf);
		}break;
		case 2:
		{
			getdatstr(aktdat);
			gettimstr(akttim);
			sprintf(buf,vtextkp3[prLang],aktdat,akttim);
			SendPrinter(buf);
		}break;
		case 3:
		{
			SendPrinter("\n\r");
		}break;
		case 4:
		{
			PrintBold();
			SendPrinterCC(vtextkp13[prLang]); 					// HARDNESS
			PrintNorm();
		}break;
		case 5:
		{
			// bkal：1，硬度校验模式；0，检查模式
			if(bkal) 																							// 硬度校验模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a1[prLang],jusdatbo); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a12[prLang],jusdatbo); 		// "last adjustment date:%s\n\r"
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a1[prLang],jusdatb); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a12[prLang],jusdatb); 		// "last adjustment date:%s\n\r"
						SendPrinter(buf);
					}
				}
			}
			else 																									// 硬度检查模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a1[prLang],jusdatbo); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a12[prLang],jusdatbo); 		// "last adjustment date:%s\n\r"
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a1[prLang],jusdatb); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a12[prLang],jusdatb); 		// "last adjustment date:%s\n\r"
						SendPrinter(buf);
					}
				}
			}
		}break;
		case 6:
		{
			// bkal：1，硬度校验模式；0，检查模式
			if(bkal) 																							// 硬度校验模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a11[prLang],eidatbo); 			// "last instrument check:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a1[prLang],eidatbo); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a11[prLang],eidatb); 			// "last instrument check:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a1[prLang],eidatb); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
				}
			}
			else 																									// 检查模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a11[prLang],eidatbo); 			// "last instrument check:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a1[prLang],eidatbo); 			// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(terminologie==0)
					{
						sprintf(buf,vtextkp14a11[prLang],eidatb); 			// "last instrument check:%s\n\r"
						SendPrinter(buf);
					}
					else 
					{
						sprintf(buf,vtextkp14a1[prLang],eidatb); 				// "last calibration date:%s\n\r"
						SendPrinter(buf);
					}
				}
			}
		}break;
	  case 7:
		{
			;
			// 微型打印机，不打印调整工具信息，206-4-2，Gavin，2019-07-30 星期二 14:16:48
			/*
			// bkal：1，硬度校验模式；0，检查模式
			if(bkal) 																							// 硬度校验模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(kalimode==1) 
					{
						sprintf(buf,vtextkp15a[prLang],kaliverso,dosnro);
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(kalimode==1) 
					{
						sprintf(buf,vtextkp15a[prLang],kalivers,dosnr);
						SendPrinter(buf);
					}
				}
			}
			else 																									// 检查模式
			{
				if(bueb) 																						// 1，硬度检查完成；0，未完成
				{
					if(kalimode==1) 
					{
						sprintf(buf,vtextkp15a[prLang],kaliverso,dosnro);
						SendPrinter(buf);
					}
				}
				else 																								// 未完成
				{
					if(kalimode==1) 
					{
						sprintf(buf,vtextkp15a[prLang],kalivers,dosnr);
						SendPrinter(buf);
					}
				}
			}
			*/
		}break;
		case 8:
		{
			SendPrinter("\r\n");
		}break;
		case 9: 														// 硬度检查，第一组测试数据
		{
			if(bueb) 
			{
				if(terminologie)
				{
					SendPrinterCC(vtextkp6[prLang]); 				// Calibration NEW
					sprintf(buf,vtextkp50[prLang],eidatb); 	// Calibration date: %-8s\n\r
					SendPrinter(buf);
				}
				else 
				{
					SendPrinterCC(vtextkp6a[prLang]); 			// Check NEW
					sprintf(buf,vtextkp50a[prLang],eidatb); // Control date:%s  \n\r
					SendPrinter(buf);
				}
				
				if(kalimode) 										// U盘
				{
					// 206-3-2，Gavin，2019-07-26 星期五 12:48:54
					///*
					sprintf(buf,vtextkp15a[prLang],kalivers,dosnr);
					SendPrinter(buf);
					//*/
					
					sprintf(buf,vtextkp8a[prLang],sollb[0],istb[0]);
					SendPrinter(buf);
				}
				else 														// 砝码
				{
					sprintf(buf,vtextkp8a1[prLang],sollb[0],istb[0]);
					SendPrinter(buf);
				}
			}
		}break;
		case 10: 														// 硬度检查，第一组测试数据比较结果
		{
			if(bueb) 
			{
				if(kalimode) 										// U盘
				{
					// 206-5, Gavin, 2019-07-27 星期六 11:29:44
					///*
					//i_sollb = sollb[0];
					//i_istb = istb[0];
					i_sollb = sollb[0]+0.5f; 			// v2.08，Gavin，2019-11-07 星期四 18:15:02
					i_istb = istb[0]+0.5f;
					if(i_sollb > i_istb)
						hil = i_sollb - i_istb;
					else
						hil = i_istb - i_sollb;
					//*/
					
					hil=round0(hil);
					if(hil>1.0001f) 
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
			  }
			  else 														// 砝码
				{
					// 206-5, Gavin, 2019-07-27 星期六 11:25:02
					/*
					if(sollb[0]>istb[0]) 
						hil=sollb[0]-round0(istb[0]);
					else 
						hil=round0(istb[0])-sollb[0];
					*/
					
					// 206-5, Gavin, 2019-07-30 星期二 14:41:06
					///*
					if(sollb[0]>istb[0]) 
						hil=sollb[0]-istb[0];
					else 
						hil=istb[0]-sollb[0];
					//*/
					
					hil=round1(hil);
					//if(hil>0.1) 								// 2019-07-27 星期六 11:35:44
					if(hil>0.1001f)
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
				}
			}
		}break;
		case 11: 														// 硬度检查，第二组测试数据
		{
			if(bueb) 
			{
				// 206-5, Gavin, 2019-07-27 星期六 11:37:02
				/*
				if(sollb[0]>istb[1]) 
					hil=sollb[1]-round0(istb[1]);
				else 
					hil=round0(istb[1])-sollb[1];
				*/
				
				if(kalimode) 										// U盘
				{
					sprintf(buf,vtextkp8a[prLang],sollb[1],istb[1]);
					SendPrinter(buf);
				}
				else 														// 砝码
				{
					sprintf(buf,vtextkp8a1[prLang],sollb[1],istb[1]);
					SendPrinter(buf);
				}
			}
		}break;
		case 12: 														// 硬度检查，第二组测试数据比较结果
		{
			if(bueb) 
			{
				if(kalimode) 										// U盘
				{
					// 206-5, Gavin, 2019-07-27 星期六 11:41:30
					///*
					//i_sollb = sollb[1];
					//i_istb = istb[1];
					i_sollb = sollb[1]+0.5f; 			// v2.08，Gavin，2019-11-07 星期四 18:17:17
					i_istb = istb[1]+0.5f;
					if(i_sollb > i_istb)
						hil = i_sollb - i_istb;
					else
						hil = i_istb - i_sollb;
					//*/
					
					hil=round0(hil);
					if(hil>1.0001f) 
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
				}
				else 														// 砝码
				{
					// 206-5, Gavin, 2019-07-27 星期六 11:37:27
					/*
					if(sollb[1]>istb[1]) 
						hil=sollb[1]-round0(istb[1]);
					else 
						hil=round0(istb[1])-sollb[1];
					*/
					
					// 206-5, Gavin, 2019-07-30 星期二 14:41:06
					///*
					if(sollb[1]>istb[1]) 
						hil=sollb[1]-istb[1];
					else 
						hil=istb[1]-sollb[1];
					//*/
					
					hil=round1(hil);
					//if(hil>0.1) 								// 2019-07-27 星期六 11:46:51
					if(hil>0.1001f)
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
				}
			}
		}break;
		case 13: 														// 硬度检查，第三组测试数据
		{
			if(bueb) 
			{
				// 206-5，Gavin，2019-07-27 星期六 11:43:11
				/*
				if(sollb[2]>istb[2]) 
					hil=sollb[2]-round0(istb[2]);
				else 
					hil=round0(istb[2])-sollb[2];
				*/
				
				if(kalimode) 
				{
					sprintf(buf,vtextkp8a[prLang],sollb[2],istb[2]);
					SendPrinter(buf);
				}
				else 
				{
					sprintf(buf,vtextkp8a1[prLang],sollb[2],istb[2]);
					SendPrinter(buf);
				}
			}
		}break;
		case 14: 														// 硬度检查，第三组测试数据比较结果
		{
			if(bueb) 
			{
				if(kalimode) 
				{
					// 206-5, Gavin, 2019-07-27 星期六 11:44:55
					///*
					//i_sollb = sollb[2];
					//i_istb = istb[2];
					i_sollb = sollb[2]+0.5f; 			// v2.08，Gavin，2019-11-07 星期四 18:17:58
					i_istb = istb[2]+0.5f;
					if(i_sollb > i_istb)
						hil = i_sollb - i_istb;
					else
						hil = i_istb - i_sollb;
					//*/
					
					hil=round0(hil);
					if(hil>1.0001f) 
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
				}
				else 
				{
					// 206-5，Gavin，2019-07-27 星期六 11:44:18
					/*
					if(sollb[2]>istb[2]) 
						hil=sollb[2]-round0(istb[2]);
					else 
						hil=round0(istb[2])-sollb[2];
					*/
					
					// 206-5, Gavin, 2019-07-30 星期二 14:41:06
					///*
					if(sollb[2]>istb[2]) 
						hil=sollb[2]-istb[2];
					else 
						hil=istb[2]-sollb[2];
					//*/
					
					hil=round1(hil);
					//if(hil>0.1) 								// 2019-07-27 星期六 11:46:14
					if(hil>0.1001f)
					{
						PrintBold();
						SendPrinter("\n\rOut of range!\n\n\r");
						PrintNorm();
					}
					else 
					{
						SendPrinter("\n\rOK\n\n\r");
					}
				}
			}
		}break;
		
/*		for(i=0;i<3;i++) {
			if(sollb[i]>istb[i]) hil=sollb[i]-istb[i];
			else hil=istb[i]-sollb[i];
			if(kalimode) {
				sprintf(buf,vtextkp8a[prLang],sollb[i],istb[i]);
				SendPrinter(buf);
				if(hil>2) {
					PrintBold();
					SendPrinter("\n\rOut of range!\n\n\r");
					PrintNorm();
				}
				else {
					SendPrinter("\n\rOK\n\n\r");
				}
			}
			else {
				sprintf(buf,vtextkp8a1[prLang],sollb[i],istb[i]);
				SendPrinter(buf);
				if(hil>0.2) {
					PrintBold();
					SendPrinter("\n\rOut of range!\n\n\r");
					PrintNorm();
				}
				else {
					SendPrinter("\n\rOK\n\n\r");
				}
			}
		}
	}*/
		case 15:
		{
			SendPrinter("\n\r");
		}break;
		case 16:
		{
			if(dmena) 
			{
				PrintBold();
				if(dickena)
					SendPrinterCC(vtextkp17b[prLang]); 			// "\n        DIAMETER/THICKNESS\n\r"
				else
					SendPrinterCC(vtextkp17[prLang]); 			// "\n             DIAMETER\n\r"

				PrintNorm();
			}
		}break;
		case 17:
		{
			if(dmena) 
			{
				// dkal：1，直径校验模式；0，检查模式
				if(dkal) 																	// 直径校验模式
				{
					// dueb: 1，直径检查完成；0，未完成
					if(dueb) 																// 1，直径检查完成；
					{
						if(terminologie==0)
						{
							sprintf(buf,vtextkp14a1[prLang],jusdatdo);
						}
						else 
						{
							sprintf(buf,vtextkp14a12[prLang],jusdatdo);
						}
						SendPrinter(buf);
					}
					else 																		// 未完成
					{
						if(terminologie==0)
						{
							sprintf(buf,vtextkp14a1[prLang],jusdatd);
						}
						else 
						{
							sprintf(buf,vtextkp14a12[prLang],jusdatd);
						}
						SendPrinter(buf);
					}
				}
				else 																			// 直径检查模式
				{
					// dueb: 1，直径检查完成；0，未完成
					if(dueb) 																// 1，直径检查完成；
					{
						if(terminologie==0)
						{
							sprintf(buf,vtextkp14a1[prLang],jusdatd);
						}
						else 
						{
							sprintf(buf,vtextkp14a12[prLang],jusdatd);
						}
						SendPrinter(buf);
					}
					else 																		// 未完成
					{
						if(terminologie==0)
						{
							sprintf(buf,vtextkp14a1[prLang],jusdatd);
						}
						else 
						{
							sprintf(buf,vtextkp14a12[prLang],jusdatd);
						}
						SendPrinter(buf);
					}
				}
			}
		}break;
		case 18:
		{
			if(dmena) 
			{
				// dkal：1，直径校验模式；0，检查模式
				if(dkal) 																	// 直径校验模式
				{
					// dueb: 1，直径检查完成；0，未完成
					if(dueb) 																// 1，直径检查完成；
					{
						if(terminologie==0) 
						{
							sprintf(buf,vtextkp14a11[prLang],eidatdo);
						}
						else 
						{
							sprintf(buf,vtextkp14a1[prLang],eidatdo);
						}
						SendPrinter(buf);
					}
					else 																		// 未完成
					{
						if(terminologie==0) 
						{
							sprintf(buf,vtextkp14a11[prLang],eidatd);
						}
						else 
						{
							sprintf(buf,vtextkp14a1[prLang],eidatd);
						}
						SendPrinter(buf);
					}
				}
				else 																			// 检查模式
				{
					// dueb: 1，直径检查完成；0，未完成
					if(dueb) 																// 1，直径检查完成；
					{
						if(terminologie==0) 
						{
							sprintf(buf,vtextkp14a11[prLang],eidatdo);
						}
						else 
						{
							sprintf(buf,vtextkp14a1[prLang],eidatdo);
						}
						SendPrinter(buf);
					}
					else 																		// 未完成
					{
						if(terminologie==0) 
						{
							sprintf(buf,vtextkp14a11[prLang],eidatd);
						}
						else 
						{
							sprintf(buf,vtextkp14a1[prLang],eidatd);
						}
						SendPrinter(buf);
					}
				}
			}
		}break;
		case 19:
		{
			SendPrinter("\r\n");
		}break;
		case 20:
		{
			if(dueb) 
			{
				if(terminologie)
				{
					SendPrinterCC(vtextkp6[prLang]); 				// Calibration NEW
					sprintf(buf,vtextkp50[prLang],eidatd); 	// Calibration date: %-8s\n\r
					SendPrinter(buf);
				}
				else 
				{
					SendPrinterCC(vtextkp6a[prLang]); 			// Check NEW
					sprintf(buf,vtextkp50a[prLang],eidatd); // Control date:%s  \n\r
					SendPrinter(buf);
				}
				
				// 170603-105, 微型打印机第一组数据
				i_solld = solld[0]*1000;
				i_solld = (i_solld+5)/10;
				solld[0] = (float)(i_solld)/100;
				
				i_istd = istd[0]*1000;
				i_istd = (i_istd+5)/10;
				istd[0] = (float)(i_istd)/100;
				
				if(i_solld > i_istd)
					i_hil = i_solld - i_istd;
				else
					i_hil = i_istd - i_solld;
				// End
				
				if(istd[0]<90000) 
				{
					sprintf(buf,vtextkp12[prLang],solld[0],istd[0]);
					SendPrinter(buf);
				}
				else 
				{
					sprintf(buf,vtextkp12Er[prLang],solld[0]);
					SendPrinter(buf);
				}
			}
		}break;
		case 21:
		{
			if(dueb && istd[0]<90000) 
			{
				// 170603-105，整数比较
				//if(hil>0.05) 
				if(i_hil > 5)
				{
					PrintBold();
					SendPrinter("\n\rOut of range!\n\n\r");
					PrintNorm();
				}
				else 
				{
					SendPrinter("\n\rOK\n\n\r");
				}
			}
		}break;
		case 22:
		{
			if(dueb) 
			{
				// 170603-106，第二组数据
				i_solld = solld[1]*1000;
				i_solld = (i_solld+5)/10;
				solld[1] = (float)(i_solld)/100;
				
				i_istd = istd[1]*1000;
				i_istd = (i_istd+5)/10;
				istd[1] = (float)(i_istd)/100;
				
				if(i_solld > i_istd) 
					i_hil = i_solld - i_istd;
				else 
					i_hil = i_istd - i_solld;
				
				if(istd[1]<90000) 
				{
					sprintf(buf,vtextkp12[prLang],solld[1],istd[1]);
					SendPrinter(buf);
				}
				else 
				{
					sprintf(buf,vtextkp12Er[prLang],solld[1]);
					SendPrinter(buf);
				}
			}
		}break;
		case 23:
		{
			if(dueb && istd[1]<90000) 
			{
				// 170603-106，整数比较
				//if(hil>0.05) 
				if(i_hil > 5)
				{
					PrintBold();
					SendPrinter("\n\rOut of range!\n\n\r");
					PrintNorm();
				}
				else 
				{
					SendPrinter("\n\rOK\n\n\r");
				}
			}
		}break;
		case 24:
		{
			if(dueb) 
			{
				// 170603-107，第三组数据
				i_solld = solld[2]*1000;
				i_solld = (i_solld+5)/10;
				solld[2] = (float)(i_solld)/100;
				
				i_istd = istd[2]*1000;
				i_istd = (i_istd+5)/10;
				istd[2] = (float)(i_istd)/100;
				
				if(i_solld > i_istd) 
					i_hil = i_solld - i_istd;
				else 
					i_hil = i_istd - i_solld;
				
				if(istd[2]<90000) 
				{
					sprintf(buf,vtextkp12[prLang],solld[2],istd[2]);
					SendPrinter(buf);
				}
				else 
				{
					sprintf(buf,vtextkp12Er[prLang],solld[2]);
					SendPrinter(buf);
				}
			}
		}break;
		case 25:
		{
			if(dueb && istd[2]<90000) 
			{
				// 170603-107，整数比较
				//if(hil>0.05) 
				if(i_hil > 5)
				{
					PrintBold();
					SendPrinter("\n\rOut of range!\n\n\r");
					PrintNorm();
				}
				else 
				{
					SendPrinter("\n\rOK\n\n\r");
				}
			}
		}break;
		case 26:
		{
			SendPrinter("\n\r");
		}break;
		case 27:
		{
			SendPrinterCC(vtextkp51[prLang]);
		}break;
		default: break;
	}
}
// END 微型打印机
//-------------------------------------------------------------

//float const TestT[10] = {  3.72,   3.71,   3.70,   3.70,  3.68,   3.70,   3.71,   3.68,   3.72,   3.71};
//float const TestB[10] = {102,    115,    106,    109,   111,     90,    104,    117,    102,     97};
//float const TestD[10] = {  6.99,   6.97,   6.99,   6.97,  6.96,   7.00,   6.98,   7.03,   6.96,   7.00};

// 根据测量数据组计算需要的结果 ------------------------------------------------
void calcauswert()
{
	int i;
	unsigned int verganz;
	float messwd,messwb,messwt;
	float xqsumb,xqsumd,xqsumt;
	float fdat0,fdat1;

	minb=1000.0; 													// 硬度最小值
	mind=1000.0; 													// 直径最小值
	mint=1000.0; 													// 厚度最小值
	
	maxb=0.0; 														// 硬度最大值
	maxd=0.0; 														// 指定最大值
	maxt=0.0; 														// 厚度最大值

	mittd=0.0; 														// 直径平均值
	mittb=0.0; 														// 硬度平均值
	mittt=0.0; 														// 厚度平均值

	xqsumb=0.0; 													// 测量结果的平方累加和
	xqsumd=0.0;
	xqsumt=0.0;

	sreld=0.0; 														// 直径偏差率
	srelb=0.0; 														// 硬度偏差率
	srelt=0.0; 														// 厚度偏差率
	
	sabwb=0.0; 														// 硬度标准偏差
	sabwd=0.0; 														// 直径标准偏差
	sabwt=0.0; 														// 厚度标准偏差
	
	auswanz=0; 														// 硬度有效测试次数
	auswanzd=0; 													// 直径有效测试次数
	auswanzt=0; 													// 厚度有效测试次数
	
	// 模拟测试数据输入 --------------------------------------
	/*abranz = 10;
	for(i=0;i<abranz;i++)
	{
		mebuH[i] = TestB[i];
		mebuD[i] = TestD[i];
		mebuT[i] = TestT[i];
	}*/
	
	if(abranz==0) 
	{
		verganz=pruefanz;
		if(dmena)
		{
			if(pruefanzd>verganz) 
				verganz=pruefanzd;
		}
	}
	else 
	{
		verganz=abranz;
	}
	
	for(i=0;i<verganz;i++) 								// 测试次数？，Gavin，2020-01-09 星期四 11:36:12
	{
		messwb=mebuH[i]/kraftfak; 					// 硬度，按单位取值
		messwd=mebuD[i]/laengfak; 					// 直径，按单位取值
		messwt=mebuT[i]/laengfak; 					// 厚度，按单位取值
		
		// 按要求取小数点后几位 --------------------------------
		if(LaeEinh==0) 											// 长度单位
		{
			messwd=round2(messwd);
			messwt=round2(messwt);
		}
		else 
		{
			messwd=round3(messwd);
			messwt=round3(messwt);
		}
		
		if(HardEinh==0) 										// 硬度单位
			messwb=round0(messwb);
		else
			messwb=round1(messwb);
		
		if(mebuH[i] < 90000) 								// 硬度统计计算
		{    //!=99999 && messwb!=99998) {
			// 统计最大最小值 --------------------------
			/*if(HardEinh==0) 									// 硬度单位
			{
				if(messwb>maxb) maxb = round0(messwb);
				if(messwb<minb) minb = round0(messwb);
			}
			else 
			{
				if(messwb>maxb) maxb = round1(messwb);
				if(messwb<minb) minb = round1(messwb);
			}*/
			
			if(messwb>maxb) maxb = messwb; 		// 计算最大值
			if(messwb<minb) minb = messwb; 		// 计算最小值
			
			mittb += messwb; 									// 计算测量结果的累加和
			xqsumb += (messwb*messwb); 				// 计算测量结果的平方累加和
			auswanz++; 												// 计算测量次数
		}
		
		if(mebuD[i] < 90000) 								// 直径统计计算
		{   //!=99999 && messwd!=99998) {
			if(dmena) 												// 直径测量使能
			{
				// 统计最大最小值 ------------------------
				/*if(LaeEinh==0) 									// 长度单位
				{
					if(messwd>maxd) maxd=round2(messwd);
					if(messwd<mind) mind=round2(messwd);
				}
				else 
				{
					if(messwd>maxd) maxd=round3(messwd);
					if(messwd<mind) mind=round3(messwd);
				}*/
				
				if(messwd>maxd) maxd = messwd; 	// 计算最大值
				if(messwd<mind) mind = messwd; 	// 计算最小值
				
				mittd += messwd; 								// 计算测量结果的累加和
				xqsumd += (messwd*messwd); 			// 计算测量结果的平方累加和
				auswanzd++; 										// 计算测量次数
			}
		}
		
		if(mebuT[i] < 90000) 								// 厚度统计计算
		{   //!=99999 && messwd!=99998) {
			if(dickena) 											// 厚度测量使能
			{
				// 统计最大最小值 ------------------------
				/*if(LaeEinh==0) 									// 长度单位
				{
					if(messwt>maxt) maxt=round2(messwt);
					if(messwt<mint) mint=round2(messwt);
				}
				else 
				{
					if(messwt>maxt) maxt=round3(messwt);
					if(messwt<mint) mint=round3(messwt);
				}*/
				
				if(messwt>maxt) maxt = messwt; 	// 计算最大值
				if(messwt<mint) mint = messwt; 	// 计算最小值
				
				mittt += messwt; 								// 计算测量结果的累加和
				xqsumt += (messwt*messwt); 			// 计算测量结果的平方累加和
				auswanzt++; 										// 计算测量次数
			}
		}
	}
	
	// 硬度，标准偏差计算，平均值计算，偏差率计算 ------------
	if(auswanz>1) 
	{
		// 计算标准偏差（算法已经验证过，与(((x1-xp)^2+(x2-xp)^2+...+(xn-xp)^2))/(n-1))^(1/2)相同) -------
		sabwb=(xqsumb-(mittb * mittb/(double)auswanz))/((double)auswanz-1.0);
		
		//if(sabwb > 0.00001)
		if(sabwb > 0.0000001f) 							// Gavin，2021-07-09 星期五 9:21:40
			sabwb=sqrt(sabwb); 								// 开根号
		else
			sabwb=0.0;
		
		// 计算平均值 --------------------------------
		mittb=mittb/(double)auswanz;
		
		if(mittb>0.0f)
		{
			//srelb=(sabwb/mittb)*100.0;
			
			if(HardEinh==0) 										// 硬度单位
			{
				fdat0 = round1(sabwb);
				fdat1 = round1(mittb);
			}
			else
			{
				fdat0 = round2(sabwb);
				fdat1 = round2(mittb);
			}
			
			srelb=(fdat0/fdat1)*100.0f;
		}
	}
	else 
	{
		if(auswanz==0)
			minb=0; 
	}
	
	// 直径，标准偏差计算，平均值计算，偏差率计算 ------------
	if(dmena) 
	{
		if(auswanzd>1) 
		{
			// 计算标准偏差（算法已经验证过，与(((x1-xp)^2+(x2-xp)^2+...+(xn-xp)^2))/(n-1))^(1/2)相同) -------
			sabwd=(xqsumd-(mittd * mittd/(double)auswanzd))/((double)auswanzd-1.0);
			
			//if(sabwd > 0.00001)
			if(sabwd > 0.0000001f) 						// Gavin，2021-07-09 星期五 9:21:40
				sabwd=sqrt(sabwd); 							// 开根号
			else
				sabwd=0.0;
			
			// 计算平均值 --------------------
			mittd=mittd/(double)auswanzd;
			
			if(mittd>0.0f)
			{
				//sreld=(sabwd/mittd)*100.0;
				
				if(LaeEinh==0) 											// 长度单位
				{
					fdat0 = round2(sabwd);
					fdat1 = round2(mittd);
				}
				else
				{
					fdat0 = round3(sabwd);
					fdat1 = round3(mittd);
				}
				
				sreld=(fdat0/fdat1)*100.0f;
			}
		}
		else 
		{
			if(auswanzd==0)
				mind=0; 
		}
	}
	
	// 厚度，标准偏差计算，平均值计算，偏差率计算 ------------
	if(dickena) 
	{
		if(auswanzt>1) 
		{
			// 计算标准偏差（算法已经验证过，与(((x1-xp)^2+(x2-xp)^2+...+(xn-xp)^2))/(n-1))^(1/2)相同) -------
			sabwt=(xqsumt-(mittt * mittt/(double)auswanzt))/((double)auswanzt-1.0);
			
			//if(sabwt > 0.00001)
			if(sabwt > 0.0000001f) 						// Gavin，2021-07-09 星期五 9:21:40
				sabwt=sqrt(sabwt); 							// 开平方，Gavin，2020-01-09 星期四 11:23:40
			else
				sabwt=0.0;
			
			// 计算平均值 ------------------------------
			mittt=mittt/(double)auswanzt;
			
			if(mittt>0.0f)
			{
				//srelt=(sabwt/mittt)*100.0;
				
				if(LaeEinh==0) 											// 长度单位
				{
					fdat0 = round2(sabwt);
					fdat1 = round2(mittt);
				}
				else
				{
					fdat0 = round3(sabwt);
					fdat1 = round3(mittt);
				}
				
				srelt=(fdat0/fdat1)*100.0f;
			}
		}
		else 
		{
			if(auswanzt==0)
				mint=0; 
		}
	}
}
