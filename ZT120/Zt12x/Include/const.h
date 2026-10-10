#ifndef _CONST_H
#define _CONST_H

//max length of the serial number string
#define SERIAL_NUM_MAX_LEN	(6)
//max length of the PCB ID string
#define PCB_ID_MAX_LEN		(10)
//max length of firm logo
#define LOGO_MAX_LEN		(10)

//maximal control time value
#define MIN_TIME_VALUE          (0)
//#define MAX_TIME_VALUE          (3600L*99+59*60)
#define MAX_TIME_VALUE          (3600L*9+59*60+59) 					// 2017-11-9 星期四 10:59:39

//#define DT_CONVERT_TIME         (3600L*100 - 1)
//#define DT_MAX_TIME             (3600L*100 - 1)
#define DT_CONVERT_TIME         (3600L*10 - 1) 							// 2018-1-9 星期二 09:23:53
#define DT_MAX_TIME             (3600L*10 - 1) 							// 2018-1-9 星期二 09:24:00

//step values for editing
//#define TIME_HOUR_STEP          (60)
//#define TIME_MIN_STEP           (1)
#define TIME_HOUR_STEP          (60*60) 					// 显示秒，2017-11-9 星期四 11:36:30
#define TIME_MIN_STEP           (60)
#define TIME_SEC_STEP 					(1)

#define SPEED_STEP              (1)
#define TEMP_STEP               (1)

//maximal and minimal control temperature value
#define MIN_TEMP_VALUE          (0.0)
#define MAX_TEMP_VALUE          (60.0)

#define MAX_TEMP_DIFF           (0.2)
#define MAX_TEMP_ERROR          (0.5)

//maximal speed difference in %
//#define MAX_SPEED_ERROR         (1.2)
#define MAX_SPEED_ERROR         (2)

#define SPEED_DELAY             (5)

//value for not-connected Pt100
#define TEMP_ERROR              (0x8000)

//maximal number of sampler Pt100 sensors
#define MAX_SENSORS_NUM         (8)

//maximal and minimal current temperature value
#define MIN_CURTEMP_VALUE       (0.1)
#define MAX_CURTEMP_VALUE       (80.0)

// maximal and minimal temperature offset value
#define MIN_OFFSET_VALUE        (-2.5)
#define MAX_OFFSET_VALUE        (2.5)

#define MIN_FILTER_VALUE        (-10.0)
#define MAX_FILTER_VALUE        (10.0)

#define MAX_BASKET_VALUE        (30.0)

// maximal and minimal speed value
//#define MIN_SPEED_VALUE         (20)
//#define MAX_SPEED_VALUE         (220)
#define MIN_SPEED_VALUE         (30)
#define MAX_SPEED_VALUE         (30)

#define MOTOR_GO_SPEED          (200)

// start beeping before end
#define START_BEEPING_VALUE     (60)

#define MIN_VESSELS_NUMBER      (6)
// gAVIN 2015-06-17---------------------
#define MAX_VESSELS_NUMBER      (8)
// Gavin 20131227-----------------------
//#define MAX_VESSELS_NUMBER      (6)

#define MIN_SENSORS_NUMBER      (6)
#define MAX_SENSORS_NUMBER      (14)

#define PASSWORD                (1234L)
#define PASSWORD_LEN 		(4) 		// 增加密码长度值，v1.03，HHT，2024-06-14

// impulses per rotation（脉冲数/转）
//#define MOTOR_IMPULSE_REV       (360L)
//#define MOTOR_IMPULSE_REV       (120L)
#define MOTOR_IMPULSE_REV       (12L) 						// 2017-11-8 星期三 20:55:08

//update intervals for displaying temperature
#define MIN_UPDATE_INTERVAL     (1)
#define MAX_UPDATE_INTERVAL     (60)

//report intervals
#define MIN_REPORT_INTERVAL     (1)
#define MAX_REPORT_INTERVAL     (60)

//lines per page in report
#define MIN_REPORT_LINES        (20)
#define MAX_REPORT_LINES        (100)

//number of preset speeds
//#define NUM_PRESET_SPEEDS       (5)     // DT120
#define NUM_PRESET_SPEEDS       (1)       // ZT120，30rpm

//number of temperature checkpoints
#define TEMP_CHECK_POINTS       (8)

//------------------------------------------------------------------------------
// Device
//------------------------------------------------------------------------------
enum PCB_TYPES
{
  PCB_UNKNOWN,
  PCB_OLD,
  PCB_NEW
};
/*
enum DEVICE_TYPES
{
	DEVICE_DT620,
	DEVICE_DT720,
	DEVICE_DT1420,
	//DEVICE_DT722,
	//DEVICE_DT724,
	//DEVICE_DT726,
	DEVICE_LAST
};
*/
enum DEVICE_TYPES
{
	DEVICE_ZT121, 							// Gavin,2017-12-3 星期日 10:54:25
	DEVICE_ZT122, 							// 2017-12-3 星期日 10:54:31
	DEVICE_LAST
};

/*
enum DEVICE_LOGOS
{
  DEVICE_LOGO_ERWEKA,
  DEVICE_LOGO_CALEVA,
  DEVICE_LOGO_TABTEST,
  DEVICE_LOGO_QTEST,
  DEVICE_LOGO_LAST
};
*/
// Gavin，2019-3-26 15:08:00，A2.1
enum DEVICE_LOGOS
{
  DEVICE_LOGO_ERWEKA,
  DEVICE_LOGO_LAST
};

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------
#define LED_NUM_DIGITS	(4)

enum LED_DISPLAYS
  {
  	/*
    LED_DISPLAY_TIME,
    LED_DISPLAY_SPEED,
    LED_DISPLAY_TEMP
    */
    LED_DISPLAY_SPEED, 									// Gavin, 2017-11-8 星期三 22:11:58
    LED_DISPLAY_TIME,
    LED_DISPLAY_TEMP
  };

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
    LED_BRIGHTNESS_NUM
  };

//------------------------------------------------------------------------------
// KBD
//------------------------------------------------------------------------------
enum KBD_CODES
  {
  	/*2017-10-28 星期六 15:22:06
    KBD_CODE_START,
    KBD_CODE_STOP,
    KBD_CODE_MENU,
    KBD_CODE_HOUR_UP,
    KBD_CODE_HOUR_DOWN,
    KBD_CODE_MIN_UP,
    KBD_CODE_MIN_DOWN,
    KBD_CODE_TEMP_UP,
    KBD_CODE_TEMP_DOWN,
    KBD_CODE_SPEED_UP,
    KBD_CODE_SPEED_DOWN,
    KBD_CODE_BLOCKED,
    KBD_CODE_LAST
    */
    KBD_CODE_START,
    KBD_CODE_STOP,
    KBD_CODE_DOWN,
    KBD_CODE_QDOWN,
    KBD_CODE_UP,
    KBD_CODE_QUP,
    KBD_CODE_BLOCKED,
    KBD_CODE_LAST
  };

//------------------------------------------------------------------------------
// Control
//------------------------------------------------------------------------------

enum TEST_MODE
  {
    TEST_MODE_LED,
    //TEST_MODE_TIMER,
    TEST_MODE_MOTOR,
    TEST_MODE_TINT,
    TEST_MODE_TEXT,
    TEST_MODE_PRINT,
    //TEST_MODE_DROP, 				// Gavin 20131224
    //TEST_MODE_SAMPLE, 			// Gavin 20131224
    //TEST_MODE_SAMPLET, 			// Gavin 20131224
    TEST_MODE_LAST
  };

enum HEAD_MODE
  {
    HEAD_MODE_LOW,
    HEAD_MODE_HIGH,
    HEAD_MODE_LAST
  };

enum PORT_MODE
  {
    PORT_MODE_USB,
    PORT_MODE_COM,
    PORT_MODE_LAST
  };

enum DEVICE_BAUDRATE
  {
    DEVICE_BAUD_1200,
    DEVICE_BAUD_9600,
    DEVICE_BAUD_LAST
  };

enum PRN_MODE
	{
		PRN_MODE_PMOD,
		PRN_MODE_PTYP,
		PRN_MODE_LANG,
		PRN_MODE_LAST
	};

enum ADJU_MODE
	{
		ADJU_MODE_INT,
		ADJU_MODE_EXT,
		ADJU_MODE_LAST
	};

enum SOUND_MODE
  {
    SOUND_MODE_OFF,
    SOUND_MODE_ON,
    SOUND_MODE_LAST
  };

enum ALARM_MODE
  {
    ALARM_MODE_OFF,
    ALARM_MODE_ON,
    ALARM_MODE_LAST
  };

enum STOP_MODE
	{
		STOP_MODE_OFF,
		STOP_MODE_ON,
		STOP_MODE_LAST
	};

enum HEAT_MODE
	{
		HEAT_MODE_OFF,
		HEAT_MODE_ON,
		HEAT_MODE_LAST
	};

enum MOTORERROR_MODE
  {
    MOTORERROR_MODE_ON,
    MOTORERROR_MODE_OFF,
    MOTORERROR_MODE_LAST
  };

enum PRINTER_MODE
  {
    PRINTER_MODE_OFF,
    PRINTER_MODE_AUTO,
    PRINTER_MODE_ASK,
    PRINTER_MODE_LAST
  };

enum METH_MODE
  {
    METH_MODE_BSKT,
    METH_MODE_PADL,
    METH_MODE_POVD,
    METH_MODE_CYLN,
    METH_MODE_LAST
  };

enum VOLUME_MODE
  {
    //VOLUME_MODE_4000,
    //VOLUME_MODE_2000,
    //VOLUME_MODE_1900,
    //VOLUME_MODE_1750,
    //VOLUME_MODE_1500,
    // Gavin 20131224-------------------
    VOLUME_MODE_1000,
    VOLUME_MODE_900,
    VOLUME_MODE_750,
    VOLUME_MODE_500,
    VOLUME_MODE_LAST
  };

enum DEPTH_MODE
  {
    DEPTH_MODE_1000,
    DEPTH_MODE_900,
    DEPTH_MODE_750,
    DEPTH_MODE_500,
    DEPTH_MODE_LAST
  };

enum SAMPLER_MODE
  {
    SAMPLER_MODE_OFF,
    SAMPLER_MODE_AUTO,
    SAMPLER_MODE_LAST
  };

enum DFORMAT_MODE
  {
    DFORMAT_MODE_EUR,
    DFORMAT_MODE_USA,
    DFORMAT_MODE_LAST
  };

enum TESTCNT_MODE
  {
    TESTCNT_MODE_DOWN,
    TESTCNT_MODE_UP,
    TESTCNT_MODE_LAST
  };

enum MOTOR_MODE
  {
    MOTOR_MODE_STOP,
    MOTOR_MODE_GO,
    MOTOR_MODE_LAST
  };

// 2015/11/26 17:17:27
/*
enum REPORT_MODE
  {
    REPORT_MODE_PLAST,
    REPORT_MODE_INTR,
    //REPORT_MODE_PAGE,
    REPORT_MODE_LANG,
    REPORT_MODE_LAST
  };
*/

enum REPORT_MODE
  {
    REPORT_MODE_PLAST,
    //REPORT_MODE_INTR, 			// 2017/1/14 11:01:53,客户要求不再需要转速和温度的更新频率参数
    //REPORT_MODE_PAGE,
    REPORT_MODE_LANG,
    REPORT_MODE_PTYP,
    REPORT_MODE_LAST
  };
// 2015/11/26 17:18:02 end

enum LANGUAGE_MODE
  {
    LANGUAGE_MODE_ENGLISH,
    LANGUAGE_MODE_GERMAN,
    LANGUAGE_MODE_FRENCH, 	// 2015/11/26 17:23:16
    LANGUAGE_MODE_ITALIAN, 	// 2015/11/26 17:23:22
    LANGUAGE_MODE_SPANISH, 	// 2015/11/26 17:23:27
    LANGUAGE_MODE_LAST
  };

// 2015/11/26 17:25:19
enum PTYPE_MODE
	{
		PTYPE_MODE_NORM, 					// 普通A4纸打印机
		PTYPE_MODE_PORT, 					// 微型热敏纸打印机
		PTYPE_MODE_LAST
	};
// 2015/11/26 17:25:26 end

enum DTIME_MODE
  {
    DTIME_MODE_TIME,
    DTIME_MODE_DAY,
    DTIME_MODE_YEAR,
    DTIME_MODE_FORMAT,
    DTIME_MODE_LAST
  };

// 增加PWMG的子菜单，v1.03，HHT，2024-06-14
enum PWMG_MODE
{
  PWMG_MODE_PWST,
  PWMG_MODE_DATE,
  PWMG_MODE_LAST
};

enum LIFTER_STATUS
  {
    LIFTER_STATUS_UP,
    LIFTER_STATUS_DN,
    LIFTER_STATUS_MU,
    LIFTER_STATUS_MD,
    LIFTER_STATUS_UK,
    LIFTER_STATUS_LAST
  };

enum PROBE_STATUS
  {
    PROBE_STATUS_UP,
    PROBE_STATUS_DN,
    PROBE_STATUS_MU,
    PROBE_STATUS_MD,
    PROBE_STATUS_LAST
  };

enum CHECK_MODE
  {
    // Gavin, 2019-03-29 星期五 15:02:46, A6
    //CHECK_MODE_PT,
    //CHECK_MODE_HEATER,
    CHECK_MODE_INT,
    CHECK_MODE_EXT,
    
    // Gavin 2013/12/30 20:06:22--------
    //CHECK_MODE_AS,
    CHECK_MODE_LAST
  };

enum MOTOR_ERROR 												// 电机报错
  {
    MOTOR_ERROR_NO,
    MOTOR_ERROR_CURR,
    MOTOR_ERROR_ENC,
    MOTOR_ERROR_BELT 										// Gavin 20131226, 传动带断裂
  };

enum REPORT_STATS
  {
    RSTS_NO,
    RSTS_READY,
    RSTS_CANCEL,
    RSTS_FINISHED,
    RSTS_RUNNING,
    RSTS_MOTORERR,
    RSTS_ERROR
  };

enum TEST_STATE
  {
    TEST_STOPPED,
    TEST_RUNNING,
    TEST_PAUSED,
    TEST_WAITING,
    TEST_MOTORSTOPPED
  };

#endif
