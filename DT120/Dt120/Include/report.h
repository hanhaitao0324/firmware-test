#ifndef _REPORT_H
  #define _REPORT_H
  
#include "timedate.h"

#define SPACE_SYMBOLS_NORM  (12)
#define SPACE_SYMBOLS_PORT 	(0)

#define REPORT_HEADER_SIZE  (22)
#define REPORT_FOOTER_SIZE  (17)

typedef struct
  {
    char __flash *name_fmt[LANGUAGE_MODE_LAST];
    //char __flash *name_fmt[PTYPE_MODE_LAST][LANGUAGE_MODE_LAST]; 			// Gavin 2015/11/29 22:28:13
    
    //char name_fmt[LANGUAGE_MODE_LAST][65];
    TByte type;
  } TReport;

enum TTypeReport
  {
    REP_STRING,
    REP_PAGE,
    REP_DEVICE_ID,
    REP_DEVICE_NAME,
    REP_HOT_SN, 							// Gavin 2016-10-24 ÐÇÆÚÒ» 14:12:49
    REP_SERIAL,
    REP_VERSION,
    REP_SERVICE_DATETIME,
    REP_HEATER_DATETIME,
    REP_SENSOR_DATETIME,
    REP_ASAMPLER_DATETIME,
    REP_CUR_DATETIME,
    REP_TEST_DATETIME,
    REP_PRESET_RUNTIME,
    REP_PRESET_SPEED,
    REP_PRESET_TEMP,
    REP_METHOD,
    REP_TEMP_OFFSET,
    REP_VOLUME,
    REP_HEAD,
    REP_INTERVAL,
    REP_TOTAL_RUNTIME,
    REP_STROKES_NUM,
    REP_STAT_HEADER0,
    REP_STAT_HEADER,
    REP_STAT_HEADER2,
    REP_STAT_HEADER3,
    REP_STAT,
    REP_MINMAX_SPEED,
    REP_MINMAX_TEMP,
    REP_TEST_RESULT,
    REP_VCAL_STRING,
    REP_TEMP_CHANNEL,
    REP_CAL_POINT,
    REP_CAL_NOMINAL,
    //REP_CAL_POINT2,
    //REP_CAL_NOMINAL2,
    REP_CAL_STATUS,
    REP_CHECK_CHANNEL,
    REP_CHECK_NOMINAL,
    REP_CHECK_REAL,
    REP_CHECK_READ,
    REP_CHECK_ASRES,
    REP_CHECK_STATUS,
    REP_USAGE_COUNTER,
    REP_SETTINGS_LED,
    REP_SETTINGS_NOV, 									// Gavin 2015/12/17 9:17:01
    REP_SETTINGS_SOUND,
    REP_SETTINGS_ALARM, 								// Gavin 2015/12/17 9:20:43
    REP_SETTINGS_PRINT,
    REP_SETTINGS_DFORMAT, 							// Gavin 2015/12/17 9:25:54
    REP_SETTINGS_MODE,
    REP_SETTINGS_MODECOUNT, 						// Gavin 2015/12/17 9:32:04
    REP_SETTINGS_DHC,
    REP_SETTINGS_EPT, 									// Gavin 2015/12/17 9:38:21
    REP_SETTINGS_DROPPER,
    REP_SETTINGS_DTAS,
    REP_SETTINGS_PPR,
    REP_SETTINGS_TEMP,
    REP_LAST,
    REP_END                     
 };

//extern TBool PRN_Busy;

// Gavin 2015/12/2 10:17:15
/*
extern __flash TReport REPORT_Test[];
extern __flash TReport REPORT_Printer[];
extern __flash TReport REPORT_Calibr[];
//extern __flash TReport REPORT_Check[];
extern __flash TReport REPORT_CheckCal[];
extern __flash TReport REPORT_CheckCalAs[];
*/

extern __flash TReport REPORT_Test_NORM[];
extern __flash TReport REPORT_Test_PORT[];

extern __flash TReport REPORT_Printer_NORM[];
extern __flash TReport REPORT_Printer_PORT[];

extern __flash TReport REPORT_Calibr_NORM[];
extern __flash TReport REPORT_Calibr_PORT[];

extern __flash TReport REPORT_CheckCal_NORM[];
extern __flash TReport REPORT_CheckCal_PORT[];

extern __flash TReport REPORT_CheckCalAs_NORM[];
extern __flash TReport REPORT_CheckCalAs_PORT[];

// print testing report
TBool REPORT_Print(TReport __flash *PrReport);

#endif
