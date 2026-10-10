#include <stdio.h>
#include "platform.h"
#include "global.h"
#include "printrtc.h"
#include "commands.h"
#include "rs485.h"
#include "timer.h"

TBool PRN_Busy=false;

//------------------------------------------------------------------------------
// PRN_Connect - connect to printer board
//------------------------------------------------------------------------------
TBool PRN_Connect(void)
{
  TByte i;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdConnect, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {
      return true;
    }
  }                
  return false;
}

//------------------------------------------------------------------------------
// PRN_Check - check printer
//------------------------------------------------------------------------------
void PRN_Check(void)
{
  globalStatus.installedPrinter=false;
  globalStatus.printerFailure=true;
    
  if (!PRN_Connect())
    return;
  globalStatus.installedPrinter=true;
  globalStatus.printerFailure=false;       
}

//------------------------------------------------------------------------------
// PRN_Init - initialize printer board
//------------------------------------------------------------------------------
TBool PRN_Init(void)
{
  TByte i;

  PRN_Busy=true;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdPrinterInit, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {
      return true;
    }
  }
  globalStatus.printerFailure=true;
  return false;
}

//------------------------------------------------------------------------------
// PRN_Reset - reset printer board (2016-10-11 星期二 10:34:07)，v2.20，Gaivn，2019-08-14 星期三 17:56:02
//------------------------------------------------------------------------------
TByte cmdReset = 0xF0;
///*
TByte PRN_Reset(void)
{
	TByte i;
	
	for (i=0; i<RS485_MAX_TRY;i++)
	{
		//RS485_SendPackage(RS485_ADDR_PRNRTC, cmdReset, 0, NULL, true);
		RS485_SendPackage(RS485_ADDR_PRNRTC, cmdReset, 0, NULL);
		if (RS485_WaitPackage(TO_RS485))
		{
			return true;
		}
	}
	
	globalStatus.printerFailure=true;
  //globalDeviceStatus.printerFailure=true;
	return stsNoResponse;
}
//*/

//------------------------------------------------------------------------------
// PRN_Done - printing ready
//------------------------------------------------------------------------------
TBool PRN_Done(void)
{
  TByte i;

  PRN_Busy=false;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdPrinterDone, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {
      return true;
    }
  }
  globalStatus.printerFailure=true;
  return false;
}

//------------------------------------------------------------------------------
// PRN_Print - print string
//------------------------------------------------------------------------------
TByte PRN_Print(char *buffer)
{
  TByte i;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdPrinterPrint, strlen(buffer), (TByte *)buffer);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      return iBuffer[PKG_OFS_CMD_STS];
    }
  }
  globalStatus.printerFailure=true;
  return stsNoResponse;
}

//------------------------------------------------------------------------------
// PRN_Status - get printer board status
//------------------------------------------------------------------------------
TByte PRN_Status(TPrinterStatus *status)
{
  TByte i;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdPrinterStatus, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      *status=*((TPrinterStatus*)&iBuffer[PKG_OFS_DATA]);         
      globalStatus.printerFailure=false;
      return iBuffer[PKG_OFS_CMD_STS];
    }
  }
  globalStatus.printerFailure=true;
  return stsNoResponse;
}

//------------------------------------------------------------------------------
// PRN_GetVersion - get printer board firmware version
//------------------------------------------------------------------------------
TByte PRN_GetVersion(TWord *Version)
{
  TByte i;
  TByte tempV[2];
  
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdPrinterStatus,0,NULL);
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdPrinterStatus, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {               
      if (iBuffer[PKG_OFS_DATA_LEN]==1)
      {
        //old version
        tempV[0]=PRN_OLDVER_MAJOR;
        tempV[1]=PRN_OLDVER_MINOR;
      } else
      {
        tempV[0]=iBuffer[PKG_OFS_DATA+1];
        tempV[1]=iBuffer[PKG_OFS_DATA+2];
      }  
      *Version=*((TWord*)&tempV);
      return iBuffer[PKG_OFS_CMD_STS];
    }
  }
  globalStatus.printerFailure=true;
  
  return stsNoResponse;
}

//------------------------------------------------------------------------------
// RTC_GetDateTime - get current date/time
//------------------------------------------------------------------------------
TBool RTC_GetDateTime(TTimeDate *dt)
{
  TByte i;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdGetTime, 0, NULL);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {
        *dt=*((TTimeDate*)&iBuffer[PKG_OFS_DATA]);
        return true;
      }
    }
  }
  return false;
}

//------------------------------------------------------------------------------
// RTC_SetDateTime - get current date/time
//------------------------------------------------------------------------------
TBool RTC_SetDateTime(TTimeDate dt)
{
  TByte i;
    
  for (i=0; i<RS485_MAX_TRY; i++)
  {
    RS485_SendPackage(RS485_ADDR_PRNRTC, cmdSetTime, sizeof(TTimeDate), (TByte *)&dt);
    if (RS485_WaitPackage(TO_RS485))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }
  return false;
}
