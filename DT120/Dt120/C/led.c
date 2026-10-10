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

TWord blinkReload;
TWord blinkCounter; 

TBool ledCycle=false;

//blink status
TBool ledBlinkTime;
TBool ledBlinkTemperature;
TBool ledBlinkSpeed;
TBool ledBlinkState;

//blink position
TByte ledBlinkPos=0;
TBool ledBlinkPosYes=false;
  
//buffers
TWord ledTime[LED_NUM_DIGITS];
TWord ledTemperature[LED_NUM_DIGITS];
TWord ledSpeed[LED_NUM_DIGITS];

TWord ledTemp[LED_NUM_DIGITS*3];

TWord __flash BrightTab[11]={0x0000,0x0004,0x0002,0x0006,0x000E,0x0005,0x0003,0x000B,0x0007,0x000F,0x00FF};

//------------------------------------------------------------------------------
// LED_SetBrightness - set LED brightness(LED设置亮度 - 设置LCD亮度)
//------------------------------------------------------------------------------
TBool LED_SetBrightness(TByte brightness)
{
  TDWord temp;
  TByte zw;
  
  TWord zw0;
  
  if (brightness>=LED_BRIGHTNESS_NUM)
    return false;
	
  //unselect all，关闭LED数码管输出，防止在工作时调整亮度影响显示内容；
  PORT_LED|=(1<<pinStrobe2)|(1<<pinStrobe1)|(1<<pinStrobe4)|(1<<pinStrobe3);
  
  if (!globalPCBType)                   // PCB类型是旧的（DM133）
  {
    zw = (brightness<<2)+1;
    temp = ((TDWord)zw<<12) + ((TDWord)zw<<6) + zw;

    macBitOff(PORT_LED_L, pinD_Latch);
    macBitOff(PORT_LED_SEL, pinD_D_Sel);
    SPI_Select(SPI_LED);
    SPI_IOByte((TByte)(temp>>16));
    SPI_IOByte(HIBYTE(temp));
    SPI_IOByte(LOBYTE(temp));
    SPI_Select(SPI_NONE);
    macBitOn(PORT_LED_L, pinD_Latch);
    macBitOff(PORT_LED_L, pinD_Latch);
    macBitOn(PORT_LED_SEL, pinD_D_Sel);
  }
  else
  if(globalPCBType == 1)                // PCB类型是新的（TLC5927）
  {
    temp = BrightTab[brightness];
    SPI_DeInit();
    SPI_Select(SPI_LED);

    macBitOff(PORT_SPI, pinSCK);
    macBitOff(PORT_LED_L, pinD_Latch);
    macBitOn(PORT_LED_SEL, pinD_D_Sel);
    
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOff(PORT_LED_SEL, pinD_D_Sel);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_LED_SEL, pinD_D_Sel);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_LED_L, pinD_Latch);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOff(PORT_LED_L, pinD_Latch);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    SPI_Init();

    SPI_IOByte(HIBYTE(temp));
    SPI_IOByte(LOBYTE(temp));
    SPI_IOByte(HIBYTE(temp));
    SPI_IOByte(LOBYTE(temp));
    SPI_IOByte(HIBYTE(temp));
    SPI_IOByte(LOBYTE(temp));
    macBitOn(PORT_LED_L, pinD_Latch);
    macBitOff(PORT_LED_L, pinD_Latch);

    SPI_DeInit();
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOff(PORT_LED_SEL, pinD_D_Sel);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_LED_SEL, pinD_D_Sel);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_SPI, pinSCK);
    macBitOff(PORT_SPI, pinSCK);
    SPI_Init();
    SPI_Select(SPI_NONE);
  }
  else
  if(globalPCBType == 2)                // PCB类型是（DM13H）
  {
    //zw0 = ((TWord)((brightness+1)*12)<<3)+4;   // 12~120
    zw0 = ((TWord)(brightness*5+1)<<3)+4;   // 1~46
    temp = ((TDWord)zw0<<20) + ((TDWord)zw0<<10) + zw0;
    
    SPI_DeInit();
    
    macBitOff(PORT_LED_L, pinD_Latch);
    macBitOff(PORT_SPI, pinSCK);
    macBitOn(PORT_LED_L, pinD_Latch);
    
    for(zw0=0;zw0<4;zw0++)
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
  }
  else
    return false;
  
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
// LED_Empty_Time - print nothing on the time display
//------------------------------------------------------------------------------
__monitor void LED_Empty_Time(void)
{
  TByte i;

  //reset buffer
  for (i=0; i<LED_NUM_DIGITS; i++)
    ledTime[i] = 0x0000;
}

//------------------------------------------------------------------------------
// LED_Print_Time - print time on the display
//------------------------------------------------------------------------------
__monitor void LED_Print_Time(TBool blink, TDWord* time)
{
  TWord hour, min, sec;
  
  if(*time)
  if (blink)
	{
	  blinkCounter = 0;
	  ledBlinkState = false;
	}
	ledBlinkTime = blink;
	
  hour = UTILS_ToHexDec((*time/3600)%100);
  min = UTILS_ToHexDec((*time%3600)/60);
  sec = UTILS_ToHexDec(*time%60);
  
  ledTime[0] = CHARACTER_SET7[(TByte)(min>>8)];
  ledTime[0] <<= 8;
  ledTime[0] += CHARACTER_SET7[(TByte)(hour>>8)];
  ledTime[1] = CHARACTER_SET7[(TByte)(min&0xff)];
  ledTime[1] <<= 8;
  ledTime[1] += CHARACTER_SET7[(TByte)(hour&0xff)];
  ledTime[2] = (TWord)CHARACTER_SET7[(TByte)(sec>>8)]<<8;
  ledTime[3] = (TWord)CHARACTER_SET7[(TByte)(sec&0xff)]<<8;
}

//------------------------------------------------------------------------------
// LED_Print_P - print a string from the Flash on the display
//------------------------------------------------------------------------------
__monitor void LED_Print_P(TByte led, TBool blink, char __flash *str)
{
  char buffer[10];
  
  strncpy_P(buffer,str,sizeof(buffer));
  LED_Print(led,blink,buffer);
}

//------------------------------------------------------------------------------
// LED_Print - print a string on the display
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
    }
    break;
    case LED_DISPLAY_TEMP:
    {
      buffer=ledTemperature;
      ledBlinkTemperature=blink;
    }
    break;
  }
    
  //transfer string
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
  //fill with blanks
  for (;i<LED_NUM_DIGITS;i++)
    buffer[i]=CHARACTER_SET14[' '];
}

//------------------------------------------------------------------------------
// LED interrupt handler
//------------------------------------------------------------------------------
void LED_IntHandler(void)
{
  static TByte digit=0; 
  TWord top, middle, bottom;    // top - time, middle - speed, bottom - temp

  //fetch LED's data
  top=(TWord)CHARACTER_SET7[EMPTY7]<<8 + CHARACTER_SET7[EMPTY7];
  middle=CHARACTER_SET14[' '];
  bottom=CHARACTER_SET14[' '];
  
  if ((ledBlinkTime&&(!ledBlinkState))||(!ledBlinkTime))
    top=ledTime[digit];
    
  if ((ledBlinkTemperature&&(!ledBlinkState))||(!ledBlinkTemperature))
    middle=ledTemperature[digit];
    
  if ((ledBlinkSpeed&&(!ledBlinkState))||(!ledBlinkSpeed))
    bottom=ledSpeed[digit];
    
  if ((ledBlinkState)&&(ledBlinkPosYes))
  {
    if (ledBlinkPos==digit)
    	middle=0x00;
    else if ((ledBlinkPos-LED_NUM_DIGITS)==digit)
    	bottom=0x00;
  }
      
  //load to the registers
  //initial condition
  if (globalPCBType == 1)                    // PCB类型是新的
    macBitOff(PORT_LED_SEL, pinD_D_Sel);     //AKC Platine
  else                                      // PCB类型是旧的
  if (globalPCBType == 0)
    macBitOn(PORT_LED_SEL, pinD_D_Sel);     //Caleva Platine
  else
  if (globalPCBType == 2);                  // DM13H
    
  
  macBitOff(PORT_LED_L, pinD_Latch);
  SPI_Select(SPI_LED);
  SPI_IOByte(HIBYTE(top));
  SPI_IOByte(LOBYTE(top));
  SPI_IOByte(HIBYTE(middle));
  SPI_IOByte(LOBYTE(middle));
  SPI_IOByte(HIBYTE(bottom));
  SPI_IOByte(LOBYTE(bottom));
  SPI_Select(SPI_NONE);
  //unselect all  
  PORT_LED|=(1<<pinStrobe2)|(1<<pinStrobe1)|(1<<pinStrobe4)|(1<<pinStrobe3);
  macBitOn(PORT_LED_L, pinD_Latch);
  macBitOff(PORT_LED_L, pinD_Latch);

  //select digit
  if (0==digit)
  {
    macBitOff(PORT_LED, pinStrobe1);
  }
  else if (1==digit)
  {
    macBitOff(PORT_LED, pinStrobe2);
  }
  else if (2==digit)
  {
    macBitOff(PORT_LED, pinStrobe3);
  }
  else if (3==digit)
  {
    macBitOff(PORT_LED, pinStrobe4);
  }
     
  //update current digit
  digit++;
  if (digit>=LED_NUM_DIGITS)
  {
    digit = 0;
  }
  
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
  } else
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
// LED_Restore - restore LEDs values
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
