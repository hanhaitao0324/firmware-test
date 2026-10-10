#ifndef _STRINGS_H
  #define _STRINGS_H

extern __flash char paserDateFormatEUR[]; 
extern __flash char paserDateFormatUSA[];
extern __flash char paserTimeFormatEUR[]; 
extern __flash char paserTimeFormatUSA_AM[];
extern __flash char paserTimeFormatUSA_PM[];
extern __flash char reportNoDate[];
extern __flash char reportNewLine[];
extern __flash char reportEmptyLine[];
extern __flash char reportFirstStation[];
extern __flash char reportFirstStationSm[];
extern __flash char reportSecondStation[];
extern __flash char reportSecondStationSm[];
extern __flash char reportFirstColumn[];
extern __flash char reportFirstColumnSm[];
extern __flash char reportSecondColumn[];
extern __flash char reportSecondColumnSm[];

extern __flash char printError[];
extern __flash char unlimitedStr[];
extern __flash char blankStr[];
extern __flash char reportNewDoubleLine[];

extern __flash char reportSetFont[];
extern __flash char reportReset[];
extern __flash char reportSetLangEn[];
extern __flash char reportSetLangGe[];
extern __flash char reportSetLangFr[];
extern __flash char reportSetLangIt[];
extern __flash char reportSetLangSp[];

extern __flash char decFmt[];

// v1.30，Gavin，增加质量及体积显示格式，2019-08-18 星期日 11:27:08
extern __flash char weihFmt[];
extern __flash char voltFmt[];

extern __flash char speedFmt[];
extern __flash char setSpeedFmt[];
extern __flash char revFmt[];
extern __flash char revEnterFmt[];

extern __flash char minSpeedFmt[];
extern __flash char mmSpeedFmt[];

// Q1，Gavin，增加了打印格式，2019-05-09 星期四 15:06:49
extern __flash char minSpeedFmt2[];
extern __flash char mmSpeedFmt2[];

extern __flash char timeFmt[];
extern __flash char testTimeFmt[];
extern __flash char yearFmt[];
extern __flash char dayFmt[];
extern __flash char timeStr[];
extern __flash char dateStr[];
extern __flash char yearStr[];
extern __flash char formatStr[];

extern __flash char printReq[];
extern __flash char printStr0[];
extern __flash char printStr1[];
extern __flash char printStr2[];
extern __flash char printStr3[];

extern __flash char errorMotor[];
extern __flash char errorUSB[];

extern __flash char okStr[];
extern __flash char errorStr[];
extern __flash char timeOver[];
extern __flash char revOver[];

// v1.30，Gavin，增加质量及体积超限显示格式，2019-08-18 星期日 11:27:08
extern __flash char weihOver[];
extern __flash char voltOver[];

extern __flash char devFmt[];

extern __flash char emptying[];

extern __flash char prnVerFmt[];

// v1.30, Gavin, 打印报告增加格式及字符串，2019-08-22 星期四 17:12:35
//extern __flash char reportAcalInit[]; 									// v1.30, Gavin, 2019-08-22 星期四 16:59:34
extern __flash char reportVolt[];
extern __flash char reportVoltFinal[];
extern __flash char reportVoltNa[];
extern __flash char reportWeih[];
extern __flash char reportTTap[];
extern __flash char reportDensity[];
extern __flash char reportHRatio[];
extern __flash char reportCIndex[];

extern __flash char reportVoltSm[];
extern __flash char reportVoltFinalSm[];
extern __flash char reportVoltNaSm[];
extern __flash char reportWeihSm[];
extern __flash char reportTTapSm[];
extern __flash char reportDensitySm[];
extern __flash char reportHRatioSm[];
extern __flash char reportCIndexSm[];

#endif
