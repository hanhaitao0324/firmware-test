#include "platform.h"
#include "report.h"

__flash TReport TestReport[]=
{        
  {"                        TEST REPORT\r\n",                               REP_STRING},
  {"_____________________________________________________________\r\n\n",   REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Firmware ver:      %s\r\n\n",                                           REP_VERSION},    
  {"USP method:        %1d\r\n\n",                                          REP_METHOD},
  {"Pre-Set Run-Time:  %s\r\n",                                             REP_PRESET_RUNTIME},
  {"Pre-Set Number of Strokes: %4d\r\n",                                    REP_STROKES_NUM},
  {"Total   Run-Time:  %02d:%02d:%02d\r\n",                                 REP_TOTAL_RUNTIME},
  {"\r\n",                                                                  REP_STRING},
  //{"     Measure   Strokes per minute\r\n",                                 REP_STAT_HEADER},
  //{"       %02d.             %03d\r\n",                                     REP_STAT},
  // Q1, Gavin, 增加以下打印内容，2019-5-4 星期六 14:09:00------------------------------------------
  {"                   %s    %s\r\n",                                    	REP_STATION},
  // Q1, Gavin, 修改以下打印内容，2019-5-4 星期六 14:12:30------------------------------------------
  //{"Min.-Max. Speed:   %s Strokes/min\r\n",                                 REP_MINMAX_SPEED},
  {"Min.-Max. Speed:   %s%s Strokes/min\r\n",                           REP_MINMAX_SPEED},
  
  {"\r\n",                                                                  REP_STRING},
  {"%s\r\n\r\n",                                                            REP_TEST_RESULT},
  {"Comments:  ________________________________________________\r\n\n",     REP_STRING},
  {" __________________________________________________________\r\n\n",     REP_STRING},
  {" __________________________________________________________\r\n\n",     REP_STRING},
  {"        TEST RESULTS\r\n\r\n",                                          REP_STRING},
  {"  Density calculation:    %s%s\r\n",                                    REP_STATION},
  {"  Volume before test run: %s%s\r\n",                                    REP_RESULTS},
  {"  Volume after test run:  %s%s\r\n",                                    REP_RESULTS},
  {"  Compressibility index:  %s%s\r\n\n\n",                                REP_RESULTS},
  {"Requirements fulfilled:    YES     NO\r\n\n\n",                         REP_STRING},
  {"Date:   %s %s     ___________________ Signature\r\n\x0C",               REP_CUR_DATETIME},
  {"",                                                                      REP_END}
};

__flash TReport TestReportSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"          TEST REPORT\r\n",            REP_STRING},
  {"______________________________\r\n\n", REP_STRING},
  {"Instrument:   %s %s-%s\r\n",           REP_DEVICE_NAME},
  {"Serial No.:   %s\r\n",                 REP_SERIAL},
  //{"ID No.:       %s\r\n",                 REP_DEVICE_ID},
  {"Firmware ver: %s\r\n\n",               REP_VERSION},    
  {"USP method:   %1d\r\n\n",              REP_METHOD},
  {"Pre-Set Run-Time:  %s\r\n",            REP_PRESET_RUNTIME},
  {"Preset Number of Strokes: %4d\r\n",    REP_STROKES_NUM},
  {"Total Run-Time:    %02d:%02d:%02d\r\n",REP_TOTAL_RUNTIME},
  {"\r\n",                                 REP_STRING},
  //{"   Measure   Strokes per minute\r\n",  REP_STAT_HEADER},
  //{"     %02d.             %03d\r\n",      REP_STAT},
  // Q2, Gavin, 2019-05-09 星期四 15:24:07
  {"          %s\r\n",                     REP_STATION},
  {"Speed:    %s Str/min\r\n",             REP_MINMAX_SPEED},
  {"         %s\r\n",                      REP_STATION},
  {"Speed:    %s Str/min\r\n",             REP_MINMAX_SPEED},
  {"\r\n",                                 REP_STRING},
  {"%s\r\n\r\n",                           REP_TEST_RESULT},
  {"Comments:  _____________________\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {" _______________________________\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {"          TEST RESULTS\r\n\r\n",       REP_STRING},
  {"Density calc.:   %s%s\r\n",            REP_STATION},
  {"Volume before:   %s%s\r\n",            REP_RESULTS},
  {"Volume after:    %s%s\r\n",            REP_RESULTS},
  {"Compress. index: %s%s\r\n",            REP_RESULTS},
  {"\r\n",                                 REP_STRING},
  {"Requirements fulfilled:  YES  NO\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {"Date:   %s %s\r\n\n",                  REP_CUR_DATETIME},
  {"__________________ Signature\r\n",     REP_STRING},
  {" \r\n\n",                              REP_STRING},
  {"",                                     REP_END}
};

// 增加ACAL普打格式，v1.30，Gavin，2019-08-22 星期四 13:57:53
__flash TReport TestReportAcal[]=
{        
  {"                        TEST REPORT\r\n",                               REP_STRING},
  {"_____________________________________________________________\r\n\n",   REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Firmware ver:      %s\r\n\n",                                           REP_VERSION},    
  {"USP method:        %1d\r\n\n",                                          REP_METHOD},
  {"Pre-Set Run-Time:  %s\r\n",                                             REP_PRESET_RUNTIME},
  {"Pre-Set Number of Strokes: %4d\r\n",                                    REP_STROKES_NUM},
  {"Total   Run-Time:  %02d:%02d:%02d\r\n",                                 REP_TOTAL_RUNTIME},
  {"\r\n",                                                                  REP_STRING},
  //{"     Measure   Strokes per minute\r\n",                                 REP_STAT_HEADER},
  //{"       %02d.             %03d\r\n",                                     REP_STAT},
  // Q1, Gavin, 增加以下打印内容，2019-5-4 星期六 14:09:00------------------------------------------
  {"                   %s    %s\r\n",                                    	REP_STATION},
  // Q1, Gavin, 修改以下打印内容，2019-5-4 星期六 14:12:30------------------------------------------
  //{"Min.-Max. Speed:   %s Strokes/min\r\n",                                 REP_MINMAX_SPEED},
  {"Min.-Max. Speed:   %s%s Strokes/min\r\n",                           REP_MINMAX_SPEED},
  
  {"\r\n",                                                                  REP_STRING},
  {"%s\r\n\r\n",                                                            REP_TEST_RESULT},
  {"Comments:  ________________________________________________\r\n\n",     REP_STRING},
  {" __________________________________________________________\r\n\n",     REP_STRING},
  {" __________________________________________________________\r\n\n",     REP_STRING},
  
  // ACAL，v1.30，Gavin，2019-08-22 星期四 15:00:33
  {"  TEST RESULTS\r\n\r\n",                                                REP_STRING},
  {"                     %-10s     %s       Taps\r\n",                   		REP_STATION},
  {"  Sample Weight:     %s  %s\r\n",                                 			REP_ACAL_RESULTS2},
  {"  Volume Initial:    %s   %s       0\r\n",                              REP_ACAL_RESULTS0},
  {"  Volume %2d:         %s   %s    %4d\r\n",                           		REP_ACAL_RESULTS1},
  
  {"\r\n\r\n           Statistics:\r\n\r\n",                                REP_STRING},
  {"                         %-10s   %-10s\r\n",                            REP_STATION},
  {"  Total taps:            %9.9s     %9.9s\r\n", 													REP_ACAL_RESULTS3},
  {"  Initial Density:       %9.9s     %9.9s\r\n", 													REP_ACAL_RESULTS4},
  {"  Tapped Density:        %9.9s     %9.9s\r\n", 													REP_ACAL_RESULTS5},
  {"  Hausner Ratio:         %9.9s     %9.9s\r\n", 													REP_ACAL_RESULTS6},
  {"  Compressibility Index: %9.9s     %.9s\r\n\n\n", 											REP_ACAL_RESULTS7},
  {"%s is used as bulk volume for compressibility index.\r\n\r\n",          REP_ACAL_RESULTS8},
  
  {"Requirements fulfilled:    YES     NO\r\n\n\n",                         REP_STRING},
  {"Date:   %s %s     ___________________ Signature\r\n\x0C",               REP_CUR_DATETIME},
  {"",                                                                      REP_END}
};

// 增加ACAL微打格式，v1.30，Gavin，2019-08-22 星期四 13:58:25
__flash TReport TestReportAcalSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"          TEST REPORT\r\n",            REP_STRING},
  {"______________________________\r\n\n", REP_STRING},
  {"Instrument:   %s %s-%s\r\n",           REP_DEVICE_NAME},
  {"Serial No.:   %s\r\n",                 REP_SERIAL},
  //{"ID No.:       %s\r\n",                 REP_DEVICE_ID},
  {"Firmware ver: %s\r\n\n",               REP_VERSION},    
  {"USP method:   %1d\r\n\n",              REP_METHOD},
  {"Pre-Set Run-Time:  %s\r\n",            REP_PRESET_RUNTIME},
  {"Preset Number of Strokes: %4d\r\n",    REP_STROKES_NUM},
  {"Total Run-Time:    %02d:%02d:%02d\r\n",REP_TOTAL_RUNTIME},
  {"\r\n",                                 REP_STRING},
  //{"   Measure   Strokes per minute\r\n",  REP_STAT_HEADER},
  //{"     %02d.             %03d\r\n",      REP_STAT},
  // Q2, Gavin, 2019-05-09 星期四 15:24:07
  {"          %s\r\n",                     REP_STATION},
  {"Speed:    %s Str/min\r\n",             REP_MINMAX_SPEED},
  {"         %s\r\n",                      REP_STATION},
  {"Speed:    %s Str/min\r\n",             REP_MINMAX_SPEED},
  {"\r\n",                                 REP_STRING},
  {"%s\r\n\r\n",                           REP_TEST_RESULT},
  {"Comments:  _____________________\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {" _______________________________\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {"Test Result\r\n\r\n",       					 REP_STRING},
  
  // ACAL，v1.30，Gavin，2019-08-22 星期四 15:00:33
  {"         %-6s %s   Taps\r\n",    			 REP_STATION},
  {"Weight:   %s%s\r\n",              		 REP_ACAL_RESULTS2},
  {"Vol. Ini.:%s%s    0\r\n",              REP_ACAL_RESULTS0},
  {"Vol.  %2d: %s%s %4d\r\n",              REP_ACAL_RESULTS1},
  
  {"\r\n\nStatistics:\r\n\r\n",            REP_STRING},
  {"             %s  %s\r\n",              REP_STATION},
  {"Total taps:   %s %s\r\n", 			       REP_ACAL_RESULTS3},
  {"Ini. Density: %s %s\r\n", 					   REP_ACAL_RESULTS4},
  {"Tap. Density: %s %s\r\n", 					   REP_ACAL_RESULTS5},
  {"Hausner Ratio:%s %s\r\n", 				     REP_ACAL_RESULTS6},
  {"Comp. Index:  %s %s\r\n\n\n", 		     REP_ACAL_RESULTS7},
  {"%s is used as bulk volume for compressibility index.\r\n\r\n",          REP_ACAL_RESULTS8},
  
  {"Requirements fulfilled: YES  NO\r\n\n\n",  REP_STRING},
  {"Date: %s %s\r\n\r\n___________________ Signature\r\n\x0C",   REP_CUR_DATETIME},
  {"",                                                       REP_END}
};

__flash TReport QReport[]=
{        
  {"                    QUALIFICATION REPORT\r\n",                          REP_STRING},
  {"_____________________________________________________________\r\n\r\n", REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Firmware ver:      %s\r\n\n\n",                                         REP_VERSION},    
  {"Device qualification:\r\n\n",                                           REP_STRING},
  {"                             Nominal       Real\r\n\n",                 REP_STRING},
  {"  Strokes position height:  %4.1f mm       %4.1f mm\r\n",               REP_OQ_HEIGHT},
  {"  Strokes position height:  %4.1f mm       %4.1f mm\r\n",               REP_OQ_HEIGHT2},
  {"  Number of strokes:        %4d          %4d\r\n",                      REP_OQ_STROKES},
  {"  Test run time:            %02d:%02d min.    %02d:%02d min.\r\n",      REP_OQ_RUNTIME},
  {"\r\n\n",                                                                REP_STRING},
  {"Requirements fulfilled:    YES     NO\r\n\n",                           REP_STRING},
  {"Date:   %s %s     ___________________ Signature\r\n\x0C",               REP_CUR_DATETIME},
  {"",                                                                      REP_END}
};

__flash TReport QReportSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"      QUALIFICATION REPORT\r\n",       REP_STRING},
  {"______________________________\r\n\n", REP_STRING},
  {"Instrument:   %s %s-%s\r\n",           REP_DEVICE_NAME},
  {"Serial No.:   %s\r\n",                 REP_SERIAL},
  //{"ID No.:       %s\r\n",                 REP_DEVICE_ID},
  {"Firmware ver: %s\r\n\n",               REP_VERSION},    
  {"Device qualification:\r\n\n",          REP_STRING},
  {"              Nominal    Real\r\n\n",  REP_STRING},
  {"Str. height: %4.1f mm   %4.1f mm\r\n", REP_OQ_HEIGHT},
  {"Str. height: %4.1f mm   %4.1f mm\r\n", REP_OQ_HEIGHT2},
  {"Str. number:  %4d      %4d\r\n",       REP_OQ_STROKES},
  {"Run time:     %02d:%02d     %02d:%02d\r\n", REP_OQ_RUNTIME},
  {"\r\n",                                 REP_STRING},
  {"Requirements fulfilled:  YES  NO\r\n", REP_STRING},
  {"\r\n",                                 REP_STRING},
  {"Date:   %s %s\r\n\n",                  REP_CUR_DATETIME},
  {"__________________ Signature\r\n",     REP_STRING},
  {" \r\n\n",                              REP_STRING},
  {"",                                     REP_END}
};

__flash TReport PrinterReport[]=
{        
  {"                        PRINTER TEST\r\n",                              REP_STRING},
  {"_______________________________________________________________\r\n\n", REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Firmware ver:      %s\r\n\n",                                           REP_VERSION},    
  {"USP method:        %1d\r\n\n",                                          REP_METHOD},
  {"\r\n",                                                                  REP_STRING},
  {"\r\n",                                                                  REP_STRING},
  {"Date:   %s %s     ___________________ Signature\r\n\x0C",               REP_CUR_DATETIME},
  {"",                                                                      REP_END}
};

__flash TReport PrinterReportSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"           PRINTER TEST\r\n",          REP_STRING},
  {"______________________________\r\n\n", REP_STRING},
  {"Instrument:   %s %s-%s\r\n",           REP_DEVICE_NAME},
  {"Serial No.:   %s\r\n",                 REP_SERIAL},
  //{"ID No.:       %s\r\n",                 REP_DEVICE_ID},
  {"Firmware ver: %s\r\n\n",               REP_VERSION},    
  {"USP method:   %1d\r\n\n",              REP_METHOD},
  {"Date:   %s %s\r\n\n",            REP_CUR_DATETIME},
  {"__________________ Signature\r\n",     REP_STRING},
  {" \r\n\n",                              REP_STRING},
  {"",                                     REP_END}
};

__flash char testAborted[20]="TEST was aborted!";
__flash char testCompleted[20]="TEST was completed!";
__flash char testFinished[30]="TEST was finished by user!";

// v1.30, Gavin, 2020-05-20 星期三 10:25:06
__flash char testIVol_V0[] = "V0";
__flash char testIVol_V10[] = "V10";
