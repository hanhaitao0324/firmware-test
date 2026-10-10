#include <stdio.h>
#include <string.h>
#include "calibrate.h"

TPoint CALIBRATE_Point[NUM_CAL_POINTS][MAX_SENSORS_NUM];

TCheckPoint CALIBRATE_CheckPoint[TEMP_CHECK_POINTS];

TBool CALIBRATE_Ok=false;

char CALIBRATE_String[MAX_SENSORS_NUM][5];

//------------------------------------------------------------------------------
void CALIBRATE_Init(void)
{
  TByte i, j;
  
  for (i=0; i<MAX_SENSORS_NUM; i++)
    for (j=0; j<NUM_CAL_POINTS; j++)
      memset(&CALIBRATE_Point[j][i], 0x00, sizeof(TPoint));
}
