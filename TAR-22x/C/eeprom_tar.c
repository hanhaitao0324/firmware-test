#include "eeprom_utils.h"
#include "eeprom.h"
#include "crc.h"    

__flash _EEPROM EEPROM_Default =
{
  "000000\x0",          //serial number
  //"ERWEKA\x0",          //firm logo
  DEVICE_LOGO_ERWEKA,   //firm logo
  1234,                 //engineering password
  120,                  //pulses per rotation
  0,                    //usage counter
  DEVICE_TYPE_TAR,      //device type (TA/TAR)
  DEVICE_MODE_120,      //device mode (120-220-420)
  0,                    //Ekey presence
  PCB_UNKNOWN,          //PCB type
  
  // 使用备用空间放置管理密码和日期修改密码，v2.20，Gavin，2019-08-08 星期四 09:27:38
  //{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                        //place for adding new factory values (20 bytes)
  {0,0,0,0,0,0,0,0,0,0,0},
                        //place for adding new factory values (15 bytes)
  9999, 								// 管理密码, v2.20, Gavin, 2019-08-08 星期四 15:54:55
  9999, 								// 管理密码校验码，v2.20，Gavin，2019-08-15 星期四 10:05:48
  9999, 								// 日期时间修改密码, v2.20, Gavin, 2019-08-08 星期四 15:54:55
  9999, 								// 日期时间密码校验码，v2.20，Gavin, 2019-08-15 星期四 10:06:12
  
  LED_BRIGHTNESS_5,     //LED brightness
  TIME_UNIT_TIME,       //units mode
  2,                    //backward rotations
  EMPTY_MODE_AUTO,      //emotying mode
  4*60,                 //time
  100,                  //rotations
  25,                   //speed
  SOUND_MODE_ON,        //sound settings
  PRINTER_MODE_AUTO,    //printer settings
  DFORMAT_MODE_EUR,     //date/time format
  30,                   //time for starting beeping
  PRINTER_TYPE_NORM,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                        //place for adding new values (20 bytes)
  0                     //CRC16
};

__eeprom _EEPROM EEPROM1;
__eeprom _EEPROM EEPROM2;

__root __eeprom char PCBID[PCB_ID_LEN+1] @ PCBID_ADDR;

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
// EEPROM_LoadDefault - load default values from Flash to EEPROM
//------------------------------------------------------------------------------
void EEPROM_LoadDefault()
{
  TWord CRC16;
  _EEPROM EEPROM_Copy;
  
  EEPROM_ReadString((char *)&EEPROM_Copy, (char __eeprom *)&EEPROM1, sizeof(_EEPROM));
  memcpy_P((TByte *)&EEPROM_Copy.LED,(TByte __flash *)&EEPROM_Default.LED,(TWord)((TWord)&EEPROM_Default.CRC16-(TWord)&EEPROM_Default.LED));
  CRC16=CalcCRC16((TByte *)&EEPROM_Copy,sizeof(_EEPROM)-2);
  EEPROM_Copy.CRC16=CRC16;
  EEPROM_WriteBlock_R((TByte __eeprom *)&EEPROM1,(TByte *)&EEPROM_Copy,sizeof(_EEPROM));
  EEPROM_WriteBlock((TByte __eeprom *)&EEPROM2,(TByte __eeprom *)&EEPROM1,sizeof(_EEPROM));
}

//------------------------------------------------------------------------------
// EEPROM_Restore - restore default values from Flash to EEPROM
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
// EEPROM_CheckIntegrity - check integrity of EEPROM and try to restore it
//------------------------------------------------------------------------------
TBool EEPROM_CheckIntegrity()
{
  TWord CRC16, CRC16_1;

  CRC16=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM1,sizeof(_EEPROM)-2);
  CRC16_1=EEPROM_CalcCRC16((TByte __eeprom *)&EEPROM2,sizeof(_EEPROM)-2);
  if(CRC16 != EEPROM1.CRC16)
  {
    if(CRC16_1 != EEPROM2.CRC16) return false;
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
// EEPROM_Check - check content of EEPROM (for new firmware versions)
//------------------------------------------------------------------------------
void EEPROM_Check(void)
{
}

