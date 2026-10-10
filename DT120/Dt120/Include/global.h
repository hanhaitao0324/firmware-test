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
    TByte gotRS232Package:1;
    TByte gotUSBPackage:1;
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

//PCB type (old=0(DM133), new=1(TLC5927), DM13H=2)
extern TByte globalPCBType;

#endif
