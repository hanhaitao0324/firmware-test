#ifndef _CONST_H
#define _CONST_H

//max length of the serial number string
#define SERIAL_NUM_MAX_LEN	  (6)
#define MODE_OUT_LEN	        (16)
//max length of the PCB ID string
#define PCB_ID_MAX_LEN		(10)
//max length of firm logo
#define LOGO_MAX_LEN		(10)

//maximal control time value
#define MIN_TIME_VALUE          (0)
#define MAX_TIME_VALUE          (3600L*99+59*60)

// start beeping before end
#define START_BEEPING_VALUE     (60)

#define PASSWORD                (1234L)

//pumps ratio
#define PUMP0_MAX_RATIO         (1000)

//------------------------------------------------------------------------------
// Device
//------------------------------------------------------------------------------
enum DEVICE_TYPES
{ 
  DEVICE_VDT1,
  DEVICE_LAST
};

enum DEVICE_LOGOS
{
  DEVICE_LOGO_ERWEKA,
  DEVICE_LOGO_CALEVA,
  DEVICE_LOGO_TABTEST,
  DEVICE_LOGO_QTEST,
  DEVICE_LOGO_LAST
};

enum SOUND_MODE
{
  SOUND_MODE_OFF,
  SOUND_MODE_ON,
  SOUND_MODE_LAST
};

enum PRINTER_MODE
{
  PRINTER_MODE_OFF,
  PRINTER_MODE_AUTO,
  PRINTER_MODE_ASK,
  PRINTER_MODE_LAST
};

enum PRINTER_TYPE
{
  PRINTER_TYPE_NORM,
  PRINTER_TYPE_PORT,
  PRINTER_TYPE_LAST
};

enum LANGUAGE_MODE
{ 
  LANGUAGE_MODE_GER,
  LANGUAGE_MODE_ENG,
  LANGUAGE_MODE_LAST
};

enum DFORMAT_MODE
{ 
  DFORMAT_MODE_EUR,
  DFORMAT_MODE_USA,
	DFORMAT_MODE_CHN,
  DFORMAT_MODE_LAST
};

enum STATUS
{
  STATUS_STANDBY,
  STATUS_WORK,
  STATUS_ALARM,
  STATUS_SETUP,
  STATUS_FOOT
};

//------------------------------------------------------------------------------
// Control
//------------------------------------------------------------------------------

#endif
