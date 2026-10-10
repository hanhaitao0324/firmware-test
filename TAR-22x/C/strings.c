#include "strings.h"

__flash char paserDateFormatEUR[]="%02d.%02d.20%02d"; 
__flash char paserDateFormatUSA[]="%02d/%02d/20%02d";
__flash char paserTimeFormatEUR[]="%02d:%02d.%02d"; 
__flash char paserTimeFormatUSA_AM[]="%d:%02d.%02dAM";
__flash char paserTimeFormatUSA_PM[]="%d:%02d.%02dPM";
__flash char reportNoDate[]="____________";
__flash char reportNewLine[]="\r\n";
__flash char reportEmptyLine[]="\x0";
__flash char reportFirstStation[]="I station \x0";
__flash char reportFirstStationSm[]=" I st. \x0";
__flash char reportSecondStation[]=" II station\x0";
__flash char reportSecondStationSm[]=" II st.\x0";
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

__flash char speedFmt[]="%5.1f";
__flash char setSpeedFmt[]=" %03d";
__flash char revFmt[]="%04d";
__flash char revEnterFmt[]="%4d";

__flash char minSpeedFmt[]="%04.1f";
__flash char mmSpeedFmt[]="%04.1f-%04.1f";

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
__flash char retryStr[]="RTY?"; 				// Gavin, 2019-08-30 ÐÇÆÚÎå 13:30:39

__flash char devFmt[]="%s-%s";

__flash char emptying[]="EMPT";

__flash char prnVerFmt[]="%02d.%02d";
