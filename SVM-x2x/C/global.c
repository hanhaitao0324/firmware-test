#include "global.h"
#include "timedate.h"

//status of the device
volatile TDeviceStatus globalStatus;

//structure with OQ Report data
TOQReport OQReport;

volatile TWord clockCounter;

//structure with information about test
volatile TTestStatus globalTestStatus;

//global string buffer
char globalStr[10];

//current date and time
TTimeDate curDTime;

TByte lastPressedKey;

TByte globalPCBType;

// v1.30, Gavin, 2019-08-23 星期五 10:47:31

// v1.30, Gavin, 2019-08-18 星期日 10:13:02
TWord testVolt[acalTestMax][2]; 				// 定义保存测试结果的内存
__flash TWord acalRev[3] =
///*
{
	10,
	
	//20, 																// 测试模式使用
	//30
	
	500,
	1250
};
//*/
TByte acalStep; 												// v1.30, Gavin, 2019-08-18 星期日 14:03:31
TByte acalStep1; 												// v1.30, 1号站次数，Gavin, 2019-08-23 星期五 14:04:43
TByte acalStep2; 												// v1.30，2号站次数，Gavin，2019-08-23 星期五 14:05:26

//__flash TWord ACAL_REV_ADD[] = 
//{
//	10,510,1760,3010,4260,5510,6760,8010,9260,10510,11760,13010,14260,15510,16760,18810
//};

TWord acalRevTotal_1;
TWord acalRevTotal_2;
