#include <stdlib.h>
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "external.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "rs485.h"
#include "sound.h"
#include "strings.h"
#include "timer.h"
#include "xtea.h"

enum EKEYMENU_SETTINGS
{
  EKEYMENU_ADDKEY,
  EKEYMENU_DATE,
  EKEYMENU_VERSION,
  EKEYMENU_SERIAL,
  EKEYMENU_DEVSERIAL,
  EKEYMENU_ERRORS,
  EKEYMENU_EXPIRE,
  EKEYMENU_PERIOD,
  EKEYMENU_MAXPERIOD,
  EKEYMENU_PERIODLEN,
  EKEYMENU_ALARMS,
  EKEYMENU_RESET,
  EKEYMENU_STATUS,
  EKEYMENU_EXIT,
  EKEYMENU_LAST
};

enum EKEYMENU_USER
{
  EKEYMENU_USER_ADDKEY,
  EKEYMENU_USER_EXPIRE,
  EKEYMENU_USER_DEVSERIAL,
  EKEYMENU_USER_PERIOD,
  //EKEYMENU_USER_STATUS,
  EKEYMENU_USER_EXIT,
  EKEYMENU_USER_LAST
};

enum EKEY_DATE
{
  EKEY_DATE_TIME,
  EKEY_DATE_DAY,
  EKEY_DATE_YEAR,
  EKEY_DATE_LAST
};

enum EKEY_ALARM
{
  EKEY_ALARM_FIRST,
  EKEY_ALARM_SECOND,
  EKEY_ALARM_LOCK,
  EKEY_ALARM_LAST
};

TMenuItemStr EKEY_MENU_TITLE="EKEY";

TMenuItemStr EKEY_MENU_SET[EKEYMENU_LAST]=
{
  "ADD",
  "DATE",
  "VER",
  "S.NUM",
  "DEV.N",
  "ERRS",
  "EXPI",
  "PERD",
  "MAX.P",
  "P.LEN",
  "ALRM",
  "RST",
  "STAT",
  "EXIT"
};

TMenuItemStr EKEY_MENU_USER[EKEYMENU_LAST]=
{
  "ADD",
  "EXPI",
  "DEV.N",
  "PERD",
  //"STAT",
  "EXIT"
};

TMenuItemStr EKEY_LIST_DATE[EKEY_DATE_LAST]=
{
  "TIME",
  "DAY",
  "YEAR"
};

TMenuItemStr EKEY_LIST_ALARM[EKEY_ALARM_LAST]=
{
  "F.ALR",
  "S.ALR",
  "LOCK"
};

TMenuItemStr EKEY_STATUS_STRING[EKEY_STATUS_LAST]=
{
  "OK",
  "1.ALR",
  "2.ALR",
  "LOCK",
  "1.ST",
  "BLCK"
};

__flash char pass[]="25Dz78";

__flash char strConfirm[]={0xAA, 0x55};

__flash char daysFmt[]="D.%03d";

char devicePassword[16];

TWord EKEY_Random;

TByte EKEY_Buff[8];

TByte toint_new(char datab)
{
   TByte res=0;

   if ((datab>=0x30)&&(datab<=0x39))
     res=datab-0x30;
   else
   if ((datab>=0x41)&&(datab<=0x46))
     res=datab-0x37;

   return res;
}

TByte hex2char(char *str)
{

   TByte ret;
   char byteH, byteL;

   // get a two-byte ASCII representation of a char from the UART
   byteH=*str++;
   byteL=*str++;

   // convert to a single 8 bit result
   ret=(TByte)toint_new(byteH)*16;
   ret+=(TByte)toint_new(byteL);
   return ret;
}

//------------------------------------------------------------------------------
void EKEY_InitBuff(void)
{
  TByte i;
  
  for (i=0; i<6; i++)
    EKEY_Buff[i]=(TByte)rand();
  strncpy_P((char *)&EKEY_Buff[6], strConfirm, 2);
}

//------------------------------------------------------------------------------
TBool EKEY_Connect(void)
{
  TByte i;

  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdConnect, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {  
      EKEY_Random=(TByte)(TCNT1&0xff);
      EKEY_Random<<=8;
      return true;
    }
  }                

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetDateTime(TTimeDate *dt)
{
  TByte i;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdGetTime, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
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
TBool EKEY_SetDateTime(TTimeDate *dt)
{
  TByte i;
    
  EKEY_InitBuff();
  memcpy(EKEY_Buff, (TByte *)dt, sizeof(TTimeDate));
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdSetTime, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetStatus(TByte *status, TWord *days)
{
  TByte i;
  TByte j;
  //char str[16];
    
  EKEY_InitBuff();
  j=(TByte)rand();
  EKEY_Buff[0]=j;
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetStatus, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_500MSEC))
    {                  
      //get 8 bytes and decode it (XTEA)
      XTEA_Decrypt((TDWord *)&iBuffer[PKG_OFS_DATA], (TDWord *)devicePassword);
      //*status=iBuffer[PKG_OFS_DATA+(j&0x07)];
      *status=iBuffer[PKG_OFS_DATA+5]&0x0F;
      *days=*((int *)&iBuffer[PKG_OFS_DATA+6]);
      //if ((iBuffer[PKG_OFS_DATA]!=0xAA)&&(iBuffer[PKG_OFS_DATA+7]!=0xAA))
      if ((iBuffer[PKG_OFS_DATA]!=j)||(iBuffer[PKG_OFS_DATA+1]!=j))
        *status=EKEY_STATUS_BLOCKED;
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_InitEncode(void)
{
  TByte i;
    
  EKEY_InitBuff();
  i=(TByte)EKEY_Random;
  EKEY_Buff[0]=i;
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  XTEA_SetNumRounds(i>>2);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyInitEncode, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_EKEY))
    { 
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_SendSerial(void)
{
  TByte i;
    
  EEPROM_ReadString(devicePassword, PCBID, PCB_ID_LEN);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyTransSerial, PCB_ID_LEN, (TByte *)devicePassword);
    if (RS485_WaitPackage(TO_EKEY))
    { 
      EKEY_Random+=(TByte)(TCNT1&0xff);
      srand(EKEY_Random);
      strncpy_P(&devicePassword[10], pass, 6);
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_SendKey(TByte *key)
{
  TByte i;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyTransKey, EKEY_KEY_LEN, key);
    if (RS485_WaitPackage(TO_500MSEC))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
      else
        return false;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetVersion(TWord *version)
{
  TByte i;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetVersion, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {
        *version=*((TWord*)&iBuffer[PKG_OFS_DATA]);
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetID(char *serial)
{
  TByte i, j;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetID, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {  
        j=iBuffer[PKG_OFS_DATA_LEN];
        if (j>PCB_ID_LEN)
          j=PCB_ID_LEN;
        memcpy(serial, &iBuffer[PKG_OFS_DATA], j);
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetExpiredDate(TTimeDate *dt)
{
  TByte i;
  
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetExpiredDate, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
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
TBool EKEY_SetAlarmPeriod(TByte *days)
{
  TByte i;
  
  EKEY_InitBuff();
  //EKEY_Buff[0]=alarm;  
  //EKEY_Buff[1]=*days;
  EKEY_Buff[0]=*days++;
  EKEY_Buff[1]=*days++;
  EKEY_Buff[2]=*days++;
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeySetAlarmPeriod, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetAlarmPeriod(TByte *days)
{
  TByte i;
  
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetAlarmPeriod, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {  
        //*days=iBuffer[PKG_OFS_DATA];
        memcpy(days, &iBuffer[PKG_OFS_DATA], 3);
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetErrors(TWord *errors)
{
  TByte i;
    
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetErrors, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {
        *errors=*((TWord*)&iBuffer[PKG_OFS_DATA]);
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetPeriod(TByte *value)
{
  TByte i;
  
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetPeriod, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {  
        *value=iBuffer[PKG_OFS_DATA];
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetMaxPeriod(TByte *value)
{
  TByte i;
  
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetMaxPeriod, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {  
        *value=iBuffer[PKG_OFS_DATA];
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_SetMaxPeriod(TByte *value)
{
  TByte i;
  
  EKEY_InitBuff();
  EKEY_Buff[0]=*value;
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeySetMaxPeriod, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_GetPeriodLength(TWord *value)
{
  TByte i;
  
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyGetPeriodLength, 0, NULL);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
      {  
        *value=*((TWord*)&iBuffer[PKG_OFS_DATA]);
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_SetPeriodLength(TWord *value)
{
  TByte i;
  
  EKEY_InitBuff();
  memcpy(EKEY_Buff, (TByte *)value, sizeof(TWord));
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeySetPeriodLength, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_EKEY))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_SendReset(void)
{
  TByte i;
  
  EKEY_InitBuff();
  XTEA_Encrypt((TDWord *)&EKEY_Buff, (TDWord *)devicePassword);
  for(i=0;i<RS485_MAX_TRY;i++)
  {
    RS485_SendPackage(RS485_ADDR_EKEY, cmdEkeyReset, 8, EKEY_Buff);
    if (RS485_WaitPackage(TO_500MSEC))
    {                  
      if (iBuffer[PKG_OFS_CMD_STS]==stsOk)
        return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// EKEY_AddKey - add new key
//------------------------------------------------------------------------------
TBool EKEY_AddKey(void)
{
  TByte i;
  TDWord value=0;
  char title[8]="";
  char tStr[8]="00000000";
  
  if (EDIT_Hex(title, tStr, 8))
  {
    //convert HEX string to TDWord
    for (i=0; i<4; i++)
    {
      value<<=8;
      value+=hex2char(&tStr[i*2]);
    }
    strcpy_P(title, EKEY_MENU_USER[EKEYMENU_USER_ADDKEY]);
    if (EKEY_SendKey((TByte *)&value))
    {
      //good
      strcpy_P(tStr, okStr);
    } else
    {
      //bad
      strcpy_P(tStr, errorStr);
    }
    EDIT_Show(title, tStr, false);
  }
  
  return true;
}
  
//------------------------------------------------------------------------------
void EKEY_SetTime(void)
{
  TWord Minutes;
  TTimeDate tmpDt;
            
  EKEY_GetDateTime(&tmpDt);
  Minutes=tmpDt.Hour*60 + tmpDt.Minute;
  
  if (EDIT_Time(timeStr, &Minutes))
  {
    tmpDt.Hour=Minutes/60;
    tmpDt.Minute=Minutes%60;
    tmpDt.Second=0;
    if (EKEY_SetDateTime(&tmpDt))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  }
}

//------------------------------------------------------------------------------
void EKEY_SetDate(void)
{
  TTimeDate tmpDt;
            
  EKEY_GetDateTime(&tmpDt);
  if (EDIT_Date(&tmpDt))
  {
    if (EKEY_SetDateTime(&tmpDt))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);                          
  }

}

//------------------------------------------------------------------------------
void EKEY_SetYear(void)
{
  TWord year;
  TTimeDate tmpDt;                                 
  
  EKEY_GetDateTime(&tmpDt);
  year=tmpDt.Year;
  if (EDIT_Integer(EKEY_LIST_DATE[EKEY_DATE_YEAR], yearFmt, &year, 0, 99, NULL))
  {
    //set year
    tmpDt.Year=year;
    if (EKEY_SetDateTime(&tmpDt))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  } else SND_Play(ssCancel);
}

//------------------------------------------------------------------------------
// EKEY_Date - configure date and time
//------------------------------------------------------------------------------
TBool EKEY_Date(void)
{
  signed char value;
  TList list;
    
  list.numItems=EKEY_DATE_LAST;
  list.items=EKEY_LIST_DATE;
  list.current=0;
  list.title=EKEY_MENU_SET[EKEYMENU_DATE];
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case EKEY_DATE_TIME: // configure time
        EKEY_SetTime();
        break;
      case EKEY_DATE_DAY: // configure date
        EKEY_SetDate();
        break;
      case EKEY_DATE_YEAR: // configure year
        EKEY_SetYear();
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
// EKEY_Version - show Ekey firmware version
//------------------------------------------------------------------------------
TBool EKEY_Version(void)
{
  TWord version;
  char str[10];
  char str2[5];
    
  if (EKEY_GetVersion(&version))
  {
    sprintf_P(str, timeFmt, version>>8, version&0xff);
    strcpy_P(str2, EKEY_MENU_SET[EKEYMENU_VERSION]);
    EDIT_Show(str2, str, false);
  }  
  return true;
}

//------------------------------------------------------------------------------
// EKEY_Serial - show Ekey serial number
//------------------------------------------------------------------------------
TBool EKEY_Serial(void)
{
  char tStr[10];
  char str[10]="N-";

  if (EKEY_GetID(tStr))
  {
    strncpy(&str[2], &tStr[4], SERIAL_NUMBER_LEN);
    EDIT_Show(str, &str[4], false);
  }
  return true;
}

//------------------------------------------------------------------------------
// EKEY_DevSerial - show device serial number
//------------------------------------------------------------------------------
TBool EKEY_DevSerial(void)
{
  char tStr[10];
  char str[10]="N-";

  EEPROM_ReadString(tStr, PCBID, PCB_ID_LEN);
  strncpy(&str[2], &tStr[4], SERIAL_NUMBER_LEN);
  EDIT_Show(str, &str[4], false);
  return true;
}

//------------------------------------------------------------------------------
// EKEY_Errors - show number of Ekey errors
//------------------------------------------------------------------------------
TBool EKEY_Errors(void)
{
  TWord errors;
  char str[10];
  char str2[5];
    
  if (EKEY_GetErrors(&errors))
  {
    sprintf_P(str, decFmt, errors);
    strcpy_P(str2, EKEY_MENU_SET[EKEYMENU_ERRORS]);
    EDIT_Show(str2, str, false);
  }  
  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_Period(void)
{
  TByte value;                                 
  char str[10];
  char str2[5];

  if (EKEY_GetPeriod(&value))
  {
    strcpy_P(str2, EKEY_MENU_SET[EKEYMENU_PERIOD]);
    sprintf_P(str, decFmt, value);
    EDIT_Show(str2, str, false);
  }
  
  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_MaxPeriod(void)
{
  TByte period;
  TWord value;
  
  EKEY_GetMaxPeriod(&period);
  value=period;
  if (EDIT_Integer(EKEY_MENU_SET[EKEYMENU_MAXPERIOD], decFmt, &value, 1, EKEY_MAX_PERIOD, NULL))
  {
    //set max period
    period=(TByte)value;
    if (EKEY_SetMaxPeriod(&period))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  } else SND_Play(ssCancel);
  
  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_PeriodLength(void)
{
  TWord value;
  
  EKEY_GetPeriodLength(&value);
  if (EDIT_Integer(EKEY_MENU_SET[EKEYMENU_PERIODLEN], decFmt, &value, EKEY_MIN_LENGTH, EKEY_MAX_LENGTH, NULL))
  {
    //set period length
    if (EKEY_SetPeriodLength(&value))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  } else SND_Play(ssCancel);
  
  return true;
}

//------------------------------------------------------------------------------
void EKEY_SetAlarm(TByte alarm)
{
  TByte buff[3];
  //TByte days;
  TWord temp;
  
  EKEY_GetAlarmPeriod(buff);
  temp=buff[alarm];
  if (EDIT_Integer(EKEY_LIST_ALARM[alarm], decFmt, &temp, 0, 250, NULL))
  {
    //set alarm period
    buff[alarm]=(TByte)temp;
    if (EKEY_SetAlarmPeriod(buff))
      SND_Play(ssOk);            
    else
      SND_Play(ssError);
  } else SND_Play(ssCancel);
}

//------------------------------------------------------------------------------
// EKEY_Alarms - configure Ekey alarms
//------------------------------------------------------------------------------
TBool EKEY_Alarms(void)
{
  signed char value;
  TList list;
    
  list.numItems=EKEY_ALARM_LAST;
  list.items=EKEY_LIST_ALARM;
  list.current=0;
  list.title=EKEY_MENU_SET[EKEYMENU_ALARMS];
  for (;;)
  {
    value=MENU_RunList(&list);
    switch(value)
    {
      case EKEY_ALARM_FIRST: // configure first alarm
      case EKEY_ALARM_SECOND: // configure second alarm
      case EKEY_ALARM_LOCK: // configure lock
        EKEY_SetAlarm(value);
        break;
      case -1:
        return true;
    }
  }
}

//------------------------------------------------------------------------------
TBool EKEY_Reset(void)
{
  if (EDIT_Bool(EKEY_MENU_SET[EKEYMENU_RESET]))
  {
    if (EKEY_SendReset())
      return true;
  }
  return false;
}

//------------------------------------------------------------------------------
TBool EKEY_Expire(void)
{
  TTimeDate tmpDt;                                 
  char str[10];
  char str2[5];

  if (EKEY_GetExpiredDate(&tmpDt))
  {
    sprintf_P(str2, timeFmt, tmpDt.Day, tmpDt.Month);
    sprintf_P(str, yearFmt, tmpDt.Year);
    EDIT_Show(str2, str, false);
  }
  
  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_Exit(void)
{
  return false;
}

//------------------------------------------------------------------------------
// EKEY_CheckStatus
//------------------------------------------------------------------------------
TBool EKEY_CheckStatus(TByte *status, TWord *days)
{
  //char str[10];
  
  if (!EKEY_GetStatus(status, days)) 
    return false;

  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_Status(void)
{
  char str[10];
  char str2[5];
  TByte status;
  TWord days;
  
  if (EKEY_CheckStatus(&status, &days))
  {
    sprintf_P(str, decFmt, status);
  } else
  {
    strcpy_P(str, errorStr);
  }
  strcpy_P(str2, EKEY_MENU_SET[EKEYMENU_STATUS]);
  EDIT_Show(str2, str, false);

  return true;
}

//------------------------------------------------------------------------------
TBool EKEY_Init(void)
{
  if (EKEY_Connect())
  {
    if (!EKEY_SendSerial())
      return false;
    if (!EKEY_InitEncode())
      return false;
    return true;
  }
  
  return false;
}

//------------------------------------------------------------------------------
// EKEY_Parser - parse command from PC
//------------------------------------------------------------------------------
TByte EKEY_Parser(char *cmd, char *answer, TByte *dataLen)
{
  TByte command;
  TByte i;
  TWord temp;
  TDWord value;
  TTimeDate dt;
  TByte buff[4];

  //cmd++;    //skip zero splitter
  command=*cmd++;
  if (strncmp_P(cmd, strConfirm, 2))
    return EKEY_ANSWER_ERROR;  
  cmd+=2;
  switch (command)
  {
    case 0xF0:
      //read Ekey mode
      i=EEPROM1.Ekey;
      memcpy(answer, &i, sizeof(TByte));
      *dataLen=sizeof(TByte);
      return EKEY_ANSWER_PRINT;

    case 0xF1:
      //set Ekey mode
      i=*cmd;
      EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Ekey, &i, sizeof(TByte));
      return EKEY_ANSWER_OK;
      
    case cmdConnect:
      if (EKEY_Connect())
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;
        
    case cmdGetTime:
      if (EKEY_GetDateTime(&dt))
      {
        //print date as answer
        memcpy(answer, (TByte *)&dt, sizeof(TTimeDate));
        *dataLen=sizeof(TTimeDate);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
      
    case cmdSetTime:
      dt.Day=hex2char(cmd);
      cmd+=2;
      dt.Month=hex2char(cmd);
      cmd+=2;
      dt.Year=hex2char(cmd);
      cmd+=2;
      dt.Hour=hex2char(cmd);
      cmd+=2;
      dt.Minute=hex2char(cmd);
      cmd+=2;
      dt.Second=hex2char(cmd);
      cmd+=2;
      //dt=*((TTimeDate*)cmd);
      if (EKEY_SetDateTime(&dt))
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;

    case cmdEkeyTransKey:
      for (i=0; i<4; i++)
      {
        value<<=8;
        value+=hex2char(cmd);
        cmd+=2;
      }
      if (EKEY_SendKey((TByte *)&value))
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;

    case cmdEkeyGetStatus:
      if (EKEY_GetStatus(&i, &temp))
      {
        //print status as answer
        memcpy(answer, &i, sizeof(TByte));
        *dataLen=sizeof(TByte);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyGetExpiredDate:
      if (EKEY_GetExpiredDate(&dt))
      {
        //print date as answer
        memcpy(answer, (TByte *)&dt, sizeof(TTimeDate));
        *dataLen=sizeof(TTimeDate);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;

    case cmdEkeyGetVersion:
      if (EKEY_GetVersion(&temp))
      {
        //print version as answer
        memcpy(answer, (TByte *)&temp, sizeof(TWord));
        *dataLen=sizeof(TWord);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyGetID:
      if (EKEY_GetID(answer))
      {
        *dataLen=PCB_ID_LEN;
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyGetAlarmPeriod:
      //i=*cmd++;
      if (EKEY_GetAlarmPeriod((TByte *)answer))
      {
        //print days as answer
        //memcpy(answer, (TByte *)&days, sizeof(TByte));
        *dataLen=sizeof(TByte)*3;
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;

    case cmdEkeySetAlarmPeriod:
      //i=*cmd++;
      buff[0]=hex2char(cmd);
      cmd+=2;
      //days=*cmd++;
      buff[1]=hex2char(cmd);
      cmd+=2;
      buff[2]=hex2char(cmd);
      if (EKEY_SetAlarmPeriod(buff))
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyGetErrors:
      if (EKEY_GetErrors(&temp))
      {
        //print errors as answer
        memcpy(answer, (TByte *)&temp, sizeof(TWord));
        *dataLen=sizeof(TWord);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyGetPeriod:
      if (EKEY_GetPeriod(&i))
      {
        //print period as answer
        memcpy(answer, (TByte *)&i, sizeof(TByte));
        *dataLen=sizeof(TByte);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
      
    case cmdEkeyGetMaxPeriod:
      if (EKEY_GetMaxPeriod(&i))
      {
        memcpy(answer, (TByte *)&i, sizeof(TByte));
        *dataLen=sizeof(TByte);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;

    case cmdEkeySetMaxPeriod:
      i=hex2char(cmd);
      //i=*cmd++;
      if (EKEY_SetMaxPeriod(&i))
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;

    case cmdEkeyGetPeriodLength:
      if (EKEY_GetPeriodLength(&temp))
      {
        //print period length as answer
        memcpy(answer, (TByte *)&temp, sizeof(TWord));
        *dataLen=sizeof(TWord);
        return EKEY_ANSWER_PRINT;
      } else
        return EKEY_ANSWER_ERROR;
      
    case cmdEkeySetPeriodLength:
      //temp=*((TWord*)cmd);
      temp=hex2char(cmd);
      cmd+=2;
      temp+=(hex2char(cmd)<<8);
      if (EKEY_SetPeriodLength(&temp))
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;
    
    case cmdEkeyReset:  
      if (EKEY_SendReset())
        return EKEY_ANSWER_OK;
      else
        return EKEY_ANSWER_ERROR;
      
    default:
      return EKEY_ANSWER_ERROR;
  }
}

//------------------------------------------------------------------------------
// EKEY_Start
//------------------------------------------------------------------------------
TBool EKEY_Start(void)
{
  TDWord value;
  TWord temp=0;
  TByte ix;
  char title[]="EKEY";
  char tStr[4]="0000";
  
  if (EDIT_Advanced(title, tStr, 4, false))
  {  
    for (ix=0;ix<LED_NUM_DIGITS;ix++)
    {
      temp*=10;
      temp+=tStr[ix]-0x30; 
    }

  //if (EDIT_Integer(EKEY_MENU_TITLE, decFmt, &temp, 0, 9999, NULL))
  //{
    //start usage
    if (temp!=EKEY_START_USAGE)
    {
      SND_Play(ssError);
      return false;
    }  
    value=temp;
    if (EKEY_SendKey((TByte *)&value))
    {  
      SND_Play(ssOk);            
      return true;
    } else
      SND_Play(ssError);
  }
  
  return false;
}

//------------------------------------------------------------------------------
// EKEY_Settings - run ekey settings menu
//------------------------------------------------------------------------------
void EKEY_Settings(void)
{
  TMenu menu;
  ptrTItemHandler handler[EKEYMENU_LAST];
    
  menu.title=EKEY_MENU_TITLE;
  menu.handler=handler;
  menu.numItems=EKEYMENU_LAST;
  menu.items=EKEY_MENU_SET;
  handler[EKEYMENU_ADDKEY]=EKEY_AddKey;
  handler[EKEYMENU_DATE]=EKEY_Date;
  handler[EKEYMENU_VERSION]=EKEY_Version;
  handler[EKEYMENU_SERIAL]=EKEY_Serial;
  handler[EKEYMENU_DEVSERIAL]=EKEY_DevSerial;
  handler[EKEYMENU_ERRORS]=EKEY_Errors;
  handler[EKEYMENU_EXPIRE]=EKEY_Expire;
  handler[EKEYMENU_PERIOD]=EKEY_Period;
  handler[EKEYMENU_MAXPERIOD]=EKEY_MaxPeriod;
  handler[EKEYMENU_PERIODLEN]=EKEY_PeriodLength;
  handler[EKEYMENU_ALARMS]=EKEY_Alarms;
  handler[EKEYMENU_RESET]=EKEY_Reset;
  handler[EKEYMENU_STATUS]=EKEY_Status;
  handler[EKEYMENU_EXIT]=EKEY_Exit;
  MENU_Run(&menu);
}

//------------------------------------------------------------------------------
// EKEY_Menu - run ekey menu
//------------------------------------------------------------------------------
void EKEY_Menu(void)
{
  TMenu menu;
  ptrTItemHandler handler[EKEYMENU_LAST];
    
  menu.title=EKEY_MENU_TITLE;
  menu.handler=handler;
  menu.numItems=EKEYMENU_USER_LAST;
  menu.items=EKEY_MENU_USER;
  handler[EKEYMENU_USER_ADDKEY]=EKEY_AddKey;
  handler[EKEYMENU_USER_EXPIRE]=EKEY_Expire;
  handler[EKEYMENU_USER_DEVSERIAL]=EKEY_DevSerial;
  handler[EKEYMENU_USER_PERIOD]=EKEY_Period;
  //handler[EKEYMENU_USER_STATUS]=EKEY_Status;
  handler[EKEYMENU_USER_EXIT]=EKEY_Exit;
  MENU_Run(&menu);
}

//------------------------------------------------------------------------------
// EKEY_Check - check status and make warnings
//------------------------------------------------------------------------------
TByte EKEY_Check(TBool check)
{
  TByte status;
  TWord days;
  char str[10];
  char str2[5];
  
  strcpy_P(str, EKEY_MENU_TITLE);
  if (!check)
  {
    strncpy_P(str2, EKEY_STATUS_STRING[EKEY_STATUS_BLOCKED], sizeof(str2));
    EDIT_Show(str, str2, true);
    EDIT_Block();
  }  
  
  if (EKEY_CheckStatus(&status, &days))
  {
    switch (status)
    {
      case EKEY_STATUS_FIRST:
        while (!EKEY_Start())
          continue;
        status=EKEY_STATUS_OK;
        break;

      case EKEY_STATUS_OK:
        break;

      case EKEY_STATUS_ALARM1:
        //strncpy_P(str2, EKEY_STATUS_STRING[status], sizeof(str2));
        sprintf_P(str2, daysFmt, days);
        EDIT_Show(str, str2, true);
        break;

      case EKEY_STATUS_ALARM2:
        //strncpy_P(str2, EKEY_STATUS_STRING[status], sizeof(str2));
        sprintf_P(str2, daysFmt, days);
        EDIT_Show(str, str2, true);
        LED_SetBrightness(LED_BRIGHTNESS_0);
        break;
        
      case EKEY_STATUS_LOCKED:
        strncpy_P(str2, EKEY_STATUS_STRING[status], sizeof(str2));
        EDIT_Show(str, str2, true);
        //EDIT_Block();
        for (;;)
        {  
          while(!KBD_KeyPressed())
            EXTERNAL_Process();
          KBD_GetKey();
          SND_Play(ssError);
        }  
        break;
        
      case EKEY_STATUS_BLOCKED:
      default:
        //block device
        strncpy_P(str2, EKEY_STATUS_STRING[status], sizeof(str2));
        EDIT_Show(str, str2, true);
        EDIT_Block();
    }
  } else
  {
    //ekey is probably disconnected, block device
    strncpy_P(str2, EKEY_STATUS_STRING[EKEY_STATUS_BLOCKED], sizeof(str2));
    EDIT_Show(str, str2, true);
    EDIT_Block();
  }
  
  return status;
}
