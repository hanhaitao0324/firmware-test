#include "const.h"
#include "control.h"
#include "edit.h"
#include "eeprom.h"
#include "engineering.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "report.h"
#include "sound.h"
#include "strings.h"
#include "version.h"

enum ENGMENU_SVM
{
  ENGMENU_TEST,
  ENGMENU_DEV_SNUM,
  ENGMENU_LOGO,
  ENGMENU_DEV_TYPE,
  ENGMENU_DEV_MODE,
  ENGMENU_PPR,
  
  // 增加RSPW菜单，v1.30，Gavin，2019-08-08 星期四 11:33:21
  ENGMENU_DEV_RSPW,
  
  ENGMENU_EXIT,
  ENGMENU_LAST
};

enum TEST_MODE
{
  TEST_MODE_LED,
  TEST_MODE_TIMER,
  TEST_MODE_MOTOR,
  TEST_MODE_PRINT,
  TEST_MODE_LAST
};

// 增加RSPW菜单，v1.30，Gavin，2019-08-08 星期四 12:05:46
enum RSPW_MODE
{
  RSPW_MODE_YES,
  RSPW_MODE_NO,
  RSPW_MODE_LAST
};

TMenuItemStr ENG_MENU_TITLE="DEV ";

TMenuItemStr ENG_MENU_SVM[ENGMENU_LAST]=
{
  "TEST",
  "SNUM",
  "LOGO",
  "TYPE",
  "MODL",
  "PPR",
  "RSPW", 					//增加密码复位菜单，v1.30，Gavin，2019-08-08 星期四 13:48:44
  "EXIT"
};

TMenuItemStr ENG_LIST_TEST[TEST_MODE_LAST]=
{
  "LED",
  "TIME",
  "MOTR",
  "PRNT"
};

TMenuItemStr ENG_LIST_LOGO[DEVICE_LOGO_LAST]=
{
  "ERW.",
  "CAL.",
  "TTST",
  "QTST"
};

TMenuItemStr ENG_LIST_TYPE[DEVICE_TYPE_LAST]=
{
  "SVM",
  "TDT"
};

/*TMenuItemStr ENG_LIST_MODE_1[DEVICE_MODE_LAST]=
{
  "121",
  "122",
  "221",
  "222",
  "223"
};

TMenuItemStr ENG_LIST_MODE_2[3]=
{
  " 12",
  " 22",
  " 23"
};*/

// 增加RSPW菜单显示，v1.30，Gavin，2019-08-08 星期四 12:03:50
TMenuItemStr ENG_LIST_RSPW[RSPW_MODE_LAST]=
{
  "YES ",
  "NO  "
};

//------------------------------------------------------------------------------
// ENGINEERING_Test - test mode
//------------------------------------------------------------------------------
TBool ENGINEERING_Test(void)
{
  signed char value;
  TList list;
  TByte Val;
  TByte Seg;
  TByte key;
  TBool stop;
  TByte DevMode;
  char str[10];
  
  DevMode=EEPROM1.DevMode;
  list.numItems=TEST_MODE_LAST;
  list.items=ENG_LIST_TEST;
  list.current=0;
  list.title=ENG_MENU_SVM[ENGMENU_TEST];
  for (;;)
  {
    value=MENU_RunList(&list);
    
    switch(value)
    {
      case TEST_MODE_LED:   // test LED
        Val=0;
        for (;;)
        {  
          Seg=0;
          while (!KBD_KeyPressed())
          {
            LED_Check(Val, Seg);
            TIMER_Start(TIMER_GENERAL, TO_200MSEC);
            while ((!TIMER_TimeOut(TIMER_GENERAL))&&(!KBD_KeyPressed()));
            TIMER_Stop(TIMER_GENERAL); 
            Seg++;
            if (Seg>15)
              Seg=0;
          }  

          if (KBD_KeyPressed())
          {
            key=KBD_GetKey();
            if (key==KBD_CODE_STOP)
            {
              SND_Play(ssCancel);
              break;
            } else
            {
              Val++;
              if (Val>LED_NUM_DIGITS)
                Val=0;
              SND_Play(ssOk);
            }
          }  
        }  
        break;
      case TEST_MODE_TIMER: // test timer
        TIMER_ResetTick();
        Val=60;
        CTL_ShowSpeed(0, false, false);
        CTL_ShowTime(Val, false);
        stop=false;
        while ((Val)&&(!stop))
        {  
          while (!TIMER_TimeTick())
          {
            if (KBD_KeyPressed())
            {
              KBD_GetKey();
              SND_Play(ssOk);
              stop=true;
              break;
            }
          }
          TIMER_ResetTick();
          Val--;
          CTL_ShowTime(Val, false);
        }
        SND_Play(ssOk);
        break;     
      case TEST_MODE_MOTOR: // test motor
        LED_Print_P(LED_DISPLAY_LEFT, false, ENG_LIST_TEST[TEST_MODE_MOTOR]);
        if ((DevMode==DEVICE_MODE_121)||(DevMode==DEVICE_MODE_221)||(DevMode==DEVICE_MODE_223))
        {  
          MOTOR_SetSpeed(SVM_SPEED_MODE1);
        } else
        {
          MOTOR_SetSpeed(SVM_SPEED_MODE2);
        }
        MOTOR_Start();
        while(!KBD_KeyPressed())
        {
          //show speed
          if (TIMER_TimeTick())
          {
            CTL_ShowSpeed((TReal32)MOTOR_GetSpeed()/10, false, true);
            TIMER_ResetTick();
          }
        }
        KBD_GetKey();        
        MOTOR_Stop();
        SND_Play(ssOk);
        break;
      case TEST_MODE_PRINT: // test printer
        if (EDIT_Bool(printReq))
          REPORT_Print(REPORT_TYPE_PRINTER);//PrinterReport);
        //SND_Play(ssOk);
        break;
      case -1:
        return true;
    }
  }

}

//------------------------------------------------------------------------------
// ENGINEERING_DevSNum - set device serial number
//------------------------------------------------------------------------------
TBool ENGINEERING_DevSNum(void)
{
  char title[]="N-  ";
  char tStr[SERIAL_NUMBER_LEN];
  TByte i;

  for (i=0;i<SERIAL_NUMBER_LEN;i++)
  {
    tStr[i]=EEPROM1.Serial[i]; 
  }
        
  if (EDIT_Advanced(title, tStr, SERIAL_NUMBER_LEN, true))
  {
    //for (i=0;i<SERIAL_NUMBER_LEN;i++)
    //  eepromSerial[i]=tStr[i]; 
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Serial, (TByte *)tStr, SERIAL_NUMBER_LEN);
  }
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_Logo - configure logo
//------------------------------------------------------------------------------
TBool ENGINEERING_Logo(void)
{
  signed char value;
  TList list;
    
  list.numItems=DEVICE_LOGO_LAST;
  list.items=ENG_LIST_LOGO;
  list.current=EEPROM1.FirmLogo;
  list.title=ENG_MENU_SVM[ENGMENU_LOGO];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FirmLogo, (TByte *)&value, sizeof(EEPROM1.FirmLogo));
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DevType - configure device type
//------------------------------------------------------------------------------
TBool ENGINEERING_DevType(void)
{
  signed char value;
  TList list;
    
  list.numItems=DEVICE_TYPE_LAST;
  list.items=ENG_LIST_TYPE;
  list.current=EEPROM1.DevType;
  list.title=ENG_MENU_SVM[ENGMENU_DEV_TYPE];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DevType, (TByte *)&value, sizeof(EEPROM1.DevType));
  //if ((value==DEVICE_TYPE_TDT)&&(EEPROM1.DevMode>DEVICE_MODE_221))  !!!!!
  //{
  //  value=0;
  //  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DevMode, (TByte *)&value, sizeof(EEPROM1.DevMode));
  //}
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DevMode - configure device mode
//------------------------------------------------------------------------------
TBool ENGINEERING_DevMode(void)
{
  signed char value;
  TList list;
    
  switch (EEPROM1.DevType)
  {
    case DEVICE_TYPE_SVM:
      list.numItems=DEVICE_MODE_LAST;
      break;
    case DEVICE_TYPE_TDT:
      //list.numItems=3;  !!!!!
      list.numItems=DEVICE_MODE_LAST;
      break;
  }  
  list.items=DEVICE_MODE[EEPROM1.DevType];
  list.current=EEPROM1.DevMode;
  list.title=ENG_MENU_SVM[ENGMENU_DEV_MODE];
  value=MENU_RunList(&list);
  if (value<0)
    return true;
  EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DevMode, (TByte *)&value, sizeof(EEPROM1.DevMode));
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_DevRspw - configure device reset password, v1.30, Gavin, 2019-08-08 星期四 11:38:40
//------------------------------------------------------------------------------
TBool ENGINEERING_DevRspw(void)
{
	char title[]="RSPW";
	signed char value;
	TList list;
	TWord pd = 0;
	
	list.numItems=RSPW_MODE_LAST;
	
	list.items=ENG_LIST_RSPW;
	list.current=RSPW_MODE_YES;
	list.title=ENG_MENU_SVM[ENGMENU_DEV_RSPW];
	
	value=MENU_RunList(&list);
	if (value == RSPW_MODE_YES)
	{
		pd = 807;
		if(CTL_CheckPW(title, pd) == 0)
		{
			pd = 9999;
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MGPassword, (TByte *)&pd, sizeof(EEPROM1.MGPassword));
			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DTPassword, (TByte *)&pd, sizeof(EEPROM1.DTPassword));
		}
	}
	
	return true;
}

TBool ENGINEERING_Exit(void)
{
  return false;
}

//------------------------------------------------------------------------------
// ENGINEERING_SetPPR - set motor PPR
//------------------------------------------------------------------------------
TBool ENGINEERING_SetPPR(void)
{
  TWord value;
  static __flash char format[]=" %3d";
  
  value=EEPROM1.PulsesPerRotat; 
  if (EDIT_Integer(ENG_MENU_SVM[ENGMENU_PPR], format, &value, MOTOR_PPR_MIN, MOTOR_PPR_MAX, NULL))
  {  
    EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PulsesPerRotat, (TByte *)&value, sizeof(EEPROM1.PulsesPerRotat));
    MOTOR_Init();
    SND_Play(ssOk);
  } else SND_Play(ssCancel);
  return true;
}

//------------------------------------------------------------------------------
// ENGINEERING_Pass - run engineering menu password check
//------------------------------------------------------------------------------
TBool ENGINEERING_Pass(void)
{
  TWord value;
  TByte ix;
  char title[]="PASS";
  char tStr[LED_NUM_DIGITS];
    
  for (ix=0; ix<LED_NUM_DIGITS; ix++)
    tStr[ix] = '*';
  value=0;
  if (EDIT_Advanced(title, tStr, LED_NUM_DIGITS, true))
  {  
    for (ix=0;ix<LED_NUM_DIGITS;ix++)
    {
      value*=10;
      value+=tStr[ix]-0x30; 
    }
    if (EEPROM1.Password==value) return true;
      else return false;
  }
  return false;
}

//------------------------------------------------------------------------------
// ENGINEERING_Menu - run engineering menu
//------------------------------------------------------------------------------
void ENGINEERING_Menu(void)
{
  TMenu menu;
  ptrTItemHandler handler[ENGMENU_LAST];
    
  if (!ENGINEERING_Pass()) return;

  menu.title=ENG_MENU_TITLE;
  menu.numItems=ENGMENU_LAST;
  menu.items=ENG_MENU_SVM;
  menu.handler=handler;
  handler[ENGMENU_TEST]=ENGINEERING_Test;
  handler[ENGMENU_DEV_SNUM]=ENGINEERING_DevSNum;
  handler[ENGMENU_LOGO]=ENGINEERING_Logo;
  handler[ENGMENU_DEV_TYPE]=ENGINEERING_DevType;
  handler[ENGMENU_DEV_MODE]=ENGINEERING_DevMode;
  handler[ENGMENU_PPR]=ENGINEERING_SetPPR;
  
  // 增加DEV菜单中的RSPW菜单，v1.30，Gavin，2019-08-08 星期四 11:35:50
  handler[ENGMENU_DEV_RSPW]=ENGINEERING_DevRspw;
  
  handler[ENGMENU_EXIT]=ENGINEERING_Exit;
    
  globalStatus.enableRxTx=true;
  MENU_Run(&menu);
  globalStatus.enableRxTx=false;
}
