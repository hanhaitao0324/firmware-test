#include "confmenu.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "engineering.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "pins.h"
#include "platform.h"
#include "spi.h"
#include "report.h"
#include "rs485.h"
#include "printrtc.h"
#include "sound.h"
#include "strings.h"
#include "test.h"
#include "testsetup.h"
#include "timer.h"
#include "usb.h"
#include "version.h"

//=============================================================
// DeviceInit
//=============================================================
void DeviceInit(void)
{
  __disable_interrupt();

  //configure ports
  DDRA=iniDDRA;
  PORTA=iniPORTA;
  DDRB=iniDDRB;
  PORTB=iniPORTB;
  DDRC=iniDDRC;
  PORTC=iniPORTC;
  DDRD=iniDDRD;
  PORTD=iniPORTD;
  PORTE=iniPORTE;
  DDRE=iniDDRE;
  PORTF=iniPORTF;
  DDRF=iniDDRF;
  PORTG=iniPORTG;
  DDRG=iniDDRG;

  if (!EEPROM_CheckIntegrity())
    EEPROM_Restore();
  
  EEPROM_Check();
	
	//check PCB type and renew it if needed
	if((!macBitTest(PIN_PCB_VER, pinPCB_Version))&& 					// PA6 = 0
		 (macBitTest(PIN_PCB_VER, pinNC_A7))) 									// PA7 = 1
	{
		globalPCBType = PCB_NEW;
	}
	else
	if((!macBitTest(PIN_PCB_VER, pinPCB_Version))&& 					// PA6 = 0
		 (!macBitTest(PIN_PCB_VER, pinNC_A7))) 									// PA7 = 0
	{
		globalPCBType = PCB_DM13H;
	}
	else 																											// 其它
		globalPCBType = PCB_OLD;
	
	//if ((EEPROM1.PCBType==PCB_UNKNOWN)||(EEPROM1.PCBType>PCB_NEW))
	if(EEPROM1.PCBType != globalPCBType) 											// Gavin, 2021-12-22 星期三 14:35:24
	{
		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PCBType, &globalPCBType, sizeof(TByte));
	}
	
	globalPCBType=EEPROM1.PCBType-1;  

  SPI_Init();
  MOTOR_Init();
  SND_Init();
  LED_Init();
  KBD_Init();
  TIMER_Init();
  RS485_Init();
  USB_Init();

  __enable_interrupt();

  SND_Play(ssOn);
}

// 重新打印报告，Gavin，v2.20 2019-08-30 星期五 14:03:01
TByte RetryPrn(void)
{
	TByte key;
	
	LED_Print_P(LED_DISPLAY_LEFT, false, printStr0);
	LED_Print_P(LED_DISPLAY_RIGHT, false, retryStr);
	
	for(;;)
	{
		while(!KBD_KeyPressed());
		
		key=KBD_GetKey();
		if(key == KBD_CODE_START)
		{
			SND_Play(ssOk);
			break;
		}
		else
		if(key == KBD_CODE_STOP)
		{
			SND_Play(ssStop);
			break;
		}
		else
		{
			SND_Play(ssError);        //repeatable
		}
	}
	
	return key;
}

__C_task void main(void)
{               
  TByte key;

  //init device
  DeviceInit();
  
  //wait - show all LED segments
  TIMER_Start(TIMER_GENERAL, TO_1SEC);
  while(!TIMER_TimeOut(TIMER_GENERAL));
  TIMER_Stop(TIMER_GENERAL);

  if (EEPROM1.Ekey)
  {
    if (EKEY_Init())
    {  
      globalStatus.ekeyStatus=EKEY_STATUS_OK;
    } 
    else
    {
      globalStatus.ekeyStatus=EKEY_STATUS_BLOCKED;
    }  
  }  

  //check printer board
  PRN_Check();

  LED_SetBrightness(EEPROM1.LED);
  
  key = kbdCurrentKeys;
  if (key==KBD_MASK_ENGINEERING)
  {
    // display version of firmware
    LED_Print_P(LED_DISPLAY_LEFT,false,VERSION_Str);
    LED_Print_P(LED_DISPLAY_RIGHT,false,VERSION_Revision);
      
    TIMER_Start(TIMER_GENERAL,TO_1SEC);
    while(!TIMER_TimeOut(TIMER_GENERAL));
    TIMER_Stop(TIMER_GENERAL); 
    if (key==KBD_MASK_ENGINEERING)
      ENGINEERING_Menu();
  }
#ifdef DEVICE_TAR
  if (key==KBD_MASK_CONFIGURATION)
  {
    // display version of firmware
    LED_Print_P(LED_DISPLAY_LEFT,false,VERSION_Str);
    LED_Print_P(LED_DISPLAY_RIGHT,false,VERSION_Revision);
      
    TIMER_Start(TIMER_GENERAL,TO_1SEC);
    while(!TIMER_TimeOut(TIMER_GENERAL));
    TIMER_Stop(TIMER_GENERAL); 
    if (key==KBD_MASK_CONFIGURATION)
      CONFIG_Menu();
  }
#endif

  EDIT_Start();
      
  globalStatus.enableRxTx=true;
  
  for (;;)
  {
    globalTestStatus.State=TEST_STOPPED;
    //CTL_NumStatRecords = SPEED_BUF_LEN;
    TESTSETUP_Run();
    if ((TEST_Run())&&(globalStatus.installedPrinter))
    {
      //print report
      //switch (eepromPrint)
      
			switch (EEPROM1.Print)
			{
			  case PRINTER_MODE_AUTO:
			  {
			    for (;;) 													// Gavin，v2.20，2019-08-30 星期五 12:39:14
		  		{
		  			REPORT_Print(REPORT_TYPE_TEST);//TestReport);
		  			if(RetryPrn() == KBD_CODE_STOP)
		    			break;
		  		}
			  }break;
			  case PRINTER_MODE_ASK:
			  {
			    if (globalStatus.installedPrinter)
				  {
				    if (EDIT_Bool(printReq))
				    {
				    	for (;;) 													// Gavin，v2.20，2019-08-30 星期五 12:39:14
		  				{
		  					REPORT_Print(REPORT_TYPE_TEST);//TestReport);
		  					if(RetryPrn() == KBD_CODE_STOP)
		    					break;
				      }
				    }
				  }
				}break;
			  case PRINTER_MODE_OFF:
			  {
			  	;
				}break;
			  default:
				break;
			}
    }
  }
}
