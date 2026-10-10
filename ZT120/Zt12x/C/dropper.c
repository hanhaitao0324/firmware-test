#include "defines.h"
#include "pins.h"
#include "global.h"
#include "external.h"
#include "dropper.h"

//------------------------------------------------------------------------------
// DROPPER_Init - init dropper
//------------------------------------------------------------------------------
void DROPPER_Init(void)
{
  //check dropper-------------
  //if ((macBitTest(PIN_DROP_IN, pinM2_Open))&&(macBitTest(PIN_DROP_IN, pinM2_Close)))
  //	globalDeviceStatus.haveDropper=false;
  //else
  //  globalDeviceStatus.haveDropper=true;
  // Gavin 20131226---------------------
  globalDeviceStatus.haveDropper=false;
  
}
  
//------------------------------------------------------------------------------
// DROPPER_Open - open dropper
//------------------------------------------------------------------------------
TBool DROPPER_Open(void)
{
  /*
  // open dropper
  if (macBitTest(PIN_DROP_IN, pinM2_Open))
    return true;
  macBitOn(PORT_DROPPER, pinM2_Out);
  TIMER_Start(TIMER_GENERAL,TO_1SEC);
  while ((!TIMER_TimeOut(TIMER_GENERAL))&&(!macBitTest(PIN_DROP_IN, pinM2_Open)))
    //EXTERNAL_Process();
    continue;
  macBitOff(PORT_DROPPER, pinM2_Out);
  if (TIMER_TimeOut(TIMER_GENERAL))
  {
    // dropper failure
    globalDeviceStatus.dropFailure=true;
    return false;
  }
  */
  
  return true;
}

//------------------------------------------------------------------------------
// DROPPER_Close - close dropper
//------------------------------------------------------------------------------
TBool DROPPER_Close(void)
{
  // close dropper
  if (macBitTest(PIN_DROP_IN, pinM2_Close))
    return true;
  macBitOn(PORT_DROPPER, pinM2_Out);
  TIMER_Start(TIMER_GENERAL,TO_1SEC);
  while ((!TIMER_TimeOut(TIMER_GENERAL))&&(!macBitTest(PIN_DROP_IN, pinM2_Close)))
    //EXTERNAL_Process();
    continue;
  macBitOff(PORT_DROPPER, pinM2_Out);
  if (TIMER_TimeOut(TIMER_GENERAL))
  {
    // dropper failure
    globalDeviceStatus.dropFailure=true;
    return false;
  }

  return true;
}
