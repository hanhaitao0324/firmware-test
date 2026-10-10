#include <stdio.h>

#include "platform.h"
#include "global.h"
#include "printrtc.h"
#include "commands.h"
#include "rs485.h"
#include "timer.h"

TBool PRN_Busy=false;

TBool PRN_Connect(void)
  {
    TByte i;

    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdConnect,0,NULL);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdConnect,0,NULL,true);
        if (RS485_WaitPackage(TO_RS485))
          {
            return true;
          }
      }                
    return false;
  }

void PRN_Check(void)
  {
   
    globalDeviceStatus.installedPrinter=false;
    
    if (!PRN_Connect())
      return;
/*
    if (!PRN_Init())  
      return;
      
    for(;;)
      {
        PRN_Status(&ps);
        if (ps.error)
          return;
        if (ps.busy)
          continue;
        break;
      }                                      
      */
    globalDeviceStatus.installedPrinter=true;
    globalDeviceStatus.printerFailure=false;       
  }

TBool PRN_Init(void)
  {
    TByte i;

    PRN_Busy=true;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdPrinterInit,0,NULL);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdPrinterInit,0,NULL,true);
        if (RS485_WaitPackage(TO_RS485))
          {
            globalDeviceStatus.printerFailure=false;
            return true;
          }
      }
    globalDeviceStatus.printerFailure=true;

    return false;
  }

//------------------------------------------------------------------------------
// PRN_Reset - reset printer board (2016-10-11 星期二 10:34:07)
//------------------------------------------------------------------------------
TByte cmdReset = 0xF0;
///*
TByte PRN_Reset(void)
{
	TByte i;
	
	for (i=0; i<RS485_MAX_TRY;i++)
	{
		RS485_SendPackage(RS485_ADDR_PRNRTC, cmdReset, 0, NULL, true);
		if (RS485_WaitPackage(TO_RS485))
		{
			return true;
		}
	}
	
	//globalStatus.printerFailure=true;
  globalDeviceStatus.printerFailure=true;
	return stsNoResponse;
}
//*/

TBool PRN_Done(void)
  {
    TByte i;

    PRN_Busy=false;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdPrinterDone,0,NULL);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdPrinterDone,0,NULL,true);
        if (RS485_WaitPackage(TO_RS485))
          {
            return true;
          }
      }
    globalDeviceStatus.printerFailure=true;
    return false;
  }


TByte PRN_Print(char *buffer)
  {
    TByte i;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdPrinterPrint,strlen(buffer),(TByte *)buffer);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdPrinterPrint,strlen(buffer),(TByte *)buffer,true);
        
        if (RS485_WaitPackage(TO_RS485))
          {                  
            return iBuffer[PKG_OFS_CMD_STS];
          }
      }
    globalDeviceStatus.printerFailure=true; 			// 打印机报错
    return stsNoResponse; 												// 没有回应
  }

TByte PRN_Status(TPrinterStatus *status)
  {
    TByte i;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdPrinterStatus,0,NULL);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdPrinterStatus,0,NULL,true);
        if (RS485_WaitPackage(TO_RS485))
          {                  
            *status=*((TPrinterStatus*)&iBuffer[PKG_OFS_DATA]);         
            return iBuffer[PKG_OFS_CMD_STS];
          }
      }
    globalDeviceStatus.printerFailure=true;
    return stsNoResponse;
  }


TBool RTC_GetDateTime(TTimeDate *dt)
  {
    TByte i;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdGetTime,0,NULL);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdGetTime,0,NULL,true);
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

TBool RTC_SetDateTime(TTimeDate dt)
  {
    TByte i;
    
    for(i=0;i<RS485_MAX_TRY;i++)
      {
        //RS485_PutPackage(RS485_ADDR_PRNRTC,cmdSetTime,sizeof(TTimeDate),(TByte *)&dt);
        RS485_SendPackage(RS485_ADDR_PRNRTC,cmdSetTime,sizeof(TTimeDate),(TByte *)&dt,true);
        if (RS485_WaitPackage(TO_RS485))
          {                  
            if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
              return true;
          }
      }
    return false;
  }
