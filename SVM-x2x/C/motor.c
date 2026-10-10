#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "global.h"
#include "timer.h"
#include "eeprom.h"
#include <stdio.h>

// Tick value for calculate speed
volatile TWord MOTOR_Tick_Rev=0;

volatile TWord MOTOR_Revolution;
                                
volatile TByte MOTOR_CON=0;

TReal32 Speed_Buf[SPEED_BUF_LEN+1];

TDWord MOTOR_Counter=0;

//volatile TBool MOTOR_SpeedBlock=false;

TWord MotorSpeed;                   

TByte MOTOR_MaxStep;
TByte MOTOR_Prescaler;

TWord IntSpeedBuf[INT_SPEED_BUF_LEN];
TWord IntSpeedBuf2[INT_SPEED_BUF_LEN];
TByte IntSpeedBufIndex;
TByte IntSpeedBufLen;

TWord MOTOR_PulsPerRot;

volatile TWord MOTOR_Value;
TWord MOTOR_Exp_Value;
volatile TWord MOTOR_Cur_Value;

// Tick value for calculate speed
volatile TWord MOTOR_Tick1s=0;
volatile TWord MOTOR_Tick=0;

TWord MOTOR_CurrSpeed;

volatile TWord PWM_B;

// initialize motor (PWM and interrupt)
void MOTOR_Init(void)
{
  // set interrupt for motor sensor
  EICRA|=((1<<ISC31));
  EICRB|=((1<<ISC61));
  EIFR|=((1<<INTF3)|(1<<INTF6));
  EIMSK|=((1<<INT3)|(1<<INT6));
  // set PWM for motor control
  TCCR1A=(1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
  TCCR1B=(1<<WGM12)|(1<<CS10);
  //TIMSK|=(1<<TOIE1);
  // initial settings
  OCR1B=MOTOR_PWM_MIN;
  clockCounter=0;
  // set Timer3 for speed stabilizing
  TCCR3B=(1<<CS31);
  //#ifdef DEVICE_SVM
  //TCCR3B|=(1<<CS30);
  //#endif
  ETIMSK|=(1<<TOIE3);
  
  MOTOR_PulsPerRot=EEPROM1.PulsesPerRotat;
  if (MOTOR_PulsPerRot<50)
  {
    MOTOR_MaxStep=32;
    MOTOR_Prescaler=64;
    TCCR3B|=(1<<CS30);
  } else
  if (MOTOR_PulsPerRot<100)
  {
    MOTOR_MaxStep=16;
    MOTOR_Prescaler=8;
  } else  
  {
    MOTOR_MaxStep=8;
    MOTOR_Prescaler=8;
  }  
}

// external interrupt from the motor sensor
#pragma vector=INT3_vect
__interrupt void MOTOR_Int_Sensor(void)
{
  //TWord NewVal;
  TWord CurPWM;
  TDWord diff;
    
  __disable_interrupt();
  MOTOR_Cur_Value = TCNT3;
  TCNT3 = 0;
  //__enable_interrupt();
  MOTOR_Tick++;
  MOTOR_Tick_Rev++;
  //if (!MOTOR_SpeedBlock)
  //{
    MOTOR_Counter+=MOTOR_Cur_Value;
    MOTOR_Tick1s++;
  //}
  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    CurPWM=OCR1B;
    if (MOTOR_Cur_Value<MOTOR_Exp_Value)
    {
      /*if (MotorSpeed<=300)
        diff = (TWord)(32*((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)/MOTOR_Exp_Value);
      else
        diff = (TWord)(128*((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)/MOTOR_Exp_Value);*/
      diff = (TWord)((((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)<<7)/MOTOR_Exp_Value);
      if (diff>MOTOR_MaxStep) diff=MOTOR_MaxStep;
      if (diff<CurPWM) CurPWM -= diff;
        else CurPWM = MOTOR_PWM_MIN;
    } else
    if (MOTOR_Cur_Value>MOTOR_Exp_Value)
    {
      /*if (MotorSpeed<=300)
        diff = (TWord)(32*((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)/MOTOR_Exp_Value);
      else
        diff = (TWord)(128*((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)/MOTOR_Exp_Value);*/
      diff = (TWord)((((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)<<7)/MOTOR_Exp_Value);
      if (diff>MOTOR_MaxStep) diff=MOTOR_MaxStep;
      CurPWM += diff;
      if (CurPWM>MOTOR_PWM_MAX) CurPWM=MOTOR_PWM_MAX;
    }
    OCR1B=CurPWM;
  }
}

#pragma vector=TIMER3_OVF_vect
__interrupt void MOTOR_Jam(void)
{
  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    OCR1B+=MOTOR_PWM_STEP;
    if (OCR1B>MOTOR_PWM_MAX)
      OCR1B=MOTOR_PWM_MAX;
  }
}

// stabilization
void MOTOR_IntHandler(void)
{
  //TWord speed;
  
  if (macBitTest(MOTOR_CON,Bit_Slow)) MOTOR_Slow();

  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    if (clockCounter>=(TMR0_INT_RATE*SECONDS_COUNTER))
    {
      if (!MOTOR_Tick)
      {
        // motor error
        macBitOff(MOTOR_CON,Bit_Stab);
        globalStatus.motorFailure=true;
        OCR1B=MOTOR_PWM_MIN;
        return;
      }
      MOTOR_Tick=0;
      clockCounter -= (TMR0_INT_RATE*SECONDS_COUNTER);
    }
    if (MOTOR_Tick_Rev>=MOTOR_PulsPerRot)
    {
      MOTOR_Tick_Rev-=(MOTOR_PulsPerRot);
      MOTOR_Revolution++;
    }
  }
}

// calculate PWM settings for current motor speed
void MOTOR_SetSpeed(TReal32 speed)
{
  TWord CountPWM;
    
  MotorSpeed = (TWord)(speed*10);
  //CountPWM = (speed/eepromNominalSpeed)*MOTOR_PWM_MAX;
  //if (CountPWM<MOTOR_PWM_START) CountPWM=MOTOR_PWM_START;
  CountPWM=MOTOR_PWM_START;
  //CountPWM = MOTOR_PWM_MAX - CountPWM;
  // save result to PWM register
  PWM_B = CountPWM;
  MOTOR_Exp_Value=(TWord)(XTAL*60/(MOTOR_Prescaler*speed*MOTOR_PulsPerRot));//MOTOR_IMPULSE_REV));
}

// initialize starting motor
void MOTOR_Start(void)
{
  TByte ix;
    
  MOTOR_Value=0;
  TCNT1=0;
  OCR1B=PWM_B;
  MOTOR_Tick1s=0;
  MOTOR_Tick=0;
  MOTOR_Counter=0;
  //MOTOR_PulsPerRot=EEPROM1.PulsesPerRotat;
  // устанавливаем бит разгона и бит работы двигателя
  macBitOn(MOTOR_CON,Bit_MOTOR);
  macBitOn(MOTOR_CON,Bit_Fast);
  macBitOn(MOTOR_CON,Bit_Stab);
  macBitOff(MOTOR_CON,Bit_Slow);
  clockCounter=0;
  IntSpeedBufIndex=0;
  IntSpeedBufLen=0;
  for (ix=0;ix<INT_SPEED_BUF_LEN; ix++)
    IntSpeedBuf[ix]=0;
}
  
// stop motor
void MOTOR_Stop(void)
{
  //macBitOff(MOTOR_CON,Bit_MOTOR);
  macBitOff(MOTOR_CON,Bit_Stab);
  macBitOff(MOTOR_CON,Bit_Fast);
  PWM_B=MOTOR_PWM_MIN;
  // set motor slow bit
  macBitOn(MOTOR_CON,Bit_Slow);
}

// starting motor
void MOTOR_Fast(void)
{
  TWord TempOCR;
    
  // if motor is running
  if ((macBitTest(MOTOR_CON,Bit_MOTOR))&&(macBitTest(MOTOR_CON,Bit_Fast)))
  {
    TempOCR=OCR1B;
    TempOCR++;
    if (TempOCR>=PWM_B)
    {
      // speed is reached
      OCR1B=PWM_B;
      // clear starting bit
       macBitOff(MOTOR_CON,Bit_Fast);
      // set stabilization bit
      macBitOn(MOTOR_CON,Bit_Stab);
    } else
    {
      OCR1B=TempOCR;
    }
  }
}

// stopping motor
void MOTOR_Slow(void)
{
  TWord TempOCR;
    
  // if stopping bit is set
  if (macBitTest(MOTOR_CON,Bit_Slow))
  {
    #ifdef DEVICE_TAR
    TempOCR=OCR1B;
    if (TempOCR>=(PWM_B+2))
      TempOCR-=2;
    else 
      TempOCR=PWM_B;
    if (TempOCR==PWM_B)
    {
      // motor is stopped
      OCR1B=PWM_B;
      // clear stopping bit
      macBitOff(MOTOR_CON,Bit_Slow);
      macBitOff(MOTOR_CON,Bit_MOTOR);
    } else
    {
      OCR1B=TempOCR;
    }
    #endif
    #ifdef DEVICE_SVM
      OCR1B=PWM_B;
      // clear stopping bit
      macBitOff(MOTOR_CON,Bit_Slow);
      macBitOff(MOTOR_CON,Bit_MOTOR);
    #endif
  }
}

//calc median value
/*TWord MOTOR_CalcAverage(void)
{
  TWord Result = 0;
  TByte ix, jx;
	
  for (jx=0; jx<(INT_SPEED_BUF_LEN-1); jx++)
  {
    for (ix=0; ix<(INT_SPEED_BUF_LEN-1); ix++)
    {
      if (IntSpeedBuf2[ix]>IntSpeedBuf2[ix+1])
      {
	Result = IntSpeedBuf2[ix];
	IntSpeedBuf2[ix] = IntSpeedBuf2[ix+1];
	IntSpeedBuf2[ix+1] = Result;
      }
    }
  }
  if (INT_SPEED_BUF_LEN%2)
    Result = IntSpeedBuf2[INT_SPEED_BUF_LEN>>1];
  else
    Result = (IntSpeedBuf2[INT_SPEED_BUF_LEN>>1] + IntSpeedBuf2[(INT_SPEED_BUF_LEN>>1)-1])/2;

  return Result;
}*/

// get current motor speed
TWord MOTOR_GetSpeed(void)
{  
  //TByte ix;
  TDWord Summ=0;
  TReal32 tspeed;
    
  if (!(macBitTest(MOTOR_CON,Bit_MOTOR)))
    return 0;
  
  //if (macBitTest(MOTOR_CON,Bit_Slow))
  //  return MotorSpeed;
  
  /*if (IntSpeedBufLen<(INT_SPEED_BUF_LEN+INT_SPEED_BUF_PAUSE))
    //return (MotorSpeed/10);
    return 0;
  for (ix=0; ix<INT_SPEED_BUF_LEN; ix++)
    Summ+=IntSpeedBuf[ix];
  //for (ix=0; ix<INT_SPEED_BUF_LEN; ix++)
  //  IntSpeedBuf2[ix]=IntSpeedBuf[ix];
  
  //return (TReal32)MOTOR_CalcAverage()/10;  
  return ((TReal32)Summ/INT_SPEED_BUF_LEN);    */
  if (!MOTOR_Tick1s)
    return 0;
  //MOTOR_SpeedBlock=true;
  __disable_interrupt();
  Summ=MOTOR_Counter/MOTOR_Tick1s;
  MOTOR_Tick1s=0;
  MOTOR_Counter=0;  
  //MOTOR_SpeedBlock=false;
  __enable_interrupt();
  tspeed=((TReal32)XTAL/(MOTOR_Prescaler*Summ*MOTOR_PulsPerRot))*600+0.5;
  MOTOR_CurrSpeed=(TWord)tspeed;
  return MOTOR_CurrSpeed;
}  

//get current motor speed without measurement
TWord MOTOR_GetCurrSpeed(void)
{
  if (!(macBitTest(MOTOR_CON,Bit_MOTOR)))
    return 0;
  return MOTOR_CurrSpeed;
}

#pragma vector=INT6_vect
__interrupt void MOTOR_Alarm(void)
{
  if (!macBitTest(PIN_MOTOR_ALR,pinMFault))
  {
    macBitOff(MOTOR_CON,Bit_Stab);
    globalStatus.motorFailure=true;
    OCR1B=MOTOR_PWM_MIN;
  }
}

// get rotations
TWord MOTOR_GetRevolutions(void)
{
  return MOTOR_Revolution;
}

// reset rotations counter
void MOTOR_ResetRevolutions(void)
{
  MOTOR_Tick_Rev=0;
  MOTOR_Revolution=0;
}

TBool MOTOR_SensorReady(void)
{
  return macBitTest(PIN_SENSOR,pinSensor);
}

void MOTOR_SetReverse(TBool On)
{
  if (On)
    macBitOn(PORT_REVERSE,pinReverse);
  else
    macBitOff(PORT_REVERSE,pinReverse);
}
