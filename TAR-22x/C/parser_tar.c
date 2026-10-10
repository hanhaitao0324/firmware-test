#include <ctype.h>
#include <string.h>
#include <stdio.h>
                  
#include "const.h"
#include "defines.h"
#include "eeprom.h"
#include "ekey.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "parser.h"
#include "platform.h"
#include "printrtc.h"
#include "sound.h"
#include "strings.h"
#include "updater.h"
#include "usb.h"
#include "version.h"

//------------------------------------------------------------------------------
enum PARSER_COMMANDS
{
  TAR_CMD_GETFRMVER,
  TAR_CMD_GETTYPE,
  TAR_CMD_SETTYPE,
  TAR_CMD_GETMODE,
  TAR_CMD_SETMODE,
  TAR_CMD_GETDEVSN,
  TAR_CMD_SETDEVSN,
  TAR_CMD_GETPCBID,
  //TAR_CMD_SETPCBID,
  TAR_CMD_GETLOGO,
  TAR_CMD_SETLOGO,
  TAR_CMD_HELP,

  TAR_CMD_PRESET,
  TAR_CMD_SETSND,
  TAR_CMD_SETPRNMODE,
  TAR_CMD_SETDTFORMAT,
  TAR_CMD_LED,
      
  TAR_CMD_GETTIME,
  TAR_CMD_GETDATE,
  TAR_CMD_SETTIME,
  TAR_CMD_SETDATE,

  TAR_CMD_SETTESTTIME,
  TAR_CMD_SETTESTSTROKES,
  TAR_CMD_SETSPEED,
  TAR_CMD_SETTESTMODE,
  TAR_CMD_GETTESTTIME,
  TAR_CMD_GETTESTSTROKES,
  TAR_CMD_GETTESTMODE,
  TAR_CMD_GETCURTIME,    
  TAR_CMD_GETCURSTROKES,    

  TAR_CMD_GETSPEED,
                   
  TAR_CMD_TESTSTART,
  TAR_CMD_TESTSTOP,

  TAR_CMD_PPR,
  TAR_CMD_SETPPR,

  TAR_CMD_PASS,
  
  TAR_CMD_GETLASTKEY,

  TAR_CMD_UPFW,
  TAR_CMD_UPBUS,
  
  TAR_CMD_EKEY,
  
  TAR_CMD_NULL,

  TAR_NUM_COMMANDS
};

//------------------------------------------------------------------------------

// - VER -
__flash char parserCmdVer[]="VER";
__flash char parserOutVer[]="%s";
__flash char parserHlpVer[]="shows firmware version: major.minor";

// - GETTYPE -
__flash char parserCmdGetType[]="GETTYPE";
__flash char parserOutGetType[]="%s";                 
__flash char parserHlpGetType[]="shows device type";

// - SETTYPE -
__flash char parserCmdSetType[]="SETTYPE";
__flash char parserInSetType[]="%d";
__flash char parserHlpSetType[]="changes device type (0-TA, 1-TAR, 2-FT)";

// - GETMODE -
__flash char parserCmdGetMode[]="GETMODE";
__flash char parserOutGetMode[]="%s";                 
__flash char parserHlpGetMode[]="shows device mode";

// - SETMODE -
__flash char parserCmdSetMode[]="SETMODE";
__flash char parserInSetMode[]="%d";
__flash char parserHlpSetMode[]="changes device mode (0-120, 1-220, 2-420)";

// - GETSERIAL -
__flash char parserCmdGetSerial[]="GETSERIAL";
__flash char parserOutGetSerial[]="%s";
__flash char parserHlpGetSerial[]="shows serial number";

// - SETSERIAL -
__flash char parserCmdSetSerial[]="SETSERIAL";
__flash char parserInSetSerial[]="%s";
__flash char parserHlpSetSerial[]="changes serial number";

// - GETPCBID -
__flash char parserCmdGetPCBID[]="GETPCBID";
__flash char parserOutGetPCBID[]="%s";
__flash char parserHlpGetPCBID[]="shows PCB ID";

// - SETPCBID -
//__flash char parserCmdSetPCBID[]="SETPCBID";
//__flash char parserInSetPCBID[]="%s";
//__flash char parserHlpSetPCBID[]="changes PCB ID";

// - GETLOGO -
__flash char parserCmdGetLogo[]="GETLOGO";
__flash char parserOutGetLogo[]="%s";
__flash char parserHlpGetLogo[]="gets firm logo";

// - SETLOGO -
__flash char parserCmdSetLogo[]="SETLOGO";
__flash char parserInSetLogo[]="%1d";
__flash char parserHlpSetLogo[]="sets firm logo (0-Erweka, 1-Caleva, 2-Tabtest, 3-Qtest)";

// - HELP -
__flash char parserCmdHelp[]="HELP";
__flash char parserHlpHelp[]="shows this help screen";

// - PRESET -
__flash char parserCmdPreset[]="PRESET";
__flash char parserHlpPreset[]="preset default values";

// - SOUND -
__flash char parserCmdSound[]="SOUND";
__flash char parserInSound[]="%d";
__flash char parserHlpSound[]="sets sound (0-off, 1-on)";

// - PRINTER -
__flash char parserCmdPrinter[]="PRINTER";
__flash char parserInPrinter[]="%d";
__flash char parserHlpPrinter[]="sets printer mode (0-off, 1-auto, 2-ask)";

// - DTFORMAT -
__flash char parserCmdDTFormat[]="DTFORMAT";
__flash char parserInDTFormat[]="%d";
__flash char parserHlpDTFormat[]="sets date/time format (0-EUR, 1-USA)";

// - LED -
__flash char parserCmdLED[]="LED";
__flash char parserInLED[]="%d";
__flash char parserHlpLED[]="sets LED brightness level (0-9)";

// - GETTIME -
__flash char parserCmdGetTime[]="GETTIME";
__flash char parserOutGetTime[]="%s";
__flash char parserHlpGetTime[]="gets current time";

// - GETDATE -
__flash char parserCmdGetDate[]="GETDATE";
__flash char parserOutGetDate[]="%s";
__flash char parserHlpGetDate[]="gets current date";

// - SETTIME -
__flash char parserCmdSetTime[]="SETTIME";
__flash char parserInSetTime[]="%02d:%02d";
__flash char parserHlpSetTime[]="sets time (hh:mm)";

// - SETDATE -
__flash char parserCmdSetDate[]="SETDATE";
__flash char parserInSetDate[]="%02d.%02d.20%02d";
__flash char parserHlpSetDate[]="sets date (dd.mm.yyyy)";

// - SETTESTTIME -
__flash char parserCmdSetTestTime[]="SETTESTTIME";
__flash char parserInSetTestTime[]="%02d:%02d:%02d";
__flash char parserHlpSetTestTime[]="sets test time (hh:mm:ss)";

// - SETTESTSTROKES -
__flash char parserCmdSetTestStrokes[]="SETTESTSTROKES";
__flash char parserInSetTestStrokes[]="%d";
__flash char parserHlpSetTestStrokes[]="sets strokes for test";

// - SETSPEED -
__flash char parserCmdSetSpeed[]="SETSPEED";
__flash char parserInSetSpeed[]="%d";
__flash char parserHlpSetSpeed[]="sets speed for test (TAR only)";

// - SETTESTMODE -
__flash char parserCmdSetTestMode[]="SETTESTMODE";
__flash char parserInSetTestMode[]="%d";
__flash char parserHlpSetTestMode[]="sets test mode (0-time, 1-revolutions)";

// - GETTESTTIME -
__flash char parserCmdGetTestTime[]="GETTESTTIME";
__flash char parserOutGetTestTime[]="%02d:%02d:%02d";
__flash char parserHlpGetTestTime[]="gets test time (hh:mm:ss)";

// - GETTESTSTROKES -
__flash char parserCmdGetTestStrokes[]="GETTESTSTROKES";
__flash char parserOutGetTestStrokes[]="%d";
__flash char parserHlpGetTestStrokes[]="gets strokes for test";

// - GETTESTMODE -
__flash char parserCmdGetTestMode[]="GETTESTMODE";
__flash char parserOutGetTestMode[]="%d";
__flash char parserHlpGetTestMode[]="gets test mode";

// - GETCURTIME -
__flash char parserCmdGetCurTime[]="GETCURTIME";
__flash char parserOutGetCurTime[]="%02d:%02d:%02d";
__flash char parserHlpGetCurTime[]="shows current test time (hh:mm:ss)";

// - GETCURSTROKES -
__flash char parserCmdGetCurStrokes[]="GETCURSTROKES";
__flash char parserOutGetCurStrokes[]="%d";
__flash char parserHlpGetCurStrokes[]="shows current test strokes";

// - GETSPEED -
__flash char parserCmdGetSpeed[]="GETSPEED";
__flash char parserOutGetSpeed[]="%.1f";
__flash char parserHlpGetSpeed[]="shows current speed";

// - TESTSTART -
__flash char parserCmdTestStart[]="TESTSTART";
__flash char parserHlpTestStart[]="starts test";

// - TESTSTOP -
__flash char parserCmdTestStop[]="TESTSTOP";
__flash char parserHlpTestStop[]="stops test";

// - PPR -
__flash char parserCmdPPR[]="PPR";
__flash char parserOutPPR[]="%03d";
__flash char parserHlpPPR[]="get pulses per rotation";

// - SETPPR -
__flash char parserCmdSetPPR[]="SETPPR";
__flash char parserInSetPPR[]="%d";
__flash char parserHlpSetPPR[]="set pulses per rotation";

// - PASS -
__flash char parserCmdPass[]="PSW";
__flash char parserInPass[]="%4d";
__flash char parserHlpPass[]="enters engineering password (4 digits)";

// - GETLASTKEY -
__flash char parserCmdGetLastKey[]="GETLASTKEY";
__flash char parserOutGetLastKey[]="%s";
__flash char parserHlpGetLastKey[]="gets last pressed key";

// - UPFW -
__flash char parserCmdUPFW[]="UPFW";

// - UPBUS -
__flash char parserCmdUPBUS[]="UPBUS";
__flash char parserInUPBUS[]="%X";

// - EKEY -
__flash char parserCmdEKEY[]="EKEY";

__flash char parserCmdNull[]="";

//------------------------------------------------------------------------------

__flash char parserMsgPrompt[]="\r\n>";

__flash char parserErrWrongCommand[]="Wrong or unsupported command";
__flash char parserErrWrongValue[]="Wrong parameter(s)";
__flash char parserErrWrongParamList[]="Wrong parameter's list";
__flash char parserErrWrongPassword[]="Wrong password";
__flash char parserErrNotLogged[]="Please authorize first!";
__flash char parserErrTestRunning[]="Command not allowed during running test";
__flash char parserErrTestNotStarted[]="Test is not started!";
__flash char parserErrNoPrinter[]="No printer board installed!";
__flash char parserPrinterError[]="Printer board error!";
__flash char parserErrOk[]="Ok";

__flash char parserHelpSeparator[]=" - ";
__flash char parserSettingsSeparator[]=": ";
__flash char parserCRLF[]="\r\n";          
__flash char parserUpdateOk[]="O\r";

//------------------------------------------------------------------------------

__flash TParsedItem parserItems[TAR_NUM_COMMANDS]=
  {
    {parserCmdVer,            NULL,                   parserOutVer,             parserHlpVer,             false,  false,  false},
    {parserCmdGetType,        NULL,                   parserOutGetType,         parserHlpGetType,         false,  false,  false},
    {parserCmdSetType,        parserInSetType,        NULL,                     parserHlpSetType,         true,   false,  false},
    {parserCmdGetMode,        NULL,                   parserOutGetMode,         parserHlpGetMode,         false,  false,  false},
    {parserCmdSetMode,        parserInSetMode,        NULL,                     parserHlpSetMode,         true,   false,  false},
    {parserCmdGetSerial,      NULL,                   parserOutGetSerial,       parserHlpGetSerial,       false,  false,  false},
    {parserCmdSetSerial,      parserInSetSerial,      NULL,                     parserHlpSetSerial,       true,   false,  false},
    {parserCmdGetPCBID,       NULL,                   parserOutGetPCBID,        parserHlpGetPCBID,        false,  false,  false},
    {parserCmdGetLogo,        NULL,                   parserOutGetLogo,         parserHlpGetLogo,         false,  false,  false},
    {parserCmdSetLogo,        parserInSetLogo,        NULL,                     parserHlpSetLogo,         true,   false,  false},
    {parserCmdHelp,           NULL,                   NULL,                     parserHlpHelp,            false,  false,  false},
    
    {parserCmdPreset,         NULL,                   NULL,                     parserHlpPreset,          false,  false,  false},
    {parserCmdSound,          parserInSound,          NULL,                     parserHlpSound,           false,  false,  false},
    {parserCmdPrinter,        parserInPrinter,        NULL,                     parserHlpPrinter,         false,  false,  true},
    {parserCmdDTFormat,       parserInDTFormat,       NULL,                     parserHlpDTFormat,        false,  false,  true},
    {parserCmdLED,            parserInLED,            NULL,                     parserHlpLED,             false,  false,  false},
    
    {parserCmdGetTime,        NULL,                   parserOutGetTime,         parserHlpGetTime,         false,  false,  true},
    {parserCmdGetDate,        NULL,                   parserOutGetDate,         parserHlpGetDate,         false,  false,  true},
    {parserCmdSetTime,        parserInSetTime,        NULL,                     parserHlpSetTime,         false,  false,  true},
    {parserCmdSetDate,        parserInSetDate,        NULL,                     parserHlpSetDate,         false,  false,  true},

    {parserCmdSetTestTime,    parserInSetTestTime,    NULL,                     parserHlpSetTestTime,     false,  false,  false},
    {parserCmdSetTestStrokes, parserInSetTestStrokes, NULL,                     parserHlpSetTestStrokes,  false,  true,   false},
    {parserCmdSetSpeed,       parserInSetSpeed,       NULL,                     parserHlpSetSpeed,        false,  true,   false},
    {parserCmdSetTestMode,    parserInSetTestMode,    NULL,                     parserHlpSetTestMode,     false,  true,   false},
    {parserCmdGetTestTime,    NULL,                   parserOutGetTestTime,     parserHlpGetTestTime,     false,  false,  false},
    {parserCmdGetTestStrokes, NULL,                   parserOutGetTestStrokes,  parserHlpGetTestStrokes,  false,  true,   false},
    {parserCmdGetTestMode,    NULL,                   parserOutGetTestMode,     parserHlpGetTestMode,     false,  false,  false},
    {parserCmdGetCurTime,     NULL,                   parserOutGetCurTime,      parserHlpGetCurTime,      false,  false,  false},
    {parserCmdGetCurStrokes,  NULL,                   parserOutGetCurStrokes,   parserHlpGetCurStrokes,   false,  true,   false},

    {parserCmdGetSpeed,       NULL,                   parserOutGetSpeed,        parserHlpGetSpeed,        false,  false,  false},

    {parserCmdTestStart,      NULL,                   NULL,                     parserHlpTestStart,       false,  false,  false},
    {parserCmdTestStop,       NULL,                   NULL,                     parserHlpTestStop,        false,  false,  false},

    {parserCmdPPR,            NULL,                   parserOutPPR,             parserHlpPPR,             false,  false,  false},
    {parserCmdSetPPR,         parserInSetPPR,         NULL,                     parserHlpSetPPR,          true,   false,  false},

    {parserCmdPass,           parserInPass,           NULL,                     parserHlpPass,            false,  false,  false},

    {parserCmdGetLastKey,     NULL,                   parserOutGetLastKey,      parserHlpGetLastKey,      true,   false,  false},

    {parserCmdUPFW,           NULL,                   NULL,                     NULL,                     false,  false,  false},
    {parserCmdUPBUS,          parserInUPBUS,          NULL,                     NULL,                     false,  false,  false},

    {parserCmdEKEY,           NULL,                   NULL,                     NULL,                     false,  false,  false},

    {parserCmdNull,           NULL,                   NULL,                     NULL,                     false,  false,  false}
  };

TBool secured=false;

extern void Update();

//------------------------------------------------------------------------------
// toupper
//------------------------------------------------------------------------------
int my_toupper(int c)
{
  if ((c>=0x61)&&(c<=0x7A))
    return (c-0x20);
  else
    return c;
}

//------------------------------------------------------------------------------
// strupr
//------------------------------------------------------------------------------
char *strupr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str=my_toupper(*str);
    str++;
  }
  return s;
}

//------------------------------------------------------------------------------
// ParserGetIndex  - search for an index in the string array
//------------------------------------------------------------------------------
signed char PARSER_GetIndex(TParsedItem __flash *items,char *buffer)
{
  signed char i;

  strupr(buffer);
  i=0;
  for (i=0; i<TAR_NUM_COMMANDS; i++)
    //if (0==strcmp_P(buffer,items[i].itemStr))
    if (0==strncmp_P(buffer, items[i].itemStr, strlen_P(items[i].itemStr)))
      return i;
  return -1;
}

//------------------------------------------------------------------------------
// PARSER_Comm - parser main function
//------------------------------------------------------------------------------
void PARSER_Comm(void)
{
  signed char index;
  char *cmd;
  int pos;
  TWord intValue0;
  char __flash *errMsg;
  TReal32 fltValue0;
  TByte cmd_length;
  char buffer[20];

  cmd=USB_IBuffer;
  
  //make an ASCIIZ string from the buffer
  pos=0;
  while (('\r'!=cmd[pos])&&('\n'!=cmd[pos])&&cmd[pos])
    pos++;
          
  if (pos>=USB_BUFFER_SIZE)
    pos=USB_BUFFER_SIZE-1;
  cmd[pos]='\0';

  //find command - skip blanks
  while (*cmd&&(*cmd==0x20))
    cmd++;
  pos=0;
  while (cmd[pos]&&(cmd[pos]!=0x20))
    pos++;
  cmd[pos]='\0';

  //check command and get its index
  index=PARSER_GetIndex(parserItems,cmd);
  //cmd=&cmd[strlen(cmd)+1];
  if (index>=0)
  {  
    cmd_length=strlen_P(parserItems[index].itemStr);
    if (strlen(cmd)>cmd_length)
    {
      cmd=&cmd[cmd_length];
    } else cmd=&cmd[strlen(cmd)+1];//&cmd[pos];
  }
  errMsg=NULL;
  
  if ((index==-1)||(index>=TAR_CMD_NULL))
  {
    errMsg=parserErrWrongCommand;
  } else
  if ((!secured)&&(parserItems[index].secure))
  {
    errMsg=parserErrNotLogged;
  } else
  if ((!globalStatus.installedPrinter)&&(parserItems[index].printerBrd))
  {
    errMsg=parserErrNoPrinter;
  } else      
  {  
    switch (index)
    {
      case TAR_CMD_GETFRMVER:    
        strcpy_P(buffer, VERSION_Revision);
        printf_P(parserItems[TAR_CMD_GETFRMVER].outFmt, buffer);
        break;
        
      case TAR_CMD_GETTYPE:          
          strcpy_P(buffer, DEVICE_TYPE[EEPROM1.DevType]);
          printf_P(parserItems[TAR_CMD_GETTYPE].outFmt, buffer);
        break;
        
      case TAR_CMD_SETTYPE:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETTYPE].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=DEVICE_TYPE_LAST)
        {
          errMsg=parserErrWrongValue;
          break;
        }
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DevType, (TByte *)&intValue0, sizeof(EEPROM1.DevType));
        errMsg=parserErrOk;
        break;
        
      case TAR_CMD_GETMODE:          
        strcpy_P(buffer, DEVICE_MODE[EEPROM1.DevType][EEPROM1.DevMode]);
        printf_P(parserItems[TAR_CMD_GETMODE].outFmt, buffer);
        break;
        
      case TAR_CMD_SETMODE:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETMODE].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=DEVICE_MODE_LAST)
        {
          errMsg=parserErrWrongValue;
          break;
        }
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DevMode, (TByte *)&intValue0, sizeof(EEPROM1.DevMode));
        errMsg=parserErrOk;
        break;
        
      case TAR_CMD_GETDEVSN:
        //read
        EEPROM_ReadString(buffer, EEPROM1.Serial, SERIAL_NUMBER_LEN);
        buffer[SERIAL_NUMBER_LEN]=0;
        printf_P(parserItems[TAR_CMD_GETDEVSN].outFmt, buffer);
        break;
        
      case TAR_CMD_SETDEVSN:
        //delete /r/n from the string
        pos=strlen(cmd);
        if (cmd)
          cmd[pos-1]='\0';
        //store                              
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Serial, (TByte *)cmd, SERIAL_NUMBER_LEN);
        errMsg=parserErrOk;
        break;
      
      case TAR_CMD_GETPCBID:
        //read
        //EEPROM_ReadString(buffer,eepromPCBID,PCB_ID_LEN);
        EEPROM_ReadString(buffer, PCBID, PCB_ID_LEN);
        buffer[PCB_ID_LEN]=0;
        printf_P(parserItems[TAR_CMD_GETPCBID].outFmt, buffer);
        break;

      case TAR_CMD_GETLOGO:
        //read logo
        strcpy_P(buffer, DEVICE_LOGO[EEPROM1.FirmLogo]);
        printf_P(parserItems[TAR_CMD_GETLOGO].outFmt, buffer);
        break;
    
      case TAR_CMD_SETLOGO:
        //set logo
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETLOGO].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=DEVICE_LOGO_LAST)
          errMsg=parserErrWrongValue;
        else
        {
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FirmLogo, (TByte *)&intValue0, sizeof(EEPROM1.FirmLogo));
          errMsg=parserErrOk;
        }
        break;
      
      case TAR_CMD_HELP:
        for (pos=0;pos<=TAR_CMD_PASS;pos++)
        {
          if (((!parserItems[pos].secure)||(secured))&&
              ((globalStatus.installedPrinter)||(!parserItems[pos].printerBrd)))
          {
            printf_P(parserItems[pos].itemStr);
            printf_P(parserHelpSeparator);
            printf_P(parserItems[pos].help);
            printf_P(parserCRLF);   
          }
        }
        break;
      
      case TAR_CMD_PRESET:
        //return to initial settings
        if (globalTestStatus.State==TEST_STOPPED)
        {
          EEPROM_LoadDefault();
          SND_Init();
          LED_SetBrightness(EEPROM1.LED);
          globalStatus.valueUpdate=true;
          errMsg=parserErrOk;
          break;
        }
        errMsg=parserErrTestRunning;
        break;
      
      case TAR_CMD_SETSND:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETSND].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        switch (intValue0)
        {
          case 0:
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&intValue0, sizeof(EEPROM1.Sound));
            SND_Off();
            errMsg=parserErrOk;
            break;
          case 1:
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&intValue0, sizeof(EEPROM1.Sound));
            SND_On();
            errMsg=parserErrOk;
            break;
          default:
            errMsg=parserErrWrongValue;
            break;
        }
        break;
      
      case TAR_CMD_SETPRNMODE:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETPRNMODE].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>PRINTER_MODE_ASK)
          errMsg=parserErrWrongValue;
        else
        {
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&intValue0, sizeof(EEPROM1.Print));
          errMsg=parserErrOk;
        }
        break;        
      
      case TAR_CMD_SETDTFORMAT:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETDTFORMAT].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=DFORMAT_MODE_LAST)
          errMsg=parserErrWrongValue;
        else
        {
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DFormat, (TByte *)&intValue0, sizeof(EEPROM1.DFormat));
          errMsg=parserErrOk;
        }
        break;        

      case TAR_CMD_LED:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_LED].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=LED_BRIGHTNESS_NUM)
        {
          errMsg=parserErrWrongValue;
          break;
        }
        if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_ALARM2))
        {  
          if (LED_SetBrightness((TByte)intValue0))
          {  
            EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&intValue0, sizeof(EEPROM1.LED));
            errMsg=parserErrOk;
          } else
          {
            errMsg=parserErrWrongValue;
          } 
        } else
        {
          errMsg=parserErrWrongValue;
        }  
        break;

      case TAR_CMD_GETTIME:
        //get time
        {
          TByte hour;
          
          if (!RTC_GetDateTime(&curDTime))
          {
            errMsg=parserPrinterError;
            break;
          }
          //printf_P(parserItems[TAR_CMD_GETTIME].outFmt,curDTime.Hour,curDTime.Minute,curDTime.Second);
          //if (eepromDFormat==DFORMAT_MODE_EUR)
          if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
            sprintf_P(buffer,paserTimeFormatEUR,curDTime.Hour,curDTime.Minute,curDTime.Second);
          else
          {
            hour = curDTime.Hour;
            if (!hour) hour=12;
              else
                if (hour>12) hour-=12;
            if (curDTime.Hour>12)
              sprintf_P(buffer,paserTimeFormatUSA_PM,hour,curDTime.Minute,curDTime.Second);
            else
              sprintf_P(buffer,paserTimeFormatUSA_AM,hour,curDTime.Minute,curDTime.Second);
          }
          printf_P(parserItems[TAR_CMD_GETTIME].outFmt,buffer);
          //errMsg=parserErrOk;
        }
        break;
        
      case TAR_CMD_GETDATE:
        //get date
        if (!RTC_GetDateTime(&curDTime))
        {
          errMsg=parserPrinterError;
          break;
        }
        //printf_P(parserItems[TAR_CMD_GETDATE].outFmt,curDTime.Day,curDTime.Month,curDTime.Year);
        //if (eepromDFormat==DFORMAT_MODE_EUR)
        if (EEPROM1.DFormat==DFORMAT_MODE_EUR)
          sprintf_P(buffer,paserDateFormatEUR,curDTime.Day,curDTime.Month,curDTime.Year);
        else
          sprintf_P(buffer,paserDateFormatUSA,curDTime.Month,curDTime.Day,curDTime.Year);
        printf_P(parserItems[TAR_CMD_GETDATE].outFmt,buffer);
        //errMsg=parserErrOk;
        break;
    
      case TAR_CMD_SETTIME:
        //set time
        {
            TTimeDate dt;
            int hour, minute;
            
            if (2!=sscanf_P(cmd,parserItems[TAR_CMD_SETTIME].inFmt,&hour,&minute))
            {
              errMsg=parserErrWrongParamList;
              break;
            }

            if (!RTC_GetDateTime(&dt))
            {
              errMsg=parserPrinterError;
              break;
            }
            dt.Hour=hour;
            dt.Minute=minute;
            if (!TIMEDATE_IsValidTime(dt))
            {
              errMsg=parserErrWrongValue;
              break;
            }
            if (RTC_SetDateTime(dt))
              errMsg=parserErrOk;
            else
              errMsg=parserPrinterError;
        }
        break;
        
      case TAR_CMD_SETDATE:
        //set date
        {
            TTimeDate dt;
            int day, month, year;
            
            if (3!=sscanf_P(cmd,parserItems[TAR_CMD_SETDATE].inFmt,&day,&month,&year))
            {
              errMsg=parserErrWrongParamList;
              break;
            }

            if (!RTC_GetDateTime(&dt))
            {
              errMsg=parserPrinterError;
              break;
            }
            dt.Day=day;
            dt.Month=month;
            dt.Year=year;
            if (!TIMEDATE_IsValidDate(dt))
            {
              errMsg=parserErrWrongValue;
              break;
            }
            if (RTC_SetDateTime(dt))
              errMsg=parserErrOk;
            else
              errMsg=parserPrinterError;
        }
        break;

      case TAR_CMD_SETTESTTIME:
        if (globalTestStatus.State!=TEST_STOPPED)
        {
          errMsg=parserErrTestRunning;
          break;
        }
        {
          int hour, minute, second;
          TWord value;
          if (3!=sscanf_P(cmd,parserItems[TAR_CMD_SETTESTTIME].inFmt,&hour,&minute,&second))
          {
            errMsg=parserErrWrongParamList;
            break;
          }
          if ((minute>59)||(second>59)||(hour>9))
          {
            errMsg=parserErrWrongValue;
            break;
          }
          value=hour*3600+minute*60+second;
          if (value>TAR_TIME_MAX)
          {
            errMsg=parserErrWrongValue;
            break;
          }  
          EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestTime, (TByte *)&value, sizeof(EEPROM1.TestTime));
          globalStatus.valueUpdate=true;
        }
        errMsg=parserErrOk;
        break;
        
      case TAR_CMD_SETTESTSTROKES:
        if (globalTestStatus.State!=TEST_STOPPED)
        {
          errMsg=parserErrTestRunning;
          break;
        }

        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETTESTSTROKES].inFmt,&intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>SVM_REV_MAX)
        {
          errMsg=parserErrWrongValue;
          break;
        }        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestRevolutions, (TByte *)&intValue0, sizeof(EEPROM1.TestRevolutions));
        globalStatus.valueUpdate=true;
        errMsg=parserErrOk;
        break;

      case TAR_CMD_SETSPEED:
        if (globalTestStatus.State!=TEST_STOPPED)
        {
          errMsg=parserErrTestRunning;
          break;
        }
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETSPEED].inFmt,&intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (EEPROM1.DevType!=DEVICE_TYPE_TAR)
        {
          errMsg=parserErrOk;
          break;
        }
        if ((intValue0>TAR_SPEED_MAX)||(intValue0<TAR_SPEED_MIN))
        {
          errMsg=parserErrWrongValue;
          break;
        }        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&intValue0, sizeof(EEPROM1.Speed));
        globalStatus.valueUpdate=true;
        errMsg=parserErrOk;
        break;

      case TAR_CMD_SETTESTMODE:
        if (globalTestStatus.State!=TEST_STOPPED)
        {
          errMsg=parserErrTestRunning;
          break;
        }

        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETTESTMODE].inFmt,&intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        if (intValue0>=TIME_UNIT_LAST)
        {
          errMsg=parserErrWrongValue;
          break;
        }        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&intValue0, sizeof(EEPROM1.TimeUnits));
        globalTestStatus.Mode=intValue0;
        globalStatus.valueUpdate=true;
        errMsg=parserErrOk;
        break;
        
      case TAR_CMD_GETTESTTIME:
        //get test time
        {
          TByte sec, min, hour;
          
          //sec=eepromTestTime%60;
          sec=EEPROM1.TestTime%60;
          //min=(eepromTestTime/60)%60;
          min=(EEPROM1.TestTime/60)%60;
          //hour=(eepromTestTime/(60*60));
          hour=(EEPROM1.TestTime/(60*60));
          printf_P(parserItems[TAR_CMD_GETTESTTIME].outFmt, hour, min, sec);
        }
        break;

      case TAR_CMD_GETTESTSTROKES:
        //get test strokes
        //printf_P(parserItems[TAR_CMD_GETTESTSTROKES].outFmt,eepromTestRevolutions);
        printf_P(parserItems[TAR_CMD_GETTESTSTROKES].outFmt,EEPROM1.TestRevolutions);
        break;

      case TAR_CMD_GETCURTIME:
        //get remaining time
        {
          TByte sec, min, hour;
          
          if (globalTestStatus.State==TEST_STOPPED)
          {
            errMsg=parserErrTestNotStarted;
            break;
          }
          //if (eepromTimeUnits!=TIME_UNIT_TIME)
          if (EEPROM1.TimeUnits!=TIME_UNIT_TIME)
          {
            errMsg=parserErrTestRunning;
            break;
          }
          sec=globalTestStatus.Value%60;
          min=(globalTestStatus.Value/60)%60;
          hour=(globalTestStatus.Value/(60*60));
          printf_P(parserItems[TAR_CMD_GETCURTIME].outFmt, hour, min, sec);
        }
        break;
    
      case TAR_CMD_GETCURSTROKES:
        //get remaining strokes
        if (globalTestStatus.State==TEST_STOPPED)
        {
          errMsg=parserErrTestNotStarted;
          break;
        }
        //if (eepromTimeUnits==TIME_UNIT_TIME)
        if (EEPROM1.TimeUnits==TIME_UNIT_TIME)
        {
          errMsg=parserErrTestRunning;
          break;
        }
        printf_P(parserItems[TAR_CMD_GETCURSTROKES].outFmt, globalTestStatus.Value);
        break;

      case TAR_CMD_GETSPEED:
        fltValue0=(TReal32)MOTOR_GetCurrSpeed()/10;
        printf_P(parserItems[TAR_CMD_GETSPEED].outFmt, fltValue0);
        break;

      case TAR_CMD_TESTSTART:
        //start test
        globalStatus.testStart=true;
        errMsg=parserErrOk;
        break;
      
      case TAR_CMD_TESTSTOP:
        //stop test
        globalStatus.testStart=false;
        errMsg=parserErrOk;
        break;
        
      case TAR_CMD_PPR:
        intValue0=EEPROM1.PulsesPerRotat;
        printf_P(parserItems[TAR_CMD_PPR].outFmt,intValue0);
        break;
        
      case TAR_CMD_SETPPR:
        if (1!=sscanf_P(cmd,parserItems[TAR_CMD_SETPPR].inFmt,&intValue0))
          {
            errMsg=parserErrWrongParamList;
            break;
          }
        if ((intValue0>1000)||(intValue0<100))
          {
            errMsg=parserErrWrongValue;
            break;
          }        
        //eepromPulsesPerRotat=intValue0;
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PulsesPerRotat, (TByte *)&intValue0, sizeof(EEPROM1.PulsesPerRotat));
        MOTOR_Init();
        errMsg=parserErrOk;
        break;

      case TAR_CMD_PASS:
        if (1!=sscanf_P(cmd, parserItems[TAR_CMD_PASS].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        //if (intValue0!=eepromPassword)
        if (intValue0!=EEPROM1.Password)
        {
          errMsg=parserErrWrongPassword;
          break;
        }
        secured=true;
        errMsg=parserErrOk;
        break;

      case TAR_CMD_GETLASTKEY:
        sprintf_P(buffer, BUTTON_NAME[lastPressedKey]);
        printf_P(parserItems[TAR_CMD_GETLASTKEY].outFmt, buffer);
        //errMsg=parserErrOk;
        break;
        
      case TAR_CMD_UPFW:
        printf_P(parserUpdateOk);
        LED_SwitchOff();
        __delay_cycles(XTAL/2);
        __disable_interrupt();
        //( *( void( * )()) UPDATER_ADDR )();
        //asm("JMP 0x1FD00");
        Update();
        break;

      case TAR_CMD_UPBUS:
        //move to updater subroutine for RS485 bus
        if (1!=sscanf_P(cmd, parserItems[TAR_CMD_UPBUS].inFmt, &intValue0))
        {
          errMsg=parserErrWrongParamList;
          break;
        }
        LED_Show(false);
        __delay_cycles(XTAL/10);
        UPDATER_BusRun(intValue0);         
        LED_Show(true);
        break;
        
      case TAR_CMD_EKEY:
        {
          TByte len;
          
          len=0;
          switch (EKEY_Parser(cmd, buffer, &len))
          {
            case EKEY_ANSWER_PRINT: 
              //printf(buffer);
              USB_SendString(buffer, len);
              break;
            case EKEY_ANSWER_OK:
              errMsg=parserErrOk;
            break;
            case EKEY_ANSWER_ERROR:
              errMsg=parserErrWrongValue;  
              break;
          }    
        }  
        break;

      case TAR_CMD_NULL:
        break;

      default:
        errMsg=parserErrWrongCommand;
        break;
    }
  }
  if (errMsg)
    printf_P(errMsg);
  printf_P(parserMsgPrompt);
  memset(USB_IBuffer, 0, USB_BUFFER_SIZE);
} 
