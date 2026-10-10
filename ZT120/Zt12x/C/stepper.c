#include "pins.h"
#include "global.h"
#include "eeprom.h"
#include "motor.h"
#include "stepper.h"

__flash char STATUS_PROBE[PROBE_STATUS_LAST][3]=
{
  "UP\x0",
  "DN\x0",
  "MU\x0",
  "MD\x0"
};

__flash TWord PROBE_VOLUME[DEPTH_MODE_LAST]={1000,900,750,500};

//maximal values for method offsets in mm*10 
__flash TWord METHOD_MAX_OFFS[METH_MODE_LAST]={300, 100, 100, 500};

//TDWord STEPPER_Pulses;
TBool STEPPER_On=false;
TBool STEPPER_Dir=false;
TWord STEPPER_Distance=0;
TWord STEPPER_Position;
TDWord STEPPER_MaxDistance;

//------------------------------------------------------------------------------
// STEPPER_Init - initialize stepper control
//------------------------------------------------------------------------------
void STEPPER_Init(void)
{
  globalDeviceStatus.haveStepper=true;
  //set interrupt to count pulses
  TIMSK|=(1<<TOIE1);
  OCR1B=0x200;
  TCCR1A|=(1<<COM1B1);
  // set interrupt for stopping sensor
  EICRB|=((1<<ISC41)|(1<<ISC40));
  EIFR|=(1<<INTF4);
  EIMSK|=(1<<INT4);
  STEPPER_Position=0;
  STEPPER_MaxDistance=(STEPPER_MAX_DISTANCE_MM*STEPPER_STEPS_PER_MM*40/EEPROM1.MotorStepMM);
}

#pragma vector=TIMER1_OVF_vect
__interrupt void STEPPER_Pulse(void)
{
  /*if (STEPPER_On)
    STEPPER_Pulses++;*/
  if (STEPPER_On)
    if (!STEPPER_Dir)
      STEPPER_Position++;
    else
      if (STEPPER_Position)
        STEPPER_Position--;
}

//------------------------------------------------------------------------------
// STEPPER_SetDir - set stepping direction
//------------------------------------------------------------------------------
void STEPPER_SetDir(TBool dir)
{
  STEPPER_Dir=dir;
  /*
  if (dir)
    macBitOn(PORT_STEP_CTL,pinM3_Dir);
  else  
    macBitOff(PORT_STEP_CTL,pinM3_Dir);
  */
}

//------------------------------------------------------------------------------
// STEPPER_Start - start stepping
//------------------------------------------------------------------------------
void STEPPER_Start(TBool dir, TBool absolute)
{
  if (!globalDeviceStatus.haveStepper)
    return;
  
  if (!absolute)
  {
    if (STEPPER_Distance==STEPPER_Position)
      return;
    if (STEPPER_Distance<STEPPER_Position)
      dir=true;
    else
      dir=false;
  } else
  {
    STEPPER_Distance=0;
  }
 
  STEPPER_Dir=dir;
  
  /*
  if ((dir)&&(macBitTest(PIN_STEP_STOP,pinM3_Sens2)))
    return;
  if ((!dir)&&(STEPPER_Position>=STEPPER_MaxDistance))
    return;
  
  if (dir)
    macBitOn(PORT_STEP_CTL,pinM3_Dir);
  else
    macBitOff(PORT_STEP_CTL,pinM3_Dir);
  //TCCR1B&=~(1<<CS10);
  //TCCR1B|=(1<<CS11);
  TCCR1B=(1<<WGM12)|(1<<CS11);
  macBitOn(PORT_STEP_CTL,pinM3_Reset);
  TIMER_Start(TIMER_STEPPER, TO_30SEC);
  */
  STEPPER_On=true;
  
}

//------------------------------------------------------------------------------
// STEPPER_Stop - stop stepping
//------------------------------------------------------------------------------
void STEPPER_Stop(void)
{
  /*
  macBitOff(PORT_STEP_CTL,pinM3_Reset);
  //TCCR1B&=~(1<<CS11);
  //TCCR1B|=(1<<CS10);
  */
  TCCR1B=(1<<WGM12)|(1<<CS10);
  STEPPER_On=false;
  TIMER_Stop(TIMER_STEPPER);
}

//------------------------------------------------------------------------------
// STEPPER_End - stopping sensor ready, external interrupt
//------------------------------------------------------------------------------
#pragma vector=INT4_vect
__interrupt void STEPPER_End(void)
{
	; //2017-10-29 星期日 10:38:51
	/*
  if (macBitTest(PIN_STEP_STOP, pinM3_Sens2))
  {
    TIMER_Stop(TIMER_STEPPER);
    globalDeviceStatus.haveStepper=true;
    STEPPER_Stop();
    STEPPER_Position=0;
  }
  */
}

//------------------------------------------------------------------------------
// STEPPER_GetStatus - get current stepper status
//------------------------------------------------------------------------------
TByte STEPPER_GetStatus(void)
{
	; //2017-10-29 星期日 10:40:07
	/*
  if ((STEPPER_On)&&(!STEPPER_Dir))
    return PROBE_STATUS_MD;
  if ((STEPPER_On)&&(STEPPER_Dir))
    return PROBE_STATUS_MU;
  if ((!STEPPER_On)&&(macBitTest(PIN_STEP_STOP,pinM3_Sens2)))
    return PROBE_STATUS_UP;
  */
  return PROBE_STATUS_DN;
}

//------------------------------------------------------------------------------
// STEPPER_IntHandler - timer interrupt for stepper
//------------------------------------------------------------------------------
void STEPPER_IntHandler(void)
{
  /*TDWord pulses;
  
  if ((EEPROM1.Head!=HEAD_MODE_LOW)||(!globalDeviceStatus.haveStepper))//(globalDeviceType<DEVICE_DT724))
    return;
  
  if ((STEPPER_On)&&(!STEPPER_Dir))
  {
    STEPPER_GetPulses(&pulses);
    if ((pulses>STEPPER_Distance)||((STEPPER_Position+pulses)>=STEPPER_MAX_DISTANCE))
    {  
      STEPPER_Stop();
      STEPPER_Position+=pulses;
    }
  }*/
  if (TIMER_TimeOut(TIMER_STEPPER))
  {
    TIMER_Stop(TIMER_STEPPER);
    STEPPER_Stop();
    globalDeviceStatus.haveStepper=false;
  }
  
  if (!globalDeviceStatus.haveStepper)
    return;
  
  if (STEPPER_On)
  {
    if ((!STEPPER_Dir)&&(STEPPER_Position>=STEPPER_MaxDistance))
    {
      STEPPER_Stop();
      return;
    }
    if (STEPPER_Distance)
    {  
      if (STEPPER_Dir)
      {  
        if (STEPPER_Position<=STEPPER_Distance)
          STEPPER_Stop();
      } else
      {
        if (STEPPER_Position>=STEPPER_Distance)
          STEPPER_Stop();
      }
    }
  }
}

//------------------------------------------------------------------------------
// STEPPER_SetDistance - set distance for stepper motor
//------------------------------------------------------------------------------
void STEPPER_SetDistance(TReal32 *Dist)
{
  STEPPER_Distance=(TDWord)(*Dist*STEPPER_STEPS_PER_MM*40/EEPROM1.MotorStepMM);
  /*if (STEPPER_Distance<=STEPPER_Position)
    STEPPER_Distance=0;
  else
    STEPPER_Distance-=STEPPER_Position;*/
}

//------------------------------------------------------------------------------
// STEPPER_GetPosition - get current stepper motor position
//------------------------------------------------------------------------------
void STEPPER_GetPosition(TReal32 *Position)
{
  TReal32 Posit;
  
  Posit=(TReal32)STEPPER_Position/(STEPPER_STEPS_PER_MM*40/EEPROM1.MotorStepMM);
  *Position=Posit;
}

//------------------------------------------------------------------------------
// STEPPER_CalcDistance - calculate distance for moving down
//------------------------------------------------------------------------------
void STEPPER_CalcDistance(TByte VolIndex)
{
  TReal32 tempV, maxV;
  
  tempV=(TReal32)EEPROM1.VolumeDepth[VolIndex]/10;
  //if (EEPROM1.Method==METH_MODE_BSKT)
  //{
    maxV=(TReal32)EEPROM1.MethodOffset[EEPROM1.Method]/10;
    if (tempV>maxV)
      tempV-=maxV;
    else
      tempV=0.0;
  //}  
  STEPPER_Distance=(TDWord)((tempV+(TReal32)EEPROM1.FilterOffset/10)*STEPPER_STEPS_PER_MM*40/EEPROM1.MotorStepMM);//STEPPER_STEPS_DIVIDER);
  //check distance for basket mode (mind basket offset)
}

//------------------------------------------------------------------------------
// STEPPER_CalcDistance - calculate distance for moving down, any volume
//------------------------------------------------------------------------------
void STEPPER_CalcDistanceNew(TWord *Volume)
{
  TByte i;
  TReal32 tempV, maxV;
  TReal32 flVal1, flVal2;
  
  for (i=1; i<DEPTH_MODE_LAST; i++)
  {
    if (PROBE_VOLUME[i]<=*Volume)
      break;  }
  if (i>=DEPTH_MODE_LAST)
  {
    STEPPER_Distance = 0;
    return;
  }  
  
  flVal1 = (TReal32)EEPROM1.VolumeDepth[i-1]/10;
  flVal2 = (TReal32)EEPROM1.VolumeDepth[i]/10;
  
  tempV = flVal2 + (flVal1-flVal2)*(*Volume-PROBE_VOLUME[i])/(PROBE_VOLUME[i-1]-PROBE_VOLUME[i]);
  
  maxV=(TReal32)EEPROM1.MethodOffset[EEPROM1.Method]/10;
  if (tempV>maxV)
    tempV-=maxV;
  else
    tempV=0.0;
  STEPPER_Distance=(TDWord)((tempV+(TReal32)EEPROM1.FilterOffset/10)*STEPPER_STEPS_PER_MM*40/EEPROM1.MotorStepMM);//STEPPER_STEPS_DIVIDER);
}
