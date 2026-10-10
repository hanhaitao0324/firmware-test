#ifndef _GLOBAL_H
  #define _GLOBAL_H

#include "defines.h"
#include "const.h"
#include "timedate.h"

typedef struct
{
  TByte gotPackage:1;
  //TByte kbdHit:1;
  //TByte timeTick:1;
  TByte enableRxTx:1;
  TByte installedPrinter:1;
  TByte printerFailure:1;
  TByte motorFailure:1;
  TByte usbFailure:1;
  TByte testStart:1;
  TByte valueUpdate:1;
  TByte ekeyStatus;
} TDeviceStatus;

typedef struct
{
  TWord Height[2];
  TWord RHeight[2];
  TWord Strokes;
  TWord RStrokes;
  TWord Time;
  TWord RTime;
} TOQReport;

typedef struct
{
  TByte State;
  TByte Mode;
  TBool Unlimited;
  TWord Ticker;
  TWord Value;
  TWord MinSpeed;
  TWord MaxSpeed;
  TByte NumStatRecords;
  TBool Ready;
} TTestStatus;

//status of the device
extern volatile TDeviceStatus globalStatus;

//structure with OQ Report data
extern TOQReport OQReport;

//structure with information about test
extern volatile TTestStatus globalTestStatus;

extern volatile TWord clockCounter;

//global string buffer
extern char globalStr[10];

//current date and time
extern TTimeDate curDTime;

extern TByte lastPressedKey;

//PCB type (old=0, new=1)
extern TByte globalPCBType;

#endif
