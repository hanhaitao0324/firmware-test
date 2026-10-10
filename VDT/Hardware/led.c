/*----------------------------------------------------------------------------
 *      Erweka - 7 Segment
 *----------------------------------------------------------------------------
 *      Name:    led.c
 *      Purpose: 7 Segment driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"    // Device header
#include "defines.h"
#include "pins.h"
#include "led.h"
#include "font.h"
#include "spi.h"
#include "utils.h"

extern u8 globalPCBType;

uint16_t blinkReload;
uint16_t blinkCounter; 

//bool ledCycle=false;

//blink status
bool ledBlinkLeft;
bool ledBlinkRight;
bool ledBlinkSpeed;
bool ledBlinkState;

//blink position
uint8_t ledBlinkPos = 0;
bool ledBlinkPosYes = false;
  
//buffers
uint16_t ledLeft[LED_NUM_DIGITS];
uint16_t ledRight[LED_NUM_DIGITS];

uint16_t ledTemp[LED_NUM_DIGITS*2];

const uint16_t BrightTab0[11] = {0x0000,0x0004,0x0002,0x0006,0x000E,0x0005,0x0003,0x000B,0x0007,0x000F,0x00FF};
const uint16_t BrightTab1[11] = {0x0004,0x0064,0x00C4,0x0124,0x0184,0x01E4,0x0244,0x02A4,0x0304,0x0364,0x03C4};
extern uint8_t dp1, dp2, dp3, dp4, dp10, dp20, dp30, dp40;

//------------------------------------------------------------------------------
// LED_Init - init module
//------------------------------------------------------------------------------
void LED_Init(void)
{
	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = D_SEL | D_LATCH;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin   = D_SCK | D_DATA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = STROBE1 | STROBE2 | STROBE3 | STROBE4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//reset buffer
	for (i=0; i<LED_NUM_DIGITS; i++)
		ledLeft[i] = ledRight[i] = 0xFFFF;
	
	GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET);  //OE
	GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
	
	//SPI_Select(SPI_LED);
	SPI1_IOByte(ledLeft[0]);
	SPI1_IOByte(ledRight[0]);
	
	//unselect all
	GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET);  //LE
	GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
	
	if (i>=LED_BRIGHTNESS_NUM)
		i = LED_BRIGHTNESS_1;
	
	LED_SetBrightness(i);
}

/*******************************************************************************
* Function Name  : TLC5927_spezial_Mode
* Description    : switch LED Driver to Confoguration Mode
* Input          : Special Mose 1, Normal Mode 0
* Output         : Writing Configuration Code in Special Mode 
* Return         : None
*******************************************************************************/
void TLC5927_SetMode(bool sp_mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = D_SCK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if (sp_mode)
	{
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 	// LE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_RESET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET); 	// LE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 	// LE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 	// LE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_RESET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET); 	// OE
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// CLK
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET);
	}
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin   = D_SCK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DM13H_SetMode(bool sp_mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(sp_mode)
	{
		SPI_Cmd(SPI1, DISABLE);
		
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin   = D_SCK | D_DATA;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_WriteBit(PORT_LED, pinLED_SEL, Bit_SET); 		// 1，片选信号
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 	// 0，数码管驱动芯片的时钟端
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 	// 0，锁存信号
		GPIO_WriteBit(PORT_LED, pinLED_SEL, Bit_RESET); 	// 0，片选
		
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET); 	// 1，锁存
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// 1，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 	// 0，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// 1，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 	// 0，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// 1，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 	// 0，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 		// 1，时钟
		GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 	// 0，时钟
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 	// 0，锁存信号
	}
	else 
	{
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin   = D_SCK | D_DATA;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		SPI_Cmd(SPI1, ENABLE);
	}
}

/*******************************************************************************
* Function Name  : LED_SetBrightness
* Description    : switch LED Driver to Confoguration Mode
* Input          : uint8_t brightness from 0x00 to 0xFF
* Output         : set LED brightness 16Bit
* Return         : true
*******************************************************************************/
bool LED_SetBrightness(uint8_t brightness)
{
	uint16_t temp;
	uint8_t i,j;
	
	//brightness = 2;
	//brightness = 10;
	
	if (brightness>=LED_BRIGHTNESS_NUM)
		return false;
	
	if(globalPCBType == PCB_TLC5927)
	{
		temp = BrightTab0[brightness];
		TLC5927_SetMode(SPEZIAL_MODE);	//switch LED Driver to Confoguration Mode
		
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET);//OE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
		
		SPI1_IOByte(temp);
		SPI1_IOByte(temp);
		
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET);//LE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
		
		TLC5927_SetMode(NORMAL_MODE);	//switch LED Driver to Confoguration Mode
	}
	else
	if(globalPCBType == PCB_DM13H)
	{
		DM13H_SetMode(SPEZIAL_MODE); 				// switch LED Driver to Confoguration Mode
		
		//temp = brightness*12;
		//temp <<= 3;
		//temp |= 0x0004;
		temp = BrightTab1[brightness];
		for(j=0;j<2;j++)
		{
			for(i=0;i<10;i++)
			{
				if(temp & 1<<(9-i))
					GPIO_WriteBit(PORT_LED, pinLED_DATA, Bit_SET); 		// 1，数据
				else
					GPIO_WriteBit(PORT_LED, pinLED_DATA, Bit_RESET); 	// 0, 数据
				
				GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_SET); 			// 1，时钟
				GPIO_WriteBit(PORT_LED, pinLED_SCK, Bit_RESET); 		// 0，时钟
			}
		}
		
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET); 				// 1，锁存
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET); 				// 0，锁存信号
		
		DM13H_SetMode(NORMAL_MODE); 				// switch LED Driver to Confoguration Mode
	}
	
	blinkReload = TIMER_BLINK_RELOAD;
	
	return true;
}

//------------------------------------------------------------------------------
// LED_GetBrightness - get LED brightness
//------------------------------------------------------------------------------
uint8_t LED_GetBrightness(void)
{
//  return EEPROM1.LED;
	return 0;
}

//------------------------------------------------------------------------------
// LED_Print - print a string on the display
//------------------------------------------------------------------------------
void LED_Print(uint8_t led, bool blink, char *str)
{
	uint16_t *buffer;
	uint8_t ch;
	uint8_t i;
	
	if (blink)
	{
		blinkCounter=0;
		ledBlinkState=false;
	}
	
	//setup buffers
	switch (led)
	{
		case LED_DISPLAY_LEFT:
		{
			buffer=ledLeft;
			ledBlinkLeft=blink;
		}break;
		case LED_DISPLAY_RIGHT:
		{
			buffer=ledRight;
			ledBlinkRight=blink;
		}break;
	}
	
	//transfer string
	i = 0;
	while ((LED_NUM_DIGITS>=i)&&(*str))
	{
		ch=*(str++);
//    if ((LED_NUM_DIGITS==i)&&(ch!='.'))
//      break;
//    if (ch=='.'){
//      if (i){
//        buffer[i-1]|=LED_MASK_DEC_POINT;
//        continue;
//      }
//      buffer[i++]=LED_MASK_DEC_POINT;
//      continue;
//    }
		
		if (ch>=CHARACTER_SET14_LEN)
			ch=0;
		
		buffer[i] = CHARACTER_SET14[ch];
		if(led==LED_DISPLAY_LEFT)
			if((i==0 && dp10) || (i==1 && dp20) || (i==2 && dp30) || (i==3 && dp40)) 
				buffer[i] |= LED_MASK_DEC_POINT;
		
		if(led==LED_DISPLAY_RIGHT)
			if((i==0 && dp1) || (i==1 && dp2) || (i==2 && dp3) || (i==3 && dp4)) 
				buffer[i] |= LED_MASK_DEC_POINT;
		
		i++;
	}
	
	//fill with blanks
	for (;i<LED_NUM_DIGITS;i++)
		buffer[i]=CHARACTER_SET14[' '];
}

//------------------------------------------------------------------------------
// LED thread
//------------------------------------------------------------------------------
void LEDRefresh_thread(const void *arg)
{
	static uint8_t digit=0;
	uint16_t left, right;
//	GPIO_InitTypeDef GPIO_InitStructure;
	
	while (1)
	{
		//fetch LED's data
		left  = CHARACTER_SET14[' '];
		right = CHARACTER_SET14[' '];
		
		if ((ledBlinkLeft && (!ledBlinkState)) || (!ledBlinkLeft))
			left = ledLeft[digit];
		
		if ((ledBlinkRight && (!ledBlinkState))|| (!ledBlinkRight))
			right = ledRight[digit];
		
		if ((ledBlinkState)&&(ledBlinkPosYes))
		{
			if (ledBlinkPos==digit) 
				left = 0x00;
			else if ((ledBlinkPos-LED_NUM_DIGITS)==digit)	
				right = 0x00;
		}
		
		//load to the registers
		//initial condition
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_SET);  //OE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
		
		//SPI_Select(SPI_LED);
		SPI1_IOByte(left);
		SPI1_IOByte(right);
		
		//unselect all
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_SET);  //LE
		GPIO_WriteBit(PORT_LED, pinLED_LATCH, Bit_RESET);//LE
		
//		GPIO_StructInit(&GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Pin   = STROBE1 | STROBE2;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_WriteBit(PORT_STROBE, pinStrobe1 | pinStrobe2, Bit_SET);
		
//		GPIO_StructInit(&GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Pin   = STROBE3 | STROBE4;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_WriteBit(PORT_STROBE, pinStrobe3 | pinStrobe4, Bit_SET);
		
		//select digit
		switch (digit)
		{
			case 0: 
//				GPIO_StructInit(&GPIO_InitStructure);
//				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//				GPIO_InitStructure.GPIO_Pin   = STROBE3;
//				GPIO_Init(GPIOC, &GPIO_InitStructure);
				GPIO_WriteBit(PORT_STROBE, pinStrobe3, Bit_RESET);
				break;
			case 1:
//				GPIO_StructInit(&GPIO_InitStructure);
//				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//				GPIO_InitStructure.GPIO_Pin   = STROBE4;
//				GPIO_Init(GPIOC, &GPIO_InitStructure);
				GPIO_WriteBit(PORT_STROBE, pinStrobe4, Bit_RESET);
				break;
			case 2:
//				GPIO_StructInit(&GPIO_InitStructure);
//				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//				GPIO_InitStructure.GPIO_Pin   = STROBE1;
//				GPIO_Init(GPIOC, &GPIO_InitStructure);
				GPIO_WriteBit(PORT_STROBE, pinStrobe1, Bit_RESET);
				break;
			case 3:
//				GPIO_StructInit(&GPIO_InitStructure);
//				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//				GPIO_InitStructure.GPIO_Pin   = STROBE2;
//				GPIO_Init(GPIOC, &GPIO_InitStructure);
				GPIO_WriteBit(PORT_STROBE, pinStrobe2, Bit_RESET);
				break;
		}
		
		GPIO_WriteBit(PORT_LED, pinLED_SEL,   Bit_RESET);  //OE
		
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
		
		osDelay(3);
	}
}

//------------------------------------------------------------------------------
// LED_Check - check LEDs
//------------------------------------------------------------------------------
void LED_Check(uint8_t Position)
{
  uint8_t i;
  
  if (Position>LED_NUM_DIGITS)
    return;
  
  if (Position)
  {  
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledLeft[i] = ledRight[i] = 0x0000;
    ledLeft[Position-1] = ledRight[Position-1] = 0xFFFF;
  } else
  {
    for (i=0; i<LED_NUM_DIGITS; i++)
      ledLeft[i] = ledRight[i] = 0xFFFF;
  }  
}

//------------------------------------------------------------------------------
// LED_SwitchOff - switch off LEDs
//------------------------------------------------------------------------------
void LED_SwitchOff(void)
{
  uint8_t i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
    ledLeft[i] = ledRight[i] = 0x0000;
}

//------------------------------------------------------------------------------
// LED_Store - store LEDs values
//------------------------------------------------------------------------------
void LED_Store(void)
{
  uint8_t i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
  {  
    ledTemp[i*3] = ledLeft[i];
    ledTemp[i*3+1] = ledRight[i];
  }   
}

//------------------------------------------------------------------------------
// LED_Restore - restore LEDs values
//------------------------------------------------------------------------------
void LED_Restore(void)
{
  uint8_t i;
  
  for (i=0; i<LED_NUM_DIGITS; i++)
  {  
    ledLeft[i] = ledTemp[i*3];
    ledRight[i] = ledTemp[i*3+1];
  }  
}
