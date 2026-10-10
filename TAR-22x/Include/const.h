#ifndef _CONST_H
#define _CONST_H

enum PCB_TYPES
{
  PCB_UNKNOWN,
  PCB_OLD,
  PCB_NEW,
  PCB_DM13H, 				// Gavin，2021-12-22 星期三 13:27:15
  PCB_TYPE_LAST
};

enum DEVICE_LOGOS
{
  DEVICE_LOGO_ERWEKA,
  DEVICE_LOGO_CALEVA,
  DEVICE_LOGO_TABTEST,
  DEVICE_LOGO_QTEST,
  DEVICE_LOGO_LAST
};

#ifdef DEVICE_SVM
enum DEVICE_TYPES
{
  DEVICE_TYPE_SVM,
  DEVICE_TYPE_TDT,
  DEVICE_TYPE_LAST
};

enum DEVICE_MODES
{
  DEVICE_MODE_121,
  DEVICE_MODE_122,
  DEVICE_MODE_221,
  DEVICE_MODE_222,
  DEVICE_MODE_223,
  DEVICE_MODE_LAST
};
#endif

#ifdef DEVICE_TAR
enum DEVICE_TYPES
{
  DEVICE_TYPE_TA,
  DEVICE_TYPE_TAR,
  DEVICE_TYPE_FT,
  DEVICE_TYPE_LAST
};

enum DEVICE_MODES
{
  DEVICE_MODE_120,
  DEVICE_MODE_220,
  DEVICE_MODE_420,
  DEVICE_MODE_LAST
};
#endif

enum TIME_UNITS
{
  TIME_UNIT_TIME,
  TIME_UNIT_REVOLUTIONS,
  TIME_UNIT_LAST
};

enum EMPTIYING_MODE
{
  EMPTY_MODE_AUTO,
  EMPTY_MODE_MANUAL,
  EMPTY_MODE_LAST
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

enum DTIME_MODE
{
  DTIME_MODE_TIME,
  DTIME_MODE_DAY,
  DTIME_MODE_YEAR,
  DTIME_MODE_FORMAT,
  DTIME_MODE_VER,
  DTIME_MODE_LAST
};

// 增加PWMG的子菜单，v2.20，Gavin，2019-08-07 星期三 15:38:33
enum PWMG_MODE
{
	PWMG_MODE_PWST,
	PWMG_MODE_DATE,
	PWMG_MODE_LAST
};

enum DFORMAT_MODE
{
  DFORMAT_MODE_EUR,
  DFORMAT_MODE_USA,
  DFORMAT_MODE_LAST
};

enum OQ_MODE
{
  OQ_MODE_HGHT,
  OQ_MODE_SPM,
  OQ_MODE_RTIM,
  OQ_MODE_OQPR,
  OQ_MODE_LAST
};

enum TEST_STATE
{
  TEST_STOPPED,
  TEST_RUNNING,
  TEST_PAUSED,
  TEST_WAITING
};

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------
enum LED_DISPLAYS
{
  LED_DISPLAY_LEFT,
  LED_DISPLAY_RIGHT
};

#define LED_NUM_DIGITS	(4)

enum LED_BRIGHTNESS
{
  LED_BRIGHTNESS_0,
  LED_BRIGHTNESS_1,
  LED_BRIGHTNESS_2,
  LED_BRIGHTNESS_3,
  LED_BRIGHTNESS_4,
  LED_BRIGHTNESS_5,
  LED_BRIGHTNESS_6,
  LED_BRIGHTNESS_7,
  LED_BRIGHTNESS_8,
  LED_BRIGHTNESS_9,
  LED_BRIGHTNESS_10,
  LED_BRIGHTNESS_NUM
};

//------------------------------------------------------------------------------
// KBD
//------------------------------------------------------------------------------
enum KBD_CODES
{
  KBD_CODE_NOKEY,
  KBD_CODE_START,
  KBD_CODE_STOP,
  KBD_CODE_SLOW_UP,
  KBD_CODE_SLOW_DOWN,
  KBD_CODE_FAST_UP,
  KBD_CODE_FAST_DOWN,
  KBD_CODE_LAST
};

//------------------------------------------------------------------------------
// SVM settings
//------------------------------------------------------------------------------
#define SVM_REV_MIN	        (0)
#define SVM_REV_MAX	        (9999L)
#define SVM_REV_BEEP            (50)

#define SVM_TIME_MIN		(0)
#define SVM_TIME_MAX		(10*60*60L-1) // 9h59m59s
#define SVM_TIME_BEEP           (30)

#define SVM_TIME_CONVERT        (60*60)

#define SVM_CHANGE_STEP	        (1)
#define SVM_CHANGE_STEP_TIME    (60)

#define SVM_SPEED_MODE1		(300) //300
#define SVM_SPEED_MODE2		(250) //250

#define SVM_MAX_SPEED           (400)

#define SVM_OQ_HEIGHT_1         (30)   // in mm/10
#define SVM_OQ_HEIGHT_2         (140)   // in mm/10

#define SVM_OQ_STROKES          (100)

#define SVM_OQ_TIME             (3*60)

#define SVM_OQ_TIME_STEP        (60)

#define SVM_OQ_MIN_TIME         (1*60)
#define SVM_OQ_MAX_TIME         (10*60)

#define SVM_OQ_MIN_HEIGHT       (0)
#define SVM_OQ_MAX_HEIGHT       (1000)

#define SVM_START_MEASURE       (2)

//------------------------------------------------------------------------------
// TAR settings
//------------------------------------------------------------------------------
#define TAR_REV_REVOL_MIN	(0)
#define TAR_REV_REVOL_MAX	(4)

#define TAR_TIME_BEEP           (30)

#define TAR_REVOL_MIN		(0)
#define TAR_REVOL_MAX		(9999)

#define TAR_SPEED_MIN		(20)
#define TAR_SPEED_MAX		(100)
#define TAR_SPEED_CHANGE_STEP	(1)

#define TAR_TIME_CONVERT        (60*60)

#define TAR_CHANGE_STEP	        (1)
#define TAR_CHANGE_STEP_TIME    (60)

#define TAR_REV_SPEED		(15)

#define TAR_REVERSE_MIN         (1)
#define TAR_REVERSE_MAX         (10)

#define TAR_TIME_MIN		(0)
#define TAR_TIME_MAX		(9*60*60L+59*60)
#define TAR_TIME_SHOWN_MAX	(99*60+59)

//------------------------------------------------------------------------------
#define SERIAL_NUMBER_LEN	(6)
#define PCB_ID_LEN              (10)

#define LOGO_MAX_LEN            (10)

#define MOTOR_MIN_SPEED         (20)
#define SERVICE_SPEED           (15)

#define PASSWORD                (1234L)
#define PASSWORD_LEN 						(4) 							// 增加密码长度值，v2.20，Gavin，2019-08-08 星期四 10:13:21

#define MOTOR_PPR_MIN           (10)
#define MOTOR_PPR_MAX           (1000)

#endif
