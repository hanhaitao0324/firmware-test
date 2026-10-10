#include "platform.h"
#include "global.h"
#include "pins.h"
#include "dhc.h"
#include "calibrate.h"
#include "rs232.h"
#include "timer.h"
#include <math.h>

__flash char dhcCommandSet[DHC_NUM_COMMANDS][10]=
{
  "\x1BZ%.1f\r",
  "\x1BG%.1f\r",
  "\x1BS\r",
  "\x1BPID\r",
  "\x1BID\r",
  "\x1BTR\r",
  "\x1BTT%.1f\r"
};

__flash char dhcError[DHC_NUM_ERRORS][3]=
{
  "O\x00",
  "E0\x00",
  "E2\x00",
  "E3\x00",
  "E4\x00",
  "E5\x00",
  "E6\x00"
};

__flash char dhcID[] = "DHC\x00";
__flash char dhcGetTempFmt[] = "%c%f";

TReal32 DHC_Temp, DHC_LastTemp;
volatile TByte DHC_Status;
TBool DHC_Request;
TByte DHC_Errors;
//TDWord DHC_Good=0;
//TDWord DHC_Bad=0;
TByte DHC_Delay=0;

//------------------------------------------------------------------------------
void DHC_Init(void)
{
  DHC_Errors=0;
  DHC_Temp=0.0;
  DHC_LastTemp=0.0;
  DHC_Status=DHC_STATUS_FREE;
  DHC_Request=false;
  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    globalDeviceStatus.dhcFailure=true;
  else
    globalDeviceStatus.dhcFailure=false;
}
  
//------------------------------------------------------------------------------
TByte DHC_Parser(char *buffer)
{
  TByte pos;
  
  //make an ASCIIZ string from the buffer
  pos=0;
  while ((('\r'!=*buffer)||('\n'!=*buffer))&&(*buffer))
  {
    buffer++;
    pos++;
  }
  buffer--;
  *buffer = 0x00;
  
  return pos;
}

//------------------------------------------------------------------------------
signed char DHC_CheckError(char *buffer)
{
  signed char ix=0;
  
  for (ix=0; ix<DHC_NUM_ERRORS; ix++)
    if (0==strncmp_P(buffer,dhcError[ix],strlen_P(dhcError[ix])))
      return ix;
  return -1;
}

//------------------------------------------------------------------------------
TBool DHC_SendCommand(char *buffer)
{
  TByte ix=0;
  
  while (ix<DHC_MAX_TRY)
  {
    RS232_PutString(buffer);
    if (RS232_WaitReply()) 
      break;
    else 
      ix++;
  }
  globalDeviceStatus.gotRS232Package=false;
  if (ix<DHC_MAX_TRY)
    return true;
  globalDeviceStatus.dhcFailure = true;
  return false;
}

//------------------------------------------------------------------------------
TByte DHC_GetStatus(void)
{
  return DHC_Status;
}

//------------------------------------------------------------------------------
void DHC_SetRequest(void)
{
  DHC_Request=true;
}

//------------------------------------------------------------------------------
void DHC_IntHandler(void)
{
  char str[10];
  TReal32 curT=0.0;
  char ch;
  
  //if (globalDeviceStatus.dhcFailure)
  //  return;
  
  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return;

  if (DHC_Delay)
  {
    DHC_Delay--;
    return;
  }
  
  if (globalDeviceStatus.gotRS232Package)
    DHC_Status=DHC_STATUS_READY;
  
  if (TIMER_TimeOut(TIMER_RS232))
  {
    DHC_Status=DHC_STATUS_FREE;
    //TIMER_Stop(TIMER_RS232);
    if (DHC_Errors<=DHC_MAX_TRY)
      DHC_Errors++;
    //DHC_Bad++;
    //return;
  }
  
  if (DHC_Errors>DHC_MAX_TRY)
  {
    globalDeviceStatus.dhcFailure=true;
    DHC_Temp=0.0;
    DHC_LastTemp=0.0;
    //return;
  }
  
  switch (DHC_Status)
  {
    case DHC_STATUS_FREE:
      if (DHC_Request)
        return;
      //request temperature from DHC (send TR)
      sprintf_P(str, dhcCommandSet[DHC_CMD_GET_TEMP]);
      RS232_PutString(str);
      TIMER_Start(TIMER_RS232, TO_RS232);
      DHC_Status=DHC_STATUS_SENDING;
      RS232_IBuffer[0]=0;
      break;
    case DHC_STATUS_READY:
      //get temperature from the string
      if (!DHC_Request)
      {
        DHC_Parser(RS232_IBuffer);
        if (sscanf_P(RS232_IBuffer, dhcGetTempFmt, &ch, &curT))
        {
          if ((ch=='T')||(ch=='I'))
          {  
            if (fabs(DHC_LastTemp-curT)<DHC_MAX_TEMP_DIFF)
              DHC_Temp=curT;
            DHC_LastTemp=curT;
            DHC_Errors=0;
            globalDeviceStatus.dhcFailure=false;
          }
        } else
        {
          globalDeviceStatus.dhcFailure=true;
          DHC_Temp=0.0;
          DHC_LastTemp=0.0;
          DHC_Errors++;
        }  
        TIMER_Stop(TIMER_RS232);
        //DHC_Good++;
      }
      globalDeviceStatus.gotRS232Package=false;
      DHC_Status=DHC_STATUS_FREE;
      DHC_Delay=100;
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
TBool DHC_FirstPCal(TReal32 *temp)
{
  char str[10];

  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;
  
  sprintf_P(str, dhcCommandSet[DHC_CMD_CAL_FIRST_POINT], *temp);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE);
  if (!DHC_SendCommand(str))
  {
    DHC_Request=false;
    return false;
  }
  if (DHC_CheckError(RS232_IBuffer)!=DHC_NO_ERR)
  {
    DHC_Request=false;
    return false;
  }
  CALIBRATE_Point[0][0].Temperature=(TWord)((*temp+0.05)*10);
  CALIBRATE_Point[0][0].actTemperature=(TWord)((DHC_Temp+0.05)*10);
  CALIBRATE_Point[0][0].hasResult=true;
  CALIBRATE_Point[0][0].channel=CALIBRATE_DHC;
  CALIBRATE_Point[1][0].hasResult=false;
  DHC_Request=false;
  return true;
}

//------------------------------------------------------------------------------
TBool DHC_SecondPCal(TReal32 *temp)
{
  char str[10];

  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;

  sprintf_P(str, dhcCommandSet[DHC_CMD_CAL_SECOND_POINT], *temp);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE);
  if (!DHC_SendCommand(str))
  {
    DHC_Request=false;
    return false;
  }
  if (DHC_CheckError(RS232_IBuffer)!=DHC_NO_ERR)
  {  
    DHC_Request=false;
    return false; 
  }
  CALIBRATE_Point[1][0].Temperature=(TWord)((*temp+0.05)*10);
  CALIBRATE_Point[1][0].actTemperature=(TWord)((DHC_Temp+0.05)*10);
  CALIBRATE_Point[1][0].hasResult=true;
  CALIBRATE_Point[1][0].channel=CALIBRATE_DHC;
  DHC_Request=false;
  return true;
}

//------------------------------------------------------------------------------
TBool DHC_Standby(void)
{
  char str[10];

  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;

  sprintf_P(str, dhcCommandSet[DHC_CMD_STANDBY]);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE);
  if (!DHC_SendCommand(str))
  {  
    DHC_Request=false;
    return false;
  }
  if (DHC_CheckError(RS232_IBuffer)!=DHC_NO_ERR)
  {  
    DHC_Request=false;
    return false;
  }
  DHC_Request=false;
  return true;
}

//------------------------------------------------------------------------------
TBool DHC_GetPID(void)
{
  char str[10];

  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;

  sprintf_P(str, dhcCommandSet[DHC_CMD_GET_PID]);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE);
  if (!DHC_SendCommand(str))
  {  
    DHC_Request=false;
    return false;
  }
  // get PID
  if (DHC_Parser(RS232_IBuffer)!=DHC_PID_STRING_LEN)
  {  
    DHC_Request=false;
    return false;
  }
  // what to do with PID???
  DHC_Request=false;
  return true;    
}

//------------------------------------------------------------------------------
TBool DHC_GetID(void)
{
  char str[10];
  TByte ix;

  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;

  sprintf_P(str, dhcCommandSet[DHC_CMD_GET_ID]);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE);
  if (!DHC_SendCommand(str))
  {
    DHC_Request=false;
    return false;
  }
  // check ID ("DHC1200")
  ix = DHC_Parser(RS232_IBuffer);
  if ((ix>(DHC_ID_STRING_LEN+1))||(ix<DHC_ID_STRING_LEN))
  {  
    DHC_Request=false;
    return false;
  }
  RS232_IBuffer[DHC_ID_STRING_LEN]=0x00;
  if (0!=strcmp_P(RS232_IBuffer, dhcID))
  {  
    DHC_Request=false;
    return false;
  }
  DHC_Request=false;
  return true;    
}

//------------------------------------------------------------------------------
TBool DHC_GetTemp(TReal32 *temp)
{
  /*char str[10];
  TReal32 curT=0.0;
  
  sprintf_P(str, dhcCommandSet[DHC_CMD_GET_TEMP]);
  if (!DHC_SendCommand(str))
    return false;
  //get temperature from the string
  DHC_Parser(rs232IBuffer);
  sscanf_P(rs232IBuffer, dhcGetTempFmt, &curT);
  *temp=curT;
  return true;*/
  *temp=DHC_Temp;
  return true;
}

//------------------------------------------------------------------------------
TBool DHC_SetTemp(TReal32 *temp)
{
  char str[10];
  
  if (globalDeviceStatus.portMode==PORT_MODE_COM)
    return false;

  if ((TWord)(*temp*10))
    sprintf_P(str, dhcCommandSet[DHC_CMD_SET_TEMP], *temp);
  else
    sprintf_P(str, dhcCommandSet[DHC_CMD_STANDBY]);
  DHC_Request=true;
  while (DHC_Status!=DHC_STATUS_FREE)
    continue;
  if (!DHC_SendCommand(str)) 
  {
    DHC_Request=false;
    return false;
  }
  if (DHC_CheckError(RS232_IBuffer)==DHC_NO_ERR)
  {
    DHC_Request=false;
    return true;
  }
  DHC_Request=false;
  return false;
}
