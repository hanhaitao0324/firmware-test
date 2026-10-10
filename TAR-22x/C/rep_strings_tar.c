#include "platform.h"
#include "report.h"

__flash TReport TestReport[]=
{        
  {"                           TEST REPORT                Page 1/1\r\n",    REP_STRING},
  {"_________________________________________________________________\r\n\n", REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  //{"Friability drum:   _ left _ right\r\n",                                 REP_STRING},
  //{"Abrasion drum:     _ left _ right\r\n",                                 REP_STRING},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  {"Version:           %s\r\n\n",                                           REP_VERSION},    
  {"Date:              %s %s\r\n\n",                                        REP_CUR_DATETIME},
  //{"User:      ________________________________________\r\n\n",             REP_STRING},
  //{"Comments:  ___________________________________________________\r\n",    REP_STRING},
  //{"           ___________________________________________________\r\n\n",  REP_STRING},
  {"Pre-Set Run-Time:     %s\r\n",                                          REP_PRESET_RUNTIME},
  {"Total   Run-Time:     %02d:%02d.%02d\r\n\n",                            REP_TOTAL_RUNTIME},
  {"Number of Revolution: %4d\r\n\n",                                       REP_REV_NUM},
  {"Nominal Speed:        %03d rpm\r\n\n",                                  REP_NOM_SPEED},
  //{"      MEASURE    RPM\r\n",                                              REP_STAT_HEADER},
  //{"        %02d       %04.1f\r\n",                                         REP_STAT},
  {"Min.-Max. Speed:      %s RPM\r\n",                                      REP_MINMAX_SPEED},
  {"\r\n\n",                                                                REP_STRING},
  {" Drum %1d _ Friability _ Abrasion  ",                                   REP_DRUM_BIGNUM},
  {"Product _______________________  ",                                     REP_DRUM},
  {"Batch/lot No:__________________  ",                                     REP_DRUM},
  {"Remarks _______________________  ",                                     REP_DRUM},
  {"Weight before test: _________mg  ",                                     REP_DRUM},
  {"Weight after test:  _________mg  ",                                     REP_DRUM},
  {"Lost: ............. _________mg  ",                                     REP_DRUM},
  {"Lost: ............. __________%  ",                                     REP_DRUM},
  {"\r\n",                                                                  REP_END_DRUM},
  {"%s\r\n\n\n",                                                            REP_TEST_RESULT},
  {"Signature ______________    User ______________\r\n\x0C",               REP_STRING},
  {"",                                                                      REP_END}
};

__flash TReport TestReportSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"           TEST REPORT\r\n",           REP_STRING},
  {"_______________________________\r\n\n",REP_STRING},
  {"Instrument: %s %s-%s\r\n",             REP_DEVICE_NAME},
  //{"Friability drum:  _ left _ right\r\n", REP_STRING},
  //{"Abrasion drum:    _ left _ right\r\n", REP_STRING},
  //{"Instrument ID No.: %s\r\n",            REP_DEVICE_ID},
  {"Serial No.:        %s\r\n",            REP_SERIAL},
  {"Version:           %s\r\n\n",          REP_VERSION},    
  {"Date:       %s %s\r\n\n",              REP_CUR_DATETIME},
  {"\r\n",                                 REP_STRING},
  //{"Comments:  _____________________\r\n", REP_STRING},
  //{"\r\n",                                 REP_STRING},
  //{"________________________________\r\n", REP_STRING},
  //{"\r\n",                                 REP_STRING},
  {"Preset Run-Time: %s\r\n",              REP_PRESET_RUNTIME},
  {"Total Run-Time:  %02d:%02d.%02d\r\n",  REP_TOTAL_RUNTIME},
  {"Number of Revolution: %4d\r\n",        REP_REV_NUM},
  {"Nominal Speed:   %03d rpm\r\n",        REP_NOM_SPEED},
  {"\r\n",                                 REP_STRING},
  //{"    MEASURE        RPM\r\n",           REP_STAT_HEADER},
  //{"      %02d           %04.1f\r\n",      REP_STAT},
  {"Speed:    %s rpm\r\n",                 REP_MINMAX_SPEED},
  {"\r\n",                                 REP_STRING},
  {"Drum %2d _ Friability _ Abrasion\r\n", REP_DRUM_NUM},
  {"Product ________________________\r\n", REP_STRING},
  {"Batch/lot No:___________________\r\n", REP_STRING},
  {"Remarks ______________________\r\n",   REP_STRING},
  {"Weight before test: __________mg\r\n", REP_STRING},
  {"Weight after test: ___________mg\r\n", REP_STRING},
  {"Lost: ............____________mg\r\n", REP_STRING},
  {"Lost: ............_____________%\r\n", REP_STRING},
  {"\r\n",                                 REP_END_DRUM},
  {"%s\r\n\n",                             REP_TEST_RESULT},
  {"User:      _____________________\r\n", REP_STRING},
  {"\r\n\n",                               REP_STRING},
  {"Signature  _____________________\r\n", REP_STRING},
  {" \r\n\n",                              REP_STRING},
  {"",                                     REP_END}
};

__flash TReport PrinterReport[]=
{        
  {"                        PRINTER TEST\r\n",                              REP_STRING},
  {"_________________________________________________________________\r\n\n", REP_STRING},
  {"Instrument:        %s %s-%s\r\n",                                       REP_DEVICE_NAME},
  //{"Instrument ID No.: %s\r\n",                                             REP_DEVICE_ID},
  {"Serial No.:        %s\r\n",                                             REP_SERIAL},
  {"Version:           %s\r\n\n",                                           REP_VERSION},    
  {"Date:              %s %s\r\n\n",                                        REP_CUR_DATETIME},
  {"\r\n\n",                                                                REP_STRING},
  {"Signature ______________\r\n\n\x0C",                                    REP_STRING},
  {"",                                                                      REP_END}
};

__flash TReport PrinterReportSm[]=
{        
  {"\x1B\x21\x02\r\n",                     REP_STRING},
  {"         PRINTER TEST\r\n",            REP_STRING},
  {"_______________________________\r\n\n",REP_STRING},
  {"Instrument: %s %s-%s\r\n",             REP_DEVICE_NAME},
  //{"Instrument ID No.: %s\r\n",            REP_DEVICE_ID},
  {"Serial No.:        %s\r\n",            REP_SERIAL},
  {"Version:           %s\r\n\n",          REP_VERSION},    
  {"Date:       %s %s\r\n\n",              REP_CUR_DATETIME},
  {"Signature __________________\r\n",     REP_STRING},
  {" \r\n\n",                              REP_STRING},
  {"",                                     REP_END}
};

__flash char testAborted[]="TEST was aborted!";
__flash char testCompleted[]="TEST was completed!";
__flash char testFinished[]="TEST was finished by user!";
