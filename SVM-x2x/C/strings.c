#include "strings.h"

__flash char paserDateFormatEUR[]="%02d.%02d.20%02d"; 
__flash char paserDateFormatUSA[]="%02d/%02d/20%02d";

//__flash char paserTimeFormatEUR[]="%02d:%02d.%02d"; 
//__flash char paserTimeFormatUSA_AM[]="%d:%02d.%02dAM";
//__flash char paserTimeFormatUSA_PM[]="%d:%02d.%02dPM";
__flash char paserTimeFormatEUR[]="%02d:%02d:%02d"; 				// Gavin, 2020-05-21 星期四 10:55:15
__flash char paserTimeFormatUSA_AM[]="%d:%02d:%02dAM";
__flash char paserTimeFormatUSA_PM[]="%d:%02d:%02dPM";

__flash char reportNoDate[]="____________";
__flash char reportNewLine[]="\r\n";
__flash char reportEmptyLine[]="\x0";
//__flash char reportFirstStation[]="I station \x0"; 				// Gavin, 2020-05-21 星期四 10:49:36
__flash char reportFirstStation[]="Station I \x0";
//__flash char reportFirstStationSm[]=" I st. \x0";
__flash char reportFirstStationSm[]=" St. I \x0";
//__flash char reportSecondStation[]=" II station\x0";
__flash char reportSecondStation[]=" Station II\x0";
//__flash char reportSecondStationSm[]=" II st.\x0";
__flash char reportSecondStationSm[]=" St. II\x0";

__flash char reportFirstColumn[]="__________\x0";
__flash char reportFirstColumnSm[]="_______\x0";
__flash char reportSecondColumn[]="|__________\x0";
__flash char reportSecondColumnSm[]="|_______\x0";

__flash char printError[]="Error!";
__flash char unlimitedStr[]="UNLIMITED";
__flash char blankStr[]="";
__flash char reportNewDoubleLine[]="\r\n\n";

__flash char reportSetFont[]="\x1B\x21\x02";
__flash char reportReset[]="\x1B\x40";

__flash char reportSetLangEn[]="\x1B\x52\x00";
__flash char reportSetLangGe[]="\x1B\x52\x02";
__flash char reportSetLangFr[]="\x1B\x52\x01";
__flash char reportSetLangIt[]="\x1B\x52\x06";
__flash char reportSetLangSp[]="\x1B\x52\x07";

__flash char decFmt[]="%04d";

// v1.30，Gavin，增加质量及体积显示格式，2019-08-18 星期日 11:27:08
__flash char weihFmt[]="%5.1f";
__flash char voltFmt[]="%4d";

__flash char speedFmt[]="%5.1f";
__flash char setSpeedFmt[]=" %03d";
__flash char revFmt[]="%04d";
__flash char revEnterFmt[]="%4d";

__flash char minSpeedFmt[]="%04.1f";
__flash char mmSpeedFmt[]="%04.1f-%04.1f";

// Q1, Gavin, 2019-05-09 星期四 15:02:06
__flash char minSpeedFmt2[]="    %04.1f";
__flash char mmSpeedFmt2[]="    %04.1f-%04.1f";

__flash char timeFmt[]="%02d.%02d";
__flash char testTimeFmt[]="%02d:%02d:%02d";
__flash char yearFmt[]="20%02d";
__flash char dayFmt[]="D %02d";
__flash char timeStr[]="TIME";
__flash char dateStr[]="DATE";
__flash char yearStr[]="YEAR";
__flash char formatStr[]="FRMT";

__flash char printReq[]="PRN?";
__flash char printStr0[]="PRN";
__flash char printStr1[]="PRN\x01";
__flash char printStr2[]="PRN\x02";
__flash char printStr3[]="PRN\x03";

__flash char errorMotor[]="MOTR";
__flash char errorUSB[]="USB ";

__flash char okStr[]="OK";
__flash char errorStr[]="Err";
__flash char timeOver[]="OVER";
__flash char revOver[]="EEEE";

// v1.30，Gavin，增加质量及体积超限显示格式，2019-08-18 星期日 11:27:08
__flash char weihOver[]="EEEE";
__flash char voltOver[]="EEEE";

__flash char devFmt[]="%s-%s";

__flash char emptying[]="EMPT";

__flash char prnVerFmt[]="%02d.%02d";

// v1.30, Gavin, 打印报告增加格式及字符串，2019-08-22 星期四 17:12:35
//__flash char reportAcalInit[] = "Initial"; 									// v1.30, Gavin, 2019-08-22 星期四 16:59:34
__flash char reportVolt[] 		= "%3d ml       ";
__flash char reportVoltFinal[]= "%3d ml(Final)";
__flash char reportVoltNa[] 	= "Na           ";
__flash char reportWeih[] 		= "%5.1f g       ";
//__flash char reportWeihNa[] 	= ""
__flash char reportTTap[] 		= "%5d    "; 									// 数据格式占用9个字符位
__flash char reportDensity[] 	= "%-4.2f g/ml";
__flash char reportHRatio[] 	= "%-4.2f     ";
__flash char reportCIndex[] 	= "%-4.1f %%   ";

__flash char reportVoltSm[] 		= "%3dml   "; 							// 数据格式占用8个字符位宽
__flash char reportVoltFinalSm[]= "%3dml(F)";
__flash char reportVoltNaSm[] 	= " Na     ";
__flash char reportWeihSm[] 		= "%5.1fg  ";

__flash char reportTTapSm[] 		= "%5d   ";
__flash char reportDensitySm[] 	= "%4.2fg/ml";
__flash char reportHRatioSm[] 	= "%4.2f    ";
__flash char reportCIndexSm[] 	= "%4.1f%%   ";
