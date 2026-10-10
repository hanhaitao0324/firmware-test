#include "strings.h"

__flash char verStr[]="VER";
__flash char blankStr[]="";

__flash char tempFmt[]=" %04.1f";
__flash char speedFmt[]="%4.1f";
__flash char speedFmt2[]=" %4.1f";
__flash char tempFmt2[]="%05.2f";
__flash char otempFmt[]=" %4.1f";
__flash char ospeedFmt[]=" %03d";
__flash char usageDayFmt[]="d%03d";
__flash char usageHourFmt[]="%02d.%02d";
__flash char filterFmt[]="%5.1f";
__flash char basketFmt[]=" %4.1f";
__flash char tempReportFmt[]="%4.1f";
__flash char tempNewReportFmt[]="%4.1f - %4.1f";
__flash char tempNewReport2Fmt[]="    %4.1f    ";
__flash char tempErrorReport[]="     EE    ";
__flash char speedNewReportFmt[]="%5.1f - %5.1f";
__flash char speedNewReport2Fmt[]="   %5.1f     ";
__flash char tempCalFmt[]="%4.1f鳦";
__flash char calReportFmt[]="%s %s %s %s %s %s %s %s";
__flash char chkTempFmt[]="T.%4.1f";
__flash char chkPointFmt[]="CHK.%1d";
__flash char chkAsFmt[]="T.  %d";
__flash char checkTempStr[]="CAL.T";

__flash char clbrFmt[]="ADJ.%d";
__flash char setTempStr[]="SET.T";

__flash char sensFmt[]="T %2d";
__flash char sensNameFmt[]="SNS%d";
__flash char calASStr[]="CA.AS";

__flash char hexFmt[]="%04X";
__flash char decFmt[]="%04d";

__flash char timeFmt[]="%02d.%02d";

// 2017/1/14 12:44:07,分钟后面增加“min”
//__flash char timeRepFmt[]="%02dh%02d";
__flash char timeRepFmt[]="%02dh%02dmin";

__flash char timeFmtWP[]="%02d%02d";
__flash char dayFmt[]="D %02d";
__flash char secondFmt[]="S %02d";

__flash char tempHeatingFmt[]=" %04.1f";
__flash char errorStr[]="Err";
__flash char okStr[]="OK";

__flash char yearFmt[]="20%02d";

__flash char errorTemp[]=" EE";
__flash char tempError[]="EE.E";
__flash char tempReportError[]="--.-";

__flash char errorMotorCurr[]="MTR.1";
__flash char errorMotorEnc[]="MTR.2";
__flash char errorMotorUnkn[]="MTR.3";
__flash char errorDHC[]="DHC ";
__flash char errorUSB[]="USB ";
__flash char errorDropper[]="DROP";
__flash char errorOverHeat[]="OVRH";

__flash char rstStr[]="RST?";
__flash char saveStr[]="SAV?";
__flash char restoreStr[]="RES?";
__flash char messWait[]="WAIT";
__flash char messOK[]="OK"; 						// 2014/7/9 17:56:23
__flash char messDrop[]="DROP";
__flash char calStepStr[]="A.POS";

__flash char presetStr[]="PRST";
__flash char maxTempStr[]="MAXT";

__flash char calibrStr[]="ADJS";
__flash char realStr[]="REAL";

__flash char printReq[]="PRN?";
__flash char printStr0[]="PRN";
__flash char printStr1[]="PRN\x01";
__flash char printStr2[]="PRN\x02";
__flash char printStr3[]="PRN\x03";
__flash char printPage[]="P.%3d";

__flash char intFmt[]="%d";
__flash char strFmt[]="%s";

__flash char timeStr[]="TIME";
__flash char dateStr[]="DATE";
__flash char yearStr[]="YEAR";

__flash char yesStr[]="YES ";
__flash char noStr[]="NO  ";

__flash char paserDateFormatEUR[]="%02d.%02d.20%02d"; 
__flash char paserDateFormatUSA[]="%02d/%02d/20%02d";
__flash char paserTimeFormatEUR[]="%02d:%02d"; 
__flash char paserTimeFormatUSA_AM[]="%d:%02dAM";
__flash char paserTimeFormatUSA_PM[]="%d:%02dPM";

__flash char devFmt[]="DT-%d2%1d";
__flash char devCalevaSt[]="11 ST";
__flash char devCalevaStPlus[]="11+ST";

//2014/3/16 13:57:10
__flash char devFmt1[]="QDT-%d";
__flash char devFmt2[]="QDT-%dS";

//2014/6/18 18:22:01
__flash char devFmtCaleva1[]="CDT-%d";
__flash char devFmtCaleva2[]="CDT-%dS";

//Gavin, v2.03, 2021-03-09 星期二 14:59:56
__flash char infiReq[]="INFI"; 					// 测试停止后，询问是否继续运转；
