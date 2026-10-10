#include "eeprom.h"
#include "crc.h"

__flash _EEPROM EEPROM_Default = 
{
  LED_BRIGHTNESS_8,     //LED brightness
  "000000",             //serial number of the device
  DEVICE_LOGO_ERWEKA,   //firm logo
  DEVICE_DT520,         //device type
  MOTOR_IMPULSE_REV,    //pulses per rotation
  // Gavin 20150617-----------
  //8,                    //number of vessels
  // Gavin 20131224-----------
  6,                    //number of vessels
  // Gavin 20150617-----------
  //8,                    //number of vessels
  // Gavin 20131224-----------
  6,                    //number of sensors
  0,                    //time of operation counter
  1234,                 //settings password
  0,                    //Ekey presence
  30,                   //stepper step in mm*10
  {0,0,0,0,0,0},        //time of heater calibration
  {0,0,0,0,0,0},        //time of Pt-sensor calibration
  {0,0,0,0,0,0},        //time of sampler calibration
  {0,0,0,0,0,0},        //time of service
  PORT_MODE_USB,        //port settings (USB or COM)
  MOTORERROR_MODE_ON,   //motor current error mode (on/off)
  PCB_UNKNOWN,          //PCB type
  
  // 使用备用空间放置管理密码和日期修改密码，v2.10，Gavin，2020-09-03 星期四 11:52:13
  //{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                        //place for adding new values, that shouldn't be changed by user
	{0,0,0,0,0,0,0,0,0}, 	//place for adding new values, that shouldn't be changed by user
	9999, 								// 管理密码, v2.10, Gavin, 2020-09-03 星期四 11:52:19
  9999, 								// 管理密码校验码
  9999, 								// 日期时间修改密码, v2.10, 2020-09-03 星期四 11:52:24
  9999, 								// 日期时间密码校验码
  
  3600L,                //test time
  100,                  //motor speed
  37.0,                 //test temperature
  {2.09641e-3},         //sensor coefficient K
  {-0.32549},           //sensor coefficient B
  {0,110,290,600},      //depth of stepper for volume (value*10)
  {320, 370, 400},      //temperature checkpoints (temp*10)
  150,                  //minimal temperature (temp*10)
  500,                  //maximal temperature (temp*10)
  0,                    //temperature offset (value*10)
  SOUND_MODE_ON,        //sound settings
  ALARM_MODE_ON,        //alarm settings
  METH_MODE_PADL,       //stirrer methode
  VOLUME_MODE_900,      //volume value for autosampler
  0,                    //filter offset
  
  //HEAD_MODE_LOW,        //head option
  // Gavin 20131228---------------------
  HEAD_MODE_HIGH,
  
  SAMPLER_MODE_OFF,     //sampler mode
  PRINTER_MODE_ASK,     //print settings
  DFORMAT_MODE_EUR,     //date/time format settings
  TESTCNT_MODE_DOWN,    //counting test run-time up or down
  //MOTOR_MODE_STOP,      //stop or not motor after test
  MOTOR_MODE_OFF,      // Gavin, v2.03, 2021-03-09 星期二 12:54:59
  START_BEEPING_VALUE,  //start beeping before end value
  MIN_UPDATE_INTERVAL,  //temperature update interval
  MIN_REPORT_INTERVAL,  //report interval
  60,                   //lines per page
  LANGUAGE_MODE_ENGLISH,//report language
  //PTYPE_MODE_NORM, 											// Gavin 2015/11/27 9:39:13
  {115, 0, 38, 159},    //method offset in mm*10
  {
  	0, 									// 第一个字节用作打印机选择，0：NORM；1：PORT；
  	'0','0','0','0','0','0', 				// 第二个字节到第七个字节为加热器序列号
  	0,0,0
  }, 										//place for adding new values
  1, 										// INFI，运转时间
  100, 									// INFI, 运转速度
  {115, 0, 38, 159, 0}, // method offset in mm*10
  0                     //CRC16
};

__flash _ECalibr ECalibr_Default = 
{
  {2.09641e-3},       //sensor coefficient K
  {-0.32549},         //sensor coefficient B
  0                   //CRC16
};
        
__eeprom _EEPROM EEPROM1;
__eeprom _EEPROM EEPROM2;

__eeprom _ECalibr ECalibr1;
__eeprom _ECalibr ECalibr2;

__root __eeprom char PCBID[PCB_ID_MAX_LEN] @PCBID_ADDR;// = "000000";

//------------------------------------------------------------------------------
// EEPROM_ReadString - read a string from the EEPROM
//------------------------------------------------------------------------------
void EEPROM_ReadString(char *dest, char __eeprom *src, TByte len)
{
  TByte pos;
    
  pos=0;
  while (pos<len)
  {
    dest[pos]=src[pos];  
    pos++;
  }
}

//------------------------------------------------------------------------------
// EEPROM_WriteString - write a string to the EEPROM
//------------------------------------------------------------------------------
void EEPROM_WriteString(char __eeprom *dest, char *src, TByte len)
{
  TByte pos;
    
  pos=0;
  while (pos<len)
  {
    dest[pos]=src[pos];  
    pos++;
  }
}

//------------------------------------------------------------------------------
TWord CalcCRC16(TByte *src, TByte len)
{
  TWord rpar=0xffff;
  TByte pos;
  TByte data;

  pos=0;
  while (pos<len)
  {
    data=src[pos];
    pos++;
    rpar=CalcCRC16_Macro(data,rpar);
  }
  return rpar;
}

//------------------------------------------------------------------------------
TWord EEPROM_CalcCRC16(TByte __eeprom *src, TByte len)
{
  TWord rpar=0xffff;
  TByte pos;
  TByte data;

  pos=0;
  while (pos<len)
  {
    data=src[pos];
    pos++;
    rpar=CalcCRC16_Macro(data,rpar);
  }
  return rpar;
}

//------------------------------------------------------------------------------
void EEPROM_WriteBlock(TByte __eeprom *dest, TByte __eeprom *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}

//------------------------------------------------------------------------------
void EEPROM_WriteBlock_P(TByte __eeprom *dest, TByte __flash *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}

//------------------------------------------------------------------------------
void EEPROM_WriteBlock_R(TByte __eeprom *dest, TByte *src, TByte len)
{
  TByte pos, ix;
    
  pos=0;
  ix=0;
  while (pos<len)
  {
    if (dest[pos]!=src[pos])
      do {
        dest[pos]=src[pos];  
        ix++;
      } while ((dest[pos]!=src[pos])&&(ix<EEPROM_ERRORS));
    pos++;
  }
}

//------------------------------------------------------------------------------
TBool EEPROM_WriteVar(TByte __eeprom *dest, TByte *src, TByte len)
{
  TWord CRC16;
  
  EEPROM_WriteBlock_R(dest, src, len);
  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM1,sizeof(_EEPROM)-2);
  EEPROM_WriteBlock_R((TByte __eeprom *)&EEPROM1.CRC16,(TByte *)&CRC16,2);
  EEPROM_WriteBlock((TByte __eeprom *)&EEPROM2,(TByte __eeprom *)&EEPROM1,sizeof(_EEPROM));
  return true;
}

//------------------------------------------------------------------------------
TBool ECalibr_Write(void)
{
  TWord CRC16;
  
  EEPROM_WriteBlock((TByte __eeprom *)&ECalibr1,(TByte __eeprom *)&EEPROM1.TempK[0], sizeof(_ECalibr)-2);
  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&ECalibr1,sizeof(_ECalibr)-2);
  EEPROM_WriteBlock_R((TByte __eeprom *)&ECalibr1.CRC16,(TByte *)&CRC16,2);
  EEPROM_WriteBlock((TByte __eeprom *)&ECalibr2,(TByte __eeprom *)&ECalibr1,sizeof(_ECalibr));
  return true;
}

//------------------------------------------------------------------------------
void EEPROM_Restore()
{
  TWord CRC16;

  EEPROM_WriteBlock_P((TByte __eeprom *)&EEPROM1,(TByte __flash *)&EEPROM_Default,sizeof(_EEPROM)-2);
  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM1,sizeof(_EEPROM)-2);
  EEPROM_WriteBlock_R((TByte __eeprom *)&EEPROM1.CRC16,(TByte *)&CRC16,2);
  EEPROM_WriteBlock((TByte __eeprom *)&EEPROM2,(TByte __eeprom *)&EEPROM1,sizeof(_EEPROM));
}

//------------------------------------------------------------------------------
void EEPROM_LoadDefault()
{
  TWord CRC16;
  _EEPROM EEPROM_Copy;
  
  EEPROM_ReadString((char *)&EEPROM_Copy, (char __eeprom *)&EEPROM1, sizeof(_EEPROM));
  memcpy_P((TByte *)&EEPROM_Copy.Time,(TByte __flash *)&EEPROM_Default.Time,(TWord)((TWord)&EEPROM_Default.CRC16-(TWord)&EEPROM_Default.Time));
  CRC16=CalcCRC16((TByte *)&EEPROM_Copy,sizeof(_EEPROM)-2);
  EEPROM_Copy.CRC16=CRC16;
  EEPROM_WriteBlock_R((TByte __eeprom *)&EEPROM1,(TByte *)&EEPROM_Copy,sizeof(_EEPROM));
  EEPROM_WriteBlock((TByte __eeprom *)&EEPROM2,(TByte __eeprom *)&EEPROM1,sizeof(_EEPROM));
}

//------------------------------------------------------------------------------
void ECalibr_Restore()
{
  TWord CRC16;

  EEPROM_WriteBlock_P((TByte __eeprom *)&ECalibr1,(TByte __flash *)&ECalibr_Default,sizeof(_ECalibr)-2);
  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&ECalibr1,sizeof(_ECalibr)-2);
  EEPROM_WriteBlock_R((TByte __eeprom *)&ECalibr1.CRC16,(TByte *)&CRC16,2);
  EEPROM_WriteBlock((TByte __eeprom *)&ECalibr2,(TByte __eeprom *)&ECalibr1,sizeof(_ECalibr));
}

//------------------------------------------------------------------------------
// EEPROM_CheckIntegrity - check integrity of EEPROM and try to restore it
//------------------------------------------------------------------------------
TBool EEPROM_CheckIntegrity()
{
  TWord CRC16, CRC16_1;

  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM1,sizeof(_EEPROM)-2);
  CRC16_1=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM2,sizeof(_EEPROM)-2);
  
  if(CRC16 != EEPROM1.CRC16)
  {
    if(CRC16_1 != EEPROM2.CRC16)
    	return false;
    //Flash_EraseBlock(&CMOS);
    EEPROM_WriteBlock((TByte __eeprom *)&EEPROM1,(TByte __eeprom *)&EEPROM2,sizeof(_EEPROM));
  }
  
  if(CRC16_1 != EEPROM2.CRC16)
  {
    //Flash_EraseBlock(&CMOS_Copy);
    EEPROM_WriteBlock((TByte __eeprom *)&EEPROM2,(TByte __eeprom *)&EEPROM1,sizeof(_EEPROM));
  }
  
  return true;
}

//------------------------------------------------------------------------------
// ECalibr_CheckIntegrity - check Calibration EEPROM and try to restore it
//------------------------------------------------------------------------------
TBool ECalibr_CheckIntegrity()
{
  TWord CRC16, CRC16_1;

  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&ECalibr1,sizeof(_ECalibr)-2);
  CRC16_1=EEPROM_CalcCRC16((TByte __eeprom *)&ECalibr2,sizeof(_ECalibr)-2);
  
  if(CRC16 != ECalibr1.CRC16)
  {
    if(CRC16_1 != ECalibr2.CRC16)
    	return false;
    
    EEPROM_WriteBlock((TByte __eeprom *)&ECalibr1,(TByte __eeprom *)&ECalibr2,sizeof(_ECalibr));
  }
  
  if(CRC16_1 != ECalibr2.CRC16)
  {
    EEPROM_WriteBlock((TByte __eeprom *)&ECalibr2,(TByte __eeprom *)&ECalibr1,sizeof(_ECalibr));
  }
  
  return true;
}

//------------------------------------------------------------------------------
// EEPROM_Check - check content of EEPROM (for new firmware versions)
//------------------------------------------------------------------------------
void EEPROM_Check(void)
{
	
}
