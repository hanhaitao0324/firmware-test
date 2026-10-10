#include "adc.h"
#include "defines.h"
#include "global.h"
#include "pins.h"
#include "platform.h"
#include "spi.h"
#include "timer.h"
#include <stdlib.h>

volatile TWord adcResult;
volatile TBool adcReady;                // 缓存中有数据标志

//sample buffer - ADC_BUFFER_SIZE samples（数据缓存）
volatile TWord adcBuffer[ADC_BUFFER_SIZE];

volatile TByte adcHead;
//volatile TWord adcTail;

volatile TByte adcCounter;              // 计数器

TWord adcLastValue;                     // 

//------------------------------------------------------------------------------
// ADC_Reset - reset ADC's logic
//------------------------------------------------------------------------------
void ADC_Reset(void)
{
	// 外部PT100----------------
  SPI_Select(SPI_ADC0);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_Select(SPI_NONE);
  
  // 内部PT100----------------2017-10-31 星期二 21:17:37
  SPI_Select(SPI_ADC1);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_Select(SPI_NONE);
}

//------------------------------------------------------------------------------
// Write byte register of ADC（字节写）
//------------------------------------------------------------------------------
//TBool ADC_WriteRegB(TByte reg, TByte value)
TBool ADC_WriteRegB(TByte reg, TByte value, TByte ch)
{
	if( ch == 0)
		SPI_Select(SPI_ADC0);
	else
		SPI_Select(SPI_ADC1);
	
  SPI_IOByte(reg);
  SPI_IOByte(value);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Write word register of ADC（字写）
//------------------------------------------------------------------------------
TBool ADC_WriteRegW(TByte reg, TWord value, TByte ch)
{
  if( ch == 0)
		SPI_Select(SPI_ADC0);
	else
		SPI_Select(SPI_ADC1);
	
  SPI_IOByte(reg);
  SPI_IOByte(HIBYTE(value));
  SPI_IOByte(LOBYTE(value));
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Read byte register of ADC（读字节）
//------------------------------------------------------------------------------
TBool ADC_ReadReg(TByte reg, TByte* value, TByte ch)
{
  if( ch == 0)
		SPI_Select(SPI_ADC0);
	else
		SPI_Select(SPI_ADC1);
	
  SPI_IOByte(reg);
  *value=SPI_IOByte(0x00);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Read data register (16bit) of ADC（读数据16位）
//------------------------------------------------------------------------------
TBool ADC_Read(TWord *value, TByte ch)
{
  TByte r;

  *value = 0;
  
  if( ch == 0)
		SPI_Select(SPI_ADC0);
	else
		SPI_Select(SPI_ADC1);
	
  SPI_IOByte(ADC_READ_DATA_REG);
  r=SPI_IOByte(0x00);
  *value=r;
  r=SPI_IOByte(0x00);
  *value<<=8;
  *value|=r;
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Write command to ADC（写命令）
//------------------------------------------------------------------------------
TBool ADC_WriteCommand(TByte command, TByte ch)
{
  if( ch == 0)
		SPI_Select(SPI_ADC0);
	else
		SPI_Select(SPI_ADC1);
	
  SPI_IOByte(command);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Internal calibration of ADC（内部校准）
//------------------------------------------------------------------------------
TBool ADC_SelfCalibration(TByte ch)
{
  if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_INTZERO_DATA, ch))
    return false;
  //SPI_Select(SPI_ADC);
  //while (ADC_SELECT_PORT&(1<<ADC_SELECT_BIT));
  //SPI_Select(SPI_NONE);
  
  return true;
}

//------------------------------------------------------------------------------
// ADC_Init - initialization code（初始化代码）
//------------------------------------------------------------------------------
void ADC_Init(void)
{
  TByte ix0, ix1;
  
  globalDeviceStatus.installedExtADC=false;
  
  //for (ix=0; ix<ADC_NUM_CHANNELS; ix++)
  //{
    adcHead=0;
    adcReady=false;
    adcCounter=ADC_BYPASS;
  //}
    
  adcLastValue=0;

  //reset ADC's logic
  ADC_Reset();

  //read identificator（读标识）
  if (!ADC_ReadReg(ADC_READ_ID_REG, &ix0, SPI_ADC0))
  {
    //ADC error
    return;
  }
  
  if (!ADC_ReadReg(ADC_READ_ID_REG, &ix1, SPI_ADC1))
  {
    //ADC error
    return;
  }
  
  if (((ix0&0x0f)!=ADC_ID)&&((ix1&0x0f)!=ADC_ID))
  {
    //ADC error
    return;
  }
  
  if((ix0&0x0f) == ADC_ID)
  {
  // write to input/output mode register（写输入/输出模式寄存器）
  if (!ADC_WriteRegB(ADC_WRITE_IO_REG, REG_IO_DATA))
    return;
  
  // write to configuration register（写配置寄存器）
  if (!ADC_WriteRegW(ADC_WRITE_CONF_REG, REG_CONF_DATA))
    return;
  
  // start zero-calibration（开始零点校准）
  //if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_INTZERO_DATA))
  //  return;
  
  TIMER_Start(TIMER_ADC, TO_100MSEC);

  //if (globalDeviceType!=DEVICE_DT620)
    globalDeviceStatus.installedExtADC=true;
}

//------------------------------------------------------------------------------
// ADC_IntHandler - ADC timer handler（定时中断处理）
//------------------------------------------------------------------------------
TBool ADC_IntHandler(void)
{               
  static TByte i=TEMPERATURE_START;
  static TByte snsrErrorCount=0;
  TByte r;
  TWord tmp_value;
  
  /*if (snsrErrorCount>ADC_MAX_TRY)
  {
    TIMER_Stop(TIMER_ADC);
    globalDeviceStatus.sensorExtError=true;
    //globalDeviceStatus.installedExtADC=false;
    return false;
  }*/

  if (TIMER_TimeOut(TIMER_ADC))	
  {
    if (i==TEMPERATURE_START)
    {
      ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_INTZERO_DATA);
      i=TEMPERATURE_OFFSET_EXT;
      TIMER_Start(TIMER_ADC, TO_ADC);
      return false;
    }
    else
    {
      TIMER_Stop(TIMER_ADC);
      ADC_Init();
      i=TEMPERATURE_START;
      snsrErrorCount++;
      return false;
    }  
  }

  if (!ADC_ReadReg(ADC_READ_STATUS_REG,&r))
    return false;
  
  if (r&0x80)
    return false;  
  
  /*if (r&0x40)
  {
    // ADC error, reset
    TIMER_Stop(TIMER_ADC);
    ADC_Init();
    i=TEMPERATURE_OFFSET_EXT;
    snsrErrorCount++;
    return false;
  }*/
  
  if (r&0x40)
  {  
    globalDeviceStatus.sensorExtError=true;
    adcCounter=ADC_BYPASS;
  }
  else
  {  
    globalDeviceStatus.sensorExtError=false;
    if (adcCounter)
      adcCounter--;
  }
  
  snsrErrorCount=0;
  
  TIMER_Stop(TIMER_ADC);

  switch(i)
  {
    case TEMPERATURE_OFFSET_EXT:
      if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA))
        return false;
      //flgWait=ADC_CONF_DELAY;
      i=TEMPERATURE_EXTERNAL;
      break;
    case TEMPERATURE_EXTERNAL:  
      if (!adcCounter)
      {  
        ADC_Read(&tmp_value);
        if (abs(tmp_value-adcLastValue)<ADC_IGNORE_VALUE)
        {  
          adcBuffer[adcHead]=tmp_value;
          adcHead++;
          if (adcHead>=ADC_BUFFER_SIZE)
          {
            adcHead=0; 
            adcReady=true;
          }
        }
        adcLastValue=tmp_value;
      }
      else
      {
        adcHead=0; 
        adcReady=false;
      }
      
      //if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA))
      //  return false;
      //flgWait=0;
      
      break;
  }

  TIMER_Start(TIMER_ADC, TO_ADC);
  
  if (i==TEMPERATURE_EXTERNAL)
    return true;
  
  return false;
}

//------------------------------------------------------------------------------
// ADC_HasSample - has the buffer any sample（判断缓存是否有采样数据）
//------------------------------------------------------------------------------
TBool ADC_HasSample(void)
{
  return adcReady;
}

//------------------------------------------------------------------------------
// ADC_GetSample - get next sample from the buffer（从缓存中获取采样数据）
//------------------------------------------------------------------------------
TWord ADC_GetSample(void)
{
  TDWord sample=0;
  TByte ix;
  
  for (ix=0; ix<ADC_BUFFER_SIZE; ix++)
    sample+=(TDWord)adcBuffer[ix];
  
  return (TWord)(sample/ADC_BUFFER_SIZE);
}

//------------------------------------------------------------------------------
// ADC_GetValue - get measured value（获取测量值）
//------------------------------------------------------------------------------
TWord ADC_GetValue(void)
{
  if (ADC_HasSample())
    adcResult = ADC_GetSample();
  else
    adcResult = 0;
  
  return adcResult;
}
