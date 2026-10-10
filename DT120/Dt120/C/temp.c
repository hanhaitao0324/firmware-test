#include "adc.h"
#include "eeprom.h"
#include "temp.h"
#include "const.h"
#include "calibrate.h"
#include "global.h"

#include <math.h>
#include <stdlib.h>

//__flash TReal32 coefA=3.90802E-3;
//__flash TReal32 coefB=-5.802E-7;

//__flash TReal32 Z1=-3.90802E-3;
//__flash TReal32 Z2=17.5934203204E-6;
//__flash TReal32 Z3=-23.208E-9;
//__flash TReal32 Z4=-1.1604E-6;

TReal32 TEMP_k[ADC_NUM_CHANNELS];
TReal32 TEMP_b[ADC_NUM_CHANNELS];
TReal32 TEMP_LastTemp, TEMP_Temp;

//TCalPoint TEMP_CalPoint[TEMP_NUM_CAL_POINTS];
TByte TEMP_CalChannel=0xFF;

//extern TBool TEMP_CalOk=false;

void TEMP_Init(void)
{
  TByte i;

  for(i=0;i<ADC_NUM_CHANNELS;i++)
  {
    TEMP_b[i]=EEPROM1.TempB[i];
    TEMP_k[i]=EEPROM1.TempK[i];
  }   
  TEMP_LastTemp=0.0;
  TEMP_Temp=0.0;
}

TReal32 TEMP_TempToR(TReal32 value)
{
  return (TReal32)(100.0*(1+coefA*value+coefB*value*value));
}

TReal32 TEMP_GetValue(TByte channel)
{
  TReal32 result;
    
  if (channel>=ADC_NUM_CHANNELS)
    return 0.0;
  if (channel==ADC_CHNL_EXTERNAL)
    if (!globalDeviceStatus.installedExtADC)
      return 0.0;
  
  if (globalDeviceStatus.sensorExtError)  //!!!!!
    return 0.0;
  
  result=ADC_GetValue()*TEMP_k[channel]+TEMP_b[channel];
  result=(Z1+sqrt(Z2+Z3*result))/Z4;
  if (fabs(TEMP_LastTemp-result)<ADC_MAX_TEMP_DIFF)
    TEMP_Temp=result;
  TEMP_LastTemp=result;
  
  if ((TEMP_Temp<MIN_TEMP)||(TEMP_Temp>MAX_TEMP))
  {
    globalDeviceStatus.sensorExtError=true;
    return 0.0;
  } else globalDeviceStatus.sensorExtError=false;
  
  return TEMP_Temp;
}

TBool TEMP_SetCalPoint(TByte channel,TByte pointNum,TReal32 *realTemp)
{                      
  TByte i;
                
  if (ADC_CHNL_EXTERNAL==channel)
    if ((!globalDeviceStatus.installedExtADC)||(globalDeviceStatus.sensorExtError))
      return false;
  if (pointNum>=NUM_CAL_POINTS)
    return false;
  if (channel>=ADC_NUM_CHANNELS)
    return false;
  if (channel!=TEMP_CalChannel)
  {
    for (i=0;i<NUM_CAL_POINTS;i++)
      //TEMP_CalPoint[i].hasResult=false;
      CALIBRATE_Point[i][0].hasResult=false;
  }
  //TEMP_CalPoint[pointNum].actTemperature=TEMP_GetValue(channel);
  //TEMP_CalPoint[pointNum].Temperature=*realTemp;
  //TEMP_CalPoint[pointNum].realR=TEMP_TempToR(*realTemp);
  //TEMP_CalPoint[pointNum].adcValue=ADC_GetValue(channel);
  //TEMP_CalPoint[pointNum].hasResult=true;
  CALIBRATE_Point[pointNum][0].actTemperature=(TWord)((TEMP_GetValue(channel)+0.05)*10);
  CALIBRATE_Point[pointNum][0].Temperature=(TWord)((*realTemp+0.05)*10);
  CALIBRATE_Point[pointNum][0].realR=TEMP_TempToR(*realTemp);
  CALIBRATE_Point[pointNum][0].adcValue=ADC_GetValue();
  CALIBRATE_Point[pointNum][0].hasResult=true;
  if (!pointNum)
    CALIBRATE_Point[1][0].hasResult=false;
  CALIBRATE_Point[pointNum][0].channel=CALIBRATE_ADC;
  TEMP_CalChannel=channel;
  return true;
}
  
TBool TEMP_Calibrate(void)
{
  TByte i;
  TByte numPoints;
  TReal32 k,b;

  //type of calibration                 
  numPoints=0;
  for (i=0;i<NUM_CAL_POINTS;i++)
    //if (TEMP_CalPoint[i].hasResult)
    if (CALIBRATE_Point[i][0].hasResult)
      numPoints++;    
  if (numPoints==NUM_CAL_POINTS)
  {
    //full - gain&offset  
    //b=TEMP_CalPoint[0].adcValue-TEMP_CalPoint[1].adcValue;
    if (abs(CALIBRATE_Point[0][0].adcValue-CALIBRATE_Point[1][0].adcValue)<100)
      return false;
    b=(TReal32)CALIBRATE_Point[0][0].adcValue-(TReal32)CALIBRATE_Point[1][0].adcValue;
    //if (fabs(b)<0.001)
    //  return false;
    //k=(TEMP_CalPoint[0].realR-TEMP_CalPoint[1].realR)/b;
    k=(CALIBRATE_Point[0][0].realR-CALIBRATE_Point[1][0].realR)/b;
    //b=TEMP_CalPoint[0].realR-k*TEMP_CalPoint[0].adcValue;
    b=CALIBRATE_Point[0][0].realR-k*CALIBRATE_Point[0][0].adcValue;
    //store
    TEMP_k[TEMP_CalChannel]=k;
    //eepromTempK[TEMP_CalChannel]=k;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempK[TEMP_CalChannel], (TByte *)&k, sizeof(TReal32));
    TEMP_b[TEMP_CalChannel]=b;
    //eepromTempB[TEMP_CalChannel]=b;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempB[TEMP_CalChannel], (TByte *)&b, sizeof(TReal32));
  } else
  {
    //offset only
    //if (!TEMP_CalPoint[0].hasResult)
    if (!CALIBRATE_Point[0][0].hasResult)
      return false;
    //b=TEMP_CalPoint[0].realR-TEMP_k[TEMP_CalChannel]*TEMP_CalPoint[0].adcValue;
    b=CALIBRATE_Point[0][0].realR-TEMP_k[TEMP_CalChannel]*CALIBRATE_Point[0][0].adcValue;
    TEMP_b[TEMP_CalChannel]=b;
    //eepromTempB[TEMP_CalChannel]=b;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempB[TEMP_CalChannel], (TByte *)&b, sizeof(TReal32));
  }
  //reset flags
  //for (i=0;i<NUM_CAL_POINTS;i++)
    //TEMP_CalPoint[i].hasResult=false;
    //CALIBRATE_Point[i].hasResult=false;
  return true;
}
