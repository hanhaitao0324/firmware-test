#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc.h"
#include "defines.h"
#include "eeprom.h"
#include "global.h"
#include "heater.h"
#include "parser.h"
#include "platform.h"
#include "rs232.h"
#include "strings.h"
#include "temp.h"
#include "version.h"

enum PARSER_COMMANDS
  {
    PARSER_CMD_GT, 						// 返回目标温度
    PARSER_CMD_GUID,
    PARSER_CMD_GLE, 					// 返回最后一个错误代码
    PARSER_CMD_GP,
    PARSER_CMD_ZE,
    PARSER_CMD_Z,
    PARSER_CMD_GE,
    PARSER_CMD_G,
    PARSER_CMD_AX,
    PARSER_CMD_A,
    PARSER_CMD_S,
    PARSER_CMD_PID,
    PARSER_CMD_VER,
    PARSER_CMD_ID,
    PARSER_CMD_TR,
    PARSER_CMD_TT,
    PARSER_CMD_TX,
    PARSER_CMD_RE,
    PARSER_CMD_C00,
    PARSER_CMD_C01,
    PARSER_CMD_C1,
    PARSER_CMD_C2,
    PARSER_CMD_C3,
    PARSER_CMD_C4,
    PARSER_CMD_TO,
    PARSER_CMD_TB,
    PARSER_CMD_LED,
    PARSER_CMD_RST,

    PARSER_CMD_UPFW,

    PARSER_CMD_NULL,

    PARSER_NUM_COMMANDS
  };

__flash char parserCmdNull[]="";

// - GT -
__flash char parserCmdGT[]="GT";
__flash char parserOutGT[]="%04.1f";

// - GUID -
__flash char parserCmdGUID[]="GUID";
__flash char parserOutGUID[]="%s";

// - GLE -
__flash char parserCmdGLE[]="GLE";
__flash char parserOutGLE[]="E%1d";

// - GP -
__flash char parserCmdGP[]="GP";
__flash char parserOutGP[]="P%02d";

// - ZE -
__flash char parserCmdZE[]="ZE";
__flash char parserInZE[]="%f";

// - Z -
__flash char parserCmdZ[]="Z";
__flash char parserInZ[]="%f";

// - GE -
__flash char parserCmdGE[]="GE";
__flash char parserInGE[]="%f";

// - G -
__flash char parserCmdG[]="G";
__flash char parserInG[]="%f";

// - AX -
__flash char parserCmdAX[]="AX";
__flash char parserOutAX[]="AX%06X";

// - A -
__flash char parserCmdA[]="A";
__flash char parserOutA[]="A%06X";

// - S -
__flash char parserCmdS[]="S";

// - PID -
__flash char parserCmdPID[]="PID";
__flash char parserOutPID[]="%s";

// - VER -
__flash char parserCmdVER[]="VER";
__flash char parserOutVER[]="V%s";

// - ID -
__flash char parserCmdID[]="ID";
__flash char parserOutID[]="%s";

// - TR -
__flash char parserCmdTR[]="TR";
__flash char parserOutTR[]="T%04.1f";

// - TT -
__flash char parserCmdTT[]="TT";
__flash char parserInTT[]="%f";

// - TX -
__flash char parserCmdTX[]="TX";
__flash char parserOutTX[]="T%04.1f";

// - RE -
__flash char parserCmdRE[]="RE";

// - C00 -
__flash char parserCmdC00[]="C00";

// - C01 -
__flash char parserCmdC01[]="C01";

// - C1 -
__flash char parserCmdC1[]="C1";

// - C2 -
__flash char parserCmdC2[]="C2";

// - C3 -
__flash char parserCmdC3[]="C3";

// - C4 -
__flash char parserCmdC4[]="C4";
__flash char parserInC4[]="%f";

// - TO -
__flash char parserCmdTO[]="TO";
__flash char parserOutTO[]="B%.1f";

// - TB -
__flash char parserCmdTB[]="TB";
__flash char parserInTB[]="%f";

// - LED -
__flash char parserCmdLED[]="LED";
__flash char parserInLED[]="%d";

// - RST -
__flash char parserCmdRST[]="RST";
__flash char parserInRST[]="%d";

// - UPFW -
__flash char parserCmdUPFW[]="UPFW";

__flash TParsedItem parserItems[PARSER_NUM_COMMANDS]=
  {
    {parserCmdGT,     NULL,         parserOutGT},
    {parserCmdGUID,   NULL,         parserOutGUID},
    {parserCmdGLE,    NULL,         parserOutGLE},
    {parserCmdGP,     NULL,         parserOutGP},
    {parserCmdZE,     parserInZE,   NULL},
    {parserCmdZ,      parserInZ,    NULL},
    {parserCmdGE,     parserInGE,   NULL},
    {parserCmdG,      parserInG,    NULL},
    {parserCmdAX,     NULL,         parserOutAX},
    {parserCmdA,      NULL,         parserOutA},
    {parserCmdS,      NULL,         NULL},
    {parserCmdPID,    NULL,         parserOutPID},
    {parserCmdVER,    NULL,         parserOutVER},
    {parserCmdID,     NULL,         parserOutID},
    {parserCmdTR,     NULL,         parserOutTR},
    {parserCmdTT,     parserInTT,   NULL},
    {parserCmdTX,     NULL,         parserOutTX},
    {parserCmdRE,     NULL,         NULL},
    {parserCmdC00,    NULL,         NULL},
    {parserCmdC01,    NULL,         NULL},
    {parserCmdC1,     NULL,         NULL},
    {parserCmdC2,     NULL,         NULL},
    {parserCmdC3,     NULL,         NULL},
    {parserCmdC4,     parserInC4,   NULL},
    {parserCmdTO,     NULL,         parserOutTO},
    {parserCmdTB,     parserInTB,   NULL},
    {parserCmdLED,    parserInLED,  NULL},
    {parserCmdRST,    parserInRST,  NULL},
    {parserCmdUPFW,   NULL,         NULL},
    {parserCmdNull,   NULL,         NULL}
  };

__flash char parserCRLF[]="\r";          

__flash char parserErrOk[]="O";
__flash char parserErrWrongCommand[]="E2";
__flash char parserErrWrongValue[]="E2";
__flash char parserErrFirstCal[]="E3";
__flash char parserErrSensor[]="E6";

TByte lastError=STS_OK;

static char buffer[40];

extern void Update();

//------------------------------------------------------------------------------
// strupr
//------------------------------------------------------------------------------
char *strupr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str=toupper(*str);
    str++;
  }
  return s;
}

//------------------------------------------------------------------------------
// ParserGetIndex  - search for an index in the string array
//------------------------------------------------------------------------------
signed char ParserGetIndex(TParsedItem __flash *items,char *buffer)
{
  signed char i;

  strupr(buffer);
  i=0;
  for (i=0;i<PARSER_NUM_COMMANDS;i++)
    if (0==strncmp_P(buffer,items[i].itemStr,strlen_P(items[i].itemStr)))
      return i;
  return -1;
}

/*TByte ParseNumber(TByte* s, TWord* n)
  {
    TByte result = 0;

    *n = 0;

    if(isdigit(*s))
      {
	*n = *n * 10 + *(s++) - '0';
	result++;
      }

    if(isdigit(*s))
      {
	*n = *n * 10 + *(s++) - '0';
	result++;
      }

    if(*s == '.')
      {
	s++;
	result++;
	if(isdigit(*s))
	  {
	    *n = *n * 10 + *(s++) - '0';
	    result++;
	  }
	else
	  *n *= 10;
      }
    else
      *n *= 10;

    return result;              
  }*/

//------------------------------------------------------------------------------
// Parse - parser main
//------------------------------------------------------------------------------
void PARSER_Comm(void)
{
  signed char index;
  char *cmd;
  int pos;
  TWord intValue0;
  char __flash *errMsg;
  TReal32 fltValue0;
  //char buffer[40];
  TByte cmd_length;
  TByte ix;

  if (!globalDeviceStatus.gotRS232Package)
    return;
  
  globalDeviceStatus.gotRS232Package=false;
  RS232_SetRx(false);
    
  //make an ASCIIZ string from the buffer
  pos=0;
  while ((('\r'!=RS232_IBuffer[pos])||('\n'!=RS232_IBuffer[pos]))&&RS232_IBuffer[pos])
    pos++;
            
  if (pos>=RS232_BUFFER_SIZE)
    pos=RS232_BUFFER_SIZE-1;
  RS232_IBuffer[pos]='\0';

  cmd=RS232_IBuffer;
  //find command - skip blanks
  while (*cmd&&isspace(*cmd))
    cmd++;
  pos=0;
  while (cmd[pos]&&(!isspace(cmd[pos])))
    pos++;
  cmd[pos]='\0';

  //check command and get its index
  index=ParserGetIndex(parserItems,cmd);
  if (index>=0)
  {  
    cmd_length=strlen_P(parserItems[index].itemStr);
    if (strlen(cmd)>cmd_length)
    {
      cmd=&cmd[cmd_length];
    }
    else
    	cmd=&cmd[pos];//strlen(cmd)+1];
  }
  errMsg=NULL;
    
  if ((index==-1)||(index>=PARSER_CMD_NULL))
  {
    errMsg=parserErrWrongCommand;
    lastError=STS_WRONG_COMMAND;
  }
  else
  {  
    switch (index)
    {
      case PARSER_CMD_GT:
        //get target temperature
        fltValue0=EEPROM1.Temp;
	printf_P(parserItems[PARSER_CMD_GT].outFmt,fltValue0);
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_GUID:
        //get GUID
        //EEPROM_ReadString(buffer, EEPROM1.GUID, sizeof(EEPROM1.GUID));
        strcpy_P(buffer, GUID);
	printf_P(parserItems[PARSER_CMD_GUID].outFmt, buffer);
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_GLE:
        //get last error code
	printf_P(parserItems[PARSER_CMD_GLE].outFmt, lastError);
        break;
        
      case PARSER_CMD_GP:
        //get heater power
        printf_P(parserItems[PARSER_CMD_GP].outFmt,HEATER_GetPower());
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_ZE:
      case PARSER_CMD_Z:
        //first-point Adjustment
        if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_Z].inFmt, &fltValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
	  break;
	}
        //fltValue0=atof(cmd);
	if ((fltValue0>MAX_TEMP_VALUE)||(fltValue0<MIN_TEMP_VALUE))
	{
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
	  break;
	}        
        if (index==PARSER_CMD_ZE)
          TEMP_SetCalPoint(ADC_CHNL_EXTERNAL, 0, fltValue0);
        else  
          TEMP_SetCalPoint(ADC_CHNL_INTERNAL, 0, fltValue0);
        TEMP_Calibrate();
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_GE:
      case PARSER_CMD_G:
        //second-point Adjustment
        if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_G].inFmt, &fltValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
	  break;
	}
        //fltValue0=atof(cmd);
	if ((fltValue0>MAX_TEMP_VALUE)||(fltValue0<MIN_TEMP_VALUE))
	{
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
	  break;
	}        
        if (index==PARSER_CMD_GE)
          TEMP_SetCalPoint(ADC_CHNL_EXTERNAL, 1, fltValue0);
        else  
          TEMP_SetCalPoint(ADC_CHNL_INTERNAL, 1, fltValue0);
        if (!TEMP_Calibrate())
        {  
          errMsg=parserErrFirstCal;
          lastError=STS_NOT_FIRST_POINT;
        } else
        {
          errMsg=parserErrOk;
          lastError=STS_OK;
        }
        break;
        
      case PARSER_CMD_AX:
        //get raw ADC value (external)
        printf_P(parserItems[PARSER_CMD_AX].outFmt, ADC_GetValue(ADC_CHNL_EXTERNAL));
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_A:
        //get raw ADC value (internal)
        printf_P(parserItems[PARSER_CMD_A].outFmt, ADC_GetValue(ADC_CHNL_INTERNAL));
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_S:
        //switches to standby mode
        //fltValue0=0.0;
        //HEATER_SetTemp(&fltValue0);
        HEATER_SetOff();
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_PID:
        //get PID
        EEPROM_ReadString(buffer, PID, PID_MAX_LEN);
        buffer[PID_MAX_LEN]=0;
	printf_P(parserItems[PARSER_CMD_PID].outFmt, buffer);
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_VER:
        //get firmware version
        strcpy_P(buffer, DEVICE_VERSION);
	printf_P(parserItems[PARSER_CMD_VER].outFmt, buffer);
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_ID:
        //get device ID
        strcpy_P(buffer, DEVICE_TYPE[globalDeviceType]);
	printf_P(parserItems[PARSER_CMD_ID].outFmt, buffer);
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_TR:
        fltValue0=TEMP_GetValue(ADC_CHNL_INTERNAL);
        if (globalDeviceStatus.sensorError)
        {  
          errMsg=parserErrSensor;
          lastError=STS_SENSOR_NOT_CONNECTED;
        } else
        {  
          printf_P(parserItems[PARSER_CMD_TR].outFmt, fltValue0);
          lastError=STS_OK;
        }
        break;
        
      case PARSER_CMD_TT:
        //set target temperature
        if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_TT].inFmt, &fltValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
	  break;
	}
        //fltValue0=atof(cmd);
        if ((TWord)(fltValue0*10))
        {  
  	  if ((fltValue0>MAX_TEMP_VALUE)||(fltValue0<MIN_TEMP_VALUE))
	  {
	    errMsg=parserErrWrongValue;
            lastError=STS_WRONG_COMMAND;
	    break;
	  }        
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Temp, (TByte *)&fltValue0, sizeof(EEPROM1.Temp));
        }  
	HEATER_SetTemp(&fltValue0);
        globalDeviceStatus.remoteEnabled=true;
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_TX: 
        //get external sensor temperature
        fltValue0=TEMP_GetValue(ADC_CHNL_EXTERNAL);
        if (globalDeviceStatus.sensorExtError)
        {  
          errMsg=parserErrSensor;
          lastError=STS_SENSOR_NOT_CONNECTED;
        } else
        {  
          printf_P(parserItems[PARSER_CMD_TX].outFmt, fltValue0);
          lastError=STS_OK;
        }
        break;
        
      case PARSER_CMD_RE:
        //remote end
        globalDeviceStatus.remoteEnabled=false;
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_C00:
        //Adjustment start for internal sensor
        TEMP_SetCalChannel(ADC_CHNL_INTERNAL);
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_C01:
        //Adjustment start for external sensor
        TEMP_SetCalChannel(ADC_CHNL_EXTERNAL);
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_C1:
        //Adjustment, first point, 0.0
        ix=TEMP_GetCalChannel();
        TEMP_SetCalPoint(ix, 0, 0.0);
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_C2:
        //Adjustment, second point, 40.0
        ix=TEMP_GetCalChannel();
        TEMP_SetCalPoint(ix, 1, 40.0);
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;
        
      case PARSER_CMD_C3:
        //do Adjustment
        if (!TEMP_Calibrate())
        {  
          errMsg=parserErrFirstCal;
          lastError=STS_NOT_FIRST_POINT;
        } else
        {  
          errMsg=parserErrOk;
          lastError=STS_OK;
        }
        break;
        
      case PARSER_CMD_C4:
        //do offset Adjustment (one point)
        if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_TT].inFmt, &fltValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
          break;
	}
        //fltValue0=atof(cmd);
        ix=TEMP_GetCalChannel();
        TEMP_SetCalPoint(ix, 0, fltValue0);
        if (!TEMP_Calibrate())
        {  
          errMsg=parserErrFirstCal;
          lastError=STS_NOT_FIRST_POINT;
        } else
        {
          errMsg=parserErrOk;
          lastError=STS_OK;
        }
        break;
      
      case PARSER_CMD_TO:
	//read temperature offset
	fltValue0=(TReal32)EEPROM1.TempOffset/10;
	printf_P(parserItems[PARSER_CMD_TO].outFmt, fltValue0);
	break;
          
      case PARSER_CMD_TB:
        //set temperature offset
        {
          signed char temp;
          if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_TB].inFmt, &fltValue0))
	  {
	    errMsg=parserErrWrongValue;
            lastError=STS_WRONG_COMMAND;
	    break;
  	  }
          //fltValue0=atof(cmd);
	  if (fltValue0>MAX_OFFSET_VALUE) fltValue0=MAX_OFFSET_VALUE;
          if (fltValue0<MIN_OFFSET_VALUE) fltValue0=MIN_OFFSET_VALUE;
          temp=(signed char)(fltValue0*10);
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempOffset, (TByte *)&temp, sizeof(EEPROM1.TempOffset));
          if (HEATER_On())
          {
            fltValue0=EEPROM1.Temp;
            HEATER_SetTemp(&fltValue0);
          }
          errMsg=parserErrOk;
        }  
        break;
      
      case PARSER_CMD_LED:
        //set LED brightness
        if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_LED].inFmt, &intValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
          break;
	}
        //intValue0=atoi(cmd);
	if (intValue0>LED_BRIGHTNESS_9)
	{
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
          break;
	}        
        if (LED_SetBrightness((TByte)intValue0))
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&intValue0, sizeof(EEPROM1.LED));
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;

      case PARSER_CMD_RST: 
        if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_LED].inFmt, &intValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
          break;
	}
        if (TEMP_Reset((TByte)intValue0))
        {
	  errMsg=parserErrWrongValue;
          lastError=STS_WRONG_COMMAND;
        }  
        errMsg=parserErrOk;
        lastError=STS_OK;
        break;

      case PARSER_CMD_UPFW:
        printf_P(parserErrOk);
        printf_P(parserCRLF);
        LED_SwitchOff();
        __delay_cycles(XTAL/2);
        __disable_interrupt();
        Update();
        break;
        
      case PARSER_CMD_NULL:
        break;
        
      default:
        errMsg=parserErrWrongCommand;
        lastError=STS_WRONG_COMMAND;
        break;
    }
  }
  if (errMsg)
    printf_P(errMsg);
  printf_P(parserCRLF);
  //memset(rs232OBuffer, 0, sizeof(rs232OBuffer));
  RS232_SetRx(true);
}
