#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "global.h"
#include "timer.h"
#include "eeprom.h"
#include <stdio.h>

// Gavin 20131226-----------------------
unsigned int CloseCNT = 0;
unsigned char MotorBelt = 0;
unsigned char OldMotorBelt = 0;

// Tick value for calculate speed
volatile TWord MOTOR_Tick_Rev=0;

volatile TWord MOTOR_Revolution;
                                
volatile TByte MOTOR_CON=0;

TDWord MOTOR_Counter=0;

volatile TBool MOTOR_SpeedBlock=false;

TWord MotorSpeed;

TWord MOTOR_CurrSpeed;

/*TWord IntSpeedBuf[INT_SPEED_BUF_LEN];
TWord IntSpeedBuf2[INT_SPEED_BUF_LEN];
TByte IntSpeedBufIndex;
TByte IntSpeedBufLen;*/

TWord MOTOR_PulsPerRot;

volatile TWord MOTOR_Value;
TWord MOTOR_Exp_Value;
volatile TWord MOTOR_Cur_Value;

// Tick value for calculate speed
volatile TWord MOTOR_Tick1s=0;
volatile TWord MOTOR_Tick=0;

volatile TWord PWM_A;

volatile TByte MOTOR_Pause;

volatile TWord MOTOR_FaultCounter;

// initialize motor (PWM and interrupt)
void MOTOR_Init(void)
{
  // set interrupt for motor sensor
  EICRB|=(1<<ISC61);
  EIFR|=(1<<INTF6);
  EIMSK|=(1<<INT6);
  if (globalDeviceStatus.motorErrorSwitch)
  {
    EICRB|=(1<<ISC71);
    EIFR|=(1<<INTF7);
    EIMSK|=(1<<INT7);
  }  
  // set PWM for motor control
  TCCR1A=(1<<COM1A1)|(1<<WGM11)|(1<<WGM10);   // 10bit fast PWM, no prescaling, clear OC1A on compare match
  TCCR1B=(1<<WGM12)|(1<<CS10);
  //TIMSK|=(1<<TOIE1);
  // initial settings
  OCR1A=MOTOR_PWM_MIN;
  clockCounter=0;

  // set Timer3 for speed stabilizing
  TCCR3B=(1<<CS31);
  ETIMSK|=(1<<TOIE3);
  
  MOTOR_CurrSpeed=0;
  MOTOR_PulsPerRot=EEPROM1.PulsesPerRotat;
  MOTOR_FaultCounter=MOTOR_FAULT_COUNT;
}

// external interrupt from the motor sensor
#pragma vector=INT6_vect
__interrupt void MOTOR_Int_Sensor(void)
{
  //TWord NewVal;
  TWord CurPWM;
  TWord diff;
    
  MOTOR_Cur_Value = TCNT3;
  TCNT3 = 0;
  //__enable_interrupt();
  MOTOR_Tick++;
  if (!MOTOR_SpeedBlock)
  {
    MOTOR_Counter+=MOTOR_Cur_Value;
    MOTOR_Tick1s++;
  }
  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    CurPWM=OCR1A;
    if (MOTOR_Cur_Value<MOTOR_Exp_Value)
    {
      /*if (MotorSpeed<=300)
        diff = (TWord)(128*((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)/MOTOR_Exp_Value);
      else
        diff = (TWord)(128*((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)/MOTOR_Exp_Value);*/
      //diff = (TWord)(128*((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)/MOTOR_Exp_Value);
      diff = (TWord)((((TDWord)MOTOR_Exp_Value-MOTOR_Cur_Value)<<7)/MOTOR_Exp_Value);
      if (diff>8) diff=8;
      if (diff<CurPWM) CurPWM -= diff;
        else CurPWM = MOTOR_PWM_MIN;
    } else
    if (MOTOR_Cur_Value>MOTOR_Exp_Value)
    {
      /*if (MotorSpeed<=300)
        diff = (TWord)(128*((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)/MOTOR_Exp_Value);
      else
        diff = (TWord)(128*((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)/MOTOR_Exp_Value);*/
      //diff = (TWord)(128*((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)/MOTOR_Exp_Value);
      diff = (TWord)((((TDWord)MOTOR_Cur_Value-MOTOR_Exp_Value)<<7)/MOTOR_Exp_Value);
      if (diff>8) diff=8;
      CurPWM += diff;
      if (CurPWM>MOTOR_PWM_MAX) CurPWM=MOTOR_PWM_MAX;
    }
    OCR1A=CurPWM;
  }
}

#pragma vector=TIMER3_OVF_vect
__interrupt void MOTOR_Jam(void)
{
  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    OCR1A+=MOTOR_PWM_STEP;
    if (OCR1A>MOTOR_PWM_MAX)
      OCR1A=MOTOR_PWM_MAX;
  }
}

// slowly stopping motor
void MOTOR_Slow(void)
{
  TWord TempOCR;
    
  // if stopping bit is set
  if (macBitTest(MOTOR_CON,Bit_Slow))
  {
    TempOCR=OCR1A;
    if (TempOCR>(MOTOR_PWM_MIN+4))
      TempOCR-=4;
    else
      TempOCR=MOTOR_PWM_MIN;
    if (TempOCR==PWM_A)
    {
      // motor is stopped
      OCR1A=PWM_A;
      // clear stopping bit
      macBitOff(MOTOR_CON,Bit_Slow);
      macBitOff(MOTOR_CON,Bit_MOTOR);
    } else
    {
      OCR1A=TempOCR;
    }
  }
}

// stabilization
void MOTOR_IntHandler(void)
{
  //TWord speed;
  
  if (macBitTest(MOTOR_CON,Bit_Slow))
  	MOTOR_Slow();

  if (macBitTest(MOTOR_CON,Bit_Stab))
  {
    if (clockCounter>=(TMR0_INT_RATE*SECONDS_COUNTER))
    {
      if (!MOTOR_Tick)
      {
        // motor error
        macBitOff(MOTOR_CON,Bit_Stab);
        globalDeviceStatus.motorFailure=MOTOR_ERROR_ENC;
        OCR1A=MOTOR_PWM_MIN;
        return;
      }
      MOTOR_Tick=0;
      clockCounter -= (TMR0_INT_RATE*SECONDS_COUNTER);
      if (MOTOR_Pause<MOTOR_STABLE)
      	MOTOR_Pause++;
      
      /*MOTOR_Tick_Rev = MOTOR_Tick1s;
      MOTOR_Tick1s = 0;

      speed = (TDWord)MOTOR_Tick_Rev*600/(MOTOR_IMPULSE_REV*SECONDS_COUNTER);
      IntSpeedBuf[IntSpeedBufIndex] = speed;
      IntSpeedBufIndex++;
      if (IntSpeedBufIndex>=INT_SPEED_BUF_LEN)
        IntSpeedBufIndex=0;
      if (IntSpeedBufLen<(INT_SPEED_BUF_LEN+INT_SPEED_BUF_PAUSE))
        IntSpeedBufLen++;
      */
      
    }
    
    // ґ«¶Їґш¶ПБСјмІв-----------------Gavin 20131226, 2013/12/28 12:18:51
    /* GavinЈ¬µз»ъН¬ІЅґшјмІвЈ»2014/4/18 15:59:45
    if(globalDeviceStatus.motorErrorSwitch) 			// M.ERR is ON
    {
      CloseCNT++;
      if(macBitTest(PIN_DROP_IN, pinM2_Close))
      {
      	MotorBelt = 1;
      	if(OldMotorBelt == 0)
      	{
      		CloseCNT = 0;
      	}
      }
      else
      {
      	MotorBelt = 0;
      }
      OldMotorBelt = MotorBelt;
      if(CloseCNT >= 3000)
      {
      	CloseCNT = 0;
      	
      	// motor belt error-------------
        macBitOff(MOTOR_CON,Bit_Stab);
        globalDeviceStatus.motorFailure=MOTOR_ERROR_BELT;
        OCR1A=MOTOR_PWM_MIN;
        return;
      }
		}
		*/
      
    /*if (MOTOR_Tick>=MOTOR_PulsPerRot)//MOTOR_IMPULSE_REV)
    {
      MOTOR_Tick-=(MOTOR_PulsPerRot);//MOTOR_IMPULSE_REV);
      MOTOR_Revolution++;
    }*/
    if (TIMER_TimeOut(TIMER_MOTOR))
    {
      TIMER_Start(TIMER_MOTOR, TO_2SEC);
      if (!MOTOR_FaultCounter)
      {  
        macBitOff(MOTOR_CON, Bit_Stab);
        globalDeviceStatus.motorFailure=MOTOR_ERROR_CURR;
        OCR1A=MOTOR_PWM_MIN;
      }
      MOTOR_FaultCounter=MOTOR_FAULT_COUNT;
    }  
  }
}

// calculate PWM settings for current motor speed
void MOTOR_SetSpeed(TReal32 speed)
{
  MotorSpeed = (TWord)(speed*10);
  PWM_A = MOTOR_PWM_START;
  MOTOR_Exp_Value=(TWord)(XTAL*60/(8*speed*MOTOR_PulsPerRot));//MOTOR_IMPULSE_REV));
  MOTOR_Pause=0;
}

// initialize starting motor
void MOTOR_Start(void)
{
  //TByte ix;
    
  MOTOR_Value=0;
  TCNT1=0;
  OCR1A=PWM_A;
  MOTOR_Tick1s=0;
  MOTOR_Tick=0;
  MOTOR_Counter=0;
  // устанавливаем бит разгона и бит работы двигателя
  macBitOn(MOTOR_CON,Bit_MOTOR);
  macBitOn(MOTOR_CON,Bit_Fast);
  macBitOn(MOTOR_CON,Bit_Stab);
  macBitOff(MOTOR_CON,Bit_Slow);
  clockCounter=0;
  TIMER_Start(TIMER_MOTOR, TO_2SEC);
  /*IntSpeedBufIndex=0;
  IntSpeedBufLen=0;
  for (ix=0;ix<INT_SPEED_BUF_LEN; ix++)
    IntSpeedBuf[ix]=0;*/
}
  
// stop motor
void MOTOR_Stop(void)
{
  //macBitOff(MOTOR_CON,Bit_MOTOR);
  macBitOff(MOTOR_CON,Bit_Stab);
  macBitOff(MOTOR_CON,Bit_Fast);
  PWM_A=MOTOR_PWM_MIN;
  // set motor slow bit
  macBitOn(MOTOR_CON,Bit_Slow);
  TIMER_Stop(TIMER_MOTOR);
}

// starting motor
/*void MOTOR_Fast(void)
{
  TWord TempOCR;
    
  // if motor is running
  if ((macBitTest(MOTOR_CON,Bit_MOTOR))&&(macBitTest(MOTOR_CON,Bit_Fast)))
  {
    TempOCR=OCR1A;
    TempOCR++;
    if (TempOCR>=PWM_A)
    {
      // speed is reached
      OCR1A=PWM_A;
      // clear starting bit
       macBitOff(MOTOR_CON,Bit_Fast);
      // set stabilization bit
      macBitOn(MOTOR_CON,Bit_Stab);
    } else
    {
      OCR1A=TempOCR;
    }
  }
}*/

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

//check if motor speed is stable
TBool MOTOR_IsStable(void)
{
  if (MOTOR_Pause<MOTOR_STABLE)
    return false;
  else
    return true;
}

// get current motor speed
TWord MOTOR_GetSpeed(void)
{  
  //TByte ix;
  TDWord Summ=0;
  TReal32 tspeed;
    
  if (!(macBitTest(MOTOR_CON,Bit_MOTOR)))
    return 0;
  
  if (macBitTest(MOTOR_CON,Bit_Slow))
    return MotorSpeed;
  
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
  MOTOR_SpeedBlock=true;
  Summ=MOTOR_Counter/MOTOR_Tick1s;
  MOTOR_SpeedBlock=false;
  tspeed=((TReal32)XTAL/(8*Summ*MOTOR_PulsPerRot))*600+0.5;
  MOTOR_Tick1s=0;
  MOTOR_Counter=0;  
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

#pragma vector=INT7_vect
__interrupt void MOTOR_Alarm(void)
{
  if (!macBitTest(PIN_MOTOR_ALR,pinMFault))
  {
    if (MOTOR_FaultCounter)
    {  
      MOTOR_FaultCounter--;
    }/* else
    {    
      macBitOff(MOTOR_CON,Bit_Stab);
      globalDeviceStatus.motorFailure=MOTOR_ERROR_CURR;
      OCR1A=MOTOR_PWM_MIN;
    }*/  
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
  MOTOR_Tick=0;
  MOTOR_Tick1s=0;
  MOTOR_Revolution=0;
}
