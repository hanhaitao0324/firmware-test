#ifndef _CALIBRATE_H
#define _CALIBRATE_H

#include "defines.h"
#include "const.h"

#define NUM_CAL_POINTS (2)

enum CALIBRATE_CHANNEL
{
  CALIBRATE_ADC,
  CALIBRATE_DHC,
  CALIBRATE_AS
};

typedef struct
  { 
    TWord Temperature;
    TWord actTemperature;
    TReal32 realR;
    TWord adcValue; 
    TBool hasResult;
    TByte channel;
  } TPoint;

typedef struct
  { 
    TWord Temperature;
    TWord actTemperature;
    TWord readTemperature;
    TBool hasResult;
    TByte channel;
  } TCheckPoint;

extern TBool CALIBRATE_Ok;

extern TPoint CALIBRATE_Point[NUM_CAL_POINTS][MAX_SENSORS_NUM];

extern TCheckPoint CALIBRATE_CheckPoint[TEMP_CHECK_POINTS];

extern char CALIBRATE_String[MAX_SENSORS_NUM][5];

extern void CALIBRATE_Init(void);

#endif
