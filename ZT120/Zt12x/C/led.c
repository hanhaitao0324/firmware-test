#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "pins.h"
#include "global.h"
#include "eeprom.h"
#include "led.h"
#include "font.h"
#include "spi.h"
#include "utils.h"
#include "timer.h"
#include "testsetup.h"

TWord blinkReload;                      // 闪烁重载
TWord blinkCounter;                     // 闪烁计数器

TBool ledCycle=false;

//blink status（闪烁状态）
TBool ledBlinkTime;                     // 时间
TBool ledBlinkTemperature;              // 温度
TBool ledBlinkSpeed;                    // 速度
TBool ledBlinkState;                    // 状态

//blink position（闪烁位置）
TByte ledBlinkPos=0;
TBool ledBlinkPosYes=false;
  
//buffers（缓存）
TWord ledTime[LED_NUM_DIGITS];          // 时间缓存4
TWord ledTemperature[LED_NUM_DIGITS];   // 温度缓存4
TWord ledSpeed[LED_NUM_DIGITS];         // 速度缓存4

TWord ledTemp[LED_NUM_DIGITS*3];        // 临时缓存4*3=12

// 亮度参数表---------------------------
//TWord __flash BrightTab[11]={0x0000,0x0004,0x0002,0x0006,0x000E,0x0005,0x0003,0x000B,0x0007,0x000F,0x00FF};

//------------------------------------------------------------------------------
// LED_SetBrightness - set LED brightness(LED设置亮度 - 设置LCD亮度)
//------------------------------------------------------------------------------
TBool LED_SetBrightness(TByte brightness)
{
  TWord zw;
  TDWord temp;
  
  if(brightness >= LED_BRIGHTNESS_NUM)
    return false;
  
  //zw = ((TWord)((brightness+1)*12)<<3)+4;     // 亮度显示值+1放大12倍，范围12~120
  zw = ((TWord)(brightness*12+1)<<3)+4;       // 亮度显示值放大5倍+1，范围1~46
  temp = ((TDWord)zw<<10)+zw;
  
  //unselect all  
  PORT_LED|=(1<<pinStrobe2)|(1<<pinStrobe1)|(1<<pinStrobe4)|(1<<pinStrobe3);
  
  SPI_DeInit();
  
  macBitOff(PORT_LED_L, pinD_Latch);
  macBitOff(PORT_SPI, pinSCK);
  macBitOn(PORT_LED_L, pinD_Latch);
  
  for(zw=0;zw<4;zw++)
  {
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
  }
  
  macBitOff(PORT_LED_L, pinD_Latch);
  
  // 0000-0000 0000-1101 1001-0011 0110-0100
  SPI_Init();
  SPI_Select(SPI_LED);
  
  SPI_IOByte((TByte)(temp>>24));
  SPI_IOByte((TByte)(temp>>16));
  SPI_IOByte((TByte)(temp>>8));
  SPI_IOByte((TByte)(temp));
  
  macBitOn(PORT_LED_L, pinD_Latch);
  macBitOff(PORT_LED_L, pinD_Latch);
  
  blinkReload = TIMER_BLINK_RELOAD;
  return true;
}

//------------------------------------------------------------------------------
// LED_GetBrightness - get LED brightness(获取LED亮度)
//------------------------------------------------------------------------------
TByte LED_GetBrightness(void)
{
  return EEPROM1.LED;
}

//------------------------------------------------------------------------------
// LED_Empty_Time - print nothing on the time display（在时间显示位置上黑屏）
//------------------------------------------------------------------------------
__monitor void LED_Empty_Time(void)
{
  TByte i;

  //reset buffer
  for (i=0; i<LED_NUM_DIGITS; i++)
    ledTime[i] = 0x0000;
}

//------------------------------------------------------------------------------
// LED_Print_Time - print time on the display（显示时间）
//------------------------------------------------------------------------------
__monitor void LED_Print_Time(TBool blink, TDWord* time)
{
  TWord hour, min, sec;
  
  if (blink)
  {
    blinkCounter = 0;
    ledBlinkState = false;
  }
  ledBlinkTime = blink;

  if (*time >= DT_MAX_TIME)     // 时钟上限，9:59:59
  {
    ledTime[0] = CHARACTER_SET14['O'];
    ledTime[1] = CHARACTER_SET14['V'];
    ledTime[2] = CHARACTER_SET14['E'];
    ledTime[3] = CHARACTER_SET14['R'];
  }
  else
  {
    hour = UTILS_ToHexDec((*time/3600)%100);
    min = UTILS_ToHexDec((*time%3600)/60);
    sec = UTILS_ToHexDec(*time%60);
    
    if(globalTestStatus.State==TEST_RUNNING)
    {
      if(hour != 0)
      {
        ledTime[0] = CHARACTER_SET14['h'];
        ledTime[1] = CHARACTER_SET14[(TByte)((hour&0xff)+0x30)]|LED_MASK_DEC_POINT;
        ledTime[2] = CHARACTER_SET14[(TByte)((min>>8)+0x30)];
        
        if(sec&0x01)
          ledTime[3] = CHARACTER_SET14[(TByte)((min&0xff)+0x30)]|LED_MASK_DEC_POINT;
        else
          ledTime[3] = CHARACTER_SET14[(TByte)((min&0xff)+0x30)];
      }
      else
      {
        ledTime[0] = CHARACTER_SET14[(TByte)((min>>8)+0x30)];
        ledTime[1] = CHARACTER_SET14[(TByte)((min&0xff)+0x30)]|LED_MASK_DEC_POINT;
        ledTime[2] = CHARACTER_SET14[(TByte)((sec>>8)+0x30)];
        ledTime[3] = CHARACTER_SET14[(TByte)((sec&0xff)+0x30)];
      }
    }
    else
    {
      if(TT_s)
      {
        ledTime[0] = CHARACTER_SET14['h'];
        ledTime[1] = CHARACTER_SET14[(TByte)((hour&0xff)+0x30)]|LED_MASK_DEC_POINT;
        ledTime[2] = CHARACTER_SET14[(TByte)((min>>8)+0x30)];
        ledTime[3] = CHARACTER_SET14[(TByte)((min&0xff)+0x30)];
      }
      else
      {
        ledTime[0] = CHARACTER_SET14['S'];
        ledTime[1] = CHARACTER_SET14[' '];
        ledTime[2] = CHARACTER_SET14[(TByte)((sec>>8)+0x30)];
        ledTime[3] = CHARACTER_SET14[(TByte)((sec&0xff)+0x30)];
      }
    }
	}
}

//------------------------------------------------------------------------------
// LED_Print_P - print a string from the Flash on the display（显示字符串）
//------------------------------------------------------------------------------
__monitor void LED_Print_P(TByte led, TBool blink, char __flash *str)
{
  char buffer[10];
  
  strncpy_P(buffer,str,sizeof(buffer));
  LED_Print(led,blink,buffer);
}

//------------------------------------------------------------------------------
// LED_Print - print a string on the display（显示字符串）
//------------------------------------------------------------------------------
__monitor void LED_Print(TByte led, TBool blink, char *str)
{
  TWord *buffer;
  TByte ch;
  TByte i;
     
  if (blink)
  {
    blinkCounter=0;
    ledBlinkState=false;
  }
  //setup buffers
  switch (led)
  {
    case LED_DISPLAY_SPEED:
    {
      buffer=ledSpeed;
      ledBlinkSpeed=blink;
    }break;
    case LED_DISPLAY_TIME:
    {
    	buffer=ledTime;
    	ledBlinkTime=blink;
    }break;
    case LED_DISPLAY_TEMP:
    {
      buffer=ledTemperature;
      ledBlinkTemperature=blink;
    }break;
  }
    
  //transfer string，转换字符串
  i=0;
  while ((LED_NUM_DIGITS>=i)&&(*str))
  {
    ch=*(str++);
    if ((LED_NUM_DIGITS==i)&&(ch!='.'))
      break;
    if (ch=='.')
    {
      if (i)
      {
        buffer[i-1]|=LED_MASK_DEC_POINT;
        continue;
      }
      buffer[i++]=LED_MASK_DEC_POINT;
      continue;
    }
    if (ch>=CHARACTER_SET14_LEN)
      ch=0;
    buffer[i++]=CHARACTER_SET14[ch];
  }
  //fill with blanks，用空格填充
  for (;i<LED_NUM_DIGITS;i++)
    buffer[i]=CHARACTER_SET14[' '];
}

//------------------------------------------------------------------------------
// LED interrupt handler（LED数码管中断处理）
//------------------------------------------------------------------------------
void LED_IntHandler(void)
{
  static TByte digit=0;
  //TWord top, middle, bottom;    // top - time, middle - speed, bottom - temp
  TWord left, right; 										// left - speed, right - temperature
  
  //fetch LED's data（取得数码管数据）
  //top=(TWord)CHARACTER_SET7[EMPTY7]<<8 + CHARACTER_SET7[EMPTY7]; 			// 清0
  //middle=CHARACTER_SET14[' ']; 																				// 清0
  //bottom=CHARACTER_SET14[' ']; 																				// 清0
  left=CHARACTER_SET14[' ']; 																					// 清0
  right=CHARACTER_SET14[' ']; 																				// 清0
  
  if ((ledBlinkTime&&(!ledBlinkState))||(!ledBlinkTime))
    //top=ledTime[digit];
    left = ledTime[digit];              // Gavin, 2017-11-8 星期三 21:37:10
    
  if ((ledBlinkTemperature&&(!ledBlinkState))||(!ledBlinkTemperature))
  {
    right=ledTemperature[digit];
    
    if(digit == 0)
    {
    	if((globalTestStatus.State!=TEST_RUNNING)&&
    		 (!TT_s))
    	{
    		ledTemperature[0] = CHARACTER_SET14['t'];
    	}
    	else
	    {
	    	// Gavin, 2019-3-28 16:28:48, A5.1
	    	if(right == SEG_EMPTY)
	    	{
	    		if(macBitTest(PIN_RELAY, pinRelay)) 							// Gavin，判断固态继电器是否接通
	    		{
	    			right |= CHARACTER_SET14['.'];
	    		}
    			
			    if((globalTestStatus.State==TEST_WAITING)||
	    		 	 (globalTestStatus.State==TEST_RUNNING))
					{
			    	if((!globalDeviceStatus.installedExtADC)|| 			// 外部PT100没有安装
		      		 (globalDeviceStatus.sensorExtError)) 				// 或者外部PT100报错
						{
							right |= CHARACTER_SET14['I'];
			      }
					}
				}
				
				/*
	    	if(macBitTest(PIN_RELAY, pinRelay)) 			// Gavin，判断固态继电器是否接通
		    {
	        if(right == SEG_EMPTY)
	          right = CHARACTER_HEAT;
	        //else
	        //  right |= SEG_DP;
	      }
	      */
	    }
	  }
  }
    
  //if ((ledBlinkSpeed&&(!ledBlinkState))||(!ledBlinkSpeed))
  //  left=ledSpeed[digit];
    
  if ((ledBlinkState)&&(ledBlinkPosYes)) 					// 闪烁
  {
    if (ledBlinkPos==digit)
    	right=0x00;
    else 
    if ((ledBlinkPos-LED_NUM_DIGITS)==digit)
    	left=0x00;
  }
  
  macBitOff(PORT_LED_L, pinD_Latch);
  
  SPI_Select(SPI_LED);
  
  SPI_IOByte(HIBYTE(right));
  SPI_IOByte(LOBYTE(right));
  SPI_IOByte(HIBYTE(left));
  SPI_IOByte(LOBYTE(left));
  
  SPI_Select(SPI_NONE);
  
  //unselect all  
  PORT_LED|=(1<<pinStrobe2)|(1<<pinStrobe1)|(1<<pinStrobe4)|(1<<pinStrobe3);
  macBitOn(PORT_LED_L, pinD_Latch);
  macBitOff(PORT_LED_L, pinD_Latch);
	
  //select digit
  if (0==digit) macBitOff(PORT_LED, pinStrobe1);
  else
  if (1==digit) macBitOff(PORT_LED, pinStrobe2);
  else
  if (2==digit) macBitOff(PORT_LED, pinStrobe3);
  else
  if (3==digit) macBitOff(PORT_LED, pinStrobe4);
  
  //update current digit
  digit++;
  if (digit>=LED_NUM_DIGITS)
  {
    digit = 0;
  }
  
  // 闪烁计数器-------------------------
  blinkCounter++;
  if (blinkCounter>=blinkReload)
  {
    ledBlinkState = !ledBlinkState;
    blinkCounter = 0;
  }
}

//------------------------------------------------------------------------------
// LED_Init - init module
//------------------------------------------------------------------------------
void LED_Init(void)
{
  TByte i;
  
  //reset buffer
  for (i=0; i<LED_NUM_DIGITS; i++)
  {
    ledSpeed[i] = ledTemperature[i] = ledTime[i] = 0xFFFF;
  }
  i = EEPROM1.LED;
  if (i>=LED_BRIGHTNESS_NUM)
  {  
    i=LED_BRIGHTNESS_1;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&i, sizeof(EEPROM1.LED));
  }
  LED_SetBrightness(i);
}

//------------------------------------------------------------------------------
// LED_Check - check LEDs
//------------------------------------------------------------------------------
void LED_Check(TByte Position)
{
  TByte i;
  
  if (Position>LED_NUM_DIGITS)
    return;
  
  if (Position)
  {  
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledSpeed[i] = ledTemperature[i] = ledTime[i] = 0x0000;
    
    ledSpeed[Position-1] = ledTemperature[Position-1] = ledTime[Position-1] = 0xFFFF;
  }
  else
  {
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledSpeed[i] = ledTemperature[i] = ledTime[i] = 0xFFFF;
  }  
}

//------------------------------------------------------------------------------
// LED_SwitchOff - switch off LEDs
//------------------------------------------------------------------------------
void LED_SwitchOff(void)
{
  TByte i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
    ledSpeed[i] = ledTemperature[i] = ledTime[i] = 0x0000;
}

//------------------------------------------------------------------------------
// LED_Store - store LEDs values
//------------------------------------------------------------------------------
void LED_Store(void)
{
  TByte i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
  {  
    ledTemp[i*3] = ledSpeed[i];
    ledTemp[i*3+1] = ledTemperature[i];
    ledTemp[i*3+2] = ledTime[i];
  }   
}

//------------------------------------------------------------------------------
// LED_Restore - restore LEDs values（恢复数码管的数值）
//------------------------------------------------------------------------------
void LED_Restore(void)
{
	TByte i;
	
	for (i=0; i<LED_NUM_DIGITS; i++)
	{
		ledSpeed[i] = ledTemp[i*3];
		ledTemperature[i] = ledTemp[i*3+1];
		ledTime[i] = ledTemp[i*3+2];
	}
}
