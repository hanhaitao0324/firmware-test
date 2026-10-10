#ifndef _EEPROM_H
  #define _EEPROM_H

#include "const.h"
#include "defines.h"
#include "platform.h"
#include "eeprom_utils.h"

#ifdef DEVICE_SVM
typedef struct
{
  char      Serial[SERIAL_NUMBER_LEN+1];
  //char      FirmLogo[LOGO_MAX_LEN+1];
  TByte     FirmLogo;
  TWord     Password;
  TWord     PulsesPerRotat;
  TDWord    UsageCounter;
  TByte     DevType;
  TByte     DevMode;
  TByte     Ekey;
  TByte     PCBType;
  TByte     Empty1[19];
  TByte     LED;
  TByte     TimeUnits;
  TWord     NumRevRevolutions;
  TWord     TestTime;
  TWord     TestRevolutions;
  TWord     Speed;
  TByte     Sound;
  TByte     Print;
  TByte     DFormat;
  TByte     StartBeep;
  TByte     PrinterType;
  TByte     Empty2[19];
  TWord     CRC16;
} _EEPROM;
#endif

#ifdef DEVICE_TAR
typedef struct
{
  char      Serial[SERIAL_NUMBER_LEN+1];
  //char      FirmLogo[LOGO_MAX_LEN+1];
  TByte     FirmLogo;
  TWord     Password;
  TWord     PulsesPerRotat;
  TDWord    UsageCounter;
  TByte     DevType;        // TA/TAR
  TByte     DevMode;        //120/220/420
  TByte     Ekey;
  TByte     PCBType;
  
  // 使用备用空间放置管理密码和日期修改密码，v2.20，Gavin，2019-08-08 星期四 09:27:38
  //TByte     Empty1[19];
  TByte     Empty1[11];
  TWord 		MGPassword;
  TWord 		MGPWC; 											// 管理密码校验码，Gavin，2019-08-15 星期四 10:03:01
  TWord 		DTPassword;
  TWord 		DTPWC; 											// 日期时间密码校验码，Gavin，2019-08-15 星期四 10:03:47
  
  TByte     LED;
  TByte     TimeUnits;
  TWord     NumRevRevolutions;
  TByte     EmptyMode;
  TWord     TestTime;
  TWord     TestRevolutions;
  TWord     Speed;
  TByte     Sound;
  TByte     Print;
  TByte     DFormat;
  TByte     StartBeep;
  TByte     PrinterType;
  TByte     Empty2[19];
  TWord     CRC16;
} _EEPROM;
#endif

extern __eeprom _EEPROM EEPROM1;
extern __eeprom _EEPROM EEPROM2;

extern __no_init __eeprom char PCBID[PCB_ID_LEN+1];

extern TBool EEPROM_WriteVar(TByte __eeprom *dest, TByte *src, TByte len);
extern void EEPROM_LoadDefault(void);
extern void EEPROM_Restore(void);
extern TBool EEPROM_CheckIntegrity(void);
extern void EEPROM_Check(void);

#endif
