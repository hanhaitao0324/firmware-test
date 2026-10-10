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
  "Cylinder"
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
  "500 "
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

/*
char __flash Test_Str_En[]="                       TEST REPORT               Page %d/%d\r\n";

// 2016/5/14 星期六 20:47:02--------------------------------------------------
//char __flash Test_Str_Ge[]="                       TESTBERICHT              Seite %d/%d\r\n";
char __flash Test_Str_Ge[]="                       Testprotokoll            Seite %d/%d\r\n";

char __flash Test_Str_Fr[]="                   COMPTE-RENDU DU TEST          Page %d/%d\r\n";
char __flash Test_Str_It[]="                     RAPPORTO DEL TEST         Pagina %d/%d\r\n";
char __flash Test_Str_Sp[]="                     INFORME DEL TEST          P\xA0gina %d/%d\r\n";
*/

char __flash Test_Str_En[]="                       TEST REPORT                         \r\n";
char __flash Test_Str_Ge[]="                       Testprotokoll                       \r\n";
char __flash Test_Str_Fr[]="                   COMPTE-RENDU DU TEST                    \r\n";
char __flash Test_Str_It[]="                     RAPPORTO DEL TEST                     \r\n";
char __flash Test_Str_Sp[]="                     INFORME DEL TEST                      \r\n";

char __flash Test_Str1_En[]="          TEST REPORT\r\n";
char __flash Test_Str1_Ge[]="          TEST REPORT\r\n";

char __flash Test_Str1_Fr[]="      COMPTE-RENDU DU TEST\r\n";
char __flash Test_Str1_It[]="       RAPPORTO DEL TEST\r\n";
char __flash Test_Str1_Sp[]="        INFORME DEL TEST\r\n";

// Gavin，2017-12-10 星期日 11:53:26----------------------------------------------
//char __flash Instrument_Str_En[]="Instrument:    %s %s (%s)        %s %s\r\n";
//char __flash Instrument_Str_Ge[]="Ger\204t:         %s %s (%s)        %s %s\r\n";
//char __flash Instrument_Str_Fr[]="Appareil:      %s %s (%s)        %s %s\r\n";
//char __flash Instrument_Str_It[]="Strumento:     %s %s (%s)        %s %s\r\n";
//char __flash Instrument_Str_Sp[]="Aparato:       %s %s (%s)        %s %s\r\n";

char __flash Instrument_Str_En[]="Instrument:              %s %s\r\n";
// Gavin, A8, 2019-04-11 星期四 11:00:56
//char __flash Instrument_Str_Ge[]="Instrument:              %s %s\r\n";
char __flash Instrument_Str_Ge[]="Ger\204t:                   %s %s\r\n";

char __flash Instrument_Str_Fr[]="Appareil:                %s %s\r\n";
char __flash Instrument_Str_It[]="Strumento:               %s %s\r\n";
char __flash Instrument_Str_Sp[]="Aparato:                 %s %s\r\n";

char __flash ISTM_Str1_En[]="Instrument:    %s %s\r\n";
// Gavin, A8, 2019-04-11 星期四 11:14:13
//char __flash ISTM_Str1_Ge[]="Instrument:    %s %s\r\n";
char __flash ISTM_Str1_Ge[]="Ger\204t:         %s %s\r\n";

char __flash ISTM_Str1_Fr[]="Instrument:    %s %s\r\n";
char __flash ISTM_Str1_It[]="Instrument:    %s %s\r\n";
char __flash ISTM_Str1_Sp[]="Instrument:    %s %s\r\n";

// Gavin, 2016-10-24 星期一 11:07:40,ZT121设备序列号--------
char __flash HOT_Str_En[]="Serial number:           %s\r\n";
char __flash HOT_Str_Ge[]="Serial number:           %s\r\n";
char __flash HOT_Str_Fr[]="Serial number:           %s\r\n";
char __flash HOT_Str_It[]="Serial number:           %s\r\n";
char __flash HOT_Str_Sp[]="Serial number:           %s\r\n";

char __flash SNB_Str_En[]="Serial number:           %s\r\n";
char __flash SNB_Str_Ge[]="Seriennummer:            %s\r\n";

char __flash SNB_Str_Fr[]="Serial number:           %s\r\n";
char __flash SNB_Str_It[]="Serial number:           %s\r\n";
char __flash SNB_Str_Sp[]="Serial number:           %s\r\n";

char __flash SNB_Str1_En[]="Serial number: %s\r\n";
char __flash SNB_Str1_Ge[]="Seriennummer:  %s\r\n";

char __flash SNB_Str1_Fr[]="Serial number: %s\r\n";
char __flash SNB_Str1_It[]="Serial number: %s\r\n";
char __flash SNB_Str1_Sp[]="Serial number: %s\r\n";

char __flash PCB_Str_En[]="PCB No.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Ge[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Fr[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_It[]="PCB Nr.:     %s (ver. %s)\r\n";
char __flash PCB_Str_Sp[]="PCB Nr.:     %s (ver. %s)\r\n";

char __flash VER_Str_En[]="Version:                 %s\r\n";
char __flash VER_Str_Ge[]="Version:                 %s\r\n";

char __flash VER_Str_Fr[]="Version:                 %s\r\n";
char __flash VER_Str_It[]="Version:                 %s\r\n";
char __flash VER_Str_Sp[]="Version:                 %s\r\n";

char __flash VER_Str1_En[]="Version:       %s\r\n";
char __flash VER_Str1_Ge[]="Version:       %s\r\n";
char __flash VER_Str1_Fr[]="Version:       %s\r\n";
char __flash VER_Str1_It[]="Version:       %s\r\n";
char __flash VER_Str1_Sp[]="Version:       %s\r\n";

char __flash CURDT_Str_En[] = "Date:                    %s %s\r\n";
char __flash CURDT_Str_Ge[] = "Datum:                   %s %s\r\n";

char __flash CURDT_Str_Fr[] = "Date:                    %s %s\r\n";
char __flash CURDT_Str_It[] = "Date:                    %s %s\r\n";
char __flash CURDT_Str_Sp[] = "Date:                    %s %s\r\n";

char __flash CURDT_Str1_En[] = "Date:          %s %s\r\n";
char __flash CURDT_Str1_Ge[] = "Datum:         %s %s\r\n";

char __flash CURDT_Str1_Fr[] = "Date:          %s %s\r\n";
char __flash CURDT_Str1_It[] = "Date:          %s %s\r\n";
char __flash CURDT_Str1_Sp[] = "Date:          %s %s\r\n";

char __flash CURDT1_Str_En[] = "Date: %s Time: %s\r\n";
char __flash CURDT1_Str_Ge[] = "Datum: %s Zeit: %s\r\n";

char __flash CURDT1_Str_Fr[] = "Date: %s Time: %s\r\n";
char __flash CURDT1_Str_It[] = "Date: %s Time: %s\r\n";
char __flash CURDT1_Str_Sp[] = "Date: %s Time: %s\r\n";

char __flash HeaterCal_Str_En[]="Heater Adjustment:       %s %s\r\n";
char __flash HeaterCal_Str_Ge[]="Justierung Heizung:      %s %s\r\n";

char __flash HeaterCal_Str_Fr[]="Calibrage du thermostat:       %s %s\r\n";
char __flash HeaterCal_Str_It[]="Taratura del termostato:       %s %s\r\n";
char __flash HeaterCal_Str_Sp[]="Calibraci\xA2n del termostato:    %s %s\r\n";

char __flash HTCal_Str1_En[]="Heater Adj.:   %s %s\r\n";
char __flash HTCal_Str1_Ge[]="Just. Heizung: %s %s\r\n";

char __flash HTCal_Str1_Fr[]="Heater Adj.:   %s %s\r\n";
char __flash HTCal_Str1_It[]="Heater Adj.:   %s %s\r\n";
char __flash HTCal_Str1_Sp[]="Heater Adj.:   %s %s\r\n";

char __flash SensorCal_Str_En[]="Sensor Adjustment:       %s %s\r\n";
char __flash SensorCal_Str_Ge[]="Justierung Sensor:       %s %s\r\n";

char __flash SensorCal_Str_Fr[]="Calibrage du senseur:          %s %s\r\n";
char __flash SensorCal_Str_It[]="Taratura del sensore:          %s %s\r\n";
char __flash SensorCal_Str_Sp[]="Calibraci\xA2n del sensor:        %s %s\r\n";

char __flash SSCal_Str1_En[]="Sensor Adj.:   %s %s\r\n";
char __flash SSCal_Str1_Ge[]="Just. Sensor:  %s %s\r\n";

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

char __flash Speed_Str_En[]="Nominal Speed:           %4.1f strokes/min\r\n";
char __flash Speed_Str_Ge[]="Soll Geschw.:            %4.1f H\201be/min\r\n";

char __flash Speed_Str_Fr[]="Vitesse:                 %4.1f strokes/min\r\n";
char __flash Speed_Str_It[]="Velocit\133:                %4.1f strokes/min\r\n";
char __flash Speed_Str_Sp[]="Velocidad:               %4.1f strokes/min\r\n";

char __flash Speed_Str1_En[]="Nominal Speed:    %4.1f RPM\r\n";
char __flash Speed_Str1_Ge[]="Soll Geschw.:     %4.1f H\201be/min\r\n";

char __flash Speed_Str1_Fr[]="Vitesse:       %03d r.p.m.\r\n";
char __flash Speed_Str1_It[]="Velocit\133:      %03d giri/min\r\n";
char __flash Speed_Str1_Sp[]="Velocidad:     %03d rev./min.\r\n";

char __flash Temp_Str_En[]="Pre-Set Temperature:     %4.1f鳦%s\r\n";
char __flash Temp_Str_Ge[]="Soll Temperatur:         %4.1f鳦%s\r\n";

char __flash Temp_Str_Fr[]="Temp\202rature:   %4.1f鳦 + 0.0鳦\r\n";
char __flash Temp_Str_It[]="Temperatura:   %4.1f鳦 + 0.0鳦\r\n";
char __flash Temp_Str_Sp[]="Temperatura:   %4.1f鳦 + 0.0鳦\r\n";

char __flash Temp_Str1_En[]="Pre-Set Temp.:   %4.1f鳦%s\r\n";
char __flash Temp_Str1_Ge[]="Soll Temperatur: %4.1f鳦%s\r\n";

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

char __flash Settime_Str_En[]="Pre-Set Run-Time:        %s\r\n";
char __flash Settime_Str_Ge[]="Maximale Laufzeit:       %s\r\n";

char __flash Settime_Str_Fr[]="Pre-Set Run-Time:        %s\r\n";
char __flash Settime_Str_It[]="Pre-Set Run-Time:        %s\r\n";
char __flash Settime_Str_Sp[]="Pre-Set Run-Time:        %s\r\n";

char __flash Settime_Str1_En[]="Pre-Set Run-Time: %s\r\n";
char __flash Settime_Str1_Ge[]="Max. Laufzeit:    %s\r\n";

char __flash Settime_Str1_Fr[]="Pre-Set Run-Time: %s\r\n";
char __flash Settime_Str1_It[]="Pre-Set Run-Time: %s\r\n";
char __flash Settime_Str1_Sp[]="Pre-Set Run-Time: %s\r\n";

char __flash Runtime_Str_En[]="Total   Run-Time:        %02d:%02d:%02d\r\n";
char __flash Runtime_Str_Ge[]="Gesamtlaufzeit:          %02d:%02d:%02d\r\n";

char __flash Runtime_Str_Fr[]="Dur\202e totale du test:          %02d:%02d:%02d\r\n";
char __flash Runtime_Str_It[]="Tempo del test totale:         %02d:%02d:%02d\r\n";
char __flash Runtime_Str_Sp[]="Tiempo total del test:         %02d:%02d:%02d\r\n";

char __flash Runtime_Str1_En[]="Total   Run-Time: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_Ge[]="Gesamtlaufzeit:   %02d:%02d:%02d\r\n";

char __flash Runtime_Str1_Fr[]="Dur\202e totale du test: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_It[]="Tempo del test totale: %02d:%02d:%02d\r\n";
char __flash Runtime_Str1_Sp[]="Tiempo total del test: %02d:%02d:%02d\r\n";

//char __flash SpStat_Str_En[]="Min Speed: %4.1f rpm           Max Speed: %4.1f rpm\r\n";
//char __flash SpStat_Str_Ge[]="Min Drehzahl: %4.1f U/min      Max Drehzahl: %4.1f U/min\r\n";
//char __flash SpStat_Str_Fr[]="Vitesse min.: %4.1f r.p.m.     Vitesse max.: %4.1f r.p.m.\r\n";
//char __flash SpStat_Str_It[]="Velocit\x085 min.: %4.1f g./m. Velocit\x85 mass.: %4.1f g./m.\r\n";
//char __flash SpStat_Str_Sp[]="Velocidad m\xA1n.: %4.1f r./m. Velocidad m\xA0x.: %4.1f r./m.\r\n";

char __flash SpStat_Str_En[]="Min-Max Speed:           %4.1f - %4.1f strokes/min\r\n";
char __flash SpStat_Str_Ge[]="Min-Max Geschw.:         %4.1f - %4.1f H\201be/min\r\n";

char __flash SpStat_Str_Fr[]="Vitesse min.-max.:       %4.1f - %4.1f r.p.m.\r\n";
char __flash SpStat_Str_It[]="Velocit\x085 min.-mass.: %4.1f - %4.1f g./m.\r\n";
char __flash SpStat_Str_Sp[]="Velocidad m\xA1n.-m\xA0x.: %4.1f - %4.1f r./m.\r\n";

char __flash SpStat_Str1_En[]="Min-Max Speed:   %4.1f-%4.1f RPM\r\n";
char __flash SpStat_Str1_Ge[]="Min-Max Geschw.: %4.1f-%4.1f H\201be/min\r\n";

char __flash SpStat_Str1_Fr[]="Vitesse min.-max.: %4.1f-%4.1f r.p.m.\r\n";
char __flash SpStat_Str1_It[]="Velocit\x085 min.-mass.: %4.1f-%4.1f g./m.\r\n";
char __flash SpStat_Str1_Sp[]="Velocidad m\xA1n.-m\xA0x.: %4.1f-%4.1f r./m.\r\n";

//char __flash TmpStat_Str_En[]="Min Temperature: %s鳦       Max Temperature: %s鳦\r\n";
//char __flash TmpStat_Str_Ge[]="Min Temperatur: %s鳦        Max Temperatur: %s鳦\r\n";
//char __flash TmpStat_Str_Fr[]="Temp\202rature min.: %s鳦   Temp\202rature max.: %s鳦\r\n";
//char __flash TmpStat_Str_It[]="Temperatura min.: %s鳦      Temperatura mass.: %s鳦\r\n";
//char __flash TmpStat_Str_Sp[]="Temperatura m\xA1n.: %s鳦   Temperatura m\xA0x.: %s鳦\r\n";

char __flash TmpStat_Str_En[]="Min-Max Temperature:     %s - %s 鳦\r\n";
char __flash TmpStat_Str_Ge[]="Min-Max Temperatur:      %s - %s 鳦\r\n";

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

char __flash Signature_Str_En[]="______________________________\r\n                      Signature\r\n\x0C";
char __flash Signature_Str_Ge[]="______________________________\r\n                     Unterschrift\r\n\x0C";

char __flash Signature_Str_Fr[]="_________________________ Signature\r\n\x0C";
char __flash Signature_Str_It[]="_________________________ Firma\r\n\x0C";
char __flash Signature_Str_Sp[]="_________________________ Firma\r\n\x0C";

char __flash Signature_Str1_En[]="______________________________\r\n        Signature\r\n\x0C";
char __flash Signature_Str1_Ge[]="______________________________\r\n      Unterschrift\r\n\x0C";

char __flash Signature_Str1_Fr[]="___________________ Signature\r\n\x0C";
char __flash Signature_Str1_It[]="___________________ Firma\r\n\x0C";
char __flash Signature_Str1_Sp[]="___________________ Firma\r\n\x0C";

// Gavin, A8, 2019-04-12 星期五 09:38:13
/*char __flash Signature_Str2_En[]="___________________ Signature\r\n\r\n\x0C";
char __flash Signature_Str2_Ge[]="___________________ Unterschrift\r\n\r\n\x0C";
char __flash Signature_Str2_Fr[]="___________________ Signature\r\n\r\n\x0C";
char __flash Signature_Str2_It[]="___________________ Firma\r\n\r\n\x0C";
char __flash Signature_Str2_Sp[]="___________________ Firma\r\n\r\n\x0C";
*/
char __flash Signature_Str2_En[]="_______________________\r\n       Signature\r\n\r\n\x0C";
char __flash Signature_Str2_Ge[]="_______________________\r\n     Unterschrift\r\n\r\n\x0C";
char __flash Signature_Str2_Fr[]="_______________________\r\n       Signature\r\n\r\n\x0C";
char __flash Signature_Str2_It[]="_______________________\r\n         Firma\r\n\r\n\x0C";
char __flash Signature_Str2_Sp[]="_______________________\r\n         Firma\r\n\r\n\x0C";

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

char __flash PrTest_Str_En[]="                   TEST REPORT PRINTER\r\n";
char __flash PrTest_Str_Ge[]="                   TEST REPORT DRUCKER\r\n";

char __flash PrTest_Str_Fr[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_It[]="                   PRINTER TEST REPORT\r\n";
char __flash PrTest_Str_Sp[]="                   PRINTER TEST REPORT\r\n";

char __flash PrTest_Str1_En[]="      TEST REPORT PRINTER\r\n";
char __flash PrTest_Str1_Ge[]="      TEST REPORT DRUCKER\r\n";

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

char __flash LED_Str_En[]="LED Brightness:      %1d\r\n";
char __flash LED_Str_Ge[]="LED Helligkeit:      %1d\r\n";

char __flash LED_Str_Fr[]="LED Brightness:    %1d\r\n";
char __flash LED_Str_It[]="LED Brightness:    %1d\r\n";
char __flash LED_Str_Sp[]="LED Brightness:    %1d\r\n";

char __flash LED_Str1_En[]="LED Brightness:      %1d\r\n";
char __flash LED_Str1_Ge[]="LED Helligkeit:      %1d\r\n";

char __flash LED_Str1_Fr[]="LED Brightness:   %1d\r\n";
char __flash LED_Str1_It[]="LED Brightness:   %1d\r\n";
char __flash LED_Str1_Sp[]="LED Brightness:   %1d\r\n";

char __flash NOV_Str1_En[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Ge[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Fr[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_It[]="Number of vessels:  %1d\r\n";
char __flash NOV_Str1_Sp[]="Number of vessels:  %1d\r\n";

char __flash Sound_Str_En[]="Sound settings:      %s\r\n";
char __flash Sound_Str_Ge[]="Sound Einstellung:   %s\r\n";

char __flash Sound_Str_Fr[]="Sound settings:    %s\r\n";
char __flash Sound_Str_It[]="Sound settings:    %s\r\n";
char __flash Sound_Str_Sp[]="Sound settings:    %s\r\n";

char __flash Alarm_Str_En[]="Alarm settings:      %s\r\n";
char __flash Alarm_Str_Ge[]="Alarm Einstellung:   %s\r\n";

char __flash Alarm_Str_Fr[]="Alarm settings:    %s\r\n";
char __flash Alarm_Str_It[]="Alarm settings:    %s\r\n";
char __flash Alarm_Str_Sp[]="Alarm settings:    %s\r\n";

char __flash Sound_Str1_En[]="Sound settings:      %s\r\n";
char __flash Sound_Str1_Ge[]="Sound Einstellung:   %s\r\n";

char __flash Sound_Str1_Fr[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_It[]="Sound settings:   %s\r\n";
char __flash Sound_Str1_Sp[]="Sound settings:   %s\r\n";

char __flash AmSet_Str1_En[]="Alarm settings:      %s\r\n";
char __flash AmSet_Str1_Ge[]="Alarm Einstellung:   %s\r\n";

char __flash AmSet_Str1_Fr[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_It[]="Alarm settings:   %s\r\n";
char __flash AmSet_Str1_Sp[]="Alarm settings:   %s\r\n";

char __flash Printer_Str_En[]="Printer settings:    %s\r\n";
char __flash Printer_Str_Ge[]="Drucker Einstellung: %s\r\n";

char __flash Printer_Str_Fr[]="Printer settings:  %s\r\n";
char __flash Printer_Str_It[]="Printer settings:  %s\r\n";
char __flash Printer_Str_Sp[]="Printer settings:  %s\r\n";

char __flash DTimeF_Str_En[]="Date/Time format:    %s\r\n";
char __flash DTimeF_Str_Ge[]="Datum/Zeit Format:   %s\r\n";

char __flash DTimeF_Str_Fr[]="Date/Time format:  %s\r\n";
char __flash DTimeF_Str_It[]="Date/Time format:  %s\r\n";
char __flash DTimeF_Str_Sp[]="Date/Time format:  %s\r\n";

char __flash Printer_Str1_En[]="Printer settings:    %s\r\n";
char __flash Printer_Str1_Ge[]="Drucker Einstellung: %s\r\n";

char __flash Printer_Str1_Fr[]="Printer settings: %s\r\n";
char __flash Printer_Str1_It[]="Printer settings: %s\r\n";
char __flash Printer_Str1_Sp[]="Printer settings: %s\r\n";

char __flash DTimeF_Str1_En[]="Date/Time format:    %s\r\n";
char __flash DTimeF_Str1_Ge[]="Datum/Zeit Format:   %s\r\n";

char __flash DTimeF_Str1_Fr[]="Date/Time format: %s\r\n";
char __flash DTimeF_Str1_It[]="Date/Time format: %s\r\n";
char __flash DTimeF_Str1_Sp[]="Date/Time format: %s\r\n";

char __flash StMode_Str_En[]="Stirring mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Ge[]="Stirring mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Fr[]="Stirring mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_It[]="Stirring mode:    %s        Time counting mode: %s\r\n";
char __flash StMode_Str_Sp[]="Stirring mode:    %s        Time counting mode: %s\r\n";

char __flash StMode_Str1_En[]="Stirring mode:    %s\r\n";
char __flash StMode_Str1_Ge[]="Stirring mode:    %s\r\n";
char __flash StMode_Str1_Fr[]="Stirring mode:    %s\r\n";
char __flash StMode_Str1_It[]="Stirring mode:    %s\r\n";
char __flash StMode_Str1_Sp[]="Stirring mode:    %s\r\n";

char __flash TCMode_Str1_En[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Ge[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Fr[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_It[]="Time counting mode: %s\r\n";
char __flash TCMode_Str1_Sp[]="Time counting mode: %s\r\n";

char __flash PPR_Str_En[]="PPR stirring motor:      %03d\r\n";
char __flash PPR_Str_Ge[]="PPR Hubmotor:            %03d\r\n";

char __flash PPR_Str_Fr[]="Ppr for stirring motor:  %03d\r\n";
char __flash PPR_Str_It[]="Ppr for stirring motor:  %03d\r\n";
char __flash PPR_Str_Sp[]="Ppr for stirring motor:  %03d\r\n";

char __flash PPR_Str1_En[]="PPR stirring motor:      %3d\r\n";
char __flash PPR_Str1_Ge[]="PPR Hubmotor:            %3d\r\n";

char __flash PPR_Str1_Fr[]="Ppr for stirring motor:  %3d\r\n";
char __flash PPR_Str1_It[]="Ppr for stirring motor:  %3d\r\n";
char __flash PPR_Str1_Sp[]="Ppr for stirring motor:  %3d\r\n";

char __flash WorkTemp_Str_En[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_Ge[]="Arbeitstemperatur:       %4.1f鳦-%4.1f鳦\r\n";

char __flash WorkTemp_Str_Fr[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_It[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str_Sp[]="Working temperature:     %4.1f鳦-%4.1f鳦\r\n";

char __flash WorkTemp_Str1_En[]="Working Temp.: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_Ge[]="Arbeitstemp.:  %4.1f鳦-%4.1f鳦\r\n";

char __flash WorkTemp_Str1_Fr[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_It[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";
char __flash WorkTemp_Str1_Sp[]="Working temperature: %4.1f鳦-%4.1f鳦\r\n";

char __flash DHC_Str_En[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Ge[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Fr[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_It[]="Found DHC:         %s       Found external Pt100: %s\r\n";
char __flash DHC_Str_Sp[]="Found DHC:         %s       Found external Pt100: %s\r\n";

char __flash EPT_Str_En[]="Found external PT100:      %s\r\n";
char __flash EPT_Str_Ge[]="Externer Sensor vorhanden: %s\r\n";

char __flash EPT_Str_Fr[]="Found external Pt100:    %s\r\n";
char __flash EPT_Str_It[]="Found external Pt100:    %s\r\n";
char __flash EPT_Str_Sp[]="Found external Pt100:    %s\r\n";

char __flash DHC_Str1_En[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Ge[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Fr[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_It[]="Found DHC:         %s\r\n";
char __flash DHC_Str1_Sp[]="Found DHC:         %s\r\n";

char __flash EPT_Str1_En[]="Found external Sensor: %s\r\n";
char __flash EPT_Str1_Ge[]="externer Sensor vorh.: %s\r\n";

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
char __flash RepOk_Str_Ge[]="Testreport ist OK:         JA     NEIN\r\n";

char __flash RepOk_Str_Fr[]="Testing report is OK:    YES     NO\r\n";
char __flash RepOk_Str_It[]="Testing report is OK:    YES     NO\r\n";
char __flash RepOk_Str_Sp[]="Testing report is OK:    YES     NO\r\n";

char __flash RepOk_Str1_En[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_Ge[]="Testreport ist OK:     JA   NEIN\r\n";

char __flash RepOk_Str1_Fr[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_It[]="Testing report is OK:  YES   NO\r\n";
char __flash RepOk_Str1_Sp[]="Testing report is OK:  YES   NO\r\n";

char __flash CalRep_Str_En[]="%sADJUSTMENT REPORT %s\r\n";
char __flash CalRep_Str_Ge[]="%sJUSTIER REPORT %s\r\n";

char __flash CalRep_Str_Fr[]="                   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CalRep_Str_It[]="                    RAPPORTO DELLA TARATURA\r\n";
char __flash CalRep_Str_Sp[]="                    INFORME DE CALIBRACION\r\n";

char __flash CalRep_Str1_En[]="%sADJUSTMENT REPORT %s\r\n";
char __flash CalRep_Str1_Ge[]="%sJUSTIER REPORT %s\r\n";

char __flash CalRep_Str1_Fr[]="   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CalRep_Str1_It[]="    RAPPORTO DELLA TARATURA\r\n";
char __flash CalRep_Str1_Sp[]="    INFORME DE CALIBRACION\r\n";

char __flash CalType_Str_En[]="Adjustment %s\r\n";
char __flash CalType_Str_Ge[]="Justierung %s\r\n";

char __flash CalType_Str_Fr[]="Calibrage    %s\r\n";
char __flash CalType_Str_It[]="Taratura     %s\r\n";
char __flash CalType_Str_Sp[]="Calibraci\xA2n  %s\r\n";

// Gavin, A6, 2019-04-01 星期一 15:38:06
//char __flash CheckRep_Str_En[]="                    ADJUSTMENT REPORT\r\n";
// Gavin, A8, 2019-04-11 星期四 15:10:39
//char __flash CheckRep_Str_En[]="                    CALIBRATION REPORT\r\n";
char __flash CheckRep_Str_En[]="%sCALIBRATION REPORT %s\r\n";

// Gavin, A1, 2019-04-02 星期二 15:59:42
//char __flash CheckRep_Str_Ge[]="                       JUSTIERUNG\r\n";
// Gavin, A8, 2019-04-11 星期四 10:56:23
//char __flash CheckRep_Str_Ge[]="                      KALIBRIERUNG\r\n";
char __flash CheckRep_Str_Ge[]="%sKALIBRIER REPORT %s\r\n";

char __flash CheckRep_Str_Fr[]="                   COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CheckRep_Str_It[]="                    RAPPORTO DELLA TARATURA\r\n";
char __flash CheckRep_Str_Sp[]="                    INFORME DE CALIBRACION\r\n";

// Gavin, v7, 2019-04-03 星期三 17:45:55
//char __flash CheckRep_Str1_En[]="        ADJUSTMENT REPORT\r\n";
// Gavin, A8, 2019-04-12 星期五 09:28:51
//char __flash CheckRep_Str1_En[]="        CALIBRATION REPORT\r\n";
char __flash CheckRep_Str1_En[]="%sCALIBRATION REPORT %s\r\n";

// Gavin, A1, 2019-04-02 星期二 16:00:10
//char __flash CheckRep_Str1_Ge[]="        Justierung\r\n";
// Gavin, A8, 2019-04-11 星期四 11:09:09
//char __flash CheckRep_Str1_Ge[]="       Kalibrierung\r\n";
char __flash CheckRep_Str1_Ge[]="%sKALIBRIER REPORT %s\r\n";

char __flash CheckRep_Str1_Fr[]="       COMPTE-RENDU DU CALIBRAGE\r\n";
char __flash CheckRep_Str1_It[]="        RAPPORTO DELLA TARATURA\r\n";
char __flash CheckRep_Str1_Sp[]="        INFORME DE CALIBRACION\r\n";

// Gavin, A6, 2019-04-01 星期一 15:44:10
//char __flash CheckType_Str_En[]="Adjustment %s\r\n";
char __flash CheckType_Str_En[]="Calibration %s\r\n";

// Gavin, A1, 2019-04-02 星期二 16:04:50
//char __flash CheckType_Str_Ge[]="Justierung %s\r\n";
char __flash CheckType_Str_Ge[]="Kalibrierung %s\r\n";

char __flash CheckType_Str_Fr[]="Calibrage %s\r\n";
char __flash CheckType_Str_It[]="Taratura %s\r\n";
char __flash CheckType_Str_Sp[]="Calibraci\xA2n %s\r\n";

// Gavin, A8, 2019-04-11 星期四 11:38:41
/*
char __flash ATemp_Str_En[]="P.%1d, Actual Temperature:    %s\r\n";
char __flash ATemp_Str_Ge[]="P.%1d, Ist Temperatur:     %s\r\n";

char __flash ATemp_Str_Fr[]="P.%1d, Temp. r\202elle:    %s\r\n";
char __flash ATemp_Str_It[]="P.%1d, Temp. effettiva: %s\r\n";
char __flash ATemp_Str_Sp[]="P.%1d, Temp. real:      %s\r\n";
*/
char __flash ATemp_Str_En[]="Real Temperature:        %s\r\n";
char __flash ATemp_Str_Ge[]="Ref. Temperatur:         %s\r\n";

char __flash ATemp_Str_Fr[]="Temp. r\202elle:    %s\r\n";
char __flash ATemp_Str_It[]="Temp. effettiva: %s\r\n";
char __flash ATemp_Str_Sp[]="Temp. real:      %s\r\n";

// Gavin, A8, 2019-04-11 星期四 11:42:02
/*
char __flash NTemp_Str_En[]="P.%1d, Nominal Temperature:   %s\r\n";
char __flash NTemp_Str_Ge[]="P.%1d, Soll Temperatur:    %s\r\n";

char __flash NTemp_Str_Fr[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str_It[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str_Sp[]="P.%1d, Temp. nominal:   %s\r\n";
*/
char __flash NTemp_Str_En[]="Readout Temperature:     %s\r\n";
char __flash NTemp_Str_Ge[]="Ist Temperatur:          %s\r\n";

char __flash NTemp_Str_Fr[]="Temp. nominale:  %s\r\n";
char __flash NTemp_Str_It[]="Temp. nominale:  %s\r\n";
char __flash NTemp_Str_Sp[]="Temp. nominal:   %s\r\n";

// Gavin, A8, 2019-04-11 星期四 11:52:27
/*
char __flash ATemp_Str1_En[]="P.%1d, Actual Temp.:    %s\r\n";
char __flash ATemp_Str1_Ge[]="P.%1d, Ist Temperatur:     %s\r\n";

char __flash ATemp_Str1_Fr[]="P.%1d, Temp. r\202elle:    %s\r\n";
char __flash ATemp_Str1_It[]="P.%1d, Temp. effettiva: %s\r\n";
char __flash ATemp_Str1_Sp[]="P.%1d, Temp. real:      %s\r\n";
*/
char __flash ATemp_Str1_En[]="Real Temp.:        %s\r\n";
char __flash ATemp_Str1_Ge[]="Ref. Temperatur:   %s\r\n";

char __flash ATemp_Str1_Fr[]="Temp. r\202elle:    %s\r\n";
char __flash ATemp_Str1_It[]="Temp. effettiva: %s\r\n";
char __flash ATemp_Str1_Sp[]="Temp. real:      %s\r\n";

// Gavin, A8, 2019-04-11 星期四 11:52:52
/*
char __flash NTemp_Str1_En[]="P.%1d, Nominal Temp.:   %s\r\n";
char __flash NTemp_Str1_Ge[]="P.%1d, Soll Temperatur:    %s\r\n";

char __flash NTemp_Str1_Fr[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str1_It[]="P.%1d, Temp. nominale:  %s\r\n";
char __flash NTemp_Str1_Sp[]="P.%1d, Temp. nominal:   %s\r\n";
*/
char __flash NTemp_Str1_En[]="Readout Temp.:     %s\r\n";
char __flash NTemp_Str1_Ge[]="Ist Temperatur:    %s\r\n";

char __flash NTemp_Str1_Fr[]="Temp. nominale:  %s\r\n";
char __flash NTemp_Str1_It[]="Temp. nominale:  %s\r\n";
char __flash NTemp_Str1_Sp[]="Temp. nominal:   %s\r\n";

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

char __flash User_Str_En[]="User:                    _________________________________\r\n";
char __flash User_Str_Ge[]="Nutzer:                  _________________________________\r\n";

char __flash User_Str_Fr[]="User:                    _________________________________\r\n";
char __flash User_Str_It[]="User:                    _________________________________\r\n";
char __flash User_Str_Sp[]="User:                    _________________________________\r\n";

char __flash BLN_Str_En[]= "Batch/Lot No.:           _________________________________\r\n";
char __flash BLN_Str_Ge[]= "Batch/Lot Nr.:           _________________________________\r\n";

char __flash BLN_Str_Fr[]= "Batch/Lot No.:           _________________________________\r\n";
char __flash BLN_Str_It[]= "Batch/Lot No.:           _________________________________\r\n";
char __flash BLN_Str_Sp[]= "Batch/Lot No.:           _________________________________\r\n";

char __flash CMT_Str_EN[]= "Comment:                 _________________________________\r\n";
char __flash CMT_Str_Ge[]= "Kommentar:               _________________________________\r\n";

char __flash CMT_Str_Fr[]= "Comment:                 _________________________________\r\n";
char __flash CMT_Str_It[]= "Comment:                 _________________________________\r\n";
char __flash CMT_Str_Sp[]= "Comment:                 _________________________________\r\n";

char __flash Line1_Str[] = "                         _________________________________\r\n";

char __flash User_Str1_En[]="User:          ________________\r\n";
char __flash User_Str1_Ge[]="Nutzer:        ________________\r\n";

char __flash User_Str1_Fr[]="User:          ________________\r\n";
char __flash User_Str1_It[]="User:          ________________\r\n";
char __flash User_Str1_Sp[]="User:          ________________\r\n";

char __flash BLN_Str1_En[]= "Batch/Lot No.: ________________\r\n";
char __flash BLN_Str1_Ge[]= "Batch/Lot Nr.: ________________\r\n";

char __flash BLN_Str1_Fr[]= "Batch/Lot No.: ________________\r\n";
char __flash BLN_Str1_It[]= "Batch/Lot No.: ________________\r\n";
char __flash BLN_Str1_Sp[]= "Batch/Lot No.: ________________\r\n";

char __flash CMT_Str1_EN[]= "Comment:       ________________\r\n";
char __flash CMT_Str1_Ge[]= "Kommentar:     ________________\r\n";

char __flash CMT_Str1_Fr[]= "Comment:       ________________\r\n";
char __flash CMT_Str1_It[]= "Comment:       ________________\r\n";
char __flash CMT_Str1_Sp[]= "Comment:       ________________\r\n";

char __flash Line1_Str1[] = "               ________________\r\n";

// Gavin, 2019-3-26 16:06:28, A1.3, 修改数字变量，包括普打和微打
char __flash TST_Str_En[]= "Test station:            %1d\r\n";
char __flash TST_Str_Ge[]= "Teststationen:           %1d\r\n";

char __flash TST_Str_Fr[]= "Test station:            %1d\r\n";
char __flash TST_Str_It[]= "Test station:            %1d\r\n";
char __flash TST_Str_Sp[]= "Test station:            %1d\r\n";

char __flash TST_Str1_En[]= "Test station:     %1d\r\n";
char __flash TST_Str1_Ge[]= "Test Stationen:   %1d\r\n";

char __flash TST_Str1_Fr[]= "Test station:     %1d\r\n";
char __flash TST_Str1_It[]= "Test station:     %1d\r\n";
char __flash TST_Str1_Sp[]= "Test station:     %1d\r\n";

char __flash DTime_Str_En[]="Disintegration Time: \r\n";
char __flash DTime_Str_Ge[]="Zerfallszeit: \r\n";

char __flash DTime_Str_Fr[]="Disintegration Time: \r\n";
char __flash DTime_Str_It[]="Disintegration Time: \r\n";
char __flash DTime_Str_Sp[]="Disintegration Time: \r\n";

// Gavin, 2019-3-26 16:53:18, A1.7
char __flash Station_Str_En[]="         STATION %1d           %s\r\n";
char __flash Station_Str_Ge[]="         STATION %1d           %s\r\n";

char __flash Station_Str_Fr[]="         STATION %1d           %s\r\n";
char __flash Station_Str_It[]="         STATION %1d           %s\r\n";
char __flash Station_Str_Sp[]="         STATION %1d           %s\r\n";

char __flash P1MS_Str_En[]="P1:_________min_________sec  %s";
char __flash P1MS_Str_Ge[]="P1:_________min_________sek  %s";

char __flash P1MS_Str_Fr[]="P1:_________min_________sec  %s";
char __flash P1MS_Str_It[]="P1:_________min_________sec  %s";
char __flash P1MS_Str_Sp[]="P1:_________min_________sec  %s";

char __flash P2MS_Str_En[]="P2:_________min_________sec  %s";
char __flash P2MS_Str_Ge[]="P2:_________min_________sek  %s";

char __flash P2MS_Str_Fr[]="P2:_________min_________sec  %s";
char __flash P2MS_Str_It[]="P2:_________min_________sec  %s";
char __flash P2MS_Str_Sp[]="P2:_________min_________sec  %s";

char __flash P3MS_Str_En[]="P3:_________min_________sec  %s";
char __flash P3MS_Str_Ge[]="P3:_________min_________sek  %s";

char __flash P3MS_Str_Fr[]="P3:_________min_________sec  %s";
char __flash P3MS_Str_It[]="P3:_________min_________sec  %s";
char __flash P3MS_Str_Sp[]="P3:_________min_________sec  %s";

char __flash P4MS_Str_En[]="P4:_________min_________sec  %s";
char __flash P4MS_Str_Ge[]="P4:_________min_________sek  %s";

char __flash P4MS_Str_Fr[]="P4:_________min_________sec  %s";
char __flash P4MS_Str_It[]="P4:_________min_________sec  %s";
char __flash P4MS_Str_Sp[]="P4:_________min_________sec  %s";

char __flash P5MS_Str_En[]="P5:_________min_________sec  %s";
char __flash P5MS_Str_Ge[]="P5:_________min_________sek  %s";

char __flash P5MS_Str_Fr[]="P5:_________min_________sec  %s";
char __flash P5MS_Str_It[]="P5:_________min_________sec  %s";
char __flash P5MS_Str_Sp[]="P5:_________min_________sec  %s";

char __flash P6MS_Str_En[]="P6:_________min_________sec  %s";
char __flash P6MS_Str_Ge[]="P6:_________min_________sek  %s";

char __flash P6MS_Str_Fr[]="P6:_________min_________sec  %s";
char __flash P6MS_Str_It[]="P6:_________min_________sec  %s";
char __flash P6MS_Str_Sp[]="P6:_________min_________sec  %s";

char __flash RQF_Str_En[]="Requirement fulfilled: \r\n";
char __flash RQF_Str_Ge[]="Spezifikation erf\201llt: \r\n";

char __flash RQF_Str_Fr[]="Requirement fulfilled: \r\n";
char __flash RQF_Str_It[]="Requirement fulfilled: \r\n";
char __flash RQF_Str_Sp[]="Requirement fulfilled: \r\n";

char __flash YON_Str_En[]="______YES         ______NO   %s";
char __flash YON_Str_Ge[]="______JA          ______NEIN   %s";

char __flash YON_Str_Fr[]="______YES         ______NO   %s";
char __flash YON_Str_It[]="______YES         ______NO   %s";
char __flash YON_Str_Sp[]="______YES         ______NO   %s";

// 测试工作报告（普打）-------------------------------------------------------------
__flash TReport REPORT_Test_NORM[]=
  {        
    {{Test_Str_En, 				Test_Str_Ge, 				Test_Str_Fr, 				Test_Str_It, 				Test_Str_Sp 			}, REP_STRING 					}, // 标题
    {{Line_Str, 					Line_Str, 					Line_Str, 					Line_Str, 					Line_Str 					}, REP_STRING 					}, // 横线
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					}, // 换行
    
    {{Instrument_Str_En, 	Instrument_Str_Ge, 	Instrument_Str_Fr, 	Instrument_Str_It, 	Instrument_Str_Sp }, REP_DEVICE_NAME 			}, // 仪器，品牌/规格/型号
    {{SNB_Str_En, 				SNB_Str_Ge, 				SNB_Str_Fr, 				SNB_Str_It, 				SNB_Str_Sp				}, REP_SERIAL						}, // 电路板序列号
    {{VER_Str_En, 				VER_Str_Ge, 				VER_Str_Fr, 				VER_Str_It, 				VER_Str_Sp 				}, REP_VERSION 					}, // 软件版本号
    {{CURDT_Str_En, 			CURDT_Str_Ge, 			CURDT_Str_Fr, 			CURDT_Str_It, 			CURDT_Str_Sp			}, REP_CUR_DATETIME			}, // 当前日期时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{HeaterCal_Str_En, 	HeaterCal_Str_Ge, 	HeaterCal_Str_Fr, 	HeaterCal_Str_It, 	HeaterCal_Str_Sp 	}, REP_HEATER_DATETIME 	}, // 内部PT100校准时间
    {{SensorCal_Str_En, 	SensorCal_Str_Ge, 	SensorCal_Str_Fr, 	SensorCal_Str_It, 	SensorCal_Str_Sp 	}, REP_SENSOR_DATETIME 	}, // 外部PT100校准时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{User_Str_En, 				User_Str_Ge, 				User_Str_Fr, 				User_Str_It, 				User_Str_Sp				}, REP_STRING 					}, // User:          ___...
    {{BLN_Str_En,         BLN_Str_Ge,         BLN_Str_Fr,         BLN_Str_It,         BLN_Str_Sp        }, REP_STRING           }, // Batch/Lot No.: ___...
    {{CMT_Str_EN,         CMT_Str_Ge,         CMT_Str_Fr,         CMT_Str_It,         CMT_Str_Sp        }, REP_STRING           }, // Comment:       ___...
    {{Line1_Str,          Line1_Str,          Line1_Str,          Line1_Str,          Line1_Str         }, REP_STRING           }, //                ___...
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{TST_Str_En,         TST_Str_Ge,         TST_Str_Fr,         TST_Str_It,         TST_Str_Sp        }, REP_TEST_STATION     }, // Test station:  1， Gavin, 2019-3-27 14:32:07, A1.10
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Settime_Str_En, 		Settime_Str_Ge, 		Settime_Str_Fr, 		Settime_Str_It, 		Settime_Str_Sp 		}, REP_PRESET_RUNTIME 	}, // 设置运行时间
    {{Runtime_Str_En, 		Runtime_Str_Ge, 		Runtime_Str_Fr, 		Runtime_Str_It, 		Runtime_Str_Sp 		}, REP_TOTAL_RUNTIME 		}, // 实际运行时间
    {{Speed_Str_En, 			Speed_Str_Ge, 			Speed_Str_Fr, 			Speed_Str_It, 			Speed_Str_Sp 			}, REP_PRESET_SPEED     }, // 设置运行转速
    {{Temp_Str_En, 				Temp_Str_Ge, 				Temp_Str_Fr, 				Temp_Str_It, 				Temp_Str_Sp 			}, REP_PRESET_TEMP 			}, // 设置运行温度
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{SpStat_Str_En, 			SpStat_Str_Ge, 			SpStat_Str_Fr, 			SpStat_Str_It, 			SpStat_Str_Sp 		}, REP_MINMAX_SPEED 		}, // 运行转速范围
    {{TmpStat_Str_En, 		TmpStat_Str_Ge, 		TmpStat_Str_Fr, 		TmpStat_Str_It, 		TmpStat_Str_Sp 		}, REP_MINMAX_TEMP 			}, // 运行温度范围
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{DTime_Str_En, 			DTime_Str_Ge, 			DTime_Str_Fr, 			DTime_Str_It, 			DTime_Str_Sp 			}, REP_STRING 					}, // Disintegration Time:
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Station_Str_En, 		Station_Str_Ge, 		Station_Str_Fr, 		Station_Str_It, 		Station_Str_Sp 		}, REP_DIS_STATION 			}, // STATION 1/2, Gavin, 2019-3-27 14:27:12, A1.8
    {{P1MS_Str_En,        P1MS_Str_Ge,        P1MS_Str_Fr,        P1MS_Str_It,        P1MS_Str_Sp,      }, REP_DIS_TIME         }, // P1:___min___sec, Gavin, 2019-3-26 16:43:44, A1.6
    {{P2MS_Str_En,        P2MS_Str_Ge,        P2MS_Str_Fr,        P2MS_Str_It,        P2MS_Str_Sp,      }, REP_DIS_TIME         }, // P2:___min___sec, A1.6
    {{P3MS_Str_En,        P3MS_Str_Ge,        P3MS_Str_Fr,        P3MS_Str_It,        P3MS_Str_Sp,      }, REP_DIS_TIME         }, // P3:___min___sec, A1.6
    {{P4MS_Str_En,        P4MS_Str_Ge,        P4MS_Str_Fr,        P4MS_Str_It,        P4MS_Str_Sp,      }, REP_DIS_TIME         }, // P4:___min___sec, A1.6
    {{P5MS_Str_En,        P5MS_Str_Ge,        P5MS_Str_Fr,        P5MS_Str_It,        P5MS_Str_Sp,      }, REP_DIS_TIME         }, // P5:___min___sec, A1.6
    {{P6MS_Str_En,        P6MS_Str_Ge,        P6MS_Str_Fr,        P6MS_Str_It,        P6MS_Str_Sp,      }, REP_DIS_TIME         }, // P6:___min___sec, A1.6
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{RQF_Str_En, 				RQF_Str_Ge, 				RQF_Str_Fr, 				RQF_Str_It, 				RQF_Str_Sp 			  }, REP_STRING 					}, // Requirement fulfilled:
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING           },
    
    {{YON_Str_En, 				YON_Str_Ge, 				YON_Str_Fr, 				YON_Str_It, 				YON_Str_Sp 			  }, REP_DIS_TIME 				}, // ___YES   ___NO, Gavin, 2019-3-27 14:46:13, A1.12
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Signature_Str_En, 	Signature_Str_Ge, 	Signature_Str_Fr, 	Signature_Str_It, 	Signature_Str_Sp 	}, REP_STRING 					}, // 签名
    {{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str 				}, REP_END              }
  };

// 测试工作报告（微打）-------------------------------------------------------------
__flash TReport REPORT_Test_PORT[]=
{
	{{Test_Str1_En,         Test_Str1_Ge,       Test_Str1_Fr,       Test_Str1_It,       Test_Str1_Sp      }, REP_STRING           }, // 0，报告标题
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 1，空行
	
	{{ISTM_Str1_En,         ISTM_Str1_Ge,       ISTM_Str1_Fr,       ISTM_Str1_It,       ISTM_Str1_Sp      }, REP_DEVICE_NAME      }, // 2，设备名称
	{{SNB_Str1_En,          SNB_Str1_Ge,        SNB_Str1_Fr,        SNB_Str1_It,        SNB_Str1_Sp       }, REP_SERIAL           }, // 3，电路板序列号
  {{VER_Str1_En,          VER_Str1_Ge,        VER_Str1_Fr,        VER_Str1_It,        VER_Str1_Sp       }, REP_VERSION          }, // 4，软件版本
  {{CURDT_Str1_En, 			  CURDT_Str1_Ge, 			CURDT_Str1_Fr, 			CURDT_Str1_It, 			CURDT_Str1_Sp			}, REP_CUR_DATETIME			}, // 5，当前日期时间
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 6，
  
	{{HTCal_Str1_En,        HTCal_Str1_Ge,      HTCal_Str1_Fr,      HTCal_Str1_It,      HTCal_Str1_Sp     }, REP_HEATER_DATETIME  }, // 7，Heater Adj.:
	{{SSCal_Str1_En,        SSCal_Str1_Ge,      SSCal_Str1_Fr,      SSCal_Str1_It,      SSCal_Str1_Sp     }, REP_SENSOR_DATETIME  }, // 8，Sensor Adj.:
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 9，空行
	
  {{User_Str1_En,         User_Str1_Ge,       User_Str1_Fr,       User_Str1_It,       User_Str1_Sp      }, REP_STRING 					}, // 10，User:          ___...
  {{BLN_Str1_En,          BLN_Str1_Ge,        BLN_Str1_Fr,        BLN_Str1_It,        BLN_Str1_Sp       }, REP_STRING           }, // 11，Batch/Lot No.: ___...
  {{CMT_Str1_EN,          CMT_Str1_Ge,        CMT_Str1_Fr,        CMT_Str1_It,        CMT_Str1_Sp       }, REP_STRING           }, // 12，Comment:       ___...
  {{Line1_Str1,           Line1_Str1,         Line1_Str1,         Line1_Str1,         Line1_Str1        }, REP_STRING           }, // 13，               ___...
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING 					}, // 14，
  
  {{TST_Str1_En,          TST_Str1_Ge,        TST_Str1_Fr,        TST_Str1_It,        TST_Str1_Sp       }, REP_TEST_STATION     }, // 15，Test station:  1，Gavin, A1, 2019-04-02 星期二 15:50:25
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING 					}, // 16，
  
  {{Settime_Str1_En,      Settime_Str1_Ge,    Settime_Str1_Fr,    Settime_Str1_It,    Settime_Str1_Sp   }, REP_PRESET_RUNTIME 	}, // 17，设置运行时间
  {{Runtime_Str1_En,      Runtime_Str1_Ge,    Runtime_Str1_Fr,    Runtime_Str1_It,    Runtime_Str1_Sp   }, REP_TOTAL_RUNTIME 		}, // 18，实际运行时间
  {{Speed_Str1_En,        Speed_Str1_Ge,      Speed_Str1_Fr,      Speed_Str1_It,      Speed_Str1_Sp     }, REP_PRESET_SPEED     }, // 19，设置运行转速
  {{Temp_Str1_En,         Temp_Str1_Ge,       Temp_Str1_Fr,       Temp_Str1_It,       Temp_Str1_Sp      }, REP_PRESET_TEMP 			}, // 20，设置运行温度
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING 					}, // 21，
    
	//{{TempOff_Str1_En, 		TempOff_Str1_Ge, 		TempOff_Str1_Fr, 		TempOff_Str1_It, 		TempOff_Str1_Sp}, 	REP_TEMP_OFFSET}, 				// 温度偏移
	
  {{SpStat_Str1_En,       SpStat_Str1_Ge,     SpStat_Str1_Fr,     SpStat_Str1_It,     SpStat_Str1_Sp 		}, REP_MINMAX_SPEED 		}, // 22，运行转速范围
  {{TmpStat_Str1_En,      TmpStat_Str1_Ge,    TmpStat_Str1_Fr,    TmpStat_Str1_It,    TmpStat_Str1_Sp   }, REP_MINMAX_TEMP 			}, // 23，运行温度范围
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING 					}, // 24，
  
  {{DTime_Str_En,         DTime_Str_Ge,       DTime_Str_Fr,       DTime_Str_It,       DTime_Str_Sp      }, REP_STRING 					}, // 25，Disintegration Time:
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING 					}, // 26，
  
  {{Station_Str_En, 			Station_Str_Ge, 		Station_Str_Fr, 		Station_Str_It, 		Station_Str_Sp 		}, REP_DIS_STATION 			}, // 27，STATION 1/2, Gavin, 2019-3-27 14:27:12, A1.8
  {{P1MS_Str_En,          P1MS_Str_Ge,        P1MS_Str_Fr,        P1MS_Str_It,        P1MS_Str_Sp,      }, REP_DIS_TIME         }, // 28，P1:___min___sec
  {{P2MS_Str_En,          P2MS_Str_Ge,        P2MS_Str_Fr,        P2MS_Str_It,        P2MS_Str_Sp,      }, REP_DIS_TIME         }, // 29，P2:___min___sec
  {{P3MS_Str_En,          P3MS_Str_Ge,        P3MS_Str_Fr,        P3MS_Str_It,        P3MS_Str_Sp,      }, REP_DIS_TIME         }, // 30，P3:___min___sec
  {{P4MS_Str_En,          P4MS_Str_Ge,        P4MS_Str_Fr,        P4MS_Str_It,        P4MS_Str_Sp,      }, REP_DIS_TIME         }, // 31，P4:___min___sec
  {{P5MS_Str_En,          P5MS_Str_Ge,        P5MS_Str_Fr,        P5MS_Str_It,        P5MS_Str_Sp,      }, REP_DIS_TIME         }, // 32，P5:___min___sec
  {{P6MS_Str_En,          P6MS_Str_Ge,        P6MS_Str_Fr,        P6MS_Str_It,        P6MS_Str_Sp,      }, REP_DIS_TIME         }, // 33，P6:___min___sec
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_DIS_TIME 				}, // 34，
  
  {{RQF_Str_En,           RQF_Str_Ge, 				RQF_Str_Fr, 				RQF_Str_It, 				RQF_Str_Sp 			  }, REP_STRING 					}, // 35，Requirement fulfilled:
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING           }, // 36，
  
  {{YON_Str_En,           YON_Str_Ge, 				YON_Str_Fr, 				YON_Str_It, 				YON_Str_Sp 			  }, REP_DIS_TIME 				}, // 37，___YES   ___NO
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					}, // 38，
  
  {{Signature_Str1_En,    Signature_Str1_Ge, 	Signature_Str1_Fr, 	Signature_Str1_It, 	Signature_Str1_Sp }, REP_STRING 					}, // 39，签名
	{{Empty_Str,            Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str         }, REP_END              }  // 40，结束
};

// 打印机测试报告（普打）---------------------------------------------------------
__flash TReport REPORT_Printer_NORM[]=
  {        
    {{PrTest_Str_En, 			PrTest_Str_En, 			PrTest_Str_En, 			PrTest_Str_En, 			PrTest_Str_En			}, REP_STRING						}, // 标题
    {{Line_Str, 					Line_Str, 					Line_Str, 					Line_Str, 					Line_Str					}, REP_STRING						}, // 横线
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str						}, REP_STRING						}, // 换行
    
    {{Instrument_Str_En, 	Instrument_Str_Ge, 	Instrument_Str_Fr, 	Instrument_Str_It, 	Instrument_Str_Sp	}, REP_DEVICE_NAME			}, // 仪器，品牌/规格/型号
    {{SNB_Str_En, 				SNB_Str_Ge, 				SNB_Str_Fr, 				SNB_Str_It, 				SNB_Str_Sp				}, REP_SERIAL						}, // 电路板序列号
    {{VER_Str_En, 				VER_Str_Ge, 				VER_Str_Fr, 				VER_Str_It, 				VER_Str_Sp				}, REP_VERSION					}, // 软件版本
    {{CURDT_Str_En, 			CURDT_Str_Ge, 			CURDT_Str_Fr, 			CURDT_Str_It, 			CURDT_Str_Sp			}, REP_CUR_DATETIME			}, // 当前日期时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str						}, REP_STRING						}, // 换行
    
    {{HeaterCal_Str_En, 	HeaterCal_Str_Ge, 	HeaterCal_Str_Fr, 	HeaterCal_Str_It, 	HeaterCal_Str_Sp	}, REP_HEATER_DATETIME	}, // 内部PT100校准时间
    {{SensorCal_Str_En, 	SensorCal_Str_Ge, 	SensorCal_Str_Fr, 	SensorCal_Str_It, 	SensorCal_Str_Sp	}, REP_SENSOR_DATETIME	}, // 外部PT100校准时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str						}, REP_STRING 					},
    
    //{{Used_Str_En, 				Used_Str_Ge, 				Used_Str_Fr, 				Used_Str_It, 				Used_Str_Sp 			}, REP_USAGE_COUNTER 		}, // 使用时间？
    //{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str						}, REP_STRING 					},
    
    {{LED_Str_En, 				LED_Str_Ge, 				LED_Str_Fr, 				LED_Str_It, 				LED_Str_Sp 				}, REP_SETTINGS_LED 		}, // 数码管亮度
    {{Sound_Str_En, 			Sound_Str_Ge, 			Sound_Str_Fr, 			Sound_Str_It, 			Sound_Str_Sp 			}, REP_SETTINGS_SOUND 	}, // 声音，打开/关闭
    {{Printer_Str_En, 		Printer_Str_Ge, 		Printer_Str_Fr, 		Printer_Str_It, 		Printer_Str_Sp 		}, REP_SETTINGS_PRINT 	}, // 打印机，关闭/自动/询问
    {{Alarm_Str_En, 			Alarm_Str_Ge, 			Alarm_Str_Fr, 			Alarm_Str_It, 			Alarm_Str_Sp 			}, REP_SETTINGS_ALARM 	}, // 报警，打开/关闭
    {{DTimeF_Str_En, 			DTimeF_Str_Ge, 			DTimeF_Str_Fr, 			DTimeF_Str_It, 			DTimeF_Str_Sp 		}, REP_SETTINGS_DFORMAT }, // 时间格式，EUR/USA
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{PPR_Str_En, 				PPR_Str_Ge, 				PPR_Str_Fr, 				PPR_Str_It, 				PPR_Str_Sp 				}, REP_SETTINGS_PPR 		}, // 编码器
    {{WorkTemp_Str_En, 		WorkTemp_Str_Ge, 		WorkTemp_Str_Fr, 		WorkTemp_Str_It, 		WorkTemp_Str_Sp 	}, REP_SETTINGS_TEMP 		}, // 工作温度范围
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{EPT_Str_En, 				EPT_Str_Ge, 				EPT_Str_Fr, 				EPT_Str_It, 				EPT_Str_Sp 				}, REP_SETTINGS_DHC 		}, // 外部PT100连接，NO/YES
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{RepOk_Str_En, 			RepOk_Str_Ge, 			RepOk_Str_Fr, 			RepOk_Str_It, 			RepOk_Str_Sp 			}, REP_STRING 					}, // 报告结果
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{CURDT1_Str_En, 			CURDT1_Str_Ge, 			CURDT1_Str_Fr, 			CURDT1_Str_It, 			CURDT1_Str_Sp			}, REP_CUR_DATETIME 		}, // 当前时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Signature_Str_En, 	Signature_Str_Ge, 	Signature_Str_Fr, 	Signature_Str_It, 	Signature_Str_Sp 	}, REP_STRING 					}, // 签名
    
    {{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str 				}, REP_END 							}
  };

// 打印机测试报告（微打）---------------------------------------------------------
__flash TReport REPORT_Printer_PORT[]=
{
	{{PrTest_Str1_En,       PrTest_Str1_En,     PrTest_Str1_En,     PrTest_Str1_En,     PrTest_Str1_En    }, REP_STRING           }, // 标题
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{ISTM_Str1_En,         ISTM_Str1_Ge,       ISTM_Str1_Fr,       ISTM_Str1_It,       ISTM_Str1_Sp      }, REP_DEVICE_NAME      }, // 设备名称
	{{SNB_Str1_En,          SNB_Str1_Ge,        SNB_Str1_Fr,        SNB_Str1_It,        SNB_Str1_Sp       }, REP_SERIAL           }, // 电路板序列号
  {{VER_Str1_En,          VER_Str1_Ge,        VER_Str1_Fr,        VER_Str1_It,        VER_Str1_Sp       }, REP_VERSION          }, // 软件版本
  {{CURDT_Str1_En, 			  CURDT_Str1_Ge, 			CURDT_Str1_Fr, 			CURDT_Str1_It, 			CURDT_Str1_Sp			}, REP_CUR_DATETIME			}, // 当前日期时间
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           },
  
	{{HTCal_Str1_En,        HTCal_Str1_Ge,      HTCal_Str1_Fr,      HTCal_Str1_It,      HTCal_Str1_Sp     }, REP_HEATER_DATETIME  }, // Heater Adj.:
	{{SSCal_Str1_En,        SSCal_Str1_Ge,      SSCal_Str1_Fr,      SSCal_Str1_It,      SSCal_Str1_Sp     }, REP_SENSOR_DATETIME  }, // Sensor Adj.:
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{LED_Str1_En,          LED_Str1_Ge,        LED_Str1_Fr,        LED_Str1_It,        LED_Str1_Sp       }, REP_SETTINGS_LED     }, // 数码管亮度
	{{Sound_Str1_En,        Sound_Str1_Ge,      Sound_Str1_Fr,      Sound_Str1_It,      Sound_Str1_Sp     }, REP_SETTINGS_SOUND   }, // 声音开/关
	{{Printer_Str1_En,      Printer_Str1_Ge,    Printer_Str1_Fr,    Printer_Str1_It,    Printer_Str1_Sp   }, REP_SETTINGS_PRINT   }, // 打印模式
	{{AmSet_Str1_En,        AmSet_Str1_Ge,      AmSet_Str1_Fr,      AmSet_Str1_It,      AmSet_Str1_Sp     }, REP_SETTINGS_ALARM   }, // 报警开/关
	{{DTimeF_Str1_En,       DTimeF_Str1_Ge,     DTimeF_Str1_Fr,     DTimeF_Str1_It,     DTimeF_Str1_Sp    }, REP_SETTINGS_DFORMAT }, // 日期时间格式
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{PPR_Str1_En,          PPR_Str1_Ge,        PPR_Str1_Fr,        PPR_Str1_It,        PPR_Str1_Sp       }, REP_SETTINGS_PPR     }, // 电机编码器
	{{WorkTemp_Str1_En,     WorkTemp_Str1_Ge,   WorkTemp_Str1_Fr,   WorkTemp_Str1_It,   WorkTemp_Str1_Sp  }, REP_SETTINGS_TEMP    }, // 温度范围
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{EPT_Str1_En,          EPT_Str1_Ge,        EPT_Str1_Fr,        EPT_Str1_It,        EPT_Str1_Sp       }, REP_SETTINGS_EPT     }, // 外部温度传感器
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{RepOk_Str1_En,        RepOk_Str1_Ge,      RepOk_Str1_Fr,      RepOk_Str1_It,      RepOk_Str1_Sp     }, REP_STRING           }, // 测试结果
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
  {{CURDT1_Str_En,        CURDT1_Str_Ge, 			CURDT1_Str_Fr, 			CURDT1_Str_It, 			CURDT1_Str_Sp			}, REP_CUR_DATETIME 		}, // 当前时间
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
  
  {{Signature_Str1_En,    Signature_Str1_Ge,  Signature_Str1_Fr,  Signature_Str1_It,  Signature_Str1_Sp }, REP_STRING 					}, // 签名
  
  {{Empty_Str,            Empty_Str,          Empty_Str,          Empty_Str,          Empty_Str         }, REP_END 							}
};

// PT100校准报告（普打）------------------------------------------------------------------------------------------------
// 调整报告（普打）
__flash TReport REPORT_Calibr_NORM[]=
  {
    {{CalRep_Str_En, 			CalRep_Str_Ge, 			CalRep_Str_Fr, 			CalRep_Str_It, 			CalRep_Str_Sp			}, REP_ADJ_TITLE 				}, // 标题
    {{Line_Str, 					Line_Str, 					Line_Str, 					Line_Str, 					Line_Str 					}, REP_STRING 					}, // 横线
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str						}, REP_STRING 					}, // 换行
    
    {{Instrument_Str_En, 	Instrument_Str_Ge, 	Instrument_Str_Fr, 	Instrument_Str_It, 	Instrument_Str_Sp	}, REP_DEVICE_NAME 			}, // 仪器，品牌/规格/型号
    {{SNB_Str_En, 				SNB_Str_Ge, 				SNB_Str_Fr, 				SNB_Str_It, 				SNB_Str_Sp				}, REP_SERIAL						}, // 电路板序列号
    {{VER_Str_En, 				VER_Str_Ge, 				VER_Str_Fr, 				VER_Str_It, 				VER_Str_Sp 				}, REP_VERSION 					}, // 软件版本号
  	{{CURDT_Str_En, 			CURDT_Str_Ge, 			CURDT_Str_Fr, 			CURDT_Str_It, 			CURDT_Str_Sp			}, REP_CUR_DATETIME			}, // 当前日期时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{CalType_Str_En, 		CalType_Str_Ge, 		CalType_Str_Fr, 		CalType_Str_It, 		CalType_Str_Sp 		}, REP_TEMP_CHANNEL 		}, // PT100通道，内部（加热器）/外部
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{CalHead_Str, 				CalHead_Str, 				CalHead_Str, 				CalHead_Str, 				CalHead_Str 			}, REP_VCAL_STRING 			}, // 没有使用
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING           },
    
    // Gavin, A8, 下面两项交换先后顺序, 2019-04-11 星期四 11:27:11
    {{NTemp_Str_En, 			NTemp_Str_Ge, 			NTemp_Str_Fr, 			NTemp_Str_It, 			NTemp_Str_Sp 			}, REP_CAL_NOMINAL 			}, // 设备自身测试温度/标准温度
    {{ATemp_Str_En, 			ATemp_Str_Ge, 			ATemp_Str_Fr, 			ATemp_Str_It, 			ATemp_Str_Sp 			}, REP_CAL_POINT 				}, // 标准仪器测试温度/实测温度
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{CalStatus_Str_En, 	CalStatus_Str_Ge, 	CalStatus_Str_Fr, 	CalStatus_Str_It, 	CalStatus_Str_Sp 	}, REP_CAL_STATUS 			}, // 校准结果，成功/失败
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{CURDT1_Str_En, 			CURDT1_Str_Ge, 			CURDT1_Str_Fr, 			CURDT1_Str_It, 			CURDT1_Str_Sp			}, REP_CUR_DATETIME 		}, // 当前时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Signature_Str_En, 	Signature_Str_Ge, 	Signature_Str_Fr, 	Signature_Str_It, 	Signature_Str_Sp 	}, REP_STRING 					}, // 签名
    {{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str 				}, REP_END 							}
  };

// PT100校准报告（微打）------------------------------------------------------------------------------------------------
// 调整报告（微打）
__flash TReport REPORT_Calibr_PORT[]=
{
	{{CalRep_Str1_En,       CalRep_Str1_Ge,     CalRep_Str1_Fr,     CalRep_Str1_It,     CalRep_Str1_Sp    }, REP_ADJ_TITLE 				}, // 标题
	{{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           }, // 空行
	
	{{ISTM_Str1_En,         ISTM_Str1_Ge,       ISTM_Str1_Fr,       ISTM_Str1_It,       ISTM_Str1_Sp      }, REP_DEVICE_NAME      }, // 设备名称
	{{SNB_Str1_En,          SNB_Str1_Ge,        SNB_Str1_Fr,        SNB_Str1_It,        SNB_Str1_Sp       }, REP_SERIAL           }, // 电路板序列号
  {{VER_Str1_En,          VER_Str1_Ge,        VER_Str1_Fr,        VER_Str1_It,        VER_Str1_Sp       }, REP_VERSION          }, // 软件版本
  {{CURDT_Str1_En, 			  CURDT_Str1_Ge, 			CURDT_Str1_Fr, 			CURDT_Str1_It, 			CURDT_Str1_Sp			}, REP_CUR_DATETIME			}, // 当前日期时间
  {{CR_Str,               CR_Str,             CR_Str,             CR_Str,             CR_Str            }, REP_STRING           },
  
  {{CalType_Str_En,       CalType_Str_Ge, 		CalType_Str_Fr, 		CalType_Str_It, 		CalType_Str_Sp 		}, REP_TEMP_CHANNEL 		}, // PT100通道，内部（加热器）/外部
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
  
  // Gavin, A8, 交换以下两行顺序, 2019-04-11 星期四 11:59:52
  {{NTemp_Str1_En,        NTemp_Str1_Ge, 			NTemp_Str1_Fr, 			NTemp_Str1_It, 			NTemp_Str1_Sp 		}, REP_CAL_NOMINAL 			}, // 设备自身测试温度/标准温度
  {{ATemp_Str1_En,        ATemp_Str1_Ge, 			ATemp_Str1_Fr, 			ATemp_Str1_It, 			ATemp_Str1_Sp 		}, REP_CAL_POINT 				}, // 标准仪器测试温度/实测温度
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
  
  {{CalStatus_Str_En,     CalStatus_Str_Ge, 	CalStatus_Str_Fr, 	CalStatus_Str_It, 	CalStatus_Str_Sp 	}, REP_CAL_STATUS 			}, // 校准结果，成功/失败
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
  
  {{CURDT1_Str_En,        CURDT1_Str_Ge, 			CURDT1_Str_Fr, 			CURDT1_Str_It, 			CURDT1_Str_Sp			}, REP_CUR_DATETIME 		}, // 当前时间
  {{CR_Str,               CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
  
  {{Signature_Str1_En,    Signature_Str1_Ge, 	Signature_Str1_Fr, 	Signature_Str1_It, 	Signature_Str1_Sp }, REP_STRING 					}, // 签名
  
  {{Empty_Str,            Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str 				}, REP_END 							}
};

// Gavin, A6, 2019-04-01 星期一 16:09:12
// 检查报告（普打）
__flash TReport REPORT_CheckCal_NORM[]=
  {
  	// Gavin, A8, 2019-04-11 星期四 10:22:29, 标题修改
    //{{CheckRep_Str_En, 		CheckRep_Str_Ge, 		CheckRep_Str_Fr, 		CheckRep_Str_It, 		CheckRep_Str_Sp 	}, REP_STRING 					},
    {{CheckRep_Str_En, 		CheckRep_Str_Ge, 		CheckRep_Str_Fr, 		CheckRep_Str_It, 		CheckRep_Str_Sp 	}, REP_CHECK_TITLE 				}, 					// 标题
    {{Line_Str, 					Line_Str, 					Line_Str, 					Line_Str, 					Line_Str 					}, REP_STRING 					},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{Instrument_Str_En, 	Instrument_Str_Ge, 	Instrument_Str_Fr, 	Instrument_Str_It, 	Instrument_Str_Sp }, REP_DEVICE_NAME 			},
    //{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp 				}, REP_HOT_SN 					}, 		// 加热器序列号
    {{SNB_Str_En, 				SNB_Str_Ge, 				SNB_Str_Fr, 				SNB_Str_It, 				SNB_Str_Sp 				}, REP_SERIAL 					}, 		// 电路板序列号
  	{{VER_Str_En, 				VER_Str_Ge, 				VER_Str_Fr, 				VER_Str_It, 				VER_Str_Sp 				}, REP_VERSION 					},    
    {{CURDT_Str_En, 			CURDT_Str_Ge, 			CURDT_Str_Fr, 			CURDT_Str_It, 			CURDT_Str_Sp			}, REP_CUR_DATETIME			}, 			// Gavin, A8, 2019-04-11 星期四 10:49:29, 当前日期时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{HeaterCal_Str_En, 	HeaterCal_Str_Ge, 	HeaterCal_Str_Fr, 	HeaterCal_Str_It, 	HeaterCal_Str_Sp 	}, REP_HEATER_DATETIME 	},		// 2016/4/20 星期三 15:17:04
    {{SensorCal_Str_En, 	SensorCal_Str_Ge, 	SensorCal_Str_Fr, 	SensorCal_Str_It, 	SensorCal_Str_Sp 	}, REP_SENSOR_DATETIME 	},		// 2016/4/20 星期三 15:18:51
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CheckType_Str_En, 	CheckType_Str_Ge, 	CheckType_Str_Fr, 	CheckType_Str_It, 	CheckType_Str_Sp 	}, REP_CHECK_CHANNEL 		},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{NomTemp_Str_En, 		NomTemp_Str_Ge, 		NomTemp_Str_Fr, 		NomTemp_Str_It, 		NomTemp_Str_Sp 		}, REP_CHECK_NOMINAL 		},
    {{ReadTemp_Str_En, 		ReadTemp_Str_Ge, 		ReadTemp_Str_Fr, 		ReadTemp_Str_It, 		ReadTemp_Str_Sp 	}, REP_CHECK_READ 			},
    {{RealTemp_Str_En, 		RealTemp_Str_Ge, 		RealTemp_Str_Fr, 		RealTemp_Str_It, 		RealTemp_Str_Sp 	}, REP_CHECK_REAL 			},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CalStatus_Str_En, 	CalStatus_Str_Ge, 	CalStatus_Str_Fr, 	CalStatus_Str_It, 	CalStatus_Str_Sp 	}, REP_CHECK_STATUS 		},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    //{{DateSign_Str_En, 		DateSign_Str_Ge, 		DateSign_Str_Fr, 		DateSign_Str_It, 		DateSign_Str_Sp 	}, REP_CUR_DATETIME 		}, 	// Gavin, A8, 2019-04-12 星期五 08:52:03
    {{CURDT1_Str_En, 			CURDT1_Str_Ge, 			CURDT1_Str_Fr, 			CURDT1_Str_It, 			CURDT1_Str_Sp			}, REP_CUR_DATETIME 		}, // 当前时间
    {{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str 						}, REP_STRING 					},
    
    {{Signature_Str_En, 	Signature_Str_Ge, 	Signature_Str_Fr, 	Signature_Str_It, 	Signature_Str_Sp 	}, REP_STRING 					}, // 签名
    {{Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str, 					Empty_Str 				}, REP_END 							}
  };

// 检查报告（微打）
__flash TReport REPORT_CheckCal_PORT[]=
{
	// Gavin, A8, 2019-04-11 星期四 10:23:45, 标题修改
	//{{CheckRep_Str1_En, 	CheckRep_Str1_Ge, 	CheckRep_Str1_Fr, 	CheckRep_Str1_It, 	CheckRep_Str1_Sp}, 		REP_STRING},
	{{CheckRep_Str1_En, 	CheckRep_Str1_Ge, 	CheckRep_Str1_Fr, 	CheckRep_Str1_It, 	CheckRep_Str1_Sp}, 		REP_CHECK_TITLE 				}, 				// 标题
	//{{Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1, 					Line_Str1}, 					REP_STRING},
	{{CR_Str, 						CR_Str, 						CR_Str, 						CR_Str, 						CR_Str}, 							REP_STRING},
	{{ISTM_Str1_En, 			ISTM_Str1_Ge, 			ISTM_Str1_Fr, 			ISTM_Str1_It, 			ISTM_Str1_Sp}, 				REP_DEVICE_NAME},
	//{{HOT_Str_En, 				HOT_Str_Ge, 				HOT_Str_Fr, 				HOT_Str_It, 				HOT_Str_Sp}, 					REP_HOT_SN}, 							// 加热器序列号
  {{SNB_Str1_En,        SNB_Str1_Ge,        SNB_Str1_Fr,        SNB_Str1_It,        SNB_Str1_Sp}, 				REP_SERIAL}, 						// 电路板序列号，Gavin, v7, 2019-04-03 星期三 17:30:47
  {{VER_Str1_En, 				VER_Str1_Ge, 				VER_Str1_Fr, 				VER_Str1_It, 				VER_Str1_Sp}, 				REP_VERSION},
	// Gavin, A8, 增加打印日期时间, 2019-04-11 星期四 11:07:04
	{{CURDT_Str1_En, 			CURDT_Str1_Ge, 			CURDT_Str1_Fr, 			CURDT_Str1_It, 			CURDT_Str1_Sp}, 			REP_CUR_DATETIME}, 			// 当前日期时间
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

__flash char reportNoAdjustment[LANGUAGE_MODE_LAST][30]=
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
  //{"Der TEST wurde ausgef\201hrt!"},
  {"Der TEST wurde durchgef\201hrt!"},
  
  {"Le TEST est accompli!"},
  {"Il TEST \x8A terminato!"},
  {"TEST terminado!"}
};

__flash char testNotCompleted[LANGUAGE_MODE_LAST][65]=
{
  {"TEST was NOT succesfully completed, parameteres out of specs!"},
  //{"Der TEST wurde NICHT ausgef\201hrt!"},
  // 2016/6/2 星期四 17:22:31---------------------------------------------------
  //{"Der TEST wurde NICHT ordnungsgem\204\341 durchgef\201hrt!"},
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
  // Gavin, A8, 2019-04-11 星期四 11:17:28
  //{"Erfolgreich"},
  {"Bestanden"},
  
  {"Accompli"},
  {"Eseguito"},
  {"Realizado"}
};

// 2016/5/22 星期日 11:59:47--------------------------------
//__flash char printCalError[LANGUAGE_MODE_LAST][10]=
__flash char printCalError[LANGUAGE_MODE_LAST][18]=
{
  //Gavin, A8, 2019-04-11 星期四 15:19:22
  //{"Error"},
  {"Failed"},
  // Gavin, A8, 2019-04-11 星期四 11:18:21
  //{"Fehler"},
  {"Durchgefallen"},
  
  {"Erreur"},
  {"Errore"},
  {"Error"}
};

// 2016/5/22 星期日 12:00:48--------------------------------
//__flash char printHeater[LANGUAGE_MODE_LAST][15]=
__flash char printHeater[LANGUAGE_MODE_LAST][18]=
{
  {"heater"},
  {"der Heizung"},
  
  {"du Thermostat"},
  {"di Termostato"},
  {"del Termostato"}
};

__flash char printExtSensor[LANGUAGE_MODE_LAST][30]=
{
  // Gavin, v7, 2019-04-03 星期三 17:41:08
  //{"external temp. sensor"},
  //{"externer Temp.-Sensor"},
  {"ext. temp. sensor"},
  {"ext. Temp. Sensor"},
  
  {"du Senseur ext\202rieur"},
  {"di Sensore esterno"},
  {"del Sensor exterior"}
};

// 校验PT100标题变量，2018-1-20 星期六 18:19:06
__flash char printHeaterBT[LANGUAGE_MODE_LAST][18]=
{
  {"HEATER"},
  {"HEIZUNG"},
  
  {"du Thermostat"},
  {"di Termostato"},
  {"del Termostato"}
};

__flash char printExtSensorBT[LANGUAGE_MODE_LAST][30]=
{
  {"TEMP. SENSOR"},
  {"TEMP. SENSOR"},
  
  {"du Senseur ext\202rieur"},
  {"di Sensore esterno"},
  {"del Sensor exterior"}
};

__flash char printSp_0[]=
{
	""
};

__flash char printSp_1[]=
{
	" "
};

__flash char printSp_3[]=
{
	"   "
};

__flash char printSp_4[]=
{
	"    "
};

__flash char printSp_14[]=
{
	"              "
};

__flash char printSp_16[]=
{
	"                "
};

__flash char printSp_18[]=
{
	"                  "
};

__flash char printSp_19[]=
{
	"                   "
};

__flash char printAutosampler[LANGUAGE_MODE_LAST][20]=
{
  {"of Autosampler"},
  {"des Autosamplers"},
  {"du Autosampler"},
  {"di Autosampler"},
  {"del Autosampler"}
};

__flash char printYES[LANGUAGE_MODE_LAST][5]=
{
	{"YES"},
	{"JA"},
	{"YES"},
	{"YES"},
	{"YES"}
};

__flash char printNO[LANGUAGE_MODE_LAST][5]=
{
	{"NO"},
	{"NEIN"},
	{"NO"},
	{"NO"},
	{"NO"}
};

__flash char printTimeOver[LANGUAGE_MODE_LAST][15]=
{
	{"Time Over!\r\n"},
	{"Time Over!\r\n"},
	
	{"Time Over!\r\n"},
	{"Time Over!\r\n"},
	{"Time Over!\r\n"}
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
	char tempBuffer3[25];
	char tempBuffer4[25];
	//char tempBuffer5[20];
	TPrinterStatus prnStatus;
	TByte key;
	TBool flag;
	TTimeDate dt;
	TBool tempAlarm=false;
	TBool speedAlarm=false;
	TByte cntHourGlass=0;
	
	TByte cntHead = 0; 										// Gavin, A1, 2019-04-04 星期四 13:11:42
	
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
	
	//LED_Print_P(LED_DISPLAY_SPEED, false, blankStr);
	LED_Print_P(LED_DISPLAY_TEMP, false, blankStr);
	
	// printer is free
	
	REPORT_PrintInit=true;
	REPORT_PrintLine=0;
	REPORT_PrintStat=0;
	REPORT_Lines=0;
	REPORT_Pages=0;
	
	REPORT_PagesNum=(FHeader.Number/(EEPROM1.ReportLines-REPORT_HEADER_SIZE))+1;
	
	if (((FHeader.Number % (TWord)(EEPROM1.ReportLines - REPORT_HEADER_SIZE)) + REPORT_FOOTER_SIZE) > (TWord)(EEPROM1.ReportLines - REPORT_HEADER_SIZE))
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
			//LED_Print_P(LED_DISPLAY_TEMP, false, printStr1);
			LED_Print_P(LED_DISPLAY_TIME, false, printStr1);
		
		if (cntHourGlass==1)
			//LED_Print_P(LED_DISPLAY_TEMP, false, printStr2);
			LED_Print_P(LED_DISPLAY_TIME, false, printStr2);
		
		if (cntHourGlass==2)
			//LED_Print_P(LED_DISPLAY_TEMP, false, printStr3);
			LED_Print_P(LED_DISPLAY_TIME, false, printStr3);
		
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
			//LED_Print_P(LED_DISPLAY_TEMP, true, printStr0);
			//LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);
			LED_Print_P(LED_DISPLAY_TIME, true, printStr0);
			LED_Print_P(LED_DISPLAY_TEMP, true, errorStr);
			
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
							case DEVICE_ZT121:
							{
								type=1;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							case DEVICE_ZT122:
							{
								type=2;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt, (int)type, (int)vessels);
							}break;
							default: ;
						}
					}break;
					// Gavin, 2019-3-26 15:23:14, A2.5
					/*
					case DEVICE_LOGO_CALEVA:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							//2014/3/19 14:01:28
							case DEVICE_ZT121:
							{
								type=1;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmtCaleva1, (int)type, (int)vessels);
							}break;
							case DEVICE_ZT122:
							{
								type=2;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmtCaleva2, (int)type, (int)vessels);
							}break;
							default: ;
						}
					}break;
					case DEVICE_LOGO_TABTEST:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							case DEVICE_ZT121:
							{
								type=1;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt1, (int)type, (int)vessels);
							}break;
							case DEVICE_ZT122:
							{
								type=2;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt2, (int)type, (int)vessels);
							}break;
							default: ;
						}
					}break;
					case DEVICE_LOGO_QTEST:
					{
						// Gavin 20131224-----------------------------
						switch(globalDeviceType)
						{
							//2014/3/19 14:01:28
							case DEVICE_ZT121:
							{
								type=1;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt1, (int)type, (int)vessels);
							}break;
							case DEVICE_ZT122:
							{
								type=2;
								vessels=EEPROM1.NumVessels;
								sprintf_P(deviceType, devFmt2, (int)type, (int)vessels);
							}break;
							default: ;
						}
					}break;
					*/
					default: ;
				}
				
				if (prnReport[REPORT_PrintLine].type==REP_HEATER_DATETIME) // 加热器时间
				{
					dt=EEPROM1.timeHeaterCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_SENSOR_DATETIME) 		// 传感器时间
				{
					dt=EEPROM1.timeSensorCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_ASAMPLER_DATETIME) 	// 样品时间
				{
					dt=EEPROM1.timeSamplerCal;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_SERVICE_DATETIME) 	// 服务时间
				{
					dt=EEPROM1.timeService;
				}
				else 
				if (prnReport[REPORT_PrintLine].type==REP_TEST_DATETIME) 			// 测试时间
				{
					dt=FHeader.StartTime;
				}
				else
				{
					RTC_GetDateTime(&dt); 	// 当前时间
				}
				
				if (!TIMEDATE_IsValidDate(dt)) 		// 时间非法？
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
							strcpy_P(tempBuffer2, reportNoAdjustment[EEPROM1.ReportLanguage]);
							strcpy_P(tempBuffer3, emptyStr);
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
						}
					}
				}
				else
				{
					// 年月日------------------
					if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
						sprintf_P(tempBuffer2,paserDateFormatEUR,dt.Day,dt.Month,dt.Year);
					else
						sprintf_P(tempBuffer2,paserDateFormatUSA,dt.Month,dt.Day,dt.Year);
					
					// 时分------------------
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
						//EEPROM_ReadString(tempBuffer5, EEPROM1.SerialNumber, SERIAL_NUM_MAX_LEN+1); // Gavin，2017-12-10 星期日 11:50:44
						//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer4, deviceType, tempBuffer5, tempBuffer2, tempBuffer3);
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer4, deviceType);
					} 
					else
					{
						if (0)//((prnReport[REPORT_PrintLine].type==REP_SENSOR_DATETIME)&&(globalDeviceType==DEVICE_DT620))
							strcpy_P(tempBuffer, newStr);
						else
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
					}
				}
			}break;
			// Gavin, 2019-3-26 15:53:13, A1.2
			case REP_TEST_STATION:
			{
				TByte type;
				
				if(globalDeviceType == DEVICE_ZT121)
				{
					type=1;
				}
				else
				if(globalDeviceType == DEVICE_ZT122)
				{
					type=2;
				}
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type);
				
			}break;
			// Gavin, 2019-3-27 14:34:51, A1.11
			case REP_DIS_STATION:
			{
				TByte type;
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM) 					// 普通打印机
				{
					if(globalDeviceType == DEVICE_ZT121)
					{
						type=1;
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, "");
					}
					else
					if(globalDeviceType == DEVICE_ZT122)
					{
						type=2;
						sprintf_P(tempBuffer2, prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, "");
						type=1;
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, tempBuffer2);
					}
				}
				
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 					// 微型打印机
				{
					if(globalDeviceType == DEVICE_ZT121)
					{
						type=1;
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, "");
					}
					else
					if(globalDeviceType == DEVICE_ZT122)
					{
						if(cntHead == 0)
						{
							type = 1;
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, "");
						}
						
						if(cntHead == 1)
						{
							type = 2;
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)type, "");
						}
					}
				}
			}break;
			// Gavin, 2019-3-26 16:41:52, A1.5
			case REP_DIS_TIME:
			{
				TByte type;
				
				if(globalDeviceType == DEVICE_ZT121)
				{
					type=1;
				}
				else
				if(globalDeviceType == DEVICE_ZT122)
				{
					type=2;
				}
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					if(type == 1)
					{
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n");
					}
					else
					if(type == 2)
					{
						sprintf_P(tempBuffer2, prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n");
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
					}
				}
				else
				if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				{
					if(type == 2) 													// ZT122
					{
						if(REPORT_PrintLine == 37) 						// 打印到需要回头打第二次的行号
						{
							cntHead++;
							if(cntHead < 2)
							{
								sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n\r\n");
								REPORT_PrintLine = 26;
							}
							else
								sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n");
						}
						else
							sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n");
					}
					else
						sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], "\r\n");
				}
			}break;
			case REP_PRESET_RUNTIME:
			{
				//if test unlimited - print UNLIMITED, otherwise print set time
				//if (globalTestStatus.Unlimited) 
				
				// 2017/1/14 12:32:20,客户要求不再需要转速和温度的更新频率参数，同时不打印对应参数
				if (!FHeader.Time)
					strcpy_P(tempBuffer2, unlimitedStr[EEPROM1.ReportLanguage]);
				else
					sprintf_P(tempBuffer2, timeRepFmt,(TWord)(FHeader.Time/3600),(TWord)(((FHeader.Time)%3600)/60),(TWord)(FHeader.Time%60));
				
				//if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				//{
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Speed, tempBuffer2);
				//}
				//else
				//if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				//{
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				//}
				
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				
				//TDWord Ticker;
				
				//Ticker=globalTestStatus.Ticker;
				//Ticker=FHeader.RunTime;
				
				//if (Ticker<DT_MAX_TIME)
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage],(TWord)(Ticker/3600),(TWord)((Ticker%3600)/60),(TWord)(Ticker%60));
				//else
				//	tempBuffer[0]=0;
				
				//Ticker = FHeader.Time;
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_PRESET_SPEED:
			{
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (TReal32)FHeader.Speed);
			}break;
			case REP_PRESET_TEMP:
			{
				/*
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
				*/
				
				if(FHeader.TempOffset >= 0)
				{
					sprintf_P(tempBuffer2, STempOffset1, (TReal32)FHeader.TempOffset/10);
				}
				else
				{
					sprintf_P(tempBuffer2, STempOffset2, fabs((TReal32)FHeader.TempOffset/10));
				}
				
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Temp, (TReal32)FHeader.TempOffset/10);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], FHeader.Temp, tempBuffer2);
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
			case REP_TOTAL_RUNTIME: 					// 实际运行时间
			{
				TDWord Ticker;
				
				//Ticker=globalTestStatus.Ticker;
				Ticker=FHeader.RunTime;
				
				if (Ticker<DT_MAX_TIME)
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage],(TWord)(Ticker/3600),(TWord)((Ticker%3600)/60),(TWord)(Ticker%60));
				else
					sprintf_P(&tempBuffer[SpaceNo], printTimeOver[EEPROM1.ReportLanguage]);
					//tempBuffer[0]=0;
			}break;
			case REP_MINMAX_SPEED: 						// 运行转速范围
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
      case REP_ADJ_TITLE: 							// PT100校验报告标题
      {
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_ADC) // 英文/德文：EXT.TEMP.SENS；相同
      	{
      		strcpy_P(tempBuffer2,printExtSensorBT[EEPROM1.ReportLanguage]);
      		switch(EEPROM1.ReportLanguage)
      		{
      			case LANGUAGE_MODE_ENGLISH: // 英文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_0);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_14);
      			}break;
      			case LANGUAGE_MODE_GERMAN: // 德文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_1);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_16);
      			}break;
      			default:
      			{
      				;
      			}break;
      		}
      	}
      	else
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_DHC) 	// 英文/德文：{"HEATER"},{"HEIZUNG"},
      	{
      		strcpy_P(tempBuffer2,printHeaterBT[EEPROM1.ReportLanguage]);
      		switch(EEPROM1.ReportLanguage)
      		{
      			case LANGUAGE_MODE_ENGLISH: // 英文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_3);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_18);
      			}break;
      			case LANGUAGE_MODE_GERMAN: // 德文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_4);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_19);
      			}break;
      			default:
      			{
      				;
      			}break;
      		}
      	}
      	else
      		strcpy_P(tempBuffer2,printAutosampler[EEPROM1.ReportLanguage]);
      	
      	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3, tempBuffer2);
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
      	if (CALIBRATE_Point[0][0].channel==CALIBRATE_AS) // 除了外部和内部通道的其它通道
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
          
          // 温度传感器校验时的名称问题，调换了变量值，2018/1/19 14:40:36
          //sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].actTemperature/10);
          sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].Temperature/10);
        }
        
        // Gavin, A8, 减少一个参数，2019-04-11 星期四 11:48:34
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_PrintStat+1, tempBuffer2);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CAL_NOMINAL:
			{
        if (CALIBRATE_Point[0][0].channel==CALIBRATE_AS) // 除了外部和内部通道的其它通道
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
          
          // 温度传感器校验时的名称问题，调换了变量值，2018/1/19 14:40:36
          //sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].Temperature/10);
          sprintf_P(tempBuffer2, tempCalFmt, (TReal32)CALIBRATE_Point[REPORT_PrintStat][0].actTemperature/10);
        }
        
        // Gavin, A8, 2019-04-11 星期四 11:46:39
				//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], REPORT_PrintStat+1, tempBuffer2);
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
			}break;
			case REP_CAL_STATUS:
			{
        if (CALIBRATE_Ok)
          strcpy_P(tempBuffer2,printCalSuccess[EEPROM1.ReportLanguage]);
        else
          strcpy_P(tempBuffer2,printCalError[EEPROM1.ReportLanguage]);
        
        sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
      }break;
			case REP_CHECK_TITLE: 						// 检查报告标题
      {
      	if (CALIBRATE_CheckPoint[0].channel==CALIBRATE_ADC) // 英文/德文：EXT.TEMP.SENS；相同
      	{
      		strcpy_P(tempBuffer2,printExtSensorBT[EEPROM1.ReportLanguage]);
      		switch(EEPROM1.ReportLanguage)
      		{
      			case LANGUAGE_MODE_ENGLISH: // 英文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_0);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_14);
      			}break;
      			case LANGUAGE_MODE_GERMAN: // 德文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_1);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_16);
      			}break;
      			default:
      			{
      				;
      			}break;
      		}
      	}
      	else
      	if (CALIBRATE_CheckPoint[0].channel==CALIBRATE_DHC) 	// 英文/德文：{"HEATER"},{"HEIZUNG"},
      	{
      		strcpy_P(tempBuffer2,printHeaterBT[EEPROM1.ReportLanguage]);
      		switch(EEPROM1.ReportLanguage)
      		{
      			case LANGUAGE_MODE_ENGLISH: // 英文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_3);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_18);
      			}break;
      			case LANGUAGE_MODE_GERMAN: // 德文
      			{
      				if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 	// 微型打印机
      					strcpy_P(tempBuffer3,printSp_4);
      				else 																				// 普通打印机
      					strcpy_P(tempBuffer3,printSp_19);
      			}break;
      			default:
      			{
      				;
      			}break;
      		}
      	}
      	else
      		strcpy_P(tempBuffer2,printAutosampler[EEPROM1.ReportLanguage]);
      	
      	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3, tempBuffer2);
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
						//if ((TWord)(abs(CALIBRATE_CheckPoint[key].readTemperature - CALIBRATE_CheckPoint[key].actTemperature)) < (TWord)(MAX_TEMP_DIFF * 10))
						//	flag=true;
						
						// Gavin, A6, 2019-04-01 星期一 16:37:52
						if((abs(CALIBRATE_CheckPoint[key].Temperature-CALIBRATE_CheckPoint[key].readTemperature)<=(TsWord)(MAX_TEMP_DIFF*10))&&
							 (abs(CALIBRATE_CheckPoint[key].Temperature-CALIBRATE_CheckPoint[key].actTemperature)<=(TsWord)(MAX_TEMP_DIFF*10))&&
							 (abs(CALIBRATE_CheckPoint[key].readTemperature-CALIBRATE_CheckPoint[key].actTemperature)<=(TsWord)(MAX_TEMP_DIFF*10)))
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
					//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.LED, (int)EEPROM1.NumVessels);
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], (int)EEPROM1.LED);
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
				//strcpy_P(tempBuffer3, CONF_LIST_ALARM[EEPROM1.Alarm]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
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
				//strcpy_P(tempBuffer3, CONF_LIST_DFORMAT[EEPROM1.DFormat]);
				
				if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				{
					//sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
					sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
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
				strcpy_P(tempBuffer2, CONF_LIST_STIR[EEPROM1.MotorStop]);
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
				//if (globalDeviceStatus.dhcFailure)
				//	strcpy_P(tempBuffer2, noStr);
				//else
				//	strcpy_P(tempBuffer2, yesStr);
				
				if ((!globalDeviceStatus.installedExtADC)||(globalDeviceStatus.sensorExtError))
					strcpy_P(tempBuffer3, printNO[EEPROM1.ReportLanguage]); //noStr);
				else
					strcpy_P(tempBuffer3, printYES[EEPROM1.ReportLanguage]); //yesStr);
				
				//if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				//{
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2, tempBuffer3);
				//}
				//else
				//if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				//{
				//	sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer2);
				//}
				
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], tempBuffer3);
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
				// 2017-12-11 星期一 20:39:46, 46是电机的减速比-------------------------
				sprintf_P(&tempBuffer[SpaceNo], prnReport[REPORT_PrintLine].name_fmt[EEPROM1.ReportLanguage], 46*(int)EEPROM1.PulsesPerRotat);
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
			
			// 当前行数等于60行，同时不是报告结尾---------------------------------------------------------
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
			
			// Gavin, A8, 2019-04-12 星期五 11:06:41
			//if (prnReport[REPORT_PrintLine].type==REP_CAL_NOMINAL)
			if (prnReport[REPORT_PrintLine].type==REP_CAL_POINT)
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
