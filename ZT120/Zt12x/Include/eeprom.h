#ifndef _EEPROM_H
  #define _EEPROM_H

#include "adc.h"
#include "const.h"
#include "defines.h"
#include "platform.h"
#include "temp.h"
#include "timedate.h"

#define EEPROM_ERRORS   (3)

typedef struct
{
  TByte     LED;
  char      SerialNumber[SERIAL_NUM_MAX_LEN+1];
  TByte     FirmLogo;
  TByte     DeviceType;
  TWord     PulsesPerRotat;
  TByte     NumVessels;
  TByte     NumSensors;
  TDWord    UsageCounter;
  TWord     Password;
  TByte     Ekey;
  TByte     MotorStepMM;
  TTimeDate timeHeaterCal;
  TTimeDate timeSensorCal;
  TTimeDate timeSamplerCal;
  TTimeDate timeService;
  TByte     Port;
  TByte     MotorError;
  TByte     PCBType;

  // 使用备用空间放置管理密码和日期修改密码，v1.03，HHT，2024-06-14
  //TByte     Empty1[17];
  TByte 		Empty1[9];
  TWord 		MGPassword;
  TWord 		MGPWC;
  TWord 		DTPassword;
  TWord 		DTPWC;
  
  TDWord    Time;
  TWord     Speed;
  TReal32   Temp;
  TReal32   TempK[ADC_NUM_CHANNELS];
  TReal32   TempB[ADC_NUM_CHANNELS];
  TWord     VolumeDepth[DEPTH_MODE_LAST];
  TWord     CheckTemp[TEMP_CHECK_POINTS];
  TWord     MinTemp;
  TWord     MaxTemp;
  TsWord    TempOffset[ADC_NUM_CHANNELS];
  TByte     Sound;
  TByte     Alarm;
  TByte     Method;
  TByte     Volume;
  signed char FilterOffset;
  TByte     Head;
  TByte     Sams;
  TByte     Print;
  TByte     DFormat;
  TByte     TestCount;
  TByte     MotorStop;
  TByte     HeatFlag;
  TWord     StartBeep;
  TByte     TempUpdateInterval;
  TByte     ReportInterval;
  TByte     ReportLines;
  TByte     ReportLanguage;
  //TByte     ReportPType; 								// Gavin 2015/11/27 9:27:53
  TWord     MethodOffset[METH_MODE_LAST];
  TByte     Empty2[22]; 								// Empty2[0] = ReportPType; 2015/12/14 14:09:14
  TWord     CRC16;
} _EEPROM;

#define ReportPType Empty2[0]

typedef struct
{
  TReal32   TempK[ADC_NUM_CHANNELS];
  TReal32   TempB[ADC_NUM_CHANNELS];
  TWord     CRC16;
} _ECalibr;

//typedef struct _EEPROM TEEPROM;

extern __eeprom _EEPROM EEPROM1;
extern __eeprom _EEPROM EEPROM2;

extern __eeprom _ECalibr ECalibr1;
extern __eeprom _ECalibr ECalibr2;

extern __no_init __eeprom char PCBID[PCB_ID_MAX_LEN]@PCBID_ADDR;

// EEPROM_ReadString - read a string from the EEPROM
extern void EEPROM_ReadString(char *dest, char __eeprom *src,TByte length);
// EEPROM_WriteString - write a string to the EEPROM
extern void EEPROM_WriteString(char __eeprom *dest, char *src,TByte length);
extern TBool EEPROM_WriteVar(TByte __eeprom *dest, TByte *src, TByte len);
extern TBool ECalibr_Write(void);
extern void EEPROM_Restore(void);
extern void EEPROM_LoadDefault(void);
extern void ECalibr_Restore(void);
extern TBool EEPROM_CheckIntegrity(void);
extern TBool ECalibr_CheckIntegrity(void);
extern void EEPROM_Check(void);

#endif
