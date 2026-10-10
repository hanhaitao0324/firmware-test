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
