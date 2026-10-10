#include "pins.h"
#include "global.h"
#include "motor.h"
#include "stepper.h"

__flash char STATUS_LIFTER[LIFTER_STATUS_LAST][3]=
{
  "UP\x0",
  "DN\x0",
  "MU\x0",
  "MD\x0",
  "UK\x0"
};

TByte LIFTER_Status;

//------------------------------------------------------------------------------
// LIFTER_Init - initialize lifter control
//------------------------------------------------------------------------------
void LIFTER_Init(void)
{
  LIFTER_Status=LIFTER_STATUS_UP;
}

//------------------------------------------------------------------------------
// LIFTER_GetStatus - get current lifter status
//------------------------------------------------------------------------------
TByte LIFTER_GetStatus(void)
{
  return LIFTER_Status;
}

//------------------------------------------------------------------------------
// LIFTER_Up - lifter up
//------------------------------------------------------------------------------
void LIFTER_Up(TWord distance)
{
  if (!distance)
    return;
  LIFTER_Status=LIFTER_STATUS_UP;
}

//------------------------------------------------------------------------------
// LIFTER_Down - lifter down
//------------------------------------------------------------------------------
void LIFTER_Down(TWord distance)
{
  if (!distance)
    return;
  LIFTER_Status=LIFTER_STATUS_DN;
}
