#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "pins.h"
#include "global.h"
#include "eeprom.h"
#include "led.h"
#include "font.h"
#include "spi.h"
#include "timer.h"

TByte ledReload;
TWord blinkReload;
TWord blinkCounter; 

TBool ledCycle=false;

//blink status
TBool ledBlinkTime;
TBool ledBlinkSpeed;
TBool ledBlinkState;

//blink position
TByte ledBlinkPos=0;
TBool ledBlinkPosYes=false;

TBool ledShow=true;
  
//buffers
TWord ledTime[LED_NUM_DIGITS];
TWord ledSpeed[LED_NUM_DIGITS];

TWord __flash BrightTab[11]={0x0000,0x0004,0x0002,0x0006,0x000E,0x0005,0x0003,0x000B,0x0007,0x000F,0x00FF};

//------------------------------------------------------------------------------
// LED_SetBrightness - set LED brightness
//------------------------------------------------------------------------------
TBool LED_SetBrightness(TByte brightness)
{
  TWord temp;
  TByte zw;
	TByte i,j;
	
  if (brightness>=LED_BRIGHTNESS_NUM)
    return false;

  if (!globalPCBType)  									// PCB_OLD
  {
    zw = (brightness<<2)+1;
    temp = ((TWord)zw<<6) + zw;
  
    macBitOff(PORT_LED_L, pinD_latch);
    macBitOff(PORT_LED_SEL, pinD_sel);
    SPI_IOByte(HIBYTE(temp));
    SPI_IOByte(LOBYTE(temp));
    macBitOn(PORT_LED_L, pinD_latch);
    macBitOff(PORT_LED_L, pinD_latch);
    macBitOn(PORT_LED_SEL, pinD_sel);
  }
	else
	if(globalPCBType == PCB_NEW-1) 				// PCB_NEW
	{
    temp=BrightTab[brightness];
    SPI_DeInit(); 											// 关闭SPI
    macBitOff(PORT_SPI, pinSCK); 				// 0，数码管驱动芯片的时钟端
    macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存信号
    macBitOn(PORT_LED_SEL, pinD_sel); 	// 1，片选信号
    
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOff(PORT_LED_SEL, pinD_sel); 	// 0，片选
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOn(PORT_LED_SEL, pinD_sel); 	// 1，片选
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOn(PORT_LED_L, pinD_latch); 	// 1，锁存
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    SPI_Init(); 												// SPI开启初始化

    SPI_IOByte(HIBYTE(temp)); 					// 传送亮度值
    SPI_IOByte(LOBYTE(temp));
    SPI_IOByte(HIBYTE(temp)); 					// 两个芯片串联
    SPI_IOByte(LOBYTE(temp));
    macBitOn(PORT_LED_L, pinD_latch); 	// 1，锁存
    macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存

    SPI_DeInit(); 											// 关闭SPI
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOff(PORT_LED_SEL, pinD_sel); 	// 0，片选
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOn(PORT_LED_SEL, pinD_sel); 	// 1，片选
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
    macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
    SPI_Init(); 												// SPI开启初始化
	}
	else
	if(globalPCBType == PCB_DM13H-1) 			// PCB_DM13H
	{
		temp=BrightTab[brightness];
		temp = brightness*12;
		temp <<= 3;
		temp |= 0x0004;
		
		SPI_DeInit(); 											// 关闭SPI
		
		macBitOn(PORT_LED_SEL, pinD_sel); 	// 1，片选信号
		macBitOff(PORT_SPI, pinSCK); 				// 0，数码管驱动芯片的时钟端
		macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存信号
		macBitOff(PORT_LED_SEL, pinD_sel); 	// 0，片选
		
		macBitOn(PORT_LED_L, pinD_latch); 	// 1，锁存
		macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
		macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
		macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
		macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
		macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
		macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
		macBitOn(PORT_SPI, pinSCK); 				// 1，时钟
		macBitOff(PORT_SPI, pinSCK); 				// 0，时钟
		macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存信号
		
		for(j=0;j<2;j++)
		{
			for(i=0;i<10;i++)
			{
				if(temp & 1<<(9-i))
					macBitOn(PORT_SPI, pinMOSI); 	// 1，数据
				else
					macBitOff(PORT_SPI, pinMOSI); // 0, 数据
				
				macBitOn(PORT_SPI, pinSCK); 		// 1，时钟
				macBitOff(PORT_SPI, pinSCK); 		// 0，时钟
			}
		}
		
		macBitOn(PORT_LED_L, pinD_latch); 	// 1，锁存
		macBitOff(PORT_LED_L, pinD_latch); 	// 0，锁存信号
		
		SPI_Init(); 												// SPI开启初始化
	}
  
	blinkReload = TIMER_BLINK_RELOAD;
	
	return true;
}

//------------------------------------------------------------------------------
// LED_GetBrightness - get LED brightness
//------------------------------------------------------------------------------
TByte LED_GetBrightness(void)
{
  return EEPROM1.LED;
}

//------------------------------------------------------------------------------
// LED_Print_P - print a string from the Flash on the display
//------------------------------------------------------------------------------
__monitor void LED_Print_P(TByte led, TBool blink, char __flash *str)
{
  char buffer[10];
  
  strncpy_P(buffer, str, sizeof(buffer));
  LED_Print(led, blink, buffer);
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
    case LED_DISPLAY_RIGHT:
    {
      buffer=ledSpeed;
      ledBlinkSpeed=blink;
    }
    break;
    case LED_DISPLAY_LEFT:
    {
      buffer=ledTime;
      ledBlinkTime=blink;
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
  TWord left, right;    // left - time, right - speed

  //fetch LED's data
  left=CHARACTER_SET14[' '];
  right=CHARACTER_SET14[' '];
  if (ledShow)
  {  
    if ((ledBlinkTime&&(!ledBlinkState))||(!ledBlinkTime))
      left=ledTime[digit];
    if ((ledBlinkSpeed&&(!ledBlinkState))||(!ledBlinkSpeed))
      right=ledSpeed[digit];
  }
    
  if ((ledBlinkState)&&(ledBlinkPosYes))
  {
    if (ledBlinkPos==digit) left=0x00;
      else if ((ledBlinkPos-LED_NUM_DIGITS)==digit) right=0x00;
  }
      
  //load to the registers
  //initial condition
  if (globalPCBType) 
    macBitOff(PORT_LED_SEL, pinD_sel);     //AKC Platine
  else    
    macBitOn(PORT_LED_SEL, pinD_sel);     //Caleva Platine
  
  macBitOff(PORT_LED_L, pinD_latch);
  SPI_IOByte(HIBYTE(right));
  SPI_IOByte(LOBYTE(right));
  SPI_IOByte(HIBYTE(left));
  SPI_IOByte(LOBYTE(left));
  //unselect all  
  PORT_LED|=(1<<pinStrobe2)|(1<<pinStrobe1)|(1<<pinStrobe4)|(1<<pinStrobe3);
  macBitOn(PORT_LED_L, pinD_latch);
  macBitOff(PORT_LED_L, pinD_latch);

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
    digit=0;
  }
  
  blinkCounter++;
  if (blinkCounter>=blinkReload)
  {
    ledBlinkState=!ledBlinkState;
    blinkCounter=0;
  }
}

//------------------------------------------------------------------------------
// LED_Alarm - control Alarm LED
//------------------------------------------------------------------------------
void LED_Alarm(TBool On)
{
  if (On)
    macBitOff(PORT_LED_1, pinLED1);
  else
    macBitOn(PORT_LED_1, pinLED1);
}

//------------------------------------------------------------------------------
// LED_Printer - control Printer LED
//------------------------------------------------------------------------------
/*void LED_Printer(TBool On)
{
  if (On)
    macBitOff(PORT_LED_1,pinLED_1);
  else
    macBitOn(PORT_LED_1,pinLED_1);
}*/

//------------------------------------------------------------------------------
// LED_Init - init module
//------------------------------------------------------------------------------
void LED_Init(void)
{
  TByte i;

  //reset buffer
  for (i=0;i<LED_NUM_DIGITS;i++)
  {
    ledSpeed[i]=ledTime[i]=0xFFFF;
  }
  //ledSpeed[LED_NUM_DIGITS-1]=ledTime[LED_NUM_DIGITS-1]=0x0000;
  i=EEPROM1.LED;
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
void LED_Check(TByte Position, TByte Segment)
{
  TByte i;
  
  if (Position>LED_NUM_DIGITS)
    return;
  
  if (Position)
  {  
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledSpeed[i]=ledTime[i]=0x0000;
    if (!Segment)
      ledSpeed[Position-1]=ledTime[Position-1]=0xFFFF;
    else
      ledSpeed[Position-1]=ledTime[Position-1]=CHARACTER_SET14[127+Segment];
  } else
  {
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledSpeed[i]=ledTime[i]=0xFFFF;
  }  
}

//------------------------------------------------------------------------------
// LED_SwitchOff - switch off LEDs
//------------------------------------------------------------------------------
void LED_SwitchOff(void)
{
  TByte i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
    ledSpeed[i]=ledTime[i]=0x0000;
}

//------------------------------------------------------------------------------
// LED_Show - show LEDs or not
//------------------------------------------------------------------------------
void LED_Show(TBool On)
{
  ledShow=On;
}
