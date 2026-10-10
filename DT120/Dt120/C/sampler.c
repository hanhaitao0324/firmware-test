#include "platform.h"
#include "global.h"
#include "eeprom.h"
#include "calibrate.h"
#include "sampler.h"
#include "commands.h"
#include "rs485.h"
#include "timer.h"

//TReal32 SAMPLER_Temp[MAX_SENSORS_NUM];
TsWord SAMPLER_Temp[MAX_SENSORS_NUM];
TBool SAMPLER_Active;
TByte SAMPLER_Errors;
//TDWord SAMPLER_Good=0;
//TDWord SAMPLER_Bad=0;
TByte SAMPLER_Delay=0;

//------------------------------------------------------------------------------
// SAMPLER_SetActive - activate sampling board requests
//------------------------------------------------------------------------------
void SAMPLER_SetActive(TBool active)
{
  SAMPLER_Active=active;
  if (!active)
    RS485_SetStatus(RS485_STATUS_FREE);
}

//------------------------------------------------------------------------------
// SAMPLER_Init - initialize sampler module
//------------------------------------------------------------------------------
void SAMPLER_Init(void)
{
  TByte i;
  
  for(i=0; i<MAX_SENSORS_NUM; i++)
    //SAMPLER_Temp[i]=0.0;
    SAMPLER_Temp[i]=TEMP_ERROR;
  globalDeviceStatus.samplerFailure=true;
  RS485_SetStatus(RS485_STATUS_FREE);
  SAMPLER_Active=false;
  SAMPLER_Errors=0;
  RS485_SetRequest(false);
}

//------------------------------------------------------------------------------
// SAMPLER_IntHandler - sampler processing procedure (periodically)
//------------------------------------------------------------------------------
void SAMPLER_IntHandler(void)
{
  TByte status;
  
  //if (!SAMPLER_Active)
  //  return;
  status = RS485_GetStatus();
  if ((!SAMPLER_Active)&&(status!=RS485_STATUS_FREE))
    return;

  //if (globalDeviceStatus.samplerFailure)
  //  return;
  
  if (SAMPLER_Delay)
  {
    SAMPLER_Delay--;
    return;
  }

  if (globalDeviceStatus.gotRS485Package)
    RS485_SetStatus(RS485_STATUS_READY);

  if ((SAMPLER_Active)&&(TIMER_TimeOut(TIMER_RS485)))
  {
    RS485_SetStatus(RS485_STATUS_FREE);
    if (SAMPLER_Errors<=RS485_MAX_TRY)
      SAMPLER_Errors++;
    SAMPLER_Active=false;
    //SAMPLER_Bad++;
    SAMPLER_Delay=2;
    return;
  }
  
  if (SAMPLER_Errors>RS485_MAX_TRY)
  {
    globalDeviceStatus.samplerFailure=true;
    //return;
  }
  
  status=RS485_GetStatus();
  switch (status)
  {
    case RS485_STATUS_FREE:
      //if (RS485_GetRequest())
      //  return;
      SAMPLER_Active=true;
      //request temperature from AutoSampler (send packet)
      RS485_SendPackage(RS485_ADDR_SAMPLER, cmdSamplerGetTemp, 0, NULL, false);
      TIMER_Start(TIMER_RS485, TO_RS485);
      RS485_SetStatus(RS485_STATUS_SENDING);
      iBuffer[0]=0xff;
      break;
    case RS485_STATUS_READY:
      //get temperature from AutoSampler
      //if (!RS485_GetRequest())
      //{
        memcpy(SAMPLER_Temp, &iBuffer[PKG_OFS_DATA], iBuffer[PKG_OFS_DATA_LEN]);
        TIMER_Stop(TIMER_RS485);
        SAMPLER_Errors=0;
        //SAMPLER_Good++;
        globalDeviceStatus.samplerFailure=false;
      //}
      globalDeviceStatus.gotRS485Package=false;
      RS485_SetStatus(RS485_STATUS_FREE);
      SAMPLER_Delay=100;
      SAMPLER_Active=false;
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
// SAMPLER_Connect - connect to sampler board
//------------------------------------------------------------------------------
TBool SAMPLER_Connect(void)
{
  TByte i;

  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_SAMPLER,cmdConnect,0,NULL);
    RS485_SendPackage(RS485_ADDR_SAMPLER, cmdConnect, 0, NULL, true);
    if (RS485_WaitPackage(TO_RS485))
    {
      return true;
    }
  }                
  return false;
}

//------------------------------------------------------------------------------
//TBool SAMPLER_GetTemp(TByte sens, TReal32 *temp)
/*TBool SAMPLER_GetTemp(void)
{
  TByte i;//, ix, addr;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    //RS485_PutPackage(addr,cmdSamplerGetTemp,1,&ix);
    RS485_PutPackage(RS485_ADDR_SAMPLER,cmdSamplerGetTemp,0,NULL);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      memcpy(SAMPLER_Temp, &iBuffer[PKG_OFS_DATA], iBuffer[PKG_OFS_DATA_LEN]);
      globalDeviceStatus.samplerFailure=false;
      return true;
    }
  }
  globalDeviceStatus.samplerFailure=true;
  return false;
}*/

//------------------------------------------------------------------------------
// SAMPLER_Calibrate - calibrate temperature for sampler board
//------------------------------------------------------------------------------
TBool SAMPLER_Calibrate(TByte point, TReal32 *temp, TByte channel)
{
  TByte i;
  TByte buffer[10];
  
  buffer[0]=point;
  memcpy(&buffer[1], (TByte *)temp, sizeof(TReal32));
  buffer[5]=channel;
  
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_SAMPLER,cmdSamplerCalibrate,sizeof(TByte)+sizeof(TReal32),buffer);
    RS485_SendPackage(RS485_ADDR_SAMPLER, cmdSamplerCalibrate, sizeof(TByte)+sizeof(TReal32)+sizeof(TByte), buffer, true);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      CALIBRATE_Point[point][channel].Temperature=(TWord)((*temp+0.05)*10);
      CALIBRATE_Point[point][channel].actTemperature=SAMPLER_Temp[channel]/10;
      CALIBRATE_Point[point][channel].hasResult=true;
      CALIBRATE_Point[point][channel].channel=CALIBRATE_AS;
      if (!point)
        CALIBRATE_Point[1][channel].hasResult=false;
      globalDeviceStatus.samplerFailure=false;
      return true;
    }
  }
  globalDeviceStatus.samplerFailure=true;
  return false;
}

//------------------------------------------------------------------------------
// SAMPLER_SetMinTemp - set minimal temperature
//------------------------------------------------------------------------------
TBool SAMPLER_SetMinTemp(TReal32 *temp)
{
  TByte i;
  
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_SAMPLER,cmdSamplerSetMinTemp,sizeof(TReal32),(TByte *)&temp);
    RS485_SendPackage(RS485_ADDR_SAMPLER, cmdSamplerSetMinTemp, sizeof(TReal32),(TByte *)&temp, true);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      globalDeviceStatus.samplerFailure=false;
      return true;
    }
  }
  globalDeviceStatus.samplerFailure=true;
  return false;
}

//------------------------------------------------------------------------------
// SAMPLER_SetMaxTemp - set maximal temperature
//------------------------------------------------------------------------------
TBool SAMPLER_SetMaxTemp(TReal32 *temp)
{
  TByte i;
  
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_SAMPLER,cmdSamplerSetMaxTemp,sizeof(TReal32),(TByte *)&temp);
    RS485_SendPackage(RS485_ADDR_SAMPLER,cmdSamplerSetMaxTemp, sizeof(TReal32), (TByte *)&temp, true);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      globalDeviceStatus.samplerFailure=false;
      return true;
    }
  }
  globalDeviceStatus.samplerFailure=true;
  return false;
}

//------------------------------------------------------------------------------
// SAMPLER_GetSerial - get serial number from the sampler board
//------------------------------------------------------------------------------
TBool SAMPLER_GetSerial(TByte *buffer)
{
  TByte i;
  
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_SAMPLER,cmdSamplerGetID,0,NULL);
    RS485_SendPackage(RS485_ADDR_SAMPLER,cmdSamplerGetID, 0, NULL, true);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      memcpy(buffer, &iBuffer[PKG_OFS_DATA], iBuffer[PKG_OFS_DATA_LEN]);
      buffer[iBuffer[PKG_OFS_DATA_LEN]]=0;
      globalDeviceStatus.samplerFailure=false;
      return true;
    }
  }
  globalDeviceStatus.samplerFailure=true;
  return false;
}
