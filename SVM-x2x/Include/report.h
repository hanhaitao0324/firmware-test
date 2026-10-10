#ifndef _REPORT_H
#define _REPORT_H
  
#include "timedate.h"

#define SPACE_SYMBOLS       (9)

typedef struct
{
  char name_fmt[70];
  TByte type;
} TReport;

enum REPORT_TYPES
{
  REPORT_TYPE_TEST,
  REPORT_TYPE_PRINTER,
  REPORT_TYPE_OQ,
  REPORT_TYPE_LAST
};

enum TTypeReport
{
  REP_STRING,
  REP_DEVICE_ID,
  REP_DEVICE_NAME,
  REP_SERIAL,
  REP_VERSION,
  REP_CUR_DATETIME,
  REP_PRESET_RUNTIME,
  REP_TOTAL_RUNTIME,
  REP_STROKES_NUM,
  REP_REV_NUM,
  REP_NOM_SPEED,
  REP_DRUM_BIGNUM,
  REP_DRUM_NUM,
  REP_DRUM,
  REP_END_DRUM,
  REP_STAT_HEADER,
  REP_STAT,
  REP_MINMAX_SPEED,
  REP_METHOD,
  REP_STATION,
  REP_RESULTS,
  REP_ACAL_RESULTS0, 					// v1.30, Gavin, 2019-08-22 星期四 17:24:44
  REP_ACAL_RESULTS1, 					// v1.30, Gavin, 2019-08-23 星期五 09:59:22
  REP_ACAL_RESULTS2, 					// v1.30, Gavin, 2019-08-23 星期五 10:00:11
  REP_ACAL_RESULTS3, 					// v1.30, Gavin, 2019-08-23 星期五 12:59:55
  REP_ACAL_RESULTS4, 					// v1.30, Gavin, 2019-08-23 星期五 13:00:00
  REP_ACAL_RESULTS5, 					// v1.30, Gavin, 2019-08-23 星期五 13:00:06
  REP_ACAL_RESULTS6, 					// v1.30, Gavin, 2019-08-23 星期五 13:00:11
  REP_ACAL_RESULTS7, 					// v1.30, Gavin, 2019-08-23 星期五 13:00:16
  REP_ACAL_RESULTS8, 					// v1.30, Gavin, 2020-05-20 星期三 10:16:28
  REP_OQ_HEIGHT,
  REP_OQ_HEIGHT2,
  REP_OQ_STROKES,
  REP_OQ_RUNTIME,
  REP_TEST_RESULT,
  REP_END                     
 };

extern __flash char unlimitedStr[];

extern __flash TReport QReport[];

// print testing report
TBool REPORT_Print(TByte ReportType);//TReport __flash *PrReport);

#endif
