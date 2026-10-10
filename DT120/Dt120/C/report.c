#include <math.h>
#include <stdlib.h>
#include "calibrate.h"
#include "confmenu.h"
#include "control.h"
#include "eeprom.h"
#include "fram.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "printrtc.h"
#include "report.h"
#include "rs485.h"
#include "sound.h"
#include "strings.h"
#include "sampler.h"
#include "temp.h"
#include "timedate.h"
#include "timer.h"
#include "version.h"

TByte REPORT_PrintLine;
TByte REPORT_NewPrintLine;

TByte REPORT_Lines;
TWord REPORT_Pages;
TWord REPORT_PagesNum;

TBool REPORT_PrintInit=false;

TWord REPORT_PrintStat;
TWord REPORT_NumStatRecords=0;

__flash char METHOD_Str[METH_MODE_LAST][15]=
{
  "Basket",
  "Paddle",
  "Paddle/disk",
  "Cylinder",
  "________", 						// Gavin,2020-12-08 星期二 13:34:29
};

__flash char VOLUME_Str[VOLUME_MODE_LAST][5]=
{
  //"4000",
  //"2000",
  //"1900",
  //"1750",
  //"1500",
  // Gavin 20131224---------------------
  "1000",
  "900 ",
  "750 ",
  "500 ",
  "____", 											// Gavin,2020-12-08 星期二 13:17:00
};

__flash char HEAD_Str[HEAD_MODE_LAST][10]=
{
  "LowHead",
  "HighHead"
};

char __flash Empty_Str[]="";
char __flash CR_Str[]="\r\n";
char __flash Line_Str[]="_____________________________________________________________\r\n";
char __flash Line_Str1[]="________________________________\r\n";
char __flash String_Str[]="%s\r\n";
char __flash Stat_Str[]="   %02dh%02d        %s            %s\r\n";
char __flash AsRes_Str[]="   %d                %s                      %s\r\n";
char __flash AsRes_Str1[]="   %d                %s                      %s\r\n";
char __flash CheckAsHeader_Str[]=   "  Nr.               [鳦]                      [鳦]\r\n";
char __flash CheckAsHeader_Str1[]=   "  Nr.               [鳦]                      [鳦]\r\n";
char __flash Comment_Str[]=" __________________________________________________________\r\n";
char __flash Comment_Str1[]="________________________________\r\n";
char __flash CalHead_Str[]="                      V.1    2    3    4    5    6    7    8\r\n";
char __flash ChPoint_Str[]="                            P.1      P.2      P.3\r\n";

char __flash Test_Str_En[]="                       TEST REPORT               Page %d/%d\r\n";

// 2016/5/14 星期六 20:47:02--------------------------------------------------
//char __flash Test_Str_Ge[]="                       TESTBERICHT              Seite %d/%d\r\n";
char __flash Test_Str_Ge[]="                       Testprotokoll            Seite %d/%d\r\n";

char __flash Test_Str_Fr[]="                   COMPTE-RENDU DU TEST          Page %d/%d\r\n";
char __flash Test_Str_It[]="                     RAPPORTO DEL TEST         Pagina %d/%d\r\n";
char __flash Test_Str_Sp[]="                     INFORME DEL TEST          P\xA0gina %d/%d\r\n";

char __flash Test_Str1_En[]="          TEST REPORT\r\n";

// 2016/5/22 星期日 11:04:10--------------------------------
//char __flash Test_Str1_Ge[]="          TESTBERICHT\r\n";
char __flash Test_Str1_Ge[]="         Testprotokoll\r\n";

char __flash Test_Str1_Fr[]="      COMPTE-RENDU DU TEST\r\n";
char __flash Test_Str1_It[]="       RAPPORTO DEL TEST\r\n";
char __flash Test_Str1_Sp[]="        INFORME DEL TEST\r\n";

//char __flash Instrument_Str_En[]="Instrument:  %s DT%d2%1d (%s)          %s %s\r\n";
//char __flash Instrument_Str_Ge[]="Ger\204t:       %s DT%d2%1d (%s)          %s %s\r\n";
//char __flash Instrument_Str_Fr[]="Appareil:    %s DT%d2%1d (%s)          %s %s\r\n";
//char __flash Instrument_Str_It[]="Strumento:   %s DT%d2%1d (%s)          %s %s\r\n";
//char __flash Instrument_Str_Sp[]="Aparato:     %s DT%d2%1d (%s)          %s %s\r\n";

char __flash Instrument_Str_En[]="Instrument:    %s %s (%s)        %s %s\r\n";
char __flash Instrument_Str_Ge[]="Ger\204t:         %s %s (%s)        %s %s\r\n";
char __flash Instrument_Str_Fr[]="Appareil:      %s %s (%s)        %s %s\r\n";
char __flash Instrument_Str_It[]="Strumento:     %s %s (%s)        %s %s\r\n";
char __flash Instrument_Str_Sp[]="Aparato:       %s %s (%s)        %s %s\r\n";

char __flash ISTM_Str1_En[]="   %s %s (%s)\r\n";
char __flash ISTM_Str1_Ge[]="   %s %s (%s)\r\n";
char __flash ISTM_Str1_Fr[]="   %s %s (%s)\r\n";
char __flash ISTM_Str1_It[]="   %s %s (%s)\r\n";
char __flash ISTM_Str1_Sp[]="   %s %s (%s)\r\n";

// Gavin, 2016-10-24 星期一 11:07:40
char __flash HOT_Str_En[]="DHC1520 Serial:%s\r\n";
char __flash HOT_Str_Ge[]="DHC1520 Serial:%s\r\n";
char __flash HOT_Str_Fr[]="DHC1520 Serial:%s\r\n";
char __flash HOT_Str_It[]="DHC1520 Serial:%s\r\n";
char __flash HOT_Str_Sp[]="DHC1520 Serial:%s\r\n";

char __flash PCB_Str_En[]="PCB No.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Ge[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Fr[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_It[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Sp[]="PCB Nr.:     %s (ver. %s)\r\n";

char __flash VER_Str_En[]="Firmware ver.: %s\r\n";
char __flash VER_Str_Ge[]="Firmware ver.: %s\r\n";
char __flash VER_Str_Fr[]="Firmware ver.: %s\r\n";
char __flash VER_Str_It[]="Firmware ver.: %s\r\n";
char __flash VER_Str_Sp[]="Firmware ver.: %s\r\n";

char __flash VER_Str1_En[]="Firmware ver.: %s\r\n";
char __flash VER_Str1_Ge[]="Firmware ver.: %s\r\n";
char __flash VER_Str1_Fr[]="Firmware ver.: %s\r\n";
char __flash VER_Str1_It[]="Firmware ver.: %s\r\n";
char __flash VER_Str1_Sp[]="Firmware ver.: %s\r\n";

char __flash HeaterCal_Str_En[]="Heater Adjustment:            %s %s\r\n";

// 2016/5/14 星期六 20:49:52--------------------------------
//char __flash HeaterCal_Str_Ge[]="Justierung der Heizung:       %s %s\r\n";
char __flash HeaterCal_Str_Ge[]="Sensor int. Just.:       %s %s\r\n";

char __flash HeaterCal_Str_Fr[]="Calibrage du thermostat:      %s %s\r\n";
char __flash HeaterCal_Str_It[]="Taratura del termostato:      %s %s\r\n";
char __flash HeaterCal_Str_Sp[]="Calibraci\xA2n del termostato:   %s %s\r\n";

char __flash HTCal_Str1_En[]="Heater Adj.:   %s %s\r\n";

// 2016/5/22 星期日 11:07:58--------------------------------
//char __flash HTCal_Str1_Ge[]="Heizung Just.: %s %s\r\n"; 										// 2016/4/20 星期三 15:29:04, 2016/4/22 星期五 18:47:57,2016/4/26 星期二 17:00:18
//char __flash HTCal_Str1_Ge[]="Sensor int. Just.: %s %s\r\n";
  char __flash HTCal_Str1_Ge[]="Sens.int.Just.: %s %s\r\n"; 										// 2016/6/3 星期五 10:08:36

char __flash HTCal_Str1_Fr[]="Heater Adj.:   %s %s\r\n";
char __flash HTCal_Str1_It[]="Heater Adj.:   %s %s\r\n";
char __flash HTCal_Str1_Sp[]="Heater Adj.:   %s %s\r\n";

char __flash SensorCal_Str_En[]="Sensor Adjustment:            %s %s\r\n";

// 2016/5/14 星期六 20:51:56--------------------------------
//char __flash SensorCal_Str_Ge[]="Justierung des F\x81hlers:       %s %s\r\n";
char __flash SensorCal_Str_Ge[]="Sensor ext. Just.:       %s %s\r\n";

char __flash SensorCal_Str_Fr[]="Calibrage du senseur:         %s %s\r\n";
char __flash SensorCal_Str_It[]="Taratura del sensore:         %s %s\r\n";
char __flash SensorCal_Str_Sp[]="Calibraci\xA2n del sensor:       %s %s\r\n";

char __flash SSCal_Str1_En[]="Sensor Adj.:   %s %s\r\n";

// 2016/5/22 星期日 11:09:11--------------------------------
//char __flash SSCal_Str1_Ge[]="F\x81hlers Just.: %s %s\r\n"; 								// 2016/4/20 星期三 15:31:23, 2016/4/22 星期五 18:49:00,2016/4/26 星期二 17:01:51
//char __flash SSCal_Str1_Ge[]="Sensor ext. Just.: %s %s\r\n";
  char __flash SSCal_Str1_Ge[]="Sens.ext.Just.: %s %s\r\n"; 										// 2016/6/3 星期五 10:09:15

char __flash SSCal_Str1_Fr[]="Sensor Adj.:   %s %s\r\n";
char __flash SSCal_Str1_It[]="Sensor Adj.:   %s %s\r\n";
char __flash SSCal_Str1_Sp[]="Sensor Adj.:   %s %s\r\n";

char __flash AsCal_Str_En[]="Autosampler Adjustment:       %s %s\r\n";
char __flash AsCal_Str_Ge[]="Justierung des Autosamplers:  %s %s\r\n";
char __flash AsCal_Str_Fr[]="Calibrage du autosampler:     %s %s\r\n";
char __flash AsCal_Str_It[]="Taratura del autosampler:     %s %s\r\n";
char __flash AsCal_Str_Sp[]="Calibraci\xA2n del autosampler:  %s %s\r\n";

char __flash AsCal_Str1_En[]="Autosampler Adjustment:       %s %s\r\n";
char __flash AsCal_Str1_Ge[]="Justierung des Autosamplers:  %s %s\r\n";
char __flash AsCal_Str1_Fr[]="Calibrage du autosampler:     %s %s\r\n";
char __flash AsCal_Str1_It[]="Taratura del autosampler:     %s %s\r\n";
char __flash AsCal_Str1_Sp[]="Calibraci\xA2n del autosampler:  %s %s\r\n";

char __flash PName_Str_En[]="Product Name:                     Dose:\r\n";

// 2016/5/14 星期六 20:54:24--------------------------------
//char __flash PName_Str_Ge[]="Name des Produkts:                Dosis:\r\n";
char __flash PName_Str_Ge[]="Produkt-Bez.:                     Dosis:\r\n"; 		// 2016/5/26 星期四 18:13:15

char __flash PName_Str_Fr[]="Nom du produit:                   Dose:\r\n";
char __flash PName_Str_It[]="Nome del prodotto:                Dose:\r\n";
char __flash PName_Str_Sp[]="Nombre del producto:              Dosis:\r\n";

char __flash PName_Str1_En[]="Product Name: \r\n";

// 2016/5/22 星期日 11:11:16--------------------------------
//char __flash PName_Str1_Ge[]="Name des Produkts: \r\n";
char __flash PName_Str1_Ge[]="Produkt-Bez.: \r\n";

char __flash PName_Str1_Fr[]="Nom du produit: \r\n";
char __flash PName_Str1_It[]="Nome del prodotto: \r\n";
char __flash PName_Str1_Sp[]="Nombre del producto: \r\n";

char __flash Dose_Str1_En[]="Dose: \r\n";
char __flash Dose_Str1_Ge[]="Dosis: \r\n";
char __flash Dose_Str1_Fr[]="Dose: \r\n";
char __flash Dose_Str1_It[]="Dose: \r\n";
char __flash Dose_Str1_Sp[]="Dosis: \r\n";

char __flash PNum_Str_En[]="Product No.:                      Form:\r\n";

// 2016/5/14 星期六 20:56:09--------------------------------
//char __flash PNum_Str_Ge[]="Nr. des Produkts:                 Art:\r\n";
char __flash PNum_Str_Ge[]="Produkt-Nr.:                      Art:\r\n"; 				// 2016/5/26 星期四 18:14:08

char __flash PNum_Str_Fr[]="Nr. de produit:                   Forme:\r\n";
char __flash PNum_Str_It[]="Nr. del prodotto:                 Forma:\r\n";
char __flash PNum_Str_Sp[]="Nr. del producto:                 Forma:\r\n";

char __flash PNum_Str1_En[]="Product No.: \r\n";

// 2016/5/22 星期日 11:13:07--------------------------------
//char __flash PNum_Str1_Ge[]="Nr. des Produkts: \r\n";
char __flash PNum_Str1_Ge[]="Produkt-Nr.: \r\n";

char __flash PNum_Str1_Fr[]="Nr. de produit: \r\n";
char __flash PNum_Str1_It[]="Nr. del prodotto: \r\n";
char __flash PNum_Str1_Sp[]="Nr. del producto: \r\n";

char __flash Form_Str1_En[]="Form: \r\n";
char __flash Form_Str1_Ge[]="Art: \r\n";
char __flash Form_Str1_Fr[]="Forme: \r\n";
char __flash Form_Str1_It[]="Forma: \r\n";
char __flash Form_Str1_Sp[]="Forma: \r\n";

char __flash Operator_Str_En[]="Operator:                         M/C No.:\r\n";

// 2016/5/14 星期六 20:57:54--------------------------------
//char __flash Operator_Str_Ge[]="Benutzer:                         M/C Nr.:\r\n";
char __flash Operator_Str_Ge[]="Benutzer:                         Chargen-Nr.:\r\n";

char __flash Operator_Str_Fr[]="Op\202rateur:                        M/C Nr.:\r\n";
char __flash Operator_Str_It[]="Operatore:                        M/C Nr.:\r\n";
char __flash Operator_Str_Sp[]="Operador:                         M/C Nr.:\r\n";

char __flash Operator_Str1_En[]="Operator: \r\n";
char __flash Operator_Str1_Ge[]="Benutzer: \r\n";
char __flash Operator_Str1_Fr[]="Op\202rateur: \r\n";
char __flash Operator_Str1_It[]="Operatore: \r\n";
char __flash Operator_Str1_Sp[]="Operador: \r\n";

char __flash MCNo_Str1_En[]="M/C No.: \r\n";

// 2016/5/26 星期四 18:46:36--------------------------------
//char __flash MCNo_Str1_Ge[]="M/C Nr.: \r\n";
char __flash MCNo_Str1_Ge[]="Chargen-Nr.: \r\n";

char __flash MCNo_Str1_Fr[]="M/C Nr.: \r\n";
char __flash MCNo_Str1_It[]="M/C Nr.: \r\n";
char __flash MCNo_Str1_Sp[]="M/C Nr.: \r\n";

char __flash Pharm_Str_En[]="Pharmacopoeia:                    Medium:\r\n";
char __flash Pharm_Str_Ge[]="Pharmakop\224e:                      Medium:\r\n";
char __flash Pharm_Str_Fr[]="Pharmacop\202e:                      Milieu:\r\n";
char __flash Pharm_Str_It[]="Farmacopea:                       Mezzo:\r\n";
char __flash Pharm_Str_Sp[]="Farmacopea:                       Medio:\r\n";

char __flash Pharm_Str1_En[]="Pharmacopoeia: \r\n";
char __flash Pharm_Str1_Ge[]="Pharmakop\224e: \r\n";
char __flash Pharm_Str1_Fr[]="Pharmacop\202e: \r\n";
char __flash Pharm_Str1_It[]="Farmacopea: \r\n";
char __flash Pharm_Str1_Sp[]="Farmacopea: \r\n";

char __flash Medium_Str1_En[]="Medium: \r\n";
char __flash Medium_Str1_Ge[]="Medium: \r\n";
char __flash Medium_Str1_Fr[]="Milieu: \r\n";
char __flash Medium_Str1_It[]="Mezzo: \r\n";
char __flash Medium_Str1_Sp[]="Medio: \r\n";

// 2017/1/14 12:46:15,在时间格式变量里面增加了min字符，这里就不加了；
//char __flash Speed_Str_En[]="Speed:         %03d rpm            Duration:    %s min\r\n";
char __flash Speed_Str_En[]="Speed:         %03d rpm            Duration:    %s\r\n";

// 2016/5/14 星期六 20:59:43--------------------------------
//char __flash Speed_Str_Ge[]="Drehzahl:      %03d U/min          Dauer:      %s\r\n";
//char __flash Speed_Str_Ge[]="Drehzahl:      %03d U/min          Testdauer:    %s\r\n"; 			// 2016/5/26 星期四 18:15:19
//char __flash Speed_Str_Ge[]="Drehzahl:      %03d U/min          Drehzahl:     %s\r\n"; 			// 2016/6/16 15:12:38
//char __flash Speed_Str_Ge[]="Drehzahl:      %03d U/min          Dauer:        %s min\r\n"; 	// 2017-1-11 星期三 8:47:05
	char __flash Speed_Str_Ge[]="Drehzahl:      %03d U/min          Dauer:        %s\r\n"; 	// 2017/1/14 12:48:33

char __flash Speed_Str_Fr[]="Vitesse:       %03d r.p.m.         Dur\202e:      %s\r\n";
char __flash Speed_Str_It[]="Velocit\133:      %03d giri/min       Durata:     %s\r\n";
char __flash Speed_Str_Sp[]="Velocidad:     %03d rev./min.      Duraci\xA2n:   %s\r\n";

// 2017/1/14 12:49:36,在时间格式变量里面增加了min字符，这里就不加了；
//char __flash Durat_Str1_En[]="Duration:      %s min\r\n";
//char __flash Durat_Str1_Ge[]="Dauer:         %s min\r\n";
char __flash Durat_Str1_En[]="Duration:      %s\r\n";
char __flash Durat_Str1_Ge[]="Dauer:         %s\r\n";

char __flash Durat_Str1_Fr[]="Dur\202e:         %s\r\n";
char __flash Durat_Str1_It[]="Durata:        %s\r\n";
char __flash Durat_Str1_Sp[]="Duraci\xA2n:      %s\r\n";

char __flash Speed_Str1_En[]="Speed:         %03d rpm\r\n";

// 2016/5/22 星期日 11:16:45--------------------------------
//char __flash Speed_Str1_Ge[]="Drehzahl:      %03d Upm\r\n";
//char __flash Speed_Str1_Ge[]="Testdauer:     %03d U/min\r\n"; 				// 2016/5/22 星期日 11:16:45
  char __flash Speed_Str1_Ge[]="Drehzahl:      %03d U/min\r\n"; 				// 2016/6/16 15:23:21

char __flash Speed_Str1_Fr[]="Vitesse:       %03d r.p.m.\r\n";
char __flash Speed_Str1_It[]="Velocit\133:      %03d giri/min\r\n";
char __flash Speed_Str1_Sp[]="Velocidad:     %03d rev./min.\r\n";

char __flash Temp_Str_En[]="Temperature:   %4.1f鳦             Method:      %s\r\n";
char __flash Temp_Str_Ge[]="Temperatur:    %4.1f鳦             Methode:      %s\r\n";
char __flash Temp_Str_Fr[]="Temp\202rature:   %4.1f鳦             M\202thode:    %s\r\n";
char __flash Temp_Str_It[]="Temperatura:   %4.1f鳦             Metodo:     %s\r\n";
char __flash Temp_Str_Sp[]="Temperatura:   %4.1f鳦             M\202todo:     %s\r\n";

char __flash Temp_Str1_En[]="Temperature:   %4.1f鳦\r\n";
char __flash Temp_Str1_Ge[]="Temperatur:    %4.1f鳦\r\n";
char __flash Temp_Str1_Fr[]="Temp\202rature:   %4.1f鳦\r\n";
char __flash Temp_Str1_It[]="Temperatura:   %4.1f鳦\r\n";
char __flash Temp_Str1_Sp[]="Temperatura:   %4.1f鳦\r\n";

char __flash Method_Str1_En[]="Method:        %s\r\n";
char __flash Method_Str1_Ge[]="Methode:       %s\r\n";
char __flash Method_Str1_Fr[]="M\202thode:       %s\r\n";
char __flash Method_Str1_It[]="Metodo:        %s\r\n";
char __flash Method_Str1_Sp[]="M\202todo:        %s\r\n";

// 2016/6/16 14:44:20-------------------------------------------------
//char __flash Volume_Str_En[]="Volume:        %s ml            Head mode:  %s\r\n";
  char __flash Volume_Str_En[]="Volume:        %s ml            Device type: %s\r\n";

//char __flash Volume_Str_Ge[]="Volumen:       %s ml            Kopf Modus:   %s\r\n"; 				// 2016/4/20 星期三 14:55:28
  char __flash Volume_Str_Ge[]="Volumen:       %s ml            Ger\204tetyp:    %s\r\n"; 				// 2016.6.16 15:30:02

char __flash Volume_Str_Fr[]="Volume:        %s ml            Head mode:  %s\r\n";
char __flash Volume_Str_It[]="Volume:        %s ml            Head mode:  %s\r\n";
char __flash Volume_Str_Sp[]="Volumen:       %s ml            Head mode:  %s\r\n";

char __flash Volume_Str1_En[]="Volume:        %s ml\r\n";
char __flash Volume_Str1_Ge[]="Volumen:       %s ml\r\n"; 																// 2016/4/20 星期三 14:55:50
char __flash Volume_Str1_Fr[]="Volume:        %s ml\r\n";
char __flash Volume_Str1_It[]="Volume:        %s ml\r\n";
char __flash Volume_Str1_Sp[]="Volumen:       %s ml\r\n";

// 2016/6/16 15:06:38-------------------------------------------------
//char __flash HdMode_Str1_En[]="Head mode:     %s\r\n";
char __flash HdMode_Str1_En[]="Device type:   %s\r\n";

//char __flash HdMode_Str1_Ge[]="Kopf Modus:    %s\r\n"; 																		// 2016/4/20 星期三 15:11:02
  char __flash HdMode_Str1_Ge[]="Ger\204tetyp:     %s\r\n"; 																		// 2016.6.16 15:32:15

char __flash HdMode_Str1_Fr[]="Head mode:     %s\r\n";
char __flash HdMode_Str1_It[]="Head mode:     %s\r\n";
char __flash HdMode_Str1_Sp[]="Head mode:     %s\r\n";

//2017/1/14 11:27:13,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数
//char __flash TempOff_Str_En[]="Temp. offset:  %3.1f鳦              Interval:    %d min.\r\n"; 	// 2016-6-22 星期三 10:18:40
char __flash TempOff_Str_En[]="Temp. offset:  %3.1f鳦 \r\n"; 	// 2017/1/14 11:28:27

// 2016/5/14 星期六 21:02:01--------------------------------
//char __flash TempOff_Str_Ge[]="Temp. offset:  %3.1f鳦              Messzyklus: %d min.\r\n";
//char __flash TempOff_Str_Ge[]="Temp. offset:  %3.1f鳦              Reportzyklus: %d min.\r\n";
char __flash TempOff_Str_Ge[]="Temp. offset:  %3.1f鳦 \r\n"; 	// 2017/1/14 11:29:13

//char __flash TempOff_Str_Fr[]="Temp. offset:  %3.1f鳦              Intervalle: %d min.\r\n";
//char __flash TempOff_Str_It[]="Temp. offset:  %3.1f鳦              Intervallo: %d min.\r\n";
//char __flash TempOff_Str_Sp[]="Temp. offset:  %3.1f鳦              Intervalo:  %d min.\r\n";

char __flash TempOff_Str_Fr[]="Temp. offset:  %3.1f鳦 \r\n"; 	// 2017/1/14 11:30:12
char __flash TempOff_Str_It[]="Temp. offset:  %3.1f鳦 \r\n"; 	// 2017/1/14 11:30:19
char __flash TempOff_Str_Sp[]="Temp. offset:  %3.1f鳦 \r\n"; 	// 2017/1/14 11:30:23

char __flash TempOff_Str1_En[]="Temp. offset:  %3.1f鳦\r\n";
char __flash TempOff_Str1_Ge[]="Temp. offset:  %3.1f鳦\r\n";
char __flash TempOff_Str1_Fr[]="Temp. offset:  %3.1f鳦\r\n";
char __flash TempOff_Str1_It[]="Temp. offset:  %3.1f鳦\r\n";
char __flash TempOff_Str1_Sp[]="Temp. offset:  %3.1f鳦\r\n";

//微打报告，2017/1/14 11:31:12,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数
/*char __flash Interval_Str1_En[]="Interval:      %d min.\r\n";

// 2016/5/22 星期日 11:19:24--------------------------------
//char __flash Interval_Str1_Ge[]="Messzyklus:    %d min.\r\n";
char __flash Interval_Str1_Ge[]="Reportzyklus:    %d min.\r\n";

char __flash Interval_Str1_Fr[]="Intervalle:    %d min.\r\n";
char __flash Interval_Str1_It[]="Intervallo:    %d min.\r\n";
char __flash Interval_Str1_Sp[]="Intervalo:     %d min.\r\n";
*/

char __flash StatHeader_Str_En[]="   Time             Speed                Temperature\r\n";
char __flash StatHeader_Str_Ge[]="   Zeit            Drehzahl              Temperatur\r\n";
char __flash StatHeader_Str_Fr[]="   Temps           Vitesse               Temp\202rature\r\n";
char __flash StatHeader_Str_It[]="   Tempo           Velocit\205              Temperatura\r\n";
char __flash StatHeader_Str_Sp[]="   Tiempo         Velocidad              Temperatura\r\n";

char __flash StatHeader2_Str_En[]="   hh:mm            [rpm]                    [鳦]\r\n";
char __flash StatHeader2_Str_Ge[]="   hh:mm           [U/min]                   [鳦]\r\n";
char __flash StatHeader2_Str_Fr[]="   hh:mm           [r.p.m.]                  [鳦]\r\n";
char __flash StatHeader2_Str_It[]="   hh:mm          [giri/min]                 [鳦]\r\n";
char __flash StatHeader2_Str_Sp[]="   hh:mm          [rev./min.]                [鳦]\r\n";

char __flash Runtime_Str_En[]="Total Run-Time:                %02d:%02d:%02d\r\n";
char __flash Runtime_Str_Ge[]="Gesamtlaufzeit:                %02d:%02d:%02d\r\n";
char __flash Runtime_Str_Fr[]="Dur\202e totale du test:          %02d:%02d:%02d\r\n";
char __flash Runtime_Str_It[]="Tempo del test totale:         %02d:%02d:%02d\r\n";
char __flash Runtime_Str_Sp[]="Tiempo total del test:         %02d:%02d:%02d\r\n";

char __flash Runtime_Str1_En[]="Total Run-Time: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_Ge[]="Gesamtlaufzeit: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_Fr[]="Dur\202e totale du test: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_It[]="Tempo del test totale: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_Sp[]="Tiempo total del test: %02d:%02d:%02d\r\n";

//char __flash SpStat_Str_En[]="Min Speed: %4.1f rpm           Max Speed: %4.1f rpm\r\n";
//char __flash SpStat_Str_Ge[]="Min Drehzahl: %4.1f U/min      Max Drehzahl: %4.1f U/min\r\n";
//char __flash SpStat_Str_Fr[]="Vitesse min.: %4.1f r.p.m.     Vitesse max.: %4.1f r.p.m.\r\n";
//char __flash SpStat_Str_It[]="Velocit\x085 min.: %4.1f g./m. Velocit\x85 mass.: %4.1f g./m.\r\n";
//char __flash SpStat_Str_Sp[]="Velocidad m\xA1n.: %4.1f r./m. Velocidad m\xA0x.: %4.1f r./m.\r\n";

char __flash SpStat_Str_En[]="Min-Max Speed:       %4.1f - %4.1f rpm\r\n";
char __flash SpStat_Str_Ge[]="Min-Max Drehzahl:    %4.1f - %4.1f U/min\r\n";
char __flash SpStat_Str_Fr[]="Vitesse min.-max.:   %4.1f - %4.1f r.p.m.\r\n";
char __flash SpStat_Str_It[]="Velocit\x085 min.-mass.: %4.1f - %4.1f g./m.\r\n";
char __flash SpStat_Str_Sp[]="Velocidad m\xA1n.-m\xA0x.: %4.1f - %4.1f r./m.\r\n";

/* 2016/5/5 星期四 09:07:12
char __flash SpStat_Str1_En[]="Min-Max Speed:   %4.1f-%4.1f rpm\r\n";
char __flash SpStat_Str1_Ge[]="Min-Max Drehzahl:%4.1f-%4.1f U/min\r\n";
char __flash SpStat_Str1_Fr[]="Vitesse min.-max.: %4.1f-%4.1f r.p.m.\r\n";
char __flash SpStat_Str1_It[]="Velocit\x085 min.-mass.: %4.1f-%4.1f g./m.\r\n";
char __flash SpStat_Str1_Sp[]="Velocidad m\xA1n.-m\xA0x.: %4.1f-%4.1f r./m.\r\n";
*/
// 2016/5/5 星期四 09:07:20
char __flash SpStat_Str1_En[]="Min-Max Speed:   %4.1f-%4.1f rpm\r\n";

// 2016/5/22 星期日 11:21:10--------------------------------
//char __flash SpStat_Str1_Ge[]="Min-Max Drehzahl:%4.1f-%4.1f Upm\r\n";
//char __flash SpStat_Str1_Ge[]="Min-Max Drehzahl:%4.1f-%4.1f U/min\r\n";
  char __flash SpStat_Str1_Ge[]="Min-Max Drehz.:%4.1f-%4.1f U/min\r\n"; 			// 2016/6/1 星期三 16:33:22

char __flash SpStat_Str1_Fr[]="Vitesse min.-max.: %4.1f-%4.1f r.p.m.\r\n";
char __flash SpStat_Str1_It[]="Velocit\x085 min.-mass.: %4.1f-%4.1f g./m.\r\n";
char __flash SpStat_Str1_Sp[]="Velocidad m\xA1n.-m\xA0x.: %4.1f-%4.1f r./m.\r\n";

//char __flash TmpStat_Str_En[]="Min Temperature: %s鳦       Max Temperature: %s鳦\r\n";
//char __flash TmpStat_Str_Ge[]="Min Temperatur: %s鳦        Max Temperatur: %s鳦\r\n";
//char __flash TmpStat_Str_Fr[]="Temp\202rature min.: %s鳦   Temp\202rature max.: %s鳦\r\n";
//char __flash TmpStat_Str_It[]="Temperatura min.: %s鳦      Temperatura mass.: %s鳦\r\n";
//char __flash TmpStat_Str_Sp[]="Temperatura m\xA1n.: %s鳦   Temperatura m\xA0x.: %s鳦\r\n";

char __flash TmpStat_Str_En[]="Min-Max Temperature: %s - %s鳦\r\n";
char __flash TmpStat_Str_Ge[]="Min-Max Temperatur:  %s - %s鳦\r\n";
char __flash TmpStat_Str_Fr[]="Temp\202rature min.-max.: %s鳦   Temp\202rature max.: %s鳦\r\n";
char __flash TmpStat_Str_It[]="Temperatura min.-mass.: %s鳦      Temperatura mass.: %s鳦\r\n";
char __flash TmpStat_Str_Sp[]="Temperatura m\xA1n.-m\xA0x.: %s鳦   Temperatura m\xA0x.: %s鳦\r\n";

char __flash TmpStat_Str1_En[]="Min-Max Temp.:   %s-%s鳦\r\n";
char __flash TmpStat_Str1_Ge[]="Min-Max Temp.:   %s-%s鳦\r\n";
char __flash TmpStat_Str1_Fr[]="Temp\202rature min.: %s鳦   Temp\202rature max.: %s鳦\r\n";
char __flash TmpStat_Str1_It[]="Temperatura min.: %s鳦      Temperatura mass.: %s鳦\r\n";
char __flash TmpStat_Str1_Sp[]="Temperatura m\xA1n.: %s鳦   Temperatura m\xA0x.: %s鳦\r\n";

char __flash Comments_Str_En[]="Comments:  ________________________________________________\r\n";
char __flash Comments_Str_Ge[]="Bemerkungen:  _____________________________________________\r\n";
char __flash Comments_Str_Fr[]="Observations:  ____________________________________________\r\n";
char __flash Comments_Str_It[]="Annotazioni:  _____________________________________________\r\n";
char __flash Comments_Str_Sp[]="Observaciones:  ___________________________________________\r\n";

char __flash Comments_Str1_En[]="Comments:  _____________________\r\n";
char __flash Comments_Str1_Ge[]="Bemerkungen:  __________________\r\n";
char __flash Comments_Str1_Fr[]="Observations:  _________________\r\n";
char __flash Comments_Str1_It[]="Annotazioni:  __________________\r\n";
char __flash Comments_Str1_Sp[]="Observaciones:  ________________\r\n";

char __flash Signature_Str_En[]="_________________________ Signature\r\n\x0C";
char __flash Signature_Str_Ge[]="_________________________ Unterschrift\r\n\x0C";
char __flash Signature_Str_Fr[]="_________________________ Signature\r\n\x0C";
char __flash Signature_Str_It[]="_________________________ Firma\r\n\x0C";
char __flash Signature_Str_Sp[]="_________________________ Firma\r\n\x0C";

char __flash Signature_Str1_En[]="___________________ Signature\r\n\x0C";
char __flash Signature_Str1_Ge[]="___________________ Unterschrift\r\n\x0C";
char __flash Signature_Str1_Fr[]="___________________ Signature\r\n\x0C";
char __flash Signature_Str1_It[]="___________________ Firma\r\n\x0C";
char __flash Signature_Str1_Sp[]="___________________ Firma\r\n\x0C";

char __flash Signature_Str2_En[]="___________________ Signature\r\n\r\n\x0C";
char __flash Signature_Str2_Ge[]="___________________ Unterschrift\r\n\r\n\x0C";
char __flash Signature_Str2_Fr[]="___________________ Signature\r\n\r\n\x0C";
char __flash Signature_Str2_It[]="___________________ Firma\r\n\r\n\x0C";
char __flash Signature_Str2_Sp[]="___________________ Firma\r\n\r\n\x0C";

char __flash DateSign_Str_En[]="Date:   %s %s     _____________________ Signature\r\n\x0C";
char __flash DateSign_Str_Ge[]="Datum:  %s %s     _____________________ Unterschrift\r\n\x0C";
char __flash DateSign_Str_Fr[]="Date:   %s %s     _____________________ Signature\r\n\x0C";
char __flash DateSign_Str_It[]="Data:   %s %s     _____________________ Firma\r\n\x0C";
char __flash DateSign_Str_Sp[]="Fecha:  %s %s     _____________________ Firma\r\n\x0C";

char __flash DateTime_Str1_En[]="Date:   %s Time:  %s\r\n";
char __flash DateTime_Str1_Ge[]="Datum:  %s Zeit:  %s\r\n"; 															// 2016/4/20 星期三 15:26:49
char __flash DateTime_Str1_Fr[]="Date:   %s Time:  %s\r\n";
char __flash DateTime_Str1_It[]="Data:   %s Time:  %s\r\n";
char __flash DateTime_Str1_Sp[]="Fecha:  %s Time:  %s\r\n";

char __flash PrTest_Str_En[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_Ge[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_Fr[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_It[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_Sp[]="                   PRINTER TEST REPORT\r\n";

char __flash PrTest_Str1_En[]="      PRINTER TEST REPORT\r\n";
char __flash PrTest_Str1_Ge[]="      PRINTER TEST REPORT\r\n";
char __flash PrTest_Str1_Fr[]="      PRINTER TEST REPORT\r\n";
char __flash PrTest_Str1_It[]="      PRINTER TEST REPORT\r\n";
char __flash PrTest_Str1_Sp[]="      PRINTER TEST REPORT\r\n";

char __flash TimeServ_Str_En[]="Time of service:              %s %s\r\n";
char __flash TimeServ_Str_Ge[]="Time of service:              %s %s\r\n";
char __flash TimeServ_Str_Fr[]="Time of service:              %s %s\r\n";
char __flash TimeServ_Str_It[]="Time of service:              %s %s\r\n";
char __flash TimeServ_Str_Sp[]="Time of service:              %s %s\r\n";

char __flash TimeServ_Str1_En[]="Service Time: %s %s\r\n";
char __flash TimeServ_Str1_Ge[]="Service Zeit: %s %s\r\n"; 																// 2016/4/20 星期三 15:43:21
char __flash TimeServ_Str1_Fr[]="Service Time: %s %s\r\n";
char __flash TimeServ_Str1_It[]="Service Time: %s %s\r\n";
char __flash TimeServ_Str1_Sp[]="Service Time: %s %s\r\n";

char __flash Used_Str_En[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str_Ge[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str_Fr[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str_It[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str_Sp[]="Used during: %03d days %02d:%02d\r\n";

char __flash Used_Str1_En[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str1_Ge[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str1_Fr[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str1_It[]="Used during: %03d days %02d:%02d\r\n";
char __flash Used_Str1_Sp[]="Used during: %03d days %02d:%02d\r\n";

char __flash LED_Str_En[]="LED Brightness:   %1d           Number of vessels:  %1d\r\n";
char __flash LED_Str_Ge[]="LED Brightness:   %1d           Number of vessels:  %1d\r\n";
char __flash LED_Str_Fr[]="LED Brightness:   %1d           Number of vessels:  %1d\r\n";
char __flash LED_Str_It[]="LED Brightness:   %1d           Number of vessels:  %1d\r\n";
char __flash LED_Str_Sp[]="LED Brightness:   %1d           Number of vessels:  %1d\r\n";

char __flash LED_Str1_En[]="LED Brightness:     %1d\r\n";
char __flash LED_Str1_Ge[]="LED Brightness:     %1d\r\n";
char __flash LED_Str1_Fr[]="LED Brightness:     %1d\r\n";
char __flash LED_Str1_It[]="LED Brightness:     %1d\r\n";
char __flash LED_Str1_Sp[]="LED Brightness:     %1d\r\n";

char __flash NOV_Str1_En[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Ge[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Fr[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_It[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Sp[]="Number of vessels:  %1d\r\n";

char __flash Sound_Str_En[]="Sound settings:   %s        Alarm settings:     %s\r\n";
char __flash Sound_Str_Ge[]="Sound settings:   %s        Alarm settings:     %s\r\n";
char __flash Sound_Str_Fr[]="Sound settings:   %s        Alarm settings:     %s\r\n";
char __flash Sound_Str_It[]="Sound settings:   %s        Alarm settings:     %s\r\n";
char __flash Sound_Str_Sp[]="Sound settings:   %s        Alarm settings:     %s\r\n";

char __flash Sound_Str1_En[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_Ge[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_Fr[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_It[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_Sp[]="Sound settings:   %s\r\n";

char __flash AmSet_Str1_En[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_Ge[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_Fr[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_It[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_Sp[]="Alarm settings:   %s\r\n";

char __flash Printer_Str_En[]="Printer settings: %s        Date/Time format:   %s\r\n";
char __flash Printer_Str_Ge[]="Printer settings: %s        Date/Time format:   %s\r\n";
char __flash Printer_Str_Fr[]="Printer settings: %s        Date/Time format:   %s\r\n";
char __flash Printer_Str_It[]="Printer settings: %s        Date/Time format:   %s\r\n";
char __flash Printer_Str_Sp[]="Printer settings: %s        Date/Time format:   %s\r\n";

char __flash Printer_Str1_En[]="Printer settings: %s\r\n";
char __flash Printer_Str1_Ge[]="Printer settings: %s\r\n";
char __flash Printer_Str1_Fr[]="Printer settings: %s\r\n";
char __flash Printer_Str1_It[]="Printer settings: %s\r\n";
char __flash Printer_Str1_Sp[]="Printer settings: %s\r\n";

char __flash DTimeF_Str1_En[]="Date/Time format:   %s\r\n";
char __flash DTimeF_Str1_Ge[]="Date/Time format:   %s\r\n";
char __flash DTimeF_Str1_Fr[]="Date/Time format:   %s\r\n";
char __flash DTimeF_Str1_It[]="Date/Time format:   %s\r\n";
char __flash DTimeF_Str1_Sp[]="Date/Time format:   %s\r\n";

// 用Infinity替换Stirring；Gavin，v2.03，2021-03-11 星期四 21:11:42
char __flash StMode_Str_En[]="Infinity mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Ge[]="Infinity mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Fr[]="Infinity mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_It[]="Infinity mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Sp[]="Infinity mode:    %s        Time counting mode: %s\r\n";

// 用Infinity替换Stirring；Gavin，v2.03，2021-03-11 星期四 21:11:42
char __flash StMode_Str1_En[]="Infinity mode:    %s\r\n";
char __flash StMode_Str1_Ge[]="Infinity mode:    %s\r\n";
char __flash StMode_Str1_Fr[]="Infinity mode:    %s\r\n";
char __flash StMode_Str1_It[]="Infinity mode:    %s\r\n";
char __flash StMode_Str1_Sp[]="Infinity mode:    %s\r\n";

char __flash TCMode_Str1_En[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Ge[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Fr[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_It[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Sp[]="Time counting mode: %s\r\n";

char __flash PPR_Str_En[]="PPR for stirring motor:  %3d\r\n";
char __flash PPR_Str_Ge[]="Ppr for stirring motor:  %3d\r\n";
char __flash PPR_Str_Fr[]="Ppr for stirring motor:  %3d\r\n";
char __flash PPR_Str_It[]="Ppr for stirring motor:  %3d\r\n";
char __flash PPR_Str_Sp[]="Ppr for stirring motor:  %3d\r\n";

char __flash PPR_Str1_En[]="PPR for stirring motor: %3d\r\n";
char __flash PPR_Str1_Ge[]="Ppr for stirring motor: %3d\r\n";
char __flash PPR_Str1_Fr[]="Ppr for stirring motor: %3d\r\n";
char __flash PPR_Str1_It[]="Ppr for stirring motor: %3d\r\n";
char __flash PPR_Str1_Sp[]="Ppr for stirring motor: %3d\r\n";

char __flash WorkTemp_Str_En[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_Ge[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_Fr[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_It[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_Sp[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";

char __flash WorkTemp_Str1_En[]="Working temp.: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_Ge[]="Working temp.: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_Fr[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_It[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_Sp[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";

char __flash DHC_Str_En[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Ge[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Fr[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_It[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Sp[]="Found DHC:         %s       Found external Pt100: %s\r\n";

char __flash DHC_Str1_En[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Ge[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Fr[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_It[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Sp[]="Found DHC:         %s\r\n";

char __flash EPT_Str1_En[]="Found external Pt100: %s\r\n";
char __flash EPT_Str1_Ge[]="Found external Pt100: %s\r\n";
char __flash EPT_Str1_Fr[]="Found external Pt100: %s\r\n";
char __flash EPT_Str1_It[]="Found external Pt100: %s\r\n";
char __flash EPT_Str1_Sp[]="Found external Pt100: %s\r\n";

char __flash Dropper_Str_En[]="Found Autodropper: %s       Found Autosampler:    %s\r\n";
char __flash Dropper_Str_Ge[]="Found Autodropper: %s       Found Autosampler:    %s\r\n";
char __flash Dropper_Str_Fr[]="Found Autodropper: %s       Found Autosampler:    %s\r\n";
char __flash Dropper_Str_It[]="Found Autodropper: %s       Found Autosampler:    %s\r\n";
char __flash Dropper_Str_Sp[]="Found Autodropper: %s       Found Autosampler:    %s\r\n";

char __flash Dropper_Str1_En[]="Found Autodropper: %s\r\n";
char __flash Dropper_Str1_Ge[]="Found Autodropper: %s\r\n";
char __flash Dropper_Str1_Fr[]="Found Autodropper: %s\r\n";
char __flash Dropper_Str1_It[]="Found Autodropper: %s\r\n";
char __flash Dropper_Str1_Sp[]="Found Autodropper: %s\r\n";

char __flash Sampler_Str1_En[]="Found Autosampler:    %s\r\n";
char __flash Sampler_Str1_Ge[]="Found Autosampler:    %s\r\n";
char __flash Sampler_Str1_Fr[]="Found Autosampler:    %s\r\n";
char __flash Sampler_Str1_It[]="Found Autosampler:    %s\r\n";
char __flash Sampler_Str1_Sp[]="Found Autosampler:    %s\r\n";

char __flash DTAS_Str_En[]="Found DTAS:        %s\r\n";
char __flash DTAS_Str_Ge[]="Found DTAS:        %s\r\n";
char __flash DTAS_Str_Fr[]="Found DTAS:        %s\r\n";
char __flash DTAS_Str_It[]="Found DTAS:        %s\r\n";
char __flash DTAS_Str_Sp[]="Found DTAS:        %s\r\n";

char __flash DTAS_Str1_En[]="Found DTAS:   %s\r\n";
char __flash DTAS_Str1_Ge[]="Found DTAS:   %s\r\n";
char __flash DTAS_Str1_Fr[]="Found DTAS:   %s\r\n";
char __flash DTAS_Str1_It[]="Found DTAS:   %s\r\n";
char __flash DTAS_Str1_Sp[]="Found DTAS:   %s\r\n";

char __flash RepOk_Str_En[]="Testing report is OK:    YES     NO\r\n";
char __flash RepOk_Str_Ge[]="Testbericht ist OK:      JA     NEIN\r\n";
char __flash RepOk_Str_Fr[]="Testing report is OK:    YES     NO\r\n";
char __flash RepOk_Str_It[]="Testing report is OK:    YES     NO\r\n";
char __flash RepOk_Str_Sp[]="Testing report is OK:    YES     NO\r\n";

char __flash RepOk_Str1_En[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_Ge[]="Testbericht ist OK:    JA   NEIN\r\n";
char __flash RepOk_Str1_Fr[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_It[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_Sp[]="Testing report is OK:  YES   NO\r\n";

char __flash CalRep_Str_En[]="                     ADJUSTMENT REPORT\r\n";

// 2016/5/22 星期日 10:32:54--------------------------------
//char __flash CalRep_Str_Ge[]="                     JUSTIERUNGSBERICHT\r\n";
char __flash CalRep_Str_Ge[]="                     Justierung\r\n";

char __flash CalRep_Str_Fr[]="                   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CalRep_Str_It[]="                    RAPPORTO DELLA TARATURA\r\n";
char __flash CalRep_Str_Sp[]="                    INFORME DE CALIBRACION\r\n";

char __flash CalRep_Str1_En[]="       ADJUSTMENT REPORT\r\n";

// 2016/5/22 星期日 11:24:30--------------------------------
//char __flash CalRep_Str1_Ge[]="       JUSTIERUNGSBERICHT\r\n";
char __flash CalRep_Str1_Ge[]="       Justierung\r\n";

char __flash CalRep_Str1_Fr[]="   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CalRep_Str1_It[]="    RAPPORTO DELLA TARATURA\r\n";
char __flash CalRep_Str1_Sp[]="    INFORME DE CALIBRACION\r\n";

char __flash CalType_Str_En[]="Adjustment %s\r\n";
char __flash CalType_Str_Ge[]="Justierung %s\r\n";
char __flash CalType_Str_Fr[]="Calibrage %s\r\n";
char __flash CalType_Str_It[]="Taratura %s\r\n";
char __flash CalType_Str_Sp[]="Calibraci\xA2n %s\r\n";

char __flash CheckRep_Str_En[]="                    CALIBRATION REPORT\r\n";

// 2016/5/14 星期六 21:16:45--------------------------------
//char __flash CheckRep_Str_Ge[]="                    KALIBRIERUNGSBERICHT\r\n";
char __flash CheckRep_Str_Ge[]="                       KALIBRIERUNG\r\n";

char __flash CheckRep_Str_Fr[]="                   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CheckRep_Str_It[]="                    RAPPORTO DELLA TARATURA\r\n";
char __flash CheckRep_Str_Sp[]="                    INFORME DE CALIBRACION\r\n";

char __flash CheckRep_Str1_En[]="        CALIBRATION REPORT\r\n";

// 2016/5/22 星期日 11:36:11--------------------------------
//char __flash CheckRep_Str1_Ge[]="        KALIBRIERUNGSBERICHT\r\n";
char __flash CheckRep_Str1_Ge[]="        Kalibrierung\r\n";

char __flash CheckRep_Str1_Fr[]="       COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CheckRep_Str1_It[]="        RAPPORTO DELLA TARATURA\r\n";
char __flash CheckRep_Str1_Sp[]="        INFORME DE CALIBRACION\r\n";

char __flash CheckType_Str_En[]="Calibration %s\r\n";
char __flash CheckType_Str_Ge[]="Kalibrierung %s\r\n";
char __flash CheckType_Str_Fr[]="Calibrage %s\r\n";
char __flash CheckType_Str_It[]="Taratura %s\r\n";
char __flash CheckType_Str_Sp[]="Calibraci\xA2n %s\r\n";

char __flash ATemp_Str_En[]="P.%1d, Actual Temp.:    %s\r\n";

// 2016/5/22 星期日 10:52:31--------------------------------
//char __flash ATemp_Str_Ge[]="P.%1d, Echte Temp.:     %s\r\n";
char __flash ATemp_Str_Ge[]="P.%1d, Ist-Temp.:     %s\r\n";

char __flash ATemp_Str_Fr[]="P.%1d, Temp. r\202elle:    %s\r\n";
char __flash ATemp_Str_It[]="P.%1d, Temp. effettiva: %s\r\n";
char __flash ATemp_Str_Sp[]="P.%1d, Temp. real:      %s\r\n";
        
//char __flash NTemp_Str_En[]="P.%1d, Nominal Temp.:   %s\r\n";
char __flash NTemp_Str_En[]="P.%1d, Real Temp.:      %s\r\n"; 				// V200-Q5.1, 2019-2-27 星期三 11:38:39

// 2016/5/22 星期日 10:53:45--------------------------------
//char __flash NTemp_Str_Ge[]="P.%1d, Nominelle Temp.: %s\r\n";
char __flash NTemp_Str_Ge[]="P.%1d, Soll-Temp.:    %s\r\n";

char __flash NTemp_Str_Fr[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str_It[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str_Sp[]="P.%1d, Temp. nominal:   %s\r\n";
        
char __flash CalStatus_Str_En[]="Status: %s\r\n";
char __flash CalStatus_Str_Ge[]="Status: %s\r\n";
char __flash CalStatus_Str_Fr[]="Statut: %s\r\n";
char __flash CalStatus_Str_It[]="Status: %s\r\n";
char __flash CalStatus_Str_Sp[]="Status: %s\r\n";

char __flash CheckAsHeader_Str_En[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str_Ge[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str_Fr[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str_It[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str_Sp[]="Sensor         Readout Temperature       Real Temperature\r\n";

char __flash CheckAsHeader_Str1_En[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str1_Ge[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str1_Fr[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str1_It[]="Sensor         Readout Temperature       Real Temperature\r\n";
char __flash CheckAsHeader_Str1_Sp[]="Sensor         Readout Temperature       Real Temperature\r\n";

//char __flash CheckRep_Str_En[]="                   CALIBRATION CHECK REPORT\r\n";
//char __flash CheckRep_Str_Ge[]="                   KALIBRIERUNGSTESTBERICHT\r\n";
//char __flash CheckRep_Str_Fr[]="                   COMPTE-RENDU DU CALIBRAGE\r\n";
//char __flash CheckRep_Str_It[]="                    RAPPORTO DELLA TARATURA\r\n";
//char __flash CheckRep_Str_Sp[]="                    INFORME DE CALIBRACION\r\n";

//char __flash NomTemp_Str_En[]="Nominal Temperature, 鳦:   %s     %s     %s\r\n";
//char __flash NomTemp_Str_Ge[]="Nominelle Temperatur, 鳦:  %s     %s     %s\r\n";
//char __flash NomTemp_Str_Fr[]="Temp\202rature nominale, 鳦:  %s     %s     %s\r\n";
//char __flash NomTemp_Str_It[]="Temperatura nominale, 鳦:  %s     %s     %s\r\n";
//char __flash NomTemp_Str_Sp[]="Temperatura nominal, 鳦:   %s     %s     %s\r\n";

char __flash NomTemp_Str_En[]="Nominal Temperature, 鳦:       %s\r\n";

// 2016/5/14 星期六 21:52:27--------------------------------
//char __flash NomTemp_Str_Ge[]="Nominelle Temperatur, 鳦:      %s\r\n";
char __flash NomTemp_Str_Ge[]="Soll-Temp.:      %s 鳦\r\n";

char __flash NomTemp_Str_Fr[]="Temp\202rature nominale, 鳦:      %s\r\n";
char __flash NomTemp_Str_It[]="Temperatura nominale, 鳦:      %s\r\n";
char __flash NomTemp_Str_Sp[]="Temperatura nominal, 鳦:       %s\r\n";

/* 2016/5/5 星期四 09:03:26
char __flash NomTemp_Str1_En[]="Nominal Temp., 鳦:   %s\r\n";
char __flash NomTemp_Str1_Ge[]="Nominelle Temp., 鳦: %s\r\n";
char __flash NomTemp_Str1_Fr[]="Temp\202rature nominale, 鳦:      %s\r\n";
char __flash NomTemp_Str1_It[]="Temperatura nominale, 鳦:      %s\r\n";
char __flash NomTemp_Str1_Sp[]="Temperatura nominal, 鳦:       %s\r\n";
*/
// 2016/5/5 星期四 09:03:32
char __flash NomTemp_Str1_En[]="Nominal Temp.:   %s鳦\r\n";

// 2016/5/22 星期日 11:31:28--------------------------------
//char __flash NomTemp_Str1_Ge[]="Nominelle Temp.: %s鳦\r\n";
char __flash NomTemp_Str1_Ge[]="Soll-Temp.:    %s鳦\r\n";

char __flash NomTemp_Str1_Fr[]="Temp\202rature nominale:      %s鳦\r\n";
char __flash NomTemp_Str1_It[]="Temperatura nominale:      %s鳦\r\n";
char __flash NomTemp_Str1_Sp[]="Temperatura nominal:       %s鳦\r\n";

//char __flash ActTemp_Str_En[]="Actual Temperature, 鳦:    %s     %s     %s\r\n";
//char __flash ActTemp_Str_Ge[]="Echte Temperatur, 鳦:      %s     %s     %s\r\n";
//char __flash ActTemp_Str_Fr[]="Temp\202rature r\202elle, 鳦:    %s     %s     %s\r\n";
//char __flash ActTemp_Str_It[]="Temperatura effettiva, 鳦: %s     %s     %s\r\n";
//char __flash ActTemp_Str_Sp[]="Temperatura real, 鳦:      %s     %s     %s\r\n";

char __flash RealTemp_Str_En[]="Real Temperature, 鳦:          %s\r\n";

// 2016/5/14 星期六 21:58:54--------------------------------
//char __flash RealTemp_Str_Ge[]="Echte Temperatur, 鳦:          %s\r\n";
char __flash RealTemp_Str_Ge[]="Ref.-Temp.:      %s 鳦\r\n";

char __flash RealTemp_Str_Fr[]="Temp\202rature r\202elle, 鳦:        %s\r\n";
char __flash RealTemp_Str_It[]="Temperatura effettiva, 鳦:     %s\r\n";
char __flash RealTemp_Str_Sp[]="Temperatura real, 鳦:          %s\r\n";

/* 2016/5/5 星期四 09:04:28
char __flash RealTemp_Str1_En[]="Real Temp., 鳦:      %s\r\n";
char __flash RealTemp_Str1_Ge[]="Echte Temp., 鳦:     %s\r\n";
char __flash RealTemp_Str1_Fr[]="Temp\202rature r\202elle, 鳦:        %s\r\n";
char __flash RealTemp_Str1_It[]="Temperatura effettiva, 鳦:     %s\r\n";
char __flash RealTemp_Str1_Sp[]="Temperatura real, 鳦:          %s\r\n";
*/
// 2016/5/5 星期四 09:04:35
char __flash RealTemp_Str1_En[]="Real Temp.:      %s鳦\r\n";

// 2016/5/22 星期日 11:30:11--------------------------------
//char __flash RealTemp_Str1_Ge[]="Echte Temp.:     %s鳦\r\n";
char __flash RealTemp_Str1_Ge[]="Ref.-Temp.:    %s鳦\r\n";

char __flash RealTemp_Str1_Fr[]="Temp\202rature r\202elle:        %s鳦\r\n";
char __flash RealTemp_Str1_It[]="Temperatura effettiva:     %s鳦\r\n";
char __flash RealTemp_Str1_Sp[]="Temperatura real:          %s鳦\r\n";

char __flash ReadTemp_Str_En[]="Readout Temperature, 鳦:       %s\r\n";

// 2016/5/14 星期六 21:56:57--------------------------------
//char __flash ReadTemp_Str_Ge[]="gelesene Temperatur, 鳦:       %s\r\n"; 									// 2016/4/20 星期三 15:20:35
char __flash ReadTemp_Str_Ge[]="Ist-Temp.:       %s 鳦\r\n";

char __flash ReadTemp_Str_Fr[]="Temp\202rature r\202elle, 鳦:        %s\r\n";
char __flash ReadTemp_Str_It[]="Temperatura effettiva, 鳦:     %s\r\n";
char __flash ReadTemp_Str_Sp[]="Temperatura real, 鳦:          %s\r\n";

/* 2016/5/5 星期四 09:02:55
char __flash ReadTemp_Str1_En[]="Readout Temp., 鳦:   %s\r\n";
char __flash ReadTemp_Str1_Ge[]="gelesene Temp., 鳦:  %s\r\n"; 														// 2016/4/20 星期三 15:39:17
char __flash ReadTemp_Str1_Fr[]="Temp\202rature r\202elle, 鳦:        %s\r\n";
char __flash ReadTemp_Str1_It[]="Temperatura effettiva, 鳦:     %s\r\n";
char __flash ReadTemp_Str1_Sp[]="Temperatura real, 鳦:          %s\r\n";
*/
// 2016/5/5 星期四 09:02:49
char __flash ReadTemp_Str1_En[]="Readout Temp.:   %s鳦\r\n";

// 2016/5/22 星期日 11:40:53--------------------------------
//char __flash ReadTemp_Str1_Ge[]="gelesene Temp.:  %s鳦\r\n"; 														// 2016/4/20 星期三 15:39:17
char __flash ReadTemp_Str1_Ge[]="Ist-Temp.:     %s鳦\r\n";

char __flash ReadTemp_Str1_Fr[]="Temp\202rature r\202elle:        %s鳦\r\n";
char __flash ReadTemp_Str1_It[]="Temperatura effettiva:     %s鳦\r\n";
char __flash ReadTemp_Str1_Sp[]="Temperatura real:          %s鳦\r\n";

__flash TReport REPORT_Test_NORM[]=
  {        
    {{Test_Str_En, Test_Str_Ge, Test_Str_Fr, Test_Str_It, Test_Str_Sp},         REP_PAGE},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_Ge, Instrument_Str_Fr, Instrument_Str_It, Instrument_Str_Sp},
                                                                                REP_DEVICE_NAME},
    {{HOT_Str_En, HOT_Str_Ge, HOT_Str_Fr, HOT_Str_It, HOT_Str_Sp}, 							REP_HOT_SN},
    {{VER_Str_En, VER_Str_Ge, VER_Str_Fr, VER_Str_It, VER_Str_Sp},              REP_VERSION},    
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{HeaterCal_Str_En, HeaterCal_Str_Ge, HeaterCal_Str_Fr, HeaterCal_Str_It, HeaterCal_Str_Sp},
                                                                                REP_HEATER_DATETIME},
    {{SensorCal_Str_En, SensorCal_Str_Ge, SensorCal_Str_Fr, SensorCal_Str_It, SensorCal_Str_Sp},
                                                                                REP_SENSOR_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{PName_Str_En, PName_Str_Ge, PName_Str_Fr, PName_Str_It, PName_Str_Sp},    REP_STRING},
    {{PNum_Str_En, PNum_Str_Ge, PNum_Str_Fr, PNum_Str_It, PNum_Str_Sp},         REP_STRING},
    {{Operator_Str_En, Operator_Str_Ge, Operator_Str_Fr, Operator_Str_It, Operator_Str_Sp},
                                                                                REP_STRING},
    {{Pharm_Str_En, Pharm_Str_Ge, Pharm_Str_Fr, Pharm_Str_It, Pharm_Str_Sp},    REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Speed_Str_En, Speed_Str_Ge, Speed_Str_Fr, Speed_Str_It, Speed_Str_Sp},    REP_PRESET_RUNTIME},
    {{Temp_Str_En, Temp_Str_Ge, Temp_Str_Fr, Temp_Str_It, Temp_Str_Sp},         REP_PRESET_TEMP},
    {{Volume_Str_En, Volume_Str_Ge, Volume_Str_Fr, Volume_Str_It, Volume_Str_Sp}, 
                                                                                REP_VOLUME},
    {{TempOff_Str_En, TempOff_Str_Ge, TempOff_Str_Fr, TempOff_Str_It, TempOff_Str_Sp},
                                                                                REP_TEMP_OFFSET},
    //{{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    //{{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STAT_HEADER0},
    //{{StatHeader_Str_En, StatHeader_Str_Ge, StatHeader_Str_Fr, StatHeader_Str_It, StatHeader_Str_Sp},
    //                                                                            REP_STAT_HEADER},
    //{{StatHeader2_Str_En, StatHeader2_Str_Ge, StatHeader2_Str_Fr, StatHeader2_Str_It, StatHeader2_Str_Sp},
    //                                                                            REP_STAT_HEADER2},
    //{{Stat_Str, Stat_Str, Stat_Str, Stat_Str, Stat_Str},                        REP_STAT},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Runtime_Str_En, Runtime_Str_Ge, Runtime_Str_Fr, Runtime_Str_It, Runtime_Str_Sp},
                                                                                REP_TOTAL_RUNTIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{SpStat_Str_En, SpStat_Str_Ge, SpStat_Str_Fr, SpStat_Str_It, SpStat_Str_Sp},
                                                                                REP_MINMAX_SPEED},
    {{TmpStat_Str_En, TmpStat_Str_Ge, TmpStat_Str_Fr, TmpStat_Str_It, TmpStat_Str_Sp},
                                                                                REP_MINMAX_TEMP},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{String_Str, String_Str, String_Str, String_Str, String_Str},              REP_TEST_RESULT},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Comments_Str_En, Comments_Str_Ge, Comments_Str_Fr, Comments_Str_It, Comments_Str_Sp},
                                                                                REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Comment_Str, Comment_Str, Comment_Str, Comment_Str, Comment_Str},         REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Comment_Str, Comment_Str, Comment_Str, Comment_Str, Comment_Str},         REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Signature_Str_En, Signature_Str_Ge, Signature_Str_Fr, Signature_Str_It, Signature_Str_Sp},
                                                                                REP_LAST},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };

__flash TReport REPORT_Test_PORT[]=
{
	{{Test_Str1_En, 			Test_Str1_Ge, 			Test_Str1_Fr, 			Test_Str1_It, 			Test_Str1_Sp}, 			REP_STRING}, 							// 报告标题
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{ISTM_Str1_En, 			ISTM_Str1_Ge, 			ISTM_Str1_Fr, 			ISTM_Str1_It, 			ISTM_Str1_Sp}, 			REP_DEVICE_NAME}, 				// 设备名称
	{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp}, 				REP_HOT_SN}, 							// 加热器序列号
  {{VER_Str1_En, 				VER_Str1_Ge, 				VER_Str1_Fr, 				VER_Str1_It, 				VER_Str1_Sp}, 			REP_VERSION}, 						// 软件版本
	{{HTCal_Str1_En, 			HTCal_Str1_Ge, 			HTCal_Str1_Fr, 			HTCal_Str1_It, 			HTCal_Str1_Sp}, 		REP_HEATER_DATETIME}, 		// Heater Adj.:
	{{SSCal_Str1_En, 			SSCal_Str1_Ge, 			SSCal_Str1_Fr, 			SSCal_Str1_It, 			SSCal_Str1_Sp}, 		REP_SENSOR_DATETIME}, 		// Sensor Adj.:
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{PName_Str1_En, 			PName_Str1_Ge, 			PName_Str1_Fr, 			PName_Str1_It, 			PName_Str1_Sp}, 		REP_STRING}, 							// Product Name:
	{{Dose_Str1_En, 			Dose_Str1_Ge, 			Dose_Str1_Fr, 			Dose_Str1_It, 			Dose_Str1_Sp}, 			REP_STRING}, 							// Dose:
	{{PNum_Str1_En, 			PNum_Str1_Ge, 			PNum_Str1_Fr, 			PNum_Str1_It, 			PNum_Str1_Sp}, 			REP_STRING}, 							// Product No.:
	{{Form_Str1_En, 			Form_Str1_Ge, 			Form_Str1_Fr, 			Form_Str1_It, 			Form_Str1_Sp}, 			REP_STRING}, 							// Form:
	{{Operator_Str1_En, 	Operator_Str1_Ge, 	Operator_Str1_Fr, 	Operator_Str1_It, 	Operator_Str1_Sp}, 	REP_STRING}, 							// Operator:
  {{MCNo_Str1_En, 			MCNo_Str1_Ge, 			MCNo_Str1_Fr, 			MCNo_Str1_It, 			MCNo_Str1_Sp}, 			REP_STRING}, 							// M/C No.:
  {{Pharm_Str1_En, 			Pharm_Str1_Ge, 			Pharm_Str1_Fr, 			Pharm_Str1_It, 			Pharm_Str1_Sp}, 		REP_STRING}, 							// Pharmacopoeia:
	{{Medium_Str1_En, 		Medium_Str1_Ge, 		Medium_Str1_Fr, 		Medium_Str1_It, 		Medium_Str1_Sp}, 		REP_STRING}, 							// Medium:
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Durat_Str1_En, 			Durat_Str1_Ge, 			Durat_Str1_Fr, 			Durat_Str1_It, 			Durat_Str1_Sp}, 		REP_PRESET_RUNTIME}, 			// 设置时间
	{{Speed_Str1_En, 			Speed_Str1_Ge, 			Speed_Str1_Fr, 			Speed_Str1_It, 			Speed_Str1_Sp}, 		REP_PRESET_SPEED}, 				// 设置转速
	{{Temp_Str1_En, 			Temp_Str1_Ge, 			Temp_Str1_Fr, 			Temp_Str1_It, 			Temp_Str1_Sp}, 			REP_PRESET_TEMP}, 				// 设置温度
	{{Method_Str1_En, 		Method_Str1_Ge, 		Method_Str1_Fr, 		Method_Str1_It, 		Method_Str1_Sp}, 		REP_METHOD}, 							// 测试方法
	{{Volume_Str1_En, 		Volume_Str1_Ge, 		Volume_Str1_Fr, 		Volume_Str1_It, 		Volume_Str1_Sp}, 		REP_VOLUME}, 							// 体积
	{{HdMode_Str1_En, 		HdMode_Str1_Ge, 		HdMode_Str1_Fr, 		HdMode_Str1_It, 		HdMode_Str1_Sp}, 		REP_HEAD}, 								// 头模式
	{{TempOff_Str1_En, 		TempOff_Str1_Ge, 		TempOff_Str1_Fr, 		TempOff_Str1_It, 		TempOff_Str1_Sp}, 	REP_TEMP_OFFSET}, 				// 温度偏移
	//2017/1/14 11:36:48,客户要求不再需要转速和温度的更新频率参数,同时不打印对应参数
	//{{Interval_Str1_En, 	Interval_Str1_Ge, 	Interval_Str1_Fr, 	Interval_Str1_It, 	Interval_Str1_Sp}, 	REP_INTERVAL}, 						// 采样间隔
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Runtime_Str1_En, 		Runtime_Str1_Ge, 		Runtime_Str1_Fr, 		Runtime_Str1_It, 		Runtime_Str1_Sp}, 	REP_TOTAL_RUNTIME}, 			// 测试完整时间
	{{SpStat_Str1_En, 		SpStat_Str1_Ge, 		SpStat_Str1_Fr, 		SpStat_Str1_It, 		SpStat_Str1_Sp}, 		REP_MINMAX_SPEED}, 				// 速度极限范围
	{{TmpStat_Str1_En, 		TmpStat_Str1_Ge, 		TmpStat_Str1_Fr, 		TmpStat_Str1_It, 		TmpStat_Str1_Sp}, 	REP_MINMAX_TEMP}, 				// 温度极限范围
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{String_Str, 				String_Str, 				String_Str, 				String_Str, 				String_Str}, 				REP_TEST_RESULT}, 				// 测试结果判定
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Comments_Str1_En, 	Comments_Str1_Ge, 	Comments_Str1_Fr, 	Comments_Str1_It, 	Comments_Str1_Sp}, 	REP_STRING}, 							// 评价
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Comment_Str1, 			Comment_Str1, 			Comment_Str1, 			Comment_Str1, 			Comment_Str1}, 			REP_STRING}, 							// 下划线
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Comment_Str1, 			Comment_Str1, 			Comment_Str1, 			Comment_Str1, 			Comment_Str1}, 			REP_STRING}, 							// 下划线
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{DateTime_Str1_En, 	DateTime_Str1_Ge, 	DateTime_Str1_Fr, 	DateTime_Str1_It, 	DateTime_Str1_Sp}, 	REP_CUR_DATETIME}, 				// 当前时间
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 						REP_STRING}, 							// 空行
	{{Signature_Str1_En, 	Signature_Str1_Ge, 	Signature_Str1_Fr, 	Signature_Str1_It, 	Signature_Str1_Sp}, REP_LAST}, 								// 签名
	{{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str}, 				REP_END} 									// 结束
};

__flash TReport REPORT_Printer_NORM[]=
  {        
    {{PrTest_Str_En, PrTest_Str_En, PrTest_Str_En, PrTest_Str_En, PrTest_Str_En},
                                                                                REP_STRING},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_En, Instrument_Str_En, Instrument_Str_En, Instrument_Str_En},
                                                                                REP_DEVICE_NAME},
    {{HOT_Str_En, HOT_Str_Ge, HOT_Str_Fr, HOT_Str_It, HOT_Str_Sp}, 							REP_HOT_SN}, 							// 加热器序列号
  	{{VER_Str_En, VER_Str_En, VER_Str_En, VER_Str_En, VER_Str_En},              REP_VERSION},    
    {{TimeServ_Str_En, TimeServ_Str_En, TimeServ_Str_En, TimeServ_Str_En, TimeServ_Str_En},
                                                                                REP_SERVICE_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{HeaterCal_Str_En, HeaterCal_Str_Ge, HeaterCal_Str_Fr, HeaterCal_Str_It, HeaterCal_Str_Sp}, 							// 2016/4/20 星期三 15:17:16
                                                                                REP_HEATER_DATETIME},
    {{SensorCal_Str_En, SensorCal_Str_Ge, SensorCal_Str_Fr, SensorCal_Str_It, SensorCal_Str_Sp}, 							// 2016/4/20 星期三 15:18:28
                                                                                REP_SENSOR_DATETIME},
    // Gavin 2013/12/29 17:05:52------------------------------------------------
    //{{AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En},    REP_ASAMPLER_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Used_Str_En, Used_Str_En, Used_Str_En, Used_Str_En, Used_Str_En},         REP_USAGE_COUNTER},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{LED_Str_En, LED_Str_En, LED_Str_En, LED_Str_En, LED_Str_En},              REP_SETTINGS_LED},
    {{Sound_Str_En, Sound_Str_En, Sound_Str_En, Sound_Str_En, Sound_Str_En},    REP_SETTINGS_SOUND},
    {{Printer_Str_En, Printer_Str_En, Printer_Str_En, Printer_Str_En, Printer_Str_En}, 
                                                                                REP_SETTINGS_PRINT},
    {{StMode_Str_En, StMode_Str_En, StMode_Str_En, StMode_Str_En, StMode_Str_En}, 
                                                                                REP_SETTINGS_MODE},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{PPR_Str_En, PPR_Str_En, PPR_Str_En, PPR_Str_En, PPR_Str_En},              REP_SETTINGS_PPR},
    {{WorkTemp_Str_En, WorkTemp_Str_En, WorkTemp_Str_En, WorkTemp_Str_En, WorkTemp_Str_En},
                                                                                REP_SETTINGS_TEMP},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DHC_Str_En, DHC_Str_En, DHC_Str_En, DHC_Str_En, DHC_Str_En},              REP_SETTINGS_DHC},
    
    // Gavin 2013/12/29 17:07:14------------------------------------------------
    //{{Dropper_Str_En, Dropper_Str_En, Dropper_Str_En, Dropper_Str_En, Dropper_Str_En}, 
    //                                                                            REP_SETTINGS_DROPPER},
    //{{DTAS_Str_En, DTAS_Str_En, DTAS_Str_En, DTAS_Str_En, DTAS_Str_En},         REP_SETTINGS_DTAS},
    
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{RepOk_Str_En, RepOk_Str_Ge, RepOk_Str_Fr, RepOk_Str_It, RepOk_Str_Sp},    REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DateSign_Str_En, DateSign_Str_Ge, DateSign_Str_Fr, DateSign_Str_It, DateSign_Str_Sp},
                                                                                REP_CUR_DATETIME},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };

__flash TReport REPORT_Printer_PORT[]=
{
	{{PrTest_Str1_En, 		PrTest_Str1_En, 		PrTest_Str1_En, 		PrTest_Str1_En, 		PrTest_Str1_En}, 			REP_STRING},
	//{{Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1}, 					REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{ISTM_Str1_En, 			ISTM_Str1_En, 			ISTM_Str1_En, 			ISTM_Str1_En, 			ISTM_Str1_En}, 				REP_DEVICE_NAME},
	{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp}, 					REP_HOT_SN}, 							// 加热器序列号
  {{VER_Str1_En, 				VER_Str1_Ge, 				VER_Str1_Fr, 				VER_Str1_It, 				VER_Str1_Sp}, 				REP_VERSION}, 						// 软件版本
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{TimeServ_Str1_En, 	TimeServ_Str1_En, 	TimeServ_Str1_En, 	TimeServ_Str1_En, 	TimeServ_Str1_En}, 		REP_SERVICE_DATETIME},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{HTCal_Str1_En, 			HTCal_Str1_Ge, 			HTCal_Str1_Fr, 			HTCal_Str1_It, 			HTCal_Str1_Sp}, 			REP_HEATER_DATETIME}, 		// 2016/4/22 星期五 10:36:06
	{{SSCal_Str1_En, 			SSCal_Str1_Ge, 			SSCal_Str1_Fr, 			SSCal_Str1_It, 			SSCal_Str1_Sp}, 			REP_SENSOR_DATETIME}, 		// 2016/4/22 星期五 10:39:12
	
	// Gavin 2013/12/29 17:05:52------------------------------------------------
	//{{AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En},    REP_ASAMPLER_DATETIME},
	
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	//{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{Used_Str1_En, 			Used_Str1_En, 			Used_Str1_En, 			Used_Str1_En, 			Used_Str1_En}, 				REP_USAGE_COUNTER},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	//{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{LED_Str1_En, 				LED_Str1_En, 				LED_Str1_En, 				LED_Str1_En, 				LED_Str1_En}, 				REP_SETTINGS_LED},
	{{NOV_Str1_En, 				NOV_Str1_En, 				NOV_Str1_En, 				NOV_Str1_En, 				NOV_Str1_En}, 				REP_SETTINGS_NOV},
	{{Sound_Str1_En, 			Sound_Str1_En, 			Sound_Str1_En, 			Sound_Str1_En, 			Sound_Str1_En}, 			REP_SETTINGS_SOUND},
	{{AmSet_Str1_En, 			AmSet_Str1_En, 			AmSet_Str1_En, 			AmSet_Str1_En, 			AmSet_Str1_En}, 			REP_SETTINGS_ALARM},
	{{Printer_Str1_En, 		Printer_Str1_En, 		Printer_Str1_En, 		Printer_Str1_En, 		Printer_Str1_En}, 		REP_SETTINGS_PRINT},
	{{DTimeF_Str1_En, 		DTimeF_Str1_En, 		DTimeF_Str1_En, 		DTimeF_Str1_En, 		DTimeF_Str1_En}, 			REP_SETTINGS_DFORMAT},
	{{StMode_Str1_En, 		StMode_Str1_En, 		StMode_Str1_En, 		StMode_Str1_En, 		StMode_Str1_En}, 			REP_SETTINGS_MODE},
	{{TCMode_Str1_En, 		TCMode_Str1_En, 		TCMode_Str1_En, 		TCMode_Str1_En, 		TCMode_Str1_En}, 			REP_SETTINGS_MODECOUNT},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{PPR_Str1_En, 				PPR_Str1_En, 				PPR_Str1_En, 				PPR_Str1_En, 				PPR_Str1_En}, 				REP_SETTINGS_PPR},
	{{WorkTemp_Str1_En, 	WorkTemp_Str1_En, 	WorkTemp_Str1_En, 	WorkTemp_Str1_En, 	WorkTemp_Str1_En}, 		REP_SETTINGS_TEMP},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{DHC_Str1_En, 				DHC_Str1_En, 				DHC_Str1_En, 				DHC_Str1_En, 				DHC_Str1_En}, 				REP_SETTINGS_DHC},
	{{EPT_Str1_En, 				EPT_Str1_En, 				EPT_Str1_En, 				EPT_Str1_En, 				EPT_Str1_En}, 				REP_SETTINGS_EPT},
	
	// Gavin 2013/12/29 17:07:14------------------------------------------------
	//{{Dropper_Str_En, Dropper_Str_En, Dropper_Str_En, Dropper_Str_En, Dropper_Str_En}, 
	//                                                                            REP_SETTINGS_DROPPER},
	//{{DTAS_Str_En, DTAS_Str_En, DTAS_Str_En, DTAS_Str_En, DTAS_Str_En},         REP_SETTINGS_DTAS},
	
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	//{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{RepOk_Str1_En, 			RepOk_Str1_Ge, 			RepOk_Str1_Fr, 			RepOk_Str1_It, 			RepOk_Str1_Sp}, 			REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	//{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{DateTime_Str1_En, 	DateTime_Str1_Ge, 	DateTime_Str1_Fr, 	DateTime_Str1_It, 	DateTime_Str1_Sp}, 		REP_CUR_DATETIME},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{Signature_Str1_En, 	Signature_Str1_Ge, 	Signature_Str1_Fr, 	Signature_Str1_It, 	Signature_Str1_Sp}, 	REP_LAST}, 								// 签名
	{{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str}, 					REP_END}
};

__flash TReport REPORT_Calibr_NORM[]=
  {
    {{CalRep_Str_En, CalRep_Str_Ge, CalRep_Str_Fr, CalRep_Str_It, CalRep_Str_Sp},
                                                                                REP_STRING},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_Ge, Instrument_Str_Fr, Instrument_Str_It, Instrument_Str_Sp},
                                                                                REP_DEVICE_NAME},
    {{HOT_Str_En, HOT_Str_Ge, HOT_Str_Fr, HOT_Str_It, HOT_Str_Sp}, 							REP_HOT_SN}, 							// 加热器序列号
  	{{VER_Str_En, VER_Str_Ge, VER_Str_Fr, VER_Str_It, VER_Str_Sp},              REP_VERSION},    
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalType_Str_En, CalType_Str_Ge, CalType_Str_Fr, CalType_Str_It, CalType_Str_Sp},
                                                                                REP_TEMP_CHANNEL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalHead_Str, CalHead_Str, CalHead_Str, CalHead_Str, CalHead_Str},         REP_VCAL_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{ATemp_Str_En, ATemp_Str_Ge, ATemp_Str_Fr, ATemp_Str_It, ATemp_Str_Sp},    REP_CAL_POINT},
    {{NTemp_Str_En, NTemp_Str_Ge, NTemp_Str_Fr, NTemp_Str_It, NTemp_Str_Sp},    REP_CAL_NOMINAL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalStatus_Str_En, CalStatus_Str_Ge, CalStatus_Str_Fr, CalStatus_Str_It, CalStatus_Str_Sp},
                                                                                REP_CAL_STATUS},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DateSign_Str_En, DateSign_Str_Ge, DateSign_Str_Fr, DateSign_Str_It, DateSign_Str_Sp},
                                                                                REP_CUR_DATETIME},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };

__flash TReport REPORT_Calibr_PORT[]=
{
	{{CalRep_Str1_En, 		CalRep_Str1_Ge, 		CalRep_Str1_Fr, 		CalRep_Str1_It, 		CalRep_Str1_Sp}, 			REP_STRING},
	//{{Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1}, 					REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{ISTM_Str1_En, 			ISTM_Str1_Ge, 			ISTM_Str1_Fr, 			ISTM_Str1_It, 			ISTM_Str1_Sp}, 				REP_DEVICE_NAME},
	{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp}, 					REP_HOT_SN}, 							// 加热器序列号
  {{VER_Str1_En, 				VER_Str1_Ge, 				VER_Str1_Fr, 				VER_Str1_It, 				VER_Str1_Sp}, 				REP_VERSION},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CalType_Str_En, 		CalType_Str_Ge, 		CalType_Str_Fr, 		CalType_Str_It, 		CalType_Str_Sp}, 			REP_TEMP_CHANNEL},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CalHead_Str, 				CalHead_Str, 				CalHead_Str, 				CalHead_Str, 				CalHead_Str}, 				REP_VCAL_STRING}, 			// ?????
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{ATemp_Str_En, 			ATemp_Str_Ge, 			ATemp_Str_Fr, 			ATemp_Str_It, 			ATemp_Str_Sp}, 				REP_CAL_POINT},
	{{NTemp_Str_En, 			NTemp_Str_Ge, 			NTemp_Str_Fr, 			NTemp_Str_It, 			NTemp_Str_Sp}, 				REP_CAL_NOMINAL},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CalStatus_Str_En, 	CalStatus_Str_Ge, 	CalStatus_Str_Fr, 	CalStatus_Str_It, 	CalStatus_Str_Sp}, 		REP_CAL_STATUS},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{DateTime_Str1_En, 	DateTime_Str1_Ge, 	DateTime_Str1_Fr, 	DateTime_Str1_It, 	DateTime_Str1_Sp}, 		REP_CUR_DATETIME},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{Signature_Str1_En, 	Signature_Str1_Ge, 	Signature_Str1_Fr, 	Signature_Str1_It, 	Signature_Str1_Sp}, 	REP_LAST}, 								// 签名
	{{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str}, 					REP_END}
};

/*__flash TReport REPORT_Check[]=
  {
    {{CheckRep_Str_En, CheckRep_Str_Ge, CheckRep_Str_Fr, CheckRep_Str_It, CheckRep_Str_Sp},
                                                                                REP_STRING},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_Ge, Instrument_Str_Fr, Instrument_Str_It, Instrument_Str_Sp},
                                                                                REP_DEVICE_NAME},
    {{PCB_Str_En, PCB_Str_Ge, PCB_Str_Fr, PCB_Str_It, PCB_Str_Sp},              REP_VERSION},    
    {{HeaterCal_Str_En, HeaterCal_Str_Ge, HeaterCal_Str_Fr, HeaterCal_Str_It, HeaterCal_Str_Sp},
                                                                                REP_HEATER_DATETIME},
    {{SensorCal_Str_En, SensorCal_Str_Ge, SensorCal_Str_Fr, SensorCal_Str_It, SensorCal_Str_Sp},
                                                                                REP_SENSOR_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{ChPoint_Str, ChPoint_Str, ChPoint_Str, ChPoint_Str, ChPoint_Str},         REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{NomTemp_Str_En, NomTemp_Str_Ge, NomTemp_Str_Fr, NomTemp_Str_It, NomTemp_Str_Sp},
                                                                                REP_CHECK_NOMINAL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{ActTemp_Str_En, ActTemp_Str_Ge, ActTemp_Str_Fr, ActTemp_Str_It, ActTemp_Str_Sp},
                                                                                REP_CHECK_REAL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalStatus_Str_En, CalStatus_Str_Ge, CalStatus_Str_Fr, CalStatus_Str_It, CalStatus_Str_Sp},
                                                                                REP_CHECK_STATUS},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DateSign_Str_En, DateSign_Str_Ge, DateSign_Str_Fr, DateSign_Str_It, DateSign_Str_Sp},
                                                                                REP_CUR_DATETIME},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };*/

__flash TReport REPORT_CheckCal_NORM[]=
  {
    {{CheckRep_Str_En, CheckRep_Str_Ge, CheckRep_Str_Fr, CheckRep_Str_It, CheckRep_Str_Sp},
                                                                                REP_STRING},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_Ge, Instrument_Str_Fr, Instrument_Str_It, Instrument_Str_Sp},
                                                                                REP_DEVICE_NAME},
    {{HOT_Str_En, HOT_Str_Ge, HOT_Str_Fr, HOT_Str_It, HOT_Str_Sp}, 							REP_HOT_SN}, 							// 加热器序列号
  	{{VER_Str_En, VER_Str_Ge, VER_Str_Fr, VER_Str_It, VER_Str_Sp},              REP_VERSION},    
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{HeaterCal_Str_En, HeaterCal_Str_Ge, HeaterCal_Str_Fr, HeaterCal_Str_It, HeaterCal_Str_Sp}, 							// 2016/4/20 星期三 15:17:04
                                                                                REP_HEATER_DATETIME},
    {{SensorCal_Str_En, SensorCal_Str_Ge, SensorCal_Str_Fr, SensorCal_Str_It, SensorCal_Str_Sp}, 							// 2016/4/20 星期三 15:18:51
                                                                                REP_SENSOR_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CheckType_Str_En, CheckType_Str_Ge, CheckType_Str_Fr, CheckType_Str_It, CheckType_Str_Sp},
                                                                                REP_CHECK_CHANNEL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{NomTemp_Str_En, NomTemp_Str_Ge, NomTemp_Str_Fr, NomTemp_Str_It, NomTemp_Str_Sp},
                                                                                REP_CHECK_NOMINAL},
    {{ReadTemp_Str_En, ReadTemp_Str_Ge, ReadTemp_Str_Fr, ReadTemp_Str_It, ReadTemp_Str_Sp},
                                                                                REP_CHECK_READ},
    {{RealTemp_Str_En, RealTemp_Str_Ge, RealTemp_Str_Fr, RealTemp_Str_It, RealTemp_Str_Sp},
                                                                                REP_CHECK_REAL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalStatus_Str_En, CalStatus_Str_Ge, CalStatus_Str_Fr, CalStatus_Str_It, CalStatus_Str_Sp},
                                                                                REP_CHECK_STATUS},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DateSign_Str_En, DateSign_Str_Ge, DateSign_Str_Fr, DateSign_Str_It, DateSign_Str_Sp},
                                                                                REP_CUR_DATETIME},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };

__flash TReport REPORT_CheckCal_PORT[]=
{
	{{CheckRep_Str1_En, 	CheckRep_Str1_Ge, 	CheckRep_Str1_Fr, 	CheckRep_Str1_It, 	CheckRep_Str1_Sp}, 		REP_STRING},
	//{{Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1}, 					REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{ISTM_Str1_En, 			ISTM_Str1_Ge, 			ISTM_Str1_Fr, 			ISTM_Str1_It, 			ISTM_Str1_Sp}, 				REP_DEVICE_NAME},
	{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp}, 					REP_HOT_SN}, 							// 加热器序列号
  {{VER_Str1_En, 				VER_Str1_Ge, 				VER_Str1_Fr, 				VER_Str1_It, 				VER_Str1_Sp}, 				REP_VERSION},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{HTCal_Str1_En, 			HTCal_Str1_Ge, 			HTCal_Str1_Fr, 			HTCal_Str1_It, 			HTCal_Str1_Sp}, 			REP_HEATER_DATETIME}, 	// 2016/4/22 星期五 10:36:26
	{{SSCal_Str1_En, 			SSCal_Str1_Ge, 			SSCal_Str1_Fr, 			SSCal_Str1_It, 			SSCal_Str1_Sp}, 			REP_SENSOR_DATETIME}, 	// 2016/4/22 星期五 10:39:32
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CheckType_Str_En, 	CheckType_Str_Ge, 	CheckType_Str_Fr, 	CheckType_Str_It, 	CheckType_Str_Sp}, 		REP_CHECK_CHANNEL},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{NomTemp_Str1_En, 		NomTemp_Str1_Ge, 		NomTemp_Str1_Fr, 		NomTemp_Str1_It, 		NomTemp_Str1_Sp}, 		REP_CHECK_NOMINAL},
	{{ReadTemp_Str1_En, 	ReadTemp_Str1_Ge, 	ReadTemp_Str1_Fr, 	ReadTemp_Str1_It, 	ReadTemp_Str1_Sp}, 		REP_CHECK_READ},
	{{RealTemp_Str1_En, 	RealTemp_Str1_Ge, 	RealTemp_Str1_Fr, 	RealTemp_Str1_It, 	RealTemp_Str1_Sp}, 		REP_CHECK_REAL},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CalStatus_Str_En, 	CalStatus_Str_Ge, 	CalStatus_Str_Fr, 	CalStatus_Str_It, 	CalStatus_Str_Sp}, 		REP_CHECK_STATUS},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{DateTime_Str1_En, 	DateTime_Str1_Ge, 	DateTime_Str1_Fr, 	DateTime_Str1_It, 	DateTime_Str1_Sp}, 		REP_CUR_DATETIME},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{Signature_Str2_En, 	Signature_Str2_Ge, 	Signature_Str2_Fr, 	Signature_Str2_It, 	Signature_Str2_Sp}, 	REP_LAST}, 								// 签名
	{{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str}, 					REP_END}
};

__flash TReport REPORT_CheckCalAs_NORM[]=
  {
    {{CheckRep_Str_En, CheckRep_Str_Ge, CheckRep_Str_Fr, CheckRep_Str_It, CheckRep_Str_Sp},
                                                                                REP_STRING},
    {{Line_Str, Line_Str, Line_Str, Line_Str, Line_Str},                        REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{Instrument_Str_En, Instrument_Str_Ge, Instrument_Str_Fr, Instrument_Str_It, Instrument_Str_Sp},
                                                                                REP_DEVICE_NAME},
    {{HOT_Str_En, HOT_Str_Ge, HOT_Str_Fr, HOT_Str_It, HOT_Str_Sp}, 							REP_HOT_SN}, 							// 加热器序列号
  	{{VER_Str_En, VER_Str_Ge, VER_Str_Fr, VER_Str_It, VER_Str_Sp},              REP_VERSION},    
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En, AsCal_Str_En},    REP_ASAMPLER_DATETIME},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CheckType_Str_En, CheckType_Str_Ge, CheckType_Str_Fr, CheckType_Str_It, CheckType_Str_Sp},
                                                                                REP_CHECK_CHANNEL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{NomTemp_Str_En, NomTemp_Str_Ge, NomTemp_Str_Fr, NomTemp_Str_It, NomTemp_Str_Sp},
                                                                                REP_CHECK_NOMINAL},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CheckAsHeader_Str_En, CheckAsHeader_Str_Ge, CheckAsHeader_Str_Fr, CheckAsHeader_Str_It, CheckAsHeader_Str_Sp}, 
                                                                                REP_STRING},
    {{CheckAsHeader_Str, CheckAsHeader_Str, CheckAsHeader_Str, CheckAsHeader_Str, CheckAsHeader_Str}, 
                                                                                REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{AsRes_Str, AsRes_Str, AsRes_Str, AsRes_Str, AsRes_Str},                   REP_CHECK_ASRES},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CalStatus_Str_En, CalStatus_Str_Ge, CalStatus_Str_Fr, CalStatus_Str_It, CalStatus_Str_Sp},
                                                                                REP_CHECK_STATUS},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{CR_Str, CR_Str, CR_Str, CR_Str, CR_Str},                                  REP_STRING},
    {{DateSign_Str_En, DateSign_Str_Ge, DateSign_Str_Fr, DateSign_Str_It, DateSign_Str_Sp},
                                                                                REP_CUR_DATETIME},
    {{Empty_Str, Empty_Str, Empty_Str, Empty_Str, Empty_Str},                   REP_END}
  };

__flash TReport REPORT_CheckCalAs_PORT[]=
{
	{{CheckRep_Str1_En, 			CheckRep_Str1_Ge, 			CheckRep_Str1_Fr, 			CheckRep_Str1_It, 			CheckRep_Str1_Sp}, 				REP_STRING},
	//{{Line_Str1, 							Line_Str1, 							Line_Str1, 							Line_Str1, 							Line_Str1}, 							REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{ISTM_Str1_En, 					ISTM_Str1_Ge, 					ISTM_Str1_Fr, 					ISTM_Str1_It, 					ISTM_Str1_Sp}, 						REP_DEVICE_NAME},
	{{HOT_Str_En, 						HOT_Str_Ge, 						HOT_Str_Fr, 						HOT_Str_It, 						HOT_Str_Sp}, 							REP_HOT_SN}, 							// 加热器序列号
  {{VER_Str1_En, 						VER_Str1_Ge, 						VER_Str1_Fr, 						VER_Str1_It, 						VER_Str1_Sp}, 						REP_VERSION},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{AsCal_Str1_En, 					AsCal_Str1_En, 					AsCal_Str1_En, 					AsCal_Str1_En, 					AsCal_Str1_En}, 					REP_ASAMPLER_DATETIME},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CheckType_Str_En, 			CheckType_Str_Ge, 			CheckType_Str_Fr, 			CheckType_Str_It, 			CheckType_Str_Sp}, 				REP_CHECK_CHANNEL},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{NomTemp_Str1_En, 				NomTemp_Str1_Ge, 				NomTemp_Str1_Fr, 				NomTemp_Str1_It, 				NomTemp_Str1_Sp}, 				REP_CHECK_NOMINAL},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CheckAsHeader_Str1_En, 	CheckAsHeader_Str1_Ge, 	CheckAsHeader_Str1_Fr, 	CheckAsHeader_Str1_It, 	CheckAsHeader_Str1_Sp}, 	REP_STRING},
	{{CheckAsHeader_Str1, 		CheckAsHeader_Str1, 		CheckAsHeader_Str1, 		CheckAsHeader_Str1, 		CheckAsHeader_Str1}, 			REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{AsRes_Str1, 						AsRes_Str1, 						AsRes_Str1, 						AsRes_Str1, 						AsRes_Str1}, 							REP_CHECK_ASRES},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CalStatus_Str_En, 			CalStatus_Str_Ge, 			CalStatus_Str_Fr, 			CalStatus_Str_It, 			CalStatus_Str_Sp}, 				REP_CHECK_STATUS},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{DateTime_Str1_En, 			DateTime_Str1_Ge, 			DateTime_Str1_Fr, 			DateTime_Str1_It, 			DateTime_Str1_Sp}, 				REP_CUR_DATETIME},
	{{CR_Str, 								CR_Str, 								CR_Str, 								CR_Str, 								CR_Str}, 									REP_STRING},
	{{Signature_Str1_En, 			Signature_Str1_Ge, 			Signature_Str1_Fr, 			Signature_Str1_It, 			Signature_Str1_Sp}, 			REP_LAST}, 								// 签名
	{{Empty_Str, 							Empty_Str, 							Empty_Str, 							Empty_Str, 							Empty_Str}, 							REP_END}
};

/*__flash TReport TestReport[]=
  {        
    {{"                       TEST REPORT               Page %d/%d\r\n", 
      "                       TESTBERICHT              Seite %d/%d\r\n", 
      "                   COMPTE-RENDU DU TEST          Page %d/%d\r\n", 
      "                     RAPPORTO DEL TEST         Pagina %d/%d\r\n", 
      "                     INFORME DEL TEST          P\xA0gina %d/%d\r\n"}, REP_PAGE},
    {{"_____________________________________________________________\r\n",
      "_____________________________________________________________\r\n",
      "_____________________________________________________________\r\n",
      "_____________________________________________________________\r\n",
      "_____________________________________________________________\r\n"},REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    //{{"                                    Date:  %s %s\r\n",
    //  "                                    Datum: %s %s\r\n",
    //  "                                    Date:  %s %s\r\n",
    //  "                                    Data:  %s %s\r\n",
    //  "                                    Fecha: %s %s\r\n"},   REP_TEST_DATETIME},
    {{"Instrument:  %s DT72%1d (%s)          %s %s\r\n",
      "Ger\204t:       %s DT72%1d (%s)          %s %s\r\n",
      "Appareil:    %s DT72%1d (%s)          %s %s\r\n",
      "Strumento:   %s DT72%1d (%s)          %s %s\r\n",
      "Aparato:     %s DT72%1d (%s)          %s %s\r\n"},        REP_DEVICE_NAME},
    //{{"Serial No.:                 %s\r\n",
    //  "Serial-Nr.:                 %s\r\n",
    //  "Num\202ro de s\202rie:            %s\r\n",
    //  "Numero di serie:            %s\r\n",
    //  "N\xA3mero de serie:            %s\r\n"},              REP_SERIAL},
    {{"PCB No.:     %s (ver. %s)\r\n",
      "PCB Nr.:     %s (ver. %s)\r\n",
      "PCB Nr.:     %s (ver. %s)\r\n",
      "PCB Nr.:     %s (ver. %s)\r\n",
      "PCB Nr.:     %s (ver. %s)\r\n"},                          REP_VERSION},    
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Heater Calibration:         %s %s\r\n",
      "Kalibration der Heizung:    %s %s\r\n",
      "Calibrage du thermostat:    %s %s\r\n",
      "Taratura del termostato:    %s %s\r\n",
      "Calibraci\xA2n del termostato: %s %s\r\n"},               REP_HEATER_DATETIME},
    {{"Sensor Calibration:         %s %s\r\n",
      "Kalibration des F\x81hlers:    %s %s\r\n",
      "Calibrage du senseur:       %s %s\r\n",
      "Taratura del sensore:       %s %s\r\n",
      "Calibraci\xA2n del sensor:     %s %s\r\n"},               REP_SENSOR_DATETIME},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Product Name:                     Dose:\r\n",
      "Name des Produkts:                Dosis:\r\n",
      "Nom du produit:                   Dose:\r\n",
      "Nome del prodotto:                Dose:\r\n",
      "Nombre del producto:              Dosis:\r\n",},          REP_STRING},
    {{"Product No.:                      Form:\r\n",
      "Nr. des Produkts:                 Art:\r\n",
      "Nr. de produit:                   Forme:\r\n",
      "Nr. del prodotto:                 Forma:\r\n",
      "Nr. del producto:                 Forma:\r\n",},          REP_STRING},
    {{"Operator:                         M/C No.:\r\n",
      "Benutzer:                         M/C Nr.:\r\n",
      "Op\202rateur:                        M/C Nr.:\r\n",
      "Operatore:                        M/C Nr.:\r\n",
      "Operador:                         M/C Nr.:\r\n",},        REP_STRING},
    {{"Pharmacopoeia:                    Medium:\r\n",
      "Pharmakop\224e:                      Medium:\r\n",
      "Pharmacop\202e:                      Milieu:\r\n",
      "Farmacopea:                       Mezzo:\r\n",
      "Farmacopea:                       Medio:\r\n",},          REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Speed:         %03d rpm            Duration:   %s\r\n",
      "Drehzahl:      %03d U/min          Dauer:      %s\r\n",
      "Vitesse:       %03d r.p.m.         Dur\202e:      %s\r\n",
      "Velocit\133:      %03d giri/min       Durata:     %s\r\n",
      "Velocidad:     %03d rev./min.      Duraci\xA2n:   %s\r\n"}, REP_PRESET_RUNTIME},
    {{"Temperature:   %4.1f鳦             Method:     %s\r\n",
      "Temperatur:    %4.1f鳦             Methode:    %s\r\n",
      "Temp\202rature:   %4.1f鳦             M\202thode:    %s\r\n",
      "Temperatura:   %4.1f鳦             Metodo:     %s\r\n",
      "Temperatura:   %4.1f鳦             M\202todo:     %s\r\n"}, REP_PRESET_TEMP},
    {{"Volume:        %s ml            Head mode:  %s\r\n",
      "Volume:        %s ml            Head mode:  %s\r\n",
      "Volume:        %s ml            Head mode:  %s\r\n",
      "Volume:        %s ml            Head mode:  %s\r\n",
      "Volumen:       %s ml            Head mode:  %s\r\n"},     REP_VOLUME},
    {{"Temp. offset:  %3.1f鳦              Interval:   %d min.\r\n",
      "Temp. offset:  %3.1f鳦              Messzyklus: %d min.\r\n",
      "Temp. offset:  %3.1f鳦              Intervalle: %d min.\r\n",
      "Temp. offset:  %3.1f鳦              Intervallo: %d min.\r\n",
      "Temp. offset:  %3.1f鳦              Intervalo:  %d min.\r\n"}, REP_TEMP_OFFSET},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STAT_HEADER0},
    {{"   Time         Speed        Temperature\r\n",
      "   Zeit        Drehzahl      Temperatur\r\n",
      "   Temps       Vitesse       Temp\202rature\r\n",
      "   Tempo       Velocit\205      Temperatura\r\n",
      "   Tiempo     Velocidad      Temperatura\r\n"},           REP_STAT_HEADER},
    {{"   hh:mm        [rpm]           [鳦]\r\n",
      "   hh:mm       [U/min]          [鳦]\r\n",
      "   hh:mm       [r.p.m.]         [鳦]\r\n",
      "   hh:mm      [giri/min]        [鳦]\r\n",
      "   hh:mm      [rev./min.]       [鳦]\r\n"},               REP_STAT_HEADER2},
    //{{"                ext.  V.1    2    3    4    5    6    7    8\r\n",
    //  "                ext.  V.1    2    3    4    5    6    7    8\r\n",
    //  "                ext.  V.1    2    3    4    5    6    7    8\r\n",
    //  "                ext.  V.1    2    3    4    5    6    7    8\r\n",
    //  "                ext.  V.1    2    3    4    5    6    7    8\r\n"}, REP_STAT_HEADER3},
    {{"   %02dh%02d        %05.1f           %s\r\n",//  %s %s %s %s %s %s %s %s\r\n",
      "   %02dh%02d        %05.1f           %s\r\n",//  %s %s %s %s %s %s %s %s\r\n",
      "   %02dh%02d        %05.1f           %s\r\n",//  %s %s %s %s %s %s %s %s\r\n",
      "   %02dh%02d        %05.1f           %s\r\n",//  %s %s %s %s %s %s %s %s\r\n",
      "   %02dh%02d        %05.1f           %s\r\n"}, REP_STAT},//  %s %s %s %s %s %s %s %s\r\n"},   REP_STAT}, 
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Total Run-Time:                %02d:%02d:%02d\r\n",
      "Gesamtlaufzeit:                %02d:%02d:%02d\r\n",
      "Dur\202e totale du test:          %02d:%02d:%02d\r\n",
      "Tempo del test totale:         %02d:%02d:%02d\r\n",
      "Tiempo total del test:         %02d:%02d:%02d\r\n"},      REP_TOTAL_RUNTIME},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Min Speed: %4.1f rpm           Max Speed: %4.1f rpm\r\n",
      "Min Drehzahl: %4.1f U/min      Max Drehzahl: %4.1f U/min\r\n",
      "Vitesse min.: %4.1f r.p.m.     Vitesse max.: %4.1f r.p.m.\r\n",
      "Velocit\x085 min.: %4.1f g./m. Velocit\x85 mass.: %4.1f g./m.\r\n",
      "Velocidad m\xA1n.: %4.1f r./m. Velocidad m\xA0x.: %4.1f r./m.\r\n"}, REP_MINMAX_SPEED},
    {{"Min Temperature: %s鳦       Max Temperature: %s鳦\r\n",
      "Min Temperatur: %s鳦        Max Temperatur: %s鳦\r\n",
      "Temp\202rature min.: %s鳦   Temp\202rature max.: %s鳦\r\n",
      "Temperatura min.: %s鳦      Temperatura mass.: %s鳦\r\n",
      "Temperatura m\xA1n.: %s鳦   Temperatura m\xA0x.: %s鳦\r\n"}, REP_MINMAX_TEMP},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"%s\r\n",
      "%s\r\n",
      "%s\r\n",
      "%s\r\n",
      "%s\r\n"},                                                 REP_TEST_RESULT},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"Comments:  ________________________________________________\r\n",
      "Bemerkungen:  _____________________________________________\r\n",
      "Observations:  ____________________________________________\r\n",
      "Annotazioni:  _____________________________________________\r\n",
      "Observaciones:  ___________________________________________\r\n"}, REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{" __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n"}, REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{" __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n",
      " __________________________________________________________\r\n"}, REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n", 
      "\r\n"},                                                   REP_STRING},
    {{"_________________________ Signature\r\n\x0C", 
      "_________________________ Unterschrift\r\n\x0C", 
      "_________________________ Signature\r\n\x0C", 
      "_________________________ Firma\r\n\x0C", 
      "_________________________ Firma\r\n\x0C"},                REP_LAST},
    {{"", 
      "", 
      "", 
      "", 
      ""},                                                       REP_END}
  };*/

/*__flash TReport QReport[]=
  {        
    {{"                   PRINTER TEST REPORT\r\n",
      "                   PRINTER TEST REPORT\r\n",
      "                   PRINTER TEST REPORT\r\n",
      "                   PRINTER TEST REPORT\r\n",
      "                   PRINTER TEST REPORT\r\n"},             REP_STRING},
    {{"_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n"},REP_STRING},
    {{"Instrument:  %s DT72%1d (%s)\r\n",
      "Ger\204t:       %s DT72%1d (%s)\r\n",
      "Appareil:    %s DT72%1d (%s)\r\n",
      "Strumento:   %s DT72%1d (%s)\r\n",
      "Aparato:     %s DT72%1d (%s)\r\n"},                       REP_DEVICE_NAME},
    {{"PCB No.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n"},                      REP_VERSION},    
    {{"Time of service:              %s %s\r\n\n",
      "Time of service:              %s %s\r\n\n",
      "Time of service:              %s %s\r\n\n",
      "Time of service:              %s %s\r\n\n",
      "Time of service:              %s %s\r\n\n"},              REP_SERVICE_DATETIME},
    {{"Heater Calibration:           %s %s\r\n",
      "Kalibration der Heizung:      %s %s\r\n",
      "Calibrage du thermostat:      %s %s\r\n",
      "Taratura del termostato:      %s %s\r\n",
      "Calibraci\xA2n del termostato:   %s %s\r\n"},             REP_HEATER_DATETIME},
    {{"Sensor Calibration:           %s %s\r\n",
      "Kalibration des F\x81hlers:      %s %s\r\n",
      "Calibrage du senseur:         %s %s\r\n",
      "Taratura del sensore:         %s %s\r\n",
      "Calibraci\xA2n del sensor:       %s %s\r\n"},             REP_SENSOR_DATETIME},
    {{"Autosampler Calibration:      %s %s\r\n\n\n",
      "Kalibration des Autosamplers: %s %s\r\n\n\n",
      "Calibrage du autosampler:     %s %s\r\n\n\n",
      "Taratura del autosampler:     %s %s\r\n\n\n",
      "Calibraci\xA2n del autosampler:  %s %s\r\n\n\n"},         REP_ASAMPLER_DATETIME},
    {{"Used during: %03d days %02d:%02d\r\n\n\n",
      "Used during: %03d days %02d:%02d\r\n\n\n",
      "Used during: %03d days %02d:%02d\r\n\n\n",
      "Used during: %03d days %02d:%02d\r\n\n\n",
      "Used during: %03d days %02d:%02d\r\n\n\n"},               REP_USAGE_COUNTER},
    {{"LED Brightness:   %1d           Number of vessels:  %1d\r\n",
      "LED Brightness:   %1d           Number of vessels:  %1d\r\n",
      "LED Brightness:   %1d           Number of vessels:  %1d\r\n",
      "LED Brightness:   %1d           Number of vessels:  %1d\r\n",
      "LED Brightness:   %1d           Number of vessels:  %1d\r\n"}, REP_SETTINGS_LED},
    {{"Sound settings:   %s        Alarm settings:     %s\r\n",
      "Sound settings:   %s        Alarm settings:     %s\r\n",
      "Sound settings:   %s        Alarm settings:     %s\r\n",
      "Sound settings:   %s        Alarm settings:     %s\r\n",
      "Sound settings:   %s        Alarm settings:     %s\r\n"}, REP_SETTINGS_SOUND},
    {{"Printer settings: %s        Date/Time format:   %s\r\n",
      "Printer settings: %s        Date/Time format:   %s\r\n",
      "Printer settings: %s        Date/Time format:   %s\r\n",
      "Printer settings: %s        Date/Time format:   %s\r\n",
      "Printer settings: %s        Date/Time format:   %s\r\n"}, REP_SETTINGS_PRINT},
    {{"Stirring mode:    %s        Time counting mode: %s\r\n\n",
      "Stirring mode:    %s        Time counting mode: %s\r\n\n",
      "Stirring mode:    %s        Time counting mode: %s\r\n\n",
      "Stirring mode:    %s        Time counting mode: %s\r\n\n",
      "Stirring mode:    %s        Time counting mode: %s\r\n\n"}, REP_SETTINGS_MODE},
    {{"PPR for stirring motor:  %3d\r\n",
      "Ppr for stirring motor:  %3d\r\n",
      "Ppr for stirring motor:  %3d\r\n",
      "Ppr for stirring motor:  %3d\r\n",
      "Ppr for stirring motor:  %3d\r\n"},                       REP_SETTINGS_PPR},
    {{"Working temperature:     %4.1f鳦-%4.1f鳦\r\n\n",
      "Working temperature:     %4.1f鳦-%4.1f鳦\r\n\n",
      "Working temperature:     %4.1f鳦-%4.1f鳦\r\n\n",
      "Working temperature:     %4.1f鳦-%4.1f鳦\r\n\n",
      "Working temperature:     %4.1f鳦-%4.1f鳦\r\n\n"},         REP_SETTINGS_TEMP},
    {{"Found DHC:         %s       Found external Pt100: %s\r\n",
      "Found DHC:         %s       Found external Pt100: %s\r\n",
      "Found DHC:         %s       Found external Pt100: %s\r\n",
      "Found DHC:         %s       Found external Pt100: %s\r\n",
      "Found DHC:         %s       Found external Pt100: %s\r\n"}, REP_SETTINGS_DHC},
    {{"Found Autodropper: %s       Found Autosampler:    %s\r\n",
      "Found Autodropper: %s       Found Autosampler:    %s\r\n",
      "Found Autodropper: %s       Found Autosampler:    %s\r\n",
      "Found Autodropper: %s       Found Autosampler:    %s\r\n",
      "Found Autodropper: %s       Found Autosampler:    %s\r\n"}, REP_SETTINGS_DROPPER},
    {{"Found DTAS:        %s\r\n\n\n",
      "Found DTAS:        %s\r\n\n\n",
      "Found DTAS:        %s\r\n\n\n",
      "Found DTAS:        %s\r\n\n\n",
      "Found DTAS:        %s\r\n\n\n"},                          REP_SETTINGS_DTAS},
    {{"Testing report is OK:    YES     NO\r\n\n\n",
      "Testbericht ist OK:      JA     NEIN\r\n\n\n",
      "Testing report is OK:    YES     NO\r\n\n\n",
      "Testing report is OK:    YES     NO\r\n\n\n",
      "Testing report is OK:    YES     NO\r\n\n\n"},            REP_STRING},
    {{"Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Datum:  %s %s     _____________________ Unterschrift\r\n\x0C",
      "Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Data:   %s %s     _____________________ Firma\r\n\x0C",
      "Fecha:  %s %s     _____________________ Firma\r\n\x0C"},  REP_CUR_DATETIME},
    {{"", 
      "", 
      "", 
      "", 
      ""},                                                       REP_END}
  };*/

/*__flash TReport CalibrReport[]=
  {
    {{"                    CALIBRATION REPORT\r\n",
      "                    KALIBRIERUNGSBERICHT\r\n",
      "                   COMPTE-RENDU DU CALIBRAGE\r\n",
      "                    RAPPORTO DELLA TARATURA\r\n",
      "                    INFORME DE CALIBRACION\r\n"},         REP_STRING},
    {{"_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n"},REP_STRING},
    {{"Instrument:  %s DT72%1d (%s)\r\n",
      "Ger\204t:       %s DT72%1d (%s)\r\n",
      "Appareil:    %s DT72%1d (%s)\r\n",
      "Strumento:   %s DT72%1d (%s)\r\n",
      "Aparato:     %s DT72%1d (%s)\r\n"},                       REP_DEVICE_NAME},
    {{"PCB No.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n"},                      REP_VERSION},    
    {{"Calibration %s\r\n\n\n",
      "Kalibrierung %s\r\n\n\n",
      "Calibrage %s\r\n\n\n",
      "Taratura %s\r\n\n\n",
      "Calibraci\xA2n %s\r\n\n\n"},                              REP_TEMP_CHANNEL},
    {{"                      V.1    2    3    4    5    6    7    8\r\n\n",
      "                      V.1    2    3    4    5    6    7    8\r\n\n",
      "                      V.1    2    3    4    5    6    7    8\r\n\n",
      "                      V.1    2    3    4    5    6    7    8\r\n\n",
      "                      V.1    2    3    4    5    6    7    8\r\n\n"}, REP_VCAL_STRING},
    {{"P.%1d, Actual Temp.:    %s\r\n",
      "P.%1d, Echte Temp.:     %s\r\n",
      "P.%1d, Temp. r\202elle:    %s\r\n",
      "P.%1d, Temp. effettiva: %s\r\n",
      "P.%1d, Temp. real:      %s\r\n"},                         REP_CAL_POINT},
    {{"P.%1d, Nominal Temp.:   %s\r\n\n\n",
      "P.%1d, Nominelle Temp.: %s\r\n\n\n",
      "P.%1d, Temp. nominale:  %s\r\n\n\n",
      "P.%1d, Temp. nominale:  %s\r\n\n\n",
      "P.%1d, Temp. nominal:   %s\r\n\n\n"},                     REP_CAL_NOMINAL},
    {{"Status: %s\r\n\n\n",
      "Status: %s\r\n\n\n",
      "Statut: %s\r\n\n\n",
      "Status: %s\r\n\n\n",
      "Status: %s\r\n\n\n"},                                     REP_CAL_STATUS},
    {{"Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Datum:  %s %s     _____________________ Unterschrift\r\n\x0C",
      "Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Data:   %s %s     _____________________ Firma\r\n\x0C",
      "Fecha:  %s %s     _____________________ Firma\r\n\x0C"},  REP_CUR_DATETIME},
    {{"", 
      "", 
      "", 
      "", 
      ""},                                                       REP_END}
  };*/

/*__flash TReport CheckReport[]=
  {
    {{"                   CALIBRATION CHECK REPORT\r\n",
      "                   KALIBRIERUNGSTESTBERICHT\r\n",
      "                   COMPTE-RENDU DU CALIBRAGE\r\n",
      "                    RAPPORTO DELLA TARATURA\r\n",
      "                    INFORME DE CALIBRACION\r\n"},         REP_STRING},
    {{"_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n",
      "_____________________________________________________________\r\n\n"},REP_STRING},
    {{"Instrument:  %s DT72%1d (%s)\r\n",
      "Ger\204t:       %s DT72%1d (%s)\r\n",
      "Appareil:    %s DT72%1d (%s)\r\n",
      "Strumento:   %s DT72%1d (%s)\r\n",
      "Aparato:     %s DT72%1d (%s)\r\n"},                       REP_DEVICE_NAME},
    {{"PCB No.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n",
      "PCB Nr.:     %s (ver. %s)\r\n\n\n"},                      REP_VERSION},    
    {{"Heater Calibration:         %s %s\r\n",
      "Kalibration der Heizung:    %s %s\r\n",
      "Calibrage du thermostat:    %s %s\r\n",
      "Taratura del termostato:    %s %s\r\n",
      "Calibraci\xA2n del termostato: %s %s\r\n"},               REP_HEATER_DATETIME},
    {{"Sensor Calibration:         %s %s\r\n\n\n",
      "Kalibration des F\x81hlers:    %s %s\r\n\n\n",
      "Calibrage du senseur:       %s %s\r\n\n\n",
      "Taratura del sensore:       %s %s\r\n\n\n",
      "Calibraci\xA2n del sensor:     %s %s\r\n\n\n"},           REP_SENSOR_DATETIME},
    {{"                            P.1      P.2      P.3\r\n\n",
      "                            P.1      P.2      P.3\r\n\n",
      "                            P.1      P.2      P.3\r\n\n",
      "                            P.1      P.2      P.3\r\n\n",
      "                            P.1      P.2      P.3\r\n\n"},REP_STRING},
    {{"Nominal Temperature, 鳦:   %s     %s     %s\r\n\n",
      "Nominelle Temperatur, 鳦:  %s     %s     %s\r\n\n",
      "Temp\202rature nominale, 鳦:  %s     %s     %s\r\n\n",
      "Temperatura nominale, 鳦:  %s     %s     %s\r\n\n",
      "Temperatura nominal, 鳦:   %s     %s     %s\r\n\n"},      REP_CHECK_NOMINAL},
    {{"Actual Temperature, 鳦:    %s     %s     %s\r\n\n\n",
      "Echte Temperatur, 鳦:      %s     %s     %s\r\n\n\n",
      "Temp\202rature r\202elle, 鳦:    %s     %s     %s\r\n\n\n",
      "Temperatura effettiva, 鳦: %s     %s     %s\r\n\n\n",
      "Temperatura real, 鳦:      %s     %s     %s\r\n\n\n"},    REP_CHECK_REAL},
    {{"Status: %s\r\n\n\n",
      "Status: %s\r\n\n\n",
      "Statut: %s\r\n\n\n",
      "Status: %s\r\n\n\n",
      "Status: %s\r\n\n\n"},                                     REP_CHECK_STATUS},
    {{"Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Datum:  %s %s     _____________________ Unterschrift\r\n\x0C",
      "Date:   %s %s     _____________________ Signature\r\n\x0C",
      "Data:   %s %s     _____________________ Firma\r\n\x0C",
      "Fecha:  %s %s     _____________________ Firma\r\n\x0C"},  REP_CUR_DATETIME},
    {{"", 
      "", 
      "", 
      "", 
      ""},                                                       REP_END}
  };*/

//TBool PRN_Busy=false;

__flash char reportNewPage[]="\x0C";
__flash char reportNoDate[]="__________________";
__flash char printError[]="Error!";
__flash char emptyStr[]="";
__flash char newStr[]="\r\n";

__flash char reportPageNum[LANGUAGE_MODE_LAST][40]=
{
  {"\n                       Page %d / %d\r"},
  {"\n                      Seite %d / %d\r"},
  {"\n                       Page %d / %d\r"},
  {"\n                     Pagina %d / %d\r"},
  {"\n                     P\xA0gina %d / %d-\r"}
};

__flash char unlimitedStr[LANGUAGE_MODE_LAST][15]=
{
  "unlimited",
  "unbegrenzt",
  "illimit\202",
  "continuamente",
  "infinito"
};

__flash char reportNoCalibration[LANGUAGE_MODE_LAST][30]=
{
  "No adjustment",
  "Nicht justiert",
  "L'\202talonnage n'est pas fait",
  "Taratura non eseguitan",
  "Sin calibraci\242n"
};

__flash char testAborted[LANGUAGE_MODE_LAST][30]=
{
  {"TEST was aborted!"},
  {"Der TEST wurde abgebrochen!"},
  {"Le TEST est interrompu!"},
  {"Il TEST \x8A stato interrotto!"},
  {"TEST interrumpido!"}
};

__flash char testAbortedMotor[LANGUAGE_MODE_LAST][50]=
{
  {"STIRRER ERROR! Test was aborted!"},
  {"STIRRER ERROR! Der TEST wurde abgebrochen!"},
  {"STIRRER ERROR! Le TEST est interrompu!"},
  {"STIRRER ERROR! Il TEST \x8A stato interrotto!"},
  {"STIRRER ERROR! TEST interrumpido!"}
};

__flash char testCompleted[LANGUAGE_MODE_LAST][35]=
{
  {"TEST was completed!"},
  
  // 2016/5/14 星期六 21:09:20------------------------------
  //{"Der TEST wurde ausgef\x81hrt!"},
  {"Der TEST wurde durchgef\x81hrt!"},
  
  {"Le TEST est accompli!"},
  {"Il TEST \x8A terminato!"},
  {"TEST terminado!"}
};

__flash char testNotCompleted[LANGUAGE_MODE_LAST][65]=
{
  {"TEST was NOT succesfully completed, parameteres out of specs!"},
  //{"Der TEST wurde NICHT ausgef\x81hrt!"},
  // 2016/6/2 星期四 17:22:31---------------------------------------------------
  //{"Der TEST wurde NICHT ordnungsgem\204\341 durchgef\x81hrt!"},
  {"Der Test wurde NICHT bestanden!"},
  
  {"Le TEST est NO accompli!"},
  {"Il TEST \x8A NO terminato!"},
  {"TEST NO terminado!"}
};

__flash char testFinished[LANGUAGE_MODE_LAST][45]=
{
  {"TEST was finished by user!"},
  {"Der TEST wurde vom Benutzer abgeschlossen!"},
  {"Le TEST est arr\x8At\202 par l'utilisateur!"},
  {"Il TEST \x8A stato arrestato dall'utente!"},
  {"TEST suspendido por el usuario!"}
};

__flash char printCalSuccess[LANGUAGE_MODE_LAST][15]=
{
  {"Success"},
  
  // 2016/5/14 星期六 22:01:46------------------------------
  //{"Erfolgreich"},
  {"bestanden"},
  
  {"Accompli"},
  {"Eseguito"},
  {"Realizado"}
};

// 2016/5/22 星期日 11:59:47--------------------------------
//__flash char printCalError[LANGUAGE_MODE_LAST][10]=
__flash char printCalError[LANGUAGE_MODE_LAST][18]=
{
  //{"Error"},
  {"Failed"}, 								// V200-Q2.1, 2019-2-27 星期三 11:30:21
  
  // 2016/5/22 星期日 10:30:17------------------------------
  //{"Fehler"},
  {"nicht bestanden"},
  
  {"Erreur"},
  {"Errore"},
  {"Error"}
};

// 2016/5/22 星期日 12:00:48--------------------------------
//__flash char printHeater[LANGUAGE_MODE_LAST][15]=
__flash char printHeater[LANGUAGE_MODE_LAST][18]=
{
  {"of the Heater"},
  
  // 2016/5/22 星期日 10:47:49------------------------------
  //{"der Heizung"},
  {"des int. Sensors"},
  
  {"du Thermostat"},
  {"di Termostato"},
  {"del Termostato"}
};

__flash char printExtSensor[LANGUAGE_MODE_LAST][30]=
{
  {"of External sensor"},
  
  // 2016/5/14 星期六 21:46:02------------------------------
  //{"des F\x81hlers"},
  {"des ext. Sensors"},
  
  {"du Senseur ext\202rieur"},
  {"di Sensore esterno"},
  {"del Sensor exterior"}
};

__flash char printAutosampler[LANGUAGE_MODE_LAST][20]=
{
  {"of Autosampler"},
  {"des Autosamplers"},
  {"du Autosampler"},
  {"di Autosampler"},
  {"del Autosampler"}
};

__flash char Print32Str[]="\x1b\x21\x02\r\n"; 							// 2015/12/8 10:39:44
__flash char Print32Str1[]="\x1b\x21\x02"; 									// 2016-9-13 星期二 13:58:07

//2017-2-10 星期五 16:55:10---------------------------------
//胡先生邮件：ERWEKA的人建议在输出报告前，加入一个空行指令，这样就可以避免标题缺失的问题出现；
__flash char SP20_CR_Str[]="                    \r\n";

TBool REPORT_Print(TReport __flash *prnReport)
{
	char tempBuffer[85];
	char tempBuffer2[65];
	char tempBuffer3[20];
	char tempBuffer4[20];
	char tempBuffer5[20];
	TPrinterStatus prnStatus;
	TByte key;
	TBool flag;
	TTimeDate dt;
	TBool tempAlarm=false;
	TBool speedAlarm=false;
	TByte cntHourGlass=0;
	
	char SpaceNo = 0;
	
	if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
		SpaceNo = SPACE_SYMBOLS_NORM;
	else
	if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
		SpaceNo = SPACE_SYMBOLS_PORT;
	
	if (!globalDeviceStatus.installedPrinter)
		return false;
	
	if((prnReport == REPORT_Test_NORM)||
		 (prnReport == REPORT_Test_PORT))
	{
		//load report info from FRAM
		FRAM_Do(MEM_READ_HEADER, 0);
		
		if (FHeader.Status==RSTS_NO) 
			return false;
		
		REPORT_NumStatRecords=FHeader.Number;
	}
	
	SAMPLER_SetActive(false);
	
	// printer isn't init
	if (!PRN_Init())
	{
		SAMPLER_SetActive(true);
		return false;
	}
	
	// Gavin 2016/10/25 13:49:15
	PRN_Reset();
	PRN_Init();
	
	LED_Print_P(LED_DISPLAY_SPEED, false, blankStr);
	
	// printer is free
	
	REPORT_PrintInit=true;
	REPORT_PrintLine=0;
	REPORT_PrintStat=0;
	REPORT_Lines=0;
	REPORT_Pages=0;
	
	REPORT_PagesNum=(FHeader.Number/(EEPROM1.ReportLines-REPORT_HEADER_SIZE))+1;
	
	if (((FHeader.Number%(EEPROM1.ReportLines-REPORT_HEADER_SIZE))+REPORT_FOOTER_SIZE)>(EEPROM1.ReportLines-REPORT_HEADER_SIZE))
		REPORT_PagesNum++;
	
	if ((fabs((TReal32)FHeader.MinTemp/10-FHeader.Temp)>MAX_TEMP_ERROR)||(fabs((TReal32)FHeader.MaxTemp/10-FHeader.Temp)>MAX_TEMP_ERROR))
		tempAlarm=true;
	
	//if (((abs(FHeader.MaxSpeed-FHeader.Speed*10)*10/FHeader.Speed)>MAX_SPEED_ERROR)||((abs(FHeader.MinSpeed-FHeader.Speed*10)*10/FHeader.Speed)>MAX_SPEED_ERROR))
	if ((abs(FHeader.MaxSpeed-FHeader.Speed*10)>MAX_SPEED_ERROR*10)||(abs(FHeader.MinSpeed-FHeader.Speed*10)>MAX_SPEED_ERROR*10))
		speedAlarm=true;
	
	// 微型打印机字体初始化-------------2016-9-13 星期二 08:00:30
	if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
	{
		strcpy_P(tempBuffer, Print32Str);
		PRN_Print(tempBuffer);
	}
	
	//2017-2-10 星期五 16:58:14---------------------
	//胡先生邮件：ERWEKA的人建议在输出报告前，加入一个空行指令，这样就可以避免标题缺失的问题出现；
	strcpy_P(tempBuffer, SP20_CR_Str);
	PRN_Print(tempBuffer);
	
	for(;;)
	{
		memset(tempBuffer, 0x20, SpaceNo);
	  
		// draw hour-glass
		if (cntHourGlass==0)
			LED_Print_P(LED_DISPLAY_TEMP, false, printStr1);
		
		if (cntHourGlass==1)
			LED_Print_P(LED_DISPLAY_TEMP, false, printStr2);
		
		if (cntHourGlass==2)
			LED_Print_P(LED_DISPLAY_TEMP, false, printStr3);
		
		cntHourGlass++;
		if (cntHourGlass>=3)
			cntHourGlass=0;
		
		sprintf_P(tempBuffer3, printPage, REPORT_Pages+1);
		LED_Print(LED_DISPLAY_SPEED, false, tempBuffer3);
		
		TIMER_Start(TIMER_GENERAL,TO_PRINTER);
		while (!TIMER_TimeOut(TIMER_GENERAL))
		{
			if (KBD_KeyPressed())
			{
				key=KBD_GetKey();
				
				if (key==KBD_CODE_STOP)
				{
					SND_Play(ssStop);
					SAMPLER_SetActive(true);
					return false;
				}
				
				SND_Play(ssError);
			}
		}
		
		TIMER_Stop(TIMER_GENERAL);
		
		// check status of printer
		PRN_Status(&prnStatus);
		
		// if printer error, set flag Failure
		//if (prnStatus.error)
		//{
		//  globalDeviceStatus.printerFailure=true;
		//   break;
		//}
		
		if (prnStatus.busy)
		{
			key=1;
			continue;
		}
		
		//if (prnStatus.paperjam)
		//  continue;
		
		if (!prnStatus.connected)
		{
			globalDeviceStatus.printerFailure=true;
			break;
		}
		
		if (globalDeviceStatus.printerFailure)
		{
			LED_Print_P(LED_DISPLAY_TEMP, true, printStr0);
			LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);
			
			for(;;)
			{
				while(!KBD_KeyPressed());
				key=KBD_GetKey();
				
				if (key==KBD_CODE_STOP)
				{
					globalDeviceStatus.printerFailure=false;
					SND_Play(ssStop);
					SAMPLER_SetActive(true);
					
					return false;
				}
				
				SND_Play(ssError);
			}
		}
		
		// 微型打印机字体初始化-------------2016-9-13 星期二 08:00:30
		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
		{
			strcpy_P(tempBuffer, Print32Str1);
			PRN_Print(tempBuffer);
		}
		
		// print report
		switch(prnReport[REPORT_PrintLine].type)
		{
			case REP_STRING:
			{
				strcpy_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage]);
			}break;
			case REP_PAGE:
			{
				REPORT_PagesNum = 1; 						// Gavin, 2016-10-23 星期日 11:57:40, 锁定1页
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_Pages+1, REPORT_PagesNum);
			}break;
			/*case REP_DEVICE_NAME:
			{
				strcpy_P(tempBuffer2, DEVICE_LOGO[EEPROM1.FirmLogo]);
				EEPROM_ReadString(tempBuffer3, EEPROM1.SerialNumber, SERIAL_NUM_MAX_LEN+1);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, (int)EEPROM1.NumVessels, tempBuffer3);
			}break;*/
			case REP_SERIAL:
			{
				EEPROM_ReadString(tempBuffer2, EEPROM1.SerialNumber, SERIAL_NUM_MAX_LEN+1);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_DEVICE_ID:
			{
				//print PCBID
				//EEPROM_ReadString(tempBuffer2,EEPROM1.PCBID,PCB_ID_MAX_LEN+1);
				EEPROM_ReadString(tempBuffer2, PCBID, PCB_ID_MAX_LEN);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				//strcpy_P(tempBuffer, prnReport[REPORT_PrintLine].name_fmt);
			}break;
			case REP_HOT_SN: 				// Gavin 2016-10-24 星期一 15:07:16
			{
				//EEPROM_ReadString(tempBuffer2,&(EEPROM1.Empty2[1]),SERIAL_NUM_MAX_LEN); 	// 加热器ID与序列号长度相同6位
				tempBuffer2[0] = EEPROM1.Empty2[1];
				tempBuffer2[1] = EEPROM1.Empty2[2];
				tempBuffer2[2] = EEPROM1.Empty2[3];
				tempBuffer2[3] = EEPROM1.Empty2[4];
				tempBuffer2[4] = EEPROM1.Empty2[5];
				tempBuffer2[5] = EEPROM1.Empty2[6];
				tempBuffer2[6] = 0;
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_VERSION:
			{
				EEPROM_ReadString(tempBuffer2, PCBID, PCB_ID_MAX_LEN+1);
				tempBuffer2[PCB_ID_MAX_LEN]=0;
				strcpy_P(tempBuffer3,DEVICE_VERSION);
				
				/*
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
				}
				*/
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
			}break;
			case REP_HEATER_DATETIME:
			case REP_SENSOR_DATETIME:
			case REP_ASAMPLER_DATETIME:
			case REP_SERVICE_DATETIME:
			case REP_TEST_DATETIME:
			case REP_CUR_DATETIME:
			case REP_DEVICE_NAME:
			{
				TByte hour;
				TByte type, vessels;
				char deviceType[8];
				
				//if (EEPROM1.FirmLogo!=DEVICE_LOGO_CALEVA)
				//{
				//else
				//{
				// Gavin 20131224-----------------------------
				//  if (globalDeviceType==DEVICE_DT520)
				//    strcpy_P(deviceType, devCalevaSt);
				//  else
				//    strcpy_P(deviceType, devCalevaStPlus);
				//}
				
				//2014/6/19 8:58:27---------------------------------------
				switch(EEPROM1.FirmLogo)
				{
					case DEVICE_LOGO_ERWEKA:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							case DEVICE_DT520:
							{
								//type=5;
								type=1; 										// Gavin 2015-06-18
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							// V200-Q1.6, 2019-2-24 星期日 20:28:40
							/*
							//2014/3/19 14:01:28
							case DEVICE_QDT6:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt1, (int)type, (int)vessels);
							}break;
							case DEVICE_QDT6S:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt2, (int)type, (int)vessels);
							}break;
							*/
							default: ;
						}
					}break;
					/* V200-Q1.7, 2019-2-27 星期三 11:25:53
					case DEVICE_LOGO_CALEVA:
					{
						// Gavin 20131224-----------------------------
						//if (globalDeviceType==DEVICE_DT520)
						//	strcpy_P(deviceType, devCalevaSt);
						//else
						//	strcpy_P(deviceType, devCalevaStPlus);
						//
						
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							case DEVICE_DT520:
							{
								//type=5;
								type=1; 										// Gavin 2015-06-18
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							// 2019-2-24 星期日 20:29:37
							// 以下屏蔽
							//2014/3/19 14:01:28
							case DEVICE_QDT6:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmtCaleva1, (int)type, (int)vessels);
							}break;
							case DEVICE_QDT6S:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmtCaleva2, (int)type, (int)vessels);
							}break;
							// 屏蔽结束
							default: ;
						}
					}break;
					case DEVICE_LOGO_TABTEST:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							case DEVICE_DT520:
							{
								//type=5;
								type=1; 										// Gavin 2015-06-18
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							// 2019-2-24 星期日 20:30:20
							// 以下屏蔽
							//2014/3/19 14:01:28
							case DEVICE_QDT6:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt1, (int)type, (int)vessels);
							}break;
							case DEVICE_QDT6S:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt2, (int)type, (int)vessels);
							}break;
							// 屏蔽结束
							default: ;
						}
					}break;
					case DEVICE_LOGO_QTEST:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							case DEVICE_DT520:
							{
								//type=5;
								type=1; 										// Gavin 2015-06-18
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							// 2019-2-24 星期日 20:31:06
							// 以下屏蔽
							//2014/3/19 14:01:28
							case DEVICE_QDT6:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt1, (int)type, (int)vessels);
							}break;
							case DEVICE_QDT6S:
							{
								type=6;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt2, (int)type, (int)vessels);
							}break;
							// 屏蔽结束
							default: ;
						}
					}break;
					*/
					default: ;
				}
				
				if (prnReport[REPORT_PrintLine].type==REP_HEATER_DATETIME) 
				{
					dt=EEPROM1.timeHeaterCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_SENSOR_DATETIME) 
				{
					dt=EEPROM1.timeSensorCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_ASAMPLER_DATETIME) 
				{
					dt=EEPROM1.timeSamplerCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_SERVICE_DATETIME) 
				{
					dt=EEPROM1.timeService;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_TEST_DATETIME) 
				{
					dt=FHeader.StartTime;
				}
				else
				{
					RTC_GetDateTime(&dt); 
				}
				
				if (!TIMEDATE_IsValidDate(dt))
				{
					if ((prnReport[REPORT_PrintLine].type==REP_CUR_DATETIME)||
						  (prnReport[REPORT_PrintLine].type==REP_TEST_DATETIME)||
						  (prnReport[REPORT_PrintLine].type==REP_SERVICE_DATETIME)||
						  (prnReport[REPORT_PrintLine].type==REP_DEVICE_NAME))
					{
						strcpy_P(tempBuffer2, reportNoDate);
						strcpy_P(tempBuffer3, emptyStr);
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
					} 
					else
					{
						if (0)//((prnReport[REPORT_PrintLine].type==REP_SENSOR_DATETIME)&&(globalDeviceType==DEVICE_DT620))
						{
							strcpy_P(tempBuffer, newStr);
						} 
						else
						{
							strcpy_P(tempBuffer2, reportNoCalibration[EEPROM1.ReportLanguage]);
							strcpy_P(tempBuffer3, emptyStr);
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
						}
					}
				}
				else
				{
					if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
						sprintf_P(tempBuffer2,paserDateFormatEUR,dt.Day,dt.Month,dt.Year);
					else
						sprintf_P(tempBuffer2,paserDateFormatUSA,dt.Month,dt.Day,dt.Year);
					
					if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
						sprintf_P(tempBuffer3,paserTimeFormatEUR,dt.Hour,dt.Minute);
					else
					{
						hour = dt.Hour;
						if (!hour) 
							hour=12;
						else
						if (hour>12) 
							hour-=12;
						
						if (dt.Hour>12)
							sprintf_P(tempBuffer3,paserTimeFormatUSA_PM,hour,dt.Minute);
						else
							sprintf_P(tempBuffer3,paserTimeFormatUSA_AM,hour,dt.Minute);
					}
					
					if (prnReport[REPORT_PrintLine].type==REP_DEVICE_NAME)
					{
						strcpy_P(tempBuffer4, DEVICE_LOGO[EEPROM1.FirmLogo]);
						EEPROM_ReadString(tempBuffer5, EEPROM1.SerialNumber, SERIAL_NUM_MAX_LEN+1);
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer4, deviceType, tempBuffer5, tempBuffer2, tempBuffer3);
					} 
					else
					{
						if (0)//((prnReport[REPORT_PrintLine].type==REP_SENSOR_DATETIME)&&(globalDeviceType==DEVICE_DT620))
							strcpy_P(tempBuffer, newStr);
						else
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
					}
				}
			}
			break;
			case REP_PRESET_RUNTIME:
			{
				//if test unlimited - print UNLIMITED, otherwise print set time
				//if (globalTestStatus.Unlimited) 
				
				// 2017/1/14 12:32:20,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数
				if (!FHeader.Time)
					strcpy_P(tempBuffer2, unlimitedStr[EEPROM1.ReportLanguage]);
				else
					sprintf_P(tempBuffer2, timeRepFmt,(TWord)(FHeader.Time/3600),(TWord)(((FHeader.Time)%3600)/60));
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Speed, tempBuffer2);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_PRESET_SPEED:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Speed);
			}break;
			case REP_PRESET_TEMP:
			{
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					strcpy_P(tempBuffer2,METHOD_Str[FHeader.Method]);
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Temp, tempBuffer2);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Temp);
				}
			}break;
			case REP_METHOD:
			{
				strcpy_P(tempBuffer2,METHOD_Str[FHeader.Method]);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_VOLUME:
			{
				strcpy_P(tempBuffer2,VOLUME_Str[FHeader.Volume]);
				strcpy_P(tempBuffer3,HEAD_Str[FHeader.Head]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_HEAD:
			{
				strcpy_P(tempBuffer2,HEAD_Str[FHeader.Head]);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_TEMP_OFFSET:
			{
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					//2017/1/14 11:41:50,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数；
					//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (TReal32)FHeader.TempOffset/10, (int)(FHeader.Interval/60));
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (TReal32)FHeader.TempOffset/10);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (TReal32)FHeader.TempOffset/10);
				}
			}break;
			//2017/1/14 11:44:08,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数；
			/*case REP_INTERVAL:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)(FHeader.Interval/60));
			}break;*/
			case REP_TOTAL_RUNTIME:
			{
				TDWord Ticker;
				
				//Ticker=globalTestStatus.Ticker;
				Ticker=FHeader.RunTime;
				
				if (Ticker<DT_MAX_TIME)
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage],(TWord)(Ticker/3600),(TWord)((Ticker%3600)/60),(TWord)(Ticker%60));
				else
					tempBuffer[0]=0;
			}break;
			case REP_MINMAX_SPEED:
			{
				// Gavin 2015/12/12 星期六 下午 2:02:54
				//if (speedAlarm)
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage],(TReal32)FHeader.MinSpeed/10,(TReal32)FHeader.MaxSpeed/10);
				//else
				//{
				//	strcpy_P(tempBuffer,newStr);
				//}
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage],(TReal32)FHeader.MinSpeed/10,(TReal32)FHeader.MaxSpeed/10);
			}break;
			case REP_MINMAX_TEMP:
			{
				if (FHeader.MinTemp==TEMP_ERROR)
					strcpy_P(tempBuffer2, errorTemp);
				else
					sprintf_P(tempBuffer2, tempReportFmt, (TReal32)FHeader.MinTemp/10);
				
				if (FHeader.MaxTemp==0)
					strcpy_P(tempBuffer3, errorTemp);
				else
					sprintf_P(tempBuffer3, tempReportFmt, (TReal32)FHeader.MaxTemp/10);
				
				// Gavin 2015/12/12 星期六 下午 2:03:47
				//if (tempAlarm)
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				//else
				//	strcpy_P(tempBuffer,newStr);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
			}break;
			case REP_STAT_HEADER0:
			case REP_STAT_HEADER:
			case REP_STAT_HEADER2:
			case REP_STAT_HEADER3:
			{
				//if no stat, don't print stat header
				if ((REPORT_NumStatRecords)&&(REPORT_PrintStat<REPORT_NumStatRecords))
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage]);
				else
					tempBuffer[0]=0;
			}break;
			case REP_STAT:
			{
				TDWord Time;
				
				//if no stat, skip print stat
				if (REPORT_NumStatRecords)
				{
					//Mem_Do(MEM_READ, REPORT_PrintStat);
					if (!FRAM_GetRecord(REPORT_PrintStat))
						break;
					
					//print stat
					Time = (TDWord)REPORT_PrintStat*FHeader.Interval;
					if (FReport.MinTemp==TEMP_ERROR)
					{
						strcpy_P(tempBuffer2, tempErrorReport);
        	} 
        	else
        	{
        		if ((!REPORT_PrintStat)||(FReport.MinTemp==FReport.MaxTemp))
        			sprintf_P(tempBuffer2, tempNewReport2Fmt, (TReal32)FReport.MinTemp/10);
        		else
        			sprintf_P(tempBuffer2, tempNewReportFmt, (TReal32)FReport.MinTemp/10, (TReal32)FReport.MaxTemp/10);
        	}
        	
        	/*for (key=0; key<MAX_SENSORS_NUM; key++)
        	{
        		if (FReport.SampT[key]==TEMP_ERROR)
        			strcpy_P(CALIBRATE_String[key], tempReportError);
        		else
        			sprintf_P(CALIBRATE_String[key], tempReportFmt, (TReal32)FReport.SampT[key]/100);
        	}*/
        	
        	if ((!REPORT_PrintStat)||(FReport.MinSpeed==FReport.MaxSpeed))
        	{
        		sprintf_P(tempBuffer3, speedNewReport2Fmt, (TReal32)FReport.MinSpeed/10);
        	}  
        	else
        	{
        		sprintf_P(tempBuffer3, speedNewReportFmt, (TReal32)FReport.MinSpeed/10, (TReal32)FReport.MaxSpeed/10);
        	}
        	
        	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)(Time/3600), (int)((Time%3600)/60), tempBuffer3, tempBuffer2);
        	//, CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2], CALIBRATE_String[3], CALIBRATE_String[4], CALIBRATE_String[5], CALIBRATE_String[6], CALIBRATE_String[7]);//(TReal32)FReport.Temp/10);
        } 
				else 
					tempBuffer[0]=0;
			}break;
      case REP_TEST_RESULT:
      {
      	//print test status (canceled, finished, completed)
      	//if (globalTestStatus.Ticker<EEPROM1.Time)
      	if (FHeader.Status==RSTS_CANCEL)
      	{
      		strcpy_P(tempBuffer2, testAborted[EEPROM1.ReportLanguage]);
      		sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
        } 
        else //print nothing
        //if ((globalTestStatus.goMode==true)||(globalTestStatus.Unlimited))
        if (FHeader.Status==RSTS_FINISHED)
        {
        	strcpy_P(tempBuffer2, testFinished[EEPROM1.ReportLanguage]);
        	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
        } 
        else
        if (FHeader.Status==RSTS_MOTORERR)
        {
        	strcpy_P(tempBuffer2, testAbortedMotor[EEPROM1.ReportLanguage]);
        	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2); 
        } 
        else
        {
        	if ((!tempAlarm)&&(!speedAlarm))
        		strcpy_P(tempBuffer2, testCompleted[EEPROM1.ReportLanguage]);
        	else
        		strcpy_P(tempBuffer2, testNotCompleted[EEPROM1.ReportLanguage]);
        	
        	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
        }
        //tempBuffer[0]=0;
      }break;
      /*case REP_TEST_COMPLETED:
      	//if test completed
      	if (globalTestStatus.Ticker>=EEPROM1.Time)
      		strcpy_P(tempBuffer, prnReport[REPORT_PrintLine].name_fmt); 
      	else //print nothing
      		tempBuffer[0]=0;
      	break;*/
      case REP_TEMP_CHANNEL:
      {
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_ADC)
      		strcpy_P(tempBuffer2,printExtSensor[EEPROM1.ReportLanguage]);
      	else
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_DHC)
      		strcpy_P(tempBuffer2,printHeater[EEPROM1.ReportLanguage]);
      	else
      		strcpy_P(tempBuffer2,printAutosampler[EEPROM1.ReportLanguage]);
      	
      	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
      }break;
      case REP_VCAL_STRING:
      {
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_AS)
      		strcpy_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage]);
      	else
      		tempBuffer[0]=0;
      }break;
      case REP_CAL_POINT:
      {
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_AS)
      	{
      		flag=false;
      		
      		for (key=0; key<MAX_SENSORS_NUM; key++)
      		{
      			if (!CALIBRATE_Point[REPORT_PrintStat][key].hasResult)
      			{
      				strcpy_P(CALIBRATE_String[key], tempReportError);
      			} 
            else
            {
            	sprintf_P(CALIBRATE_String[key], tempReportFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][key].actTemperature/10);
            	flag=true;
            }  
          }
          
          if (!flag)
          {
            tempBuffer[0]=0;
            break;
          } 
          else
          {  
            sprintf_P(tempBuffer2, calReportFmt, CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2], CALIBRATE_String[3], CALIBRATE_String[4], CALIBRATE_String[5], CALIBRATE_String[6], CALIBRATE_String[7]);
          }  
        } 
        else
        {  
          if (!CALIBRATE_Point[REPORT_PrintStat][0].hasResult)
          {  
            tempBuffer[0]=0;
            break;
          }
          
          sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].actTemperature/10);
        }
        
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_PrintStat+1, tempBuffer2);
			}break;
			case REP_CAL_NOMINAL:
			{
        if (CALIBRATE_Point[0][0].channel==CALIBRATE_AS)
        {
          flag=false;
          
          for (key=0; key<MAX_SENSORS_NUM; key++)
          {
            if (!CALIBRATE_Point[REPORT_PrintStat][key].hasResult)
            {  
              strcpy_P(CALIBRATE_String[key], tempReportError);
            } 
            else
            {  
              sprintf_P(CALIBRATE_String[key], tempReportFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][key].Temperature/10);
              flag=true;
            }  
          }
          
          if (!flag)
          {
            tempBuffer[0]=0;
            break;
          } 
          else
          {  
            sprintf_P(tempBuffer2, calReportFmt, CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2], CALIBRATE_String[3], CALIBRATE_String[4], CALIBRATE_String[5], CALIBRATE_String[6], CALIBRATE_String[7]);
          }
        } 
        else
        {  
          if (!CALIBRATE_Point[REPORT_PrintStat][0].hasResult)
          {
            tempBuffer[0]=0;
            break;
          }
          
          sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].Temperature/10);
        }
        
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_PrintStat+1, tempBuffer2);
			}break;
			case REP_CAL_STATUS:
			{
        if (CALIBRATE_Ok)
          strcpy_P(tempBuffer2,printCalSuccess[EEPROM1.ReportLanguage]);
        else
          strcpy_P(tempBuffer2,printCalError[EEPROM1.ReportLanguage]);
        
        sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
      }break;
			case REP_CHECK_CHANNEL:
			{
				if (CALIBRATE_CheckPoint[0].channel==CALIBRATE_ADC)
					strcpy_P(tempBuffer2,printExtSensor[EEPROM1.ReportLanguage]);
				else
				if (CALIBRATE_CheckPoint[0].channel==CALIBRATE_DHC)
					strcpy_P(tempBuffer2,printHeater[EEPROM1.ReportLanguage]);
				else
					strcpy_P(tempBuffer2,printAutosampler[EEPROM1.ReportLanguage]);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CHECK_NOMINAL:
			{
				//for (key=0; key<TEMP_CHECK_POINTS; key++)
				//  sprintf_P(CALIBRATE_String[key], tempReportFmt, (TReal32)CALIBRATE_CheckPoint[key].Temperature/10);
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2]);
				sprintf_P(tempBuffer2, tempReportFmt, (TReal32)CALIBRATE_CheckPoint[0].Temperature/10);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CHECK_READ:
			{
				//for (key=0; key<TEMP_CHECK_POINTS; key++)
				//  if (!CALIBRATE_CheckPoint[key].hasResult)
				//    strcpy_P(CALIBRATE_String[key], tempReportError);
				//  else
				//    sprintf_P(CALIBRATE_String[key], tempReportFmt, (TReal32)CALIBRATE_CheckPoint[key].actTemperature/10);
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2]);
				
				if (!CALIBRATE_CheckPoint[0].hasResult)
					strcpy_P(tempBuffer2, tempReportError);
				else
					sprintf_P(tempBuffer2, tempReportFmt, (TReal32)CALIBRATE_CheckPoint[0].readTemperature/10);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CHECK_REAL:  
			{
				sprintf_P(tempBuffer2, tempReportFmt, (TReal32)CALIBRATE_CheckPoint[0].actTemperature/10);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CHECK_ASRES:
			{
				if (!CALIBRATE_CheckPoint[REPORT_PrintStat].hasResult)
				{
					strcpy_P(tempBuffer2, tempReportError);
				} 
				else
				{
					sprintf_P(tempBuffer2, tempReportFmt, (TReal32)CALIBRATE_CheckPoint[REPORT_PrintStat].readTemperature/10);
				}
				
				sprintf_P(tempBuffer3, tempReportFmt, (TReal32)CALIBRATE_CheckPoint[REPORT_PrintStat].actTemperature/10);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_PrintStat+1, tempBuffer2, tempBuffer3);
			}break;
			case REP_CHECK_STATUS:
			{
				flag=false;
				
				for (key=0; key<TEMP_CHECK_POINTS; key++)
				{
					if (CALIBRATE_CheckPoint[key].hasResult)
					{
						//if (abs(CALIBRATE_CheckPoint[key].readTemperature-CALIBRATE_CheckPoint[key].actTemperature)<(TWord)(MAX_TEMP_DIFF*10))
						//	flag=true;
						
						// V200-Q4.1, 2019-2-25 星期一 15:54:43
						if((abs(CALIBRATE_CheckPoint[key].Temperature-CALIBRATE_CheckPoint[key].readTemperature)<=(TWord)(MAX_TEMP_DIFF*10))&&
							 (abs(CALIBRATE_CheckPoint[key].Temperature-CALIBRATE_CheckPoint[key].actTemperature)<=(TWord)(MAX_TEMP_DIFF*10))&&
							 (abs(CALIBRATE_CheckPoint[key].readTemperature-CALIBRATE_CheckPoint[key].actTemperature)<=(TWord)(MAX_TEMP_DIFF*10)))
						{
							flag=true;
						}
					}
				}
				
				if (!flag)
					strcpy_P(tempBuffer2,printCalError[EEPROM1.ReportLanguage]);
				else
					strcpy_P(tempBuffer2,printCalSuccess[EEPROM1.ReportLanguage]);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_USAGE_COUNTER:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)(EEPROM1.UsageCounter/(3600L*24)), (int)((EEPROM1.UsageCounter/3600)%24), (int)((EEPROM1.UsageCounter%3600)/60));
			}break;
			case REP_SETTINGS_LED:
			{
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.LED, (int)EEPROM1.NumVessels);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.LED);
				}
			}break;
			case REP_SETTINGS_NOV:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.NumVessels);
			}break;
			case REP_SETTINGS_SOUND:
			{
				strcpy_P(tempBuffer2, CONF_LIST_SOUND[EEPROM1.Sound]);
				strcpy_P(tempBuffer3, CONF_LIST_ALARM[EEPROM1.Alarm]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_SETTINGS_ALARM:
			{
				strcpy_P(tempBuffer3, CONF_LIST_ALARM[EEPROM1.Alarm]);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
			}break;
			case REP_SETTINGS_PRINT:
			{
				strcpy_P(tempBuffer2, CONF_LIST_PRINT[EEPROM1.Print]);
				strcpy_P(tempBuffer3, CONF_LIST_DFORMAT[EEPROM1.DFormat]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_SETTINGS_DFORMAT:
			{
				strcpy_P(tempBuffer3, CONF_LIST_DFORMAT[EEPROM1.DFormat]);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
			}break;
			case REP_SETTINGS_MODE:
			{
				//strcpy_P(tempBuffer2, CONF_LIST_STIR[EEPROM1.MotorStop]);
				strcpy_P(tempBuffer2, INFI_LIST_RUN[EEPROM1.MotorStop]); 			// Gavin, v2.03, 2021-03-09 星期二 13:13:27
				strcpy_P(tempBuffer3, CONF_LIST_COUNT[EEPROM1.TestCount]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_SETTINGS_MODECOUNT:
			{
				strcpy_P(tempBuffer3, CONF_LIST_COUNT[EEPROM1.TestCount]);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
			}break;
			case REP_SETTINGS_DHC:
			{
				if (globalDeviceStatus.dhcFailure)
					strcpy_P(tempBuffer2, noStr);
				else
					strcpy_P(tempBuffer2, yesStr);
				
				if ((!globalDeviceStatus.installedExtADC)||(globalDeviceStatus.sensorExtError))
					strcpy_P(tempBuffer3, noStr);
				else
					strcpy_P(tempBuffer3, yesStr);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				}
			}break;
			case REP_SETTINGS_EPT:
			{
				if ((!globalDeviceStatus.installedExtADC)||(globalDeviceStatus.sensorExtError))
					strcpy_P(tempBuffer3, noStr);
				else
					strcpy_P(tempBuffer3, yesStr);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
			}break;
			case REP_SETTINGS_DROPPER:
			{
				if (!globalDeviceStatus.haveDropper)
					strcpy_P(tempBuffer2, noStr);
				else
					strcpy_P(tempBuffer2, yesStr);
				
				if (!globalDeviceStatus.haveStepper)
					strcpy_P(tempBuffer3, noStr);
				else
					strcpy_P(tempBuffer3, yesStr);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
			}break;
			case REP_SETTINGS_DTAS:
			{
				if (globalDeviceStatus.samplerFailure)
					strcpy_P(tempBuffer2, noStr);
				else
					strcpy_P(tempBuffer2, yesStr);
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_SETTINGS_PPR:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.PulsesPerRotat);
			}break;
			case REP_SETTINGS_TEMP:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (TReal32)EEPROM1.MinTemp/10, (TReal32)EEPROM1.MaxTemp/10);
			}break;
			case REP_LAST:  
			{
				strcpy_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage]);
			}break;
			case REP_END:
			{
				REPORT_PrintInit=false;
				PRN_Done();
				SAMPLER_SetActive(true);
				
				return true;
			}
			default:
			{
				strcpy_P(&tempBuffer[SpaceNo],printError);
			}break;
		}
		
		if (PRN_Print(tempBuffer))
		{
			if (tempBuffer[0])
			{
				REPORT_Lines++;
			} 
			else
			{
				if (prnReport[REPORT_PrintLine].type==REP_STAT_HEADER0)
				{
					REPORT_PrintLine=REPORT_NewPrintLine;
					continue;
				}
			}
			
			if ((REPORT_Lines==EEPROM1.ReportLines)&&(prnReport[REPORT_PrintLine].type!=REP_LAST))
			{
				//start next page
				REPORT_Lines=0;
				REPORT_Pages++;
				//sprintf_P(&tempBuffer[SpaceNo], reportPageNum[EEPROM1.ReportLanguage], REPORT_Pages, REPORT_PagesNum);
				//PRN_Print(tempBuffer);
				strcpy_P(tempBuffer, reportNewPage);
				PRN_Print(tempBuffer);
				
				if (prnReport[REPORT_PrintLine].type==REP_STAT)
				{
					REPORT_PrintStat++;
					/*if (REPORT_PrintStat<REPORT_NumStatRecords)
					{
						//REPORT_PrintLine--;
						REPORT_PrintLine=0;
						continue;
					}*/
				} 
				else
				{
					REPORT_NewPrintLine=REPORT_PrintLine+1;
				}
				
				TIMER_Start(TIMER_GENERAL,TO_10SEC);
				while (!TIMER_TimeOut(TIMER_GENERAL))
				{
					if (KBD_KeyPressed())
					{
						key=KBD_GetKey();
						
						if (key==KBD_CODE_STOP)
						{
							SND_Play(ssStop);
							SAMPLER_SetActive(true);
							return false;
						}
						
						SND_Play(ssError);
					}
				}
				
				TIMER_Stop(TIMER_GENERAL);
				REPORT_PrintLine=0;
				continue;
			}
			
			//TIMER_Start(TIMER_GENERAL,TO_200MSEC);
			//while (!TIMER_TimeOut(TIMER_GENERAL));
			//TIMER_Stop(TIMER_GENERAL);
			// if stat is printed don't increment REPORT_PrintLine
			if (prnReport[REPORT_PrintLine].type==REP_STAT)
			{
				// increment REPORT_PrintStat
				REPORT_PrintStat++;
				// if all stat records are printed, goto next line
				if (REPORT_PrintStat<REPORT_NumStatRecords) 
					continue;
			}
			
			if (prnReport[REPORT_PrintLine].type==REP_CAL_NOMINAL)
			{
				// increment REPORT_PrintStat
				REPORT_PrintStat++;
				
				// if all stat records are printed, goto next line
				if (REPORT_PrintStat<NUM_CAL_POINTS)
				{
					REPORT_PrintLine--;
					continue;
				}
			}
			
			if (prnReport[REPORT_PrintLine].type==REP_CHECK_ASRES)
			{
				// increment REPORT_PrintStat
				REPORT_PrintStat++;
				
				// if all stat records are printed, goto next line
				if (REPORT_PrintStat<TEMP_CHECK_POINTS)
				{
					REPORT_PrintLine--;
					continue;
				}
			}
			
			REPORT_PrintLine++;
			continue;
		}
	}
	
	return true;
}
