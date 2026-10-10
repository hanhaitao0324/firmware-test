#include <stdio.h>

#include "adc.h"
#include "calibrate.h"
#include "commands.h"
#include "confmenu.h"
#include "control.h"
#include "defines.h"
#include "dropper.h"
#include "edit.h"
#include "eeprom.h"
#include "ekey.h"
#include "engineering.h"
#include "global.h"
#include "kbd.h"
#include "led.h"      
#include "motor.h"
#include "parser.h"
#include "pins.h"
#include "platform.h"
#include "printrtc.h"
#include "report.h"
#include "rs232.h" 											// 2017-10-24 星期二 23:02:15，不使用串口，保留程序结构
#include "rs485.h"
#include "usb.h"
#include "sound.h"
#include "spi.h"
#include "temp.h"
#include "timer.h"
#include "fram.h"
#include "dhc.h"
#include "sampler.h"
#include "stepper.h"
#include "strings.h"
#include "syserror.h"
#include "testsetup.h"
#include "test.h"
#include "version.h"

#include "heater.h"

//------------------------------------------------------------------------------
// DeviceInit - init the device
//------------------------------------------------------------------------------
void DeviceInit(void)
{
  __disable_interrupt();
  //configure ports
  PORTA=iniPORTA;
  DDRA=iniDDRA;
  PORTB=iniPORTB;
  DDRB=iniDDRB;
  PORTC=iniPORTC;
  DDRC=iniDDRC;
  PORTD=iniPORTD;
  DDRD=iniDDRD;
  PORTE=iniPORTE;
  DDRE=iniDDRE;
  PORTF=iniPORTF;
  DDRF=iniDDRF;
  PORTG=iniPORTG;
  DDRG=iniDDRG;
  //read globals

  if (!EEPROM_CheckIntegrity())
    EEPROM_Restore();
  if (!ECalibr_CheckIntegrity())
    ECalibr_Restore();
  
  EEPROM_Check();

  //check PCB type and renew it if needed
  if ((EEPROM1.PCBType==PCB_UNKNOWN)||(EEPROM1.PCBType>PCB_NEW))
  {  
    if (!macBitTest(PIN_PCB_VER, pinPCB_Version)) 					// DM133(R80不接),TLC5927(R80焊接)
      globalPCBType=PCB_NEW;
    else
      globalPCBType=PCB_OLD;
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PCBType, &globalPCBType, sizeof(TByte));
  }
  globalPCBType=EEPROM1.PCBType-1;  
	
  //globalDeviceType=EEPROM1.DeviceType;
  // Gavin 20131227---------------------
  if(EEPROM1.DeviceType >= DEVICE_LAST)
  {
  	EEPROM_Restore();
  }
  globalDeviceType=EEPROM1.DeviceType;
  
  globalDeviceStatus.Head=EEPROM1.Head;
  globalDeviceStatus.remoteMode=false;
  globalDeviceStatus.portMode=EEPROM1.Port;
  if (!EEPROM1.MotorError)
    globalDeviceStatus.motorErrorSwitch=true;
  else
    globalDeviceStatus.motorErrorSwitch=false;

  CALIBRATE_Init();
  //configure peripherals
  //SPI
  SPI_Init();
  
  //timers
  TIMER_Init();
  
  //ADC
  ADC_Init(SPI_ADC0);
  ADC_Init(SPI_ADC1);
  
  //RS-232
  //RS232_Init(globalDeviceStatus.portMode);
  
  //RS-485
  RS485_Init();
  
  //USB
  USB_Init();
  
  //motor module
  MOTOR_Init();
  
  //temperature module
  TEMP_Init();
  
  //sound
  SND_Init();
  
  //FRAM
  FRAM_Init();
  
  //DHC module
  DHC_Init();
  
  //dropper module
  DROPPER_Init();
  
  //stepper module
  //if (globalDeviceType>=DEVICE_DT724)
  //  STEPPER_Init(); 				// 2017-11-7 星期二 21:54:55
  
  //autosampler module
  SAMPLER_Init();
	
	// 加热器初始化
	HEATER_Init();
	
  __enable_interrupt();

  if (globalDeviceStatus.haveDropper)
    DROPPER_Close();

  //LED
  LED_Init();
  SND_Play(ssOn);
  
  macBitOff(PORT_LED0, pinLED); 				// 点亮PCB贴片LED指示灯
}

//------------------------------------------------------------------------------
// main routine
//------------------------------------------------------------------------------
__C_task void main(void)
{                
  TByte key;      
  
  DeviceInit();
  
  STEPPER_Start(false, true);
  TIMER_Start(TIMER_GENERAL,TO_500MSEC);
  while(!TIMER_TimeOut(TIMER_GENERAL));
  TIMER_Stop(TIMER_GENERAL); 
  
  STEPPER_Stop();
  globalDeviceStatus.haveStepper=false;
  
  if (EEPROM1.Ekey)
  {
    if (EKEY_Init())
    {  
      globalDeviceStatus.ekeyStatus=EKEY_STATUS_OK;
    }
    else
    {
      globalDeviceStatus.ekeyStatus=EKEY_STATUS_BLOCKED;
    }  
  }  
  
  //check printer board
  PRN_Check();
  
  //re-init DHC because of starting delay
  DHC_Init();
  
  SAMPLER_SetActive(true);
    
  key = kbdCurrentKeys;
  
  TEMP_GetValue(ADC_CHNL_EXTERNAL);
  
  if (key==KBD_MASK_ENGINEERING) 				// 开始+停止+Up
  {
    // show version
    //LED_Print_P(LED_DISPLAY_TEMP, false, verStr);        
    //LED_Print_P(LED_DISPLAY_SPEED, false, DEVICE_VERSION);        
    LED_Print_P(LED_DISPLAY_TIME, false, verStr); 					// 2017-11-9 星期四 13:58:15
    LED_Print_P(LED_DISPLAY_TEMP, false, DEVICE_VERSION); 	// 2017-11-9 星期四 13:58:23
    
    TIMER_Start(TIMER_GENERAL,TO_1SEC);
    while(!TIMER_TimeOut(TIMER_GENERAL));
    TIMER_Stop(TIMER_GENERAL);       

    ENGINEERING_Menu(); 								// 工程菜单
  }
  else
  if(key==KBD_MASK_CONFIGURATION) 			// 开始+停止
  {
    CONFIG_Menu(); 											// 配置菜单
  }
  
  EDIT_Start(); 												// 显示滚动字幕

  TESTSETUP_SetTemp(); 									// 温度设置
  
  for (;;)
  {
    TESTSETUP_Run(); 										// 设置转速和时间，本项目中转速固定30转/分
    if (TEST_Run())
    {
      //print report
      switch (EEPROM1.Print)
      {
        case PRINTER_MODE_AUTO:
				{
					//REPORT_Print(REPORT_Test); 						// Gavin 2015/12/2 10:02:57
					
					if(EEPROM1.ReportPType == PTYPE_MODE_NORM) 				// A4普通纸打印机
		  			REPORT_Print(REPORT_Test_NORM);
		  		else
		  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT) 				// 热敏纸微型打印机
		  			REPORT_Print(REPORT_Test_PORT);
				}break;
        case PRINTER_MODE_ASK:
				{
					if (globalDeviceStatus.installedPrinter)
					{
						if (EDIT_Bool(printReq))
						{
							//REPORT_Print(REPORT_Test); 				// Gavin 2015/12/2 10:02:57
							
							if(EEPROM1.ReportPType == PTYPE_MODE_NORM)
				  			REPORT_Print(REPORT_Test_NORM);
				  		else
				  		if(EEPROM1.ReportPType == PTYPE_MODE_PORT)
				  			REPORT_Print(REPORT_Test_PORT);
						}
					}
				}break;
        case PRINTER_MODE_OFF:
        default:
          break;
      }
    }
    
    //HEATER_Regulate(); 									// 加热器调整，2017-11-7 星期二 20:07:15
  }
}

