#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "const.h"
#include "defines.h"
#include "timedate.h"

typedef struct
  {
    TByte timeTick:1;
    TByte enableUSB:1;
    TByte gotRS485Package:1;
    TByte gotRS232MPackage:1;       // Gavin, 2017-11-7 ÐÇÆÚ¶þ 10:48:20
    TByte gotRS232HPackage:1;
    TByte gotUSBPackage:1;
    TByte sensorInsError:1;
    TByte installedInsADC:1;        // Gavin, 2017-11-4 ÐÇÆÚÁù 14:32:26
    TByte sensorExtError:1;
    TByte installedExtADC:1;
    TByte installedPrinter:1;
    TByte printerFailure:1;
    TByte USBFailure:1;
    TByte dhcFailure:1;
    TByte haveDropper:1;
    TByte dropFailure:1;
    TByte haveStepper:1;
    TByte samplerFailure:1;
    TByte ovrHeating:1;
    TByte kbdHit:1;  
    TByte valueUpdate:1;
    TByte remoteMode:1;
    TByte droppedMode:1;
    TByte motorErrorSwitch:1;
    TByte portMode;
    TByte motorFailure; 
    TByte ekeyStatus;
    TByte Head;
  } TDeviceStatus;

typedef struct
{
  TByte State;
  TBool Unlimited;
  TDWord Ticker;
  TDWord Value;
  TReal32 MinSpeed;
  TReal32 MaxSpeed;
  TWord MinTemp;
  TWord MaxTemp;
  TBool goMode;
  TBool unlimMode;
} TTestStatus;

extern volatile TWord clockCounter;

extern volatile TDeviceStatus globalDeviceStatus;

extern volatile TTestStatus globalTestStatus;

//device type
extern TByte globalDeviceType;

// last pressed key (for testing)
extern TByte lastPressedKey;

//current date and time
extern TTimeDate curDTime;

//PCB type (old=0, new=1)
extern TByte globalPCBType;

#endif
