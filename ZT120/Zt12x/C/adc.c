#include <stdlib.h>
#include "adc.h"
#include "defines.h"
#include "global.h"
#include "pins.h"
#include "platform.h"
#include "spi.h"
#include "timer.h"

volatile TWord adcResult[ADC_NUM_CHANNELS]; 								// ADC转换结果
volatile TBool adcReady[ADC_NUM_CHANNELS];                	// 缓存中有数据标志

//sample buffer - ADC_BUFFER_SIZE samples（数据缓存）
volatile TWord adcBuffer[ADC_NUM_CHANNELS][ADC_BUFFER_SIZE]; 	// ADC缓存

volatile TByte adcHead[ADC_NUM_CHANNELS]; 									// 缓存指针
//volatile TWord adcTail;

volatile TByte adcCounter[ADC_NUM_CHANNELS];              	// 计数器

TWord adcLastValue[ADC_NUM_CHANNELS];                     	// 最近一次ADC数值

//------------------------------------------------------------------------------
// ADC_Reset - reset ADC's logic
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
void ADC_Reset(TByte ch)
{
  // 外部PT100----------------2017-11-4 星期六 11:50:31
  SPI_Select(ch); 											// 
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_IOByte(ADC_EMPTY_BYTE);
  SPI_Select(SPI_NONE);
}

//------------------------------------------------------------------------------
// Write byte register of ADC（字节写）
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
//TBool ADC_WriteRegB(TByte reg, TByte value)
TBool ADC_WriteRegB(TByte reg, TByte value, TByte ch)
{
	SPI_Select(ch);
	
  SPI_IOByte(reg);
  SPI_IOByte(value);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Write word register of ADC（字写）
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
TBool ADC_WriteRegW(TByte reg, TWord value, TByte ch)
{
  SPI_Select(ch);
  
  SPI_IOByte(reg);
  SPI_IOByte(HIBYTE(value));
  SPI_IOByte(LOBYTE(value));
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Read byte register of ADC（读字节）
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
TBool ADC_ReadReg(TByte reg, TByte* value, TByte ch)
{
  SPI_Select(ch);
	
  SPI_IOByte(reg);
  *value=SPI_IOByte(0x00);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Read data register (16bit) of ADC（读数据16位）
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
TBool ADC_Read(TWord *value, TByte ch)
{
  TByte r;
  
  *value = 0;
  
  SPI_Select(ch);
  
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
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
TBool ADC_WriteCommand(TByte command, TByte ch)
{
  SPI_Select(ch);
  
  SPI_IOByte(command);
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// Internal Adjustment of ADC（内部校准）
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
TBool ADC_SelfAdjustment(TByte ch)
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
// ch=0（外部SPI_ADC0）
// ch=1（内部SPI_ADC1）
//------------------------------------------------------------------------------
void ADC_Init(TByte ch)
{
  TByte ix;
  
  switch(ch)
  {
  	case ADC_CHNL_EXTERNAL:
  		globalDeviceStatus.installedExtADC=false;
  		break;
  	case ADC_CHNL_INTERNAL:
  		globalDeviceStatus.installedInsADC=false;
  		break;
  	default: break;
  }
  
  //for (ix=0; ix<ADC_NUM_CHANNELS; ix++)
  //{
    adcHead[ch]=0;
    adcReady[ch]=false;
    adcCounter[ch]=ADC_BYPASS;
  //}
    
  adcLastValue[ch]=0;

  //reset ADC's logic
  ADC_Reset(ch);

  //read identificator（读标识）
  if (!ADC_ReadReg(ADC_READ_ID_REG, &ix, ch))
  {
    //ADC error
    return;
  }
  
  if((ix&0x0f) == ADC_ID)
  {
    // write to input/output mode register（写输入/输出模式寄存器）
    if (!ADC_WriteRegB(ADC_WRITE_IO_REG, REG_IO_DATA, ch))
      return;
    
    // write to configuration register（写配置寄存器）
    if (!ADC_WriteRegW(ADC_WRITE_CONF_REG, REG_CONF_DATA, ch))
      return;
    
    switch(ch)
    {
    	case ADC_CHNL_EXTERNAL:
    	{
    		TIMER_Start(TIMER_ADC0, TO_100MSEC); 								// 启动定时器
    		
    		//if (globalDeviceType!=DEVICE_DT620)
      	globalDeviceStatus.installedExtADC=true; 						// 外部温度传感器已安装
    	}break;
    	case ADC_CHNL_INTERNAL:
    	{
    		TIMER_Start(TIMER_ADC1, TO_100MSEC); 								// 启动定时器
    		globalDeviceStatus.installedInsADC=true; 						// 内部温度传感器已安装
    	}break;
    	default: break;
    }
  }
}

//------------------------------------------------------------------------------
// ADC0_IntHandler - ADC0 timer handler（定时中断处理）
//------------------------------------------------------------------------------
TBool ADC0_IntHandler(void)
{               
  static TByte i=TEMPERATURE_START;     					// 温度开始
  static TByte snsrErrorCount0=0;        					// 传感器报错计数器
  TByte r;
  TWord tmp_value;
  
  /*if (snsrErrorCount>ADC_MAX_TRY)
  {
    TIMER_Stop(TIMER_ADC);
    globalDeviceStatus.sensorExtError=true;
    //globalDeviceStatus.installedExtADC=false;
    return false;
  }*/
  
  if (TIMER_TimeOut(TIMER_ADC0))	      					// 判断ADC定时器超时？
  {
    if (i==TEMPERATURE_START)           					// 温度刚开始？
    {
      // 内部零校准---------------------
      ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_INTZERO_DATA, SPI_ADC0);       // 2017-11-3 星期五 11:21:39
      i=TEMPERATURE_OFFSET_EXT;
      TIMER_Start(TIMER_ADC0, TO_ADC);   					// ADC定时器赋值
      return false;
    }
    else                                					// 温度非开始状态
    {
      TIMER_Stop(TIMER_ADC0);           					// 停止ADC定时器
      ADC_Init(SPI_ADC0);               					// ADC初始化
      i=TEMPERATURE_START;              					// 设为开始状态
      snsrErrorCount0++;                 					// 传感器错误计数器累加
      return false;
    }  
  }
  
  // 读ADC状态--------------------------
  if (!ADC_ReadReg(ADC_READ_STATUS_REG,&r, SPI_ADC0))       // 2017-11-3 星期五 11:22:38
    return false;
  
  if (r&0x80)                           					// ADC数据未准备好
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
  
  if (r&0x40)                           					// 传感器报错
  {  
    globalDeviceStatus.sensorExtError=true;
    adcCounter[SPI_ADC0]=ADC_BYPASS;              // 报错后抛掉的数据
  }
  else                                  					// 未报错
  {  
    if(globalDeviceStatus.sensorExtError==true)     // HHT  v1.03  240617-0857  传感器恢复正常后执行一次初始化，防止读取温度数值出现异常现象
        ADC_Init(SPI_ADC0);
    
    globalDeviceStatus.sensorExtError=false;
    if (adcCounter[SPI_ADC0])                     // 为报错后头四个数据被抛掉
      adcCounter[SPI_ADC0]--;
  }
  
  snsrErrorCount0=0;                     					// 传感器报错计数器
  
  TIMER_Stop(TIMER_ADC0);               					// ADC定时器停止

  switch(i)
  {
    case TEMPERATURE_OFFSET_EXT:        					// 外部温度偏移量
      // 连续转换模式-------------------
      if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA, SPI_ADC0))          // 2017-11-3 星期五 11:22:52
        return false;
      //flgWait=ADC_CONF_DELAY;
      i=TEMPERATURE_EXTERNAL;
      break;
    case TEMPERATURE_EXTERNAL:          					// 外部温度
      if (!adcCounter[SPI_ADC0])                  // 数据已经稳定
      {  
        ADC_Read(&tmp_value, SPI_ADC0);           // 2017-11-3 星期五 11:23:42
        if (abs(tmp_value-adcLastValue[SPI_ADC0])<ADC_IGNORE_VALUE)     // 求前后两个值的差值的绝对值是否小于0x0100
        {  
          adcBuffer[SPI_ADC0][adcHead[SPI_ADC0]]=tmp_value;           // 写入ADC数据缓存
          adcHead[SPI_ADC0]++;                              // 缓存头指针累加
          if (adcHead[SPI_ADC0]>=ADC_BUFFER_SIZE)           // 缓存上限维护
          {
            adcHead[SPI_ADC0]=0; 
            adcReady[SPI_ADC0]=true;
          }
        }
        adcLastValue[SPI_ADC0]=tmp_value;
      }
      else                              // 数据未稳定
      {
        adcHead[SPI_ADC0]=0; 
        adcReady[SPI_ADC0]=false;
      }
      
      //if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA))
      //  return false;
      //flgWait=0;
      
      break;
  }

  TIMER_Start(TIMER_ADC0, TO_ADC);         // 启动ADC定时器，500mS？
  
  if (i==TEMPERATURE_EXTERNAL)
    return true;
  
  return false;
}

//------------------------------------------------------------------------------
// ADC1_IntHandler - ADC1 timer handler（定时中断处理）
//------------------------------------------------------------------------------
TBool ADC1_IntHandler(void)
{               
  static TByte j=TEMPERATURE_START;     					// 温度开始
  static TByte snsrErrorCount1=0;        					// 传感器报错计数器
  TByte r;
  TWord tmp_value;
  
  if (TIMER_TimeOut(TIMER_ADC1))	      					// 判断ADC定时器超时？
  {
    if (j==TEMPERATURE_START)           					// 温度刚开始？
    {
      // 内部零校准---------------------
      ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_INTZERO_DATA, SPI_ADC1);       // 2017-11-3 星期五 11:21:39
      j=TEMPERATURE_OFFSET_EXT;
      TIMER_Start(TIMER_ADC1, TO_ADC);   					// ADC定时器赋值
      return false;
    }
    else                                					// 温度非开始状态
    {
      TIMER_Stop(TIMER_ADC1);           					// 停止ADC定时器
      ADC_Init(SPI_ADC1);               					// ADC初始化
      j=TEMPERATURE_START;              					// 设为开始状态
      snsrErrorCount1++;                 					// 传感器错误计数器累加
      return false;
    }  
  }
  
  // 读ADC状态--------------------------
  if (!ADC_ReadReg(ADC_READ_STATUS_REG,&r, SPI_ADC1))       // 2017-11-3 星期五 11:22:38
    return false;
  
  if (r&0x80)                           					// ADC数据未准备好
    return false;  
  
  if (r&0x40)                           					// 传感器报错
  {  
    globalDeviceStatus.sensorInsError=true;
    adcCounter[SPI_ADC1]=ADC_BYPASS;              // 报错后抛掉的数据
  }
  else                                  					// 未报错
  {
    globalDeviceStatus.sensorInsError=false;
    if (adcCounter[SPI_ADC1])                     // 为报错后头四个数据被抛掉
      adcCounter[SPI_ADC1]--;
  }
  
  snsrErrorCount1=0;                     					// 传感器报错计数器
  
  TIMER_Stop(TIMER_ADC1);               					// ADC定时器停止

  switch(j)
  {
    case TEMPERATURE_OFFSET_EXT:        					// 外部温度偏移量
      // 连续转换模式-------------------
      if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA, SPI_ADC1))          // 2017-11-3 星期五 11:22:52
        return false;
      //flgWait=ADC_CONF_DELAY;
      j=TEMPERATURE_EXTERNAL;
      break;
    case TEMPERATURE_EXTERNAL:          					// 外部温度
      if (!adcCounter[SPI_ADC1])                  // 数据已经稳定
      {  
        ADC_Read(&tmp_value, SPI_ADC1);           // 2017-11-3 星期五 11:23:42
        if (abs(tmp_value-adcLastValue[SPI_ADC1])<ADC_IGNORE_VALUE)     // 求前后两个值的差值的绝对值是否小于0x0100
        {  
          adcBuffer[SPI_ADC1][adcHead[SPI_ADC1]]=tmp_value;           // 写入ADC数据缓存
          adcHead[SPI_ADC1]++;                              // 缓存头指针累加
          if (adcHead[SPI_ADC1]>=ADC_BUFFER_SIZE)           // 缓存上限维护
          {
            adcHead[SPI_ADC1]=0; 
            adcReady[SPI_ADC1]=true;
          }
        }
        adcLastValue[SPI_ADC1]=tmp_value;
      }
      else                              					// 数据未稳定
      {
        adcHead[SPI_ADC1]=0; 
        adcReady[SPI_ADC1]=false;
      }
      
      //if (!ADC_WriteRegW(ADC_WRITE_MODE_REG, REG_MODE_DATA))
      //  return false;
      //flgWait=0;
      
      break;
  }

  TIMER_Start(TIMER_ADC1, TO_ADC);         				// 启动ADC定时器，500mS？
  
  if (j==TEMPERATURE_EXTERNAL)
    return true;
  
  return false;
}

//------------------------------------------------------------------------------
// ADC_HasSample - has the buffer any sample（判断缓存是否有采样数据）
//------------------------------------------------------------------------------
TBool ADC_HasSample(TByte ch)
{
  return adcReady[ch];
}

//------------------------------------------------------------------------------
// ADC_GetSample - get next sample from the buffer（从缓存中获取采样数据）
//------------------------------------------------------------------------------
TWord ADC_GetSample(TByte ch)
{
  TDWord sample=0;
  TByte ix;
  
  for (ix=0; ix<ADC_BUFFER_SIZE; ix++)
    sample+=(TDWord)adcBuffer[ch][ix];
  
  return (TWord)(sample/ADC_BUFFER_SIZE);
}

//------------------------------------------------------------------------------
// ADC_GetValue - get measured value（获取测量值）
//------------------------------------------------------------------------------
TWord ADC_GetValue(TByte ch)
{
  if (ADC_HasSample(ch))
    adcResult[ch] = ADC_GetSample(ch);
  else
    adcResult[ch] = 0;
  
  return adcResult[ch];
}
