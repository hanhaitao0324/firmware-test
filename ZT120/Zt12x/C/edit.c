#include <stdio.h>

#include "edit.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "platform.h"
#include "sound.h"
#include "strings.h"
#include "timedate.h"
#include "version.h"

//------------------------------------------------------------------------------
// EDIT_Bool - edit boolean
//------------------------------------------------------------------------------
TBool EDIT_Bool(char __flash *title)
{
  TByte key;
  char val_yes[]=" YES";
  char val_no[]="  NO";
  TBool val=true;
    
  //print title
  //LED_Print_P(LED_DISPLAY_TEMP, false, title);
	LED_Print_P(LED_DISPLAY_TIME, false, title); 							// 2017-11-9 星期四 13:50:21
	
  for (;;)
  {
    if (val) 
    	//LED_Print(LED_DISPLAY_SPEED, false, val_yes);
    	LED_Print(LED_DISPLAY_TEMP, false, val_yes); 				// 2017-11-9 星期四 13:51:10
    else 
    	//LED_Print(LED_DISPLAY_SPEED, false, val_no);
    	LED_Print(LED_DISPLAY_TEMP, false, val_no); 					// 2017-11-9 星期四 13:51:17
          
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
        
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        if (val)
          return true;
        else
          return false;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        if (!val)
          val=true;
        SND_Play(ssSlowUp);//TMR0_INT_RATE/8);
        break;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        if (val)
          val=false;
        SND_Play(ssSlowDown);//TMR0_INT_RATE/8);           
        break;
      default:
        SND_Play(ssError);//TMR0_INT_RATE/2);
        break;
    }       
  }
}

//------------------------------------------------------------------------------
// EDIT_Integer - edit integer
//------------------------------------------------------------------------------
TBool EDIT_Integer(char __flash *title,char __flash *format,TWord *value,
  TWord lo,TWord hi,TEditCallback *callback)
{
  char str[10];
  TWord val;
  TByte key;
    
  //print title
  //LED_Print_P(LED_DISPLAY_TEMP,false,title); 								// 温度显示数码管位置
  LED_Print_P(LED_DISPLAY_TIME,false,title); 								// 2017-11-9 星期四 13:48:26
  
  val=*value;
  for (;;)
  {
    //format and print value
    
    sprintf_P(str,format,val);
    //LED_Print(LED_DISPLAY_SPEED,false,str); 								// 速度显示数码管位置
    LED_Print(LED_DISPLAY_TEMP,false,str); 								// 2017-11-9 星期四 13:49:03
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        *value=val;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        val++;
        if (val>hi)
        {  
          val=hi;
          SND_Play(ssError);
        }
        else 
        	SND_Play(ssSlowUp);
        
        if (callback)
          callback(val);
        break;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        if ((signed short)val-1>(signed short)lo)
        {  
          val--;
          SND_Play(ssSlowDown);
        }
        else
        {  
          val=lo;
          SND_Play(ssError);
        }
        
        if (callback)
          callback(val);
        break;
      default:
        SND_Play(ssError);
        break;
    }       
  }
}

//------------------------------------------------------------------------------
// EDIT_Time - edit time in minutes
//------------------------------------------------------------------------------
TBool EDIT_Time(char __flash *title, TWord *minutes)
{
#define MAX_MINUTES   (60*24)

  char str[10];
  TWord val;
  TByte key;
  
  //print title
  //LED_Print_P(LED_DISPLAY_TEMP,false,title);
  LED_Print_P(LED_DISPLAY_TIME,false,title); 			// 2017-11-9 星期四 13:52:26
  val=*minutes;
  for (;;)
  {
    //format and print value
    sprintf_P(str, timeFmt, val/60, val%60);
    //LED_Print(LED_DISPLAY_SPEED,false,str);
		LED_Print(LED_DISPLAY_TEMP,false,str); 			// 2017-11-9 星期四 13:52:33
		
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        //SND_Play(ssOk);
        *minutes=val;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        val++;
        if (val==MAX_MINUTES) val=0;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        if (val)
          val--;
        else
          val=MAX_MINUTES-1;
        SND_Play(ssSlowDown);
        break;
      //case KBD_CODE_TEMP_UP:
      case KBD_CODE_QUP:
        val+=60;
        if (val>=MAX_MINUTES) val-=MAX_MINUTES;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_TEMP_DOWN:
      case KBD_CODE_QDOWN:
        if (val>=60)
          val-=60;
        else
          val=val+MAX_MINUTES-60;
        SND_Play(ssSlowDown);
        break;
      default:
        SND_Play(ssError);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// EDIT_Date - edit date (day and month)
//------------------------------------------------------------------------------
TBool EDIT_Date(TTimeDate *dt)
{
  char str[10];
  TByte key;
  TTimeDate tmpDt=*dt;

  for (;;)
  {
    // 駂eck date
    TIMEDATE_Validate(&tmpDt);

    //LED_Print_P(LED_DISPLAY_TEMP, false, TIMEDATE_MonthName[tmpDt.Month]);
    LED_Print_P(LED_DISPLAY_TIME, false, TIMEDATE_MonthName[tmpDt.Month]); 			// 2017-11-9 星期四 13:53:56
    sprintf_P(str, dayFmt, tmpDt.Day);
    //LED_Print(LED_DISPLAY_SPEED, false, str);
		LED_Print(LED_DISPLAY_TEMP, false, str); 								// 2017-11-9 星期四 13:54:05
		
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }

    key=KBD_GetKey();
    switch(key)
    {
      case KBD_CODE_START:
        //SND_Play(ssOk);
        *dt=tmpDt;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return false;
      //case KBD_CODE_TEMP_UP:
      case KBD_CODE_QUP:
        tmpDt.Month++;
        SND_Play(ssFastUp);
        break;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        tmpDt.Day++;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_TEMP_DOWN:
      case KBD_CODE_QDOWN:
        if (tmpDt.Month)
        tmpDt.Month--;
        SND_Play(ssFastDown);
        break;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        if (tmpDt.Day)
          tmpDt.Day--;
        SND_Play(ssSlowDown);           
        break;
      default:
        SND_Play(ssError);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// EDIT_Show - just show text
//------------------------------------------------------------------------------
TBool EDIT_Show(char *title, char *text, TBool blink)
{
  //LED_Print(LED_DISPLAY_TEMP, false, title);
  //LED_Print(LED_DISPLAY_SPEED, blink, text);
  LED_Print(LED_DISPLAY_TIME, false, title); 			// 2017-11-9 星期四 14:10:23
  LED_Print(LED_DISPLAY_TEMP, blink, text); 			// 2017-11-9 星期四 14:10:29
  
  while(!KBD_KeyPressed())
  {
    EXTERNAL_Process();
  }
  
  KBD_GetKey();
  
  SND_Play(ssCancel);
  
  return true;
}

//------------------------------------------------------------------------------
// EDIT_Block - block keyboard
//------------------------------------------------------------------------------
void EDIT_Block(void)
{
  while (1)
  {  
    while(!KBD_KeyPressed())
      continue;
    KBD_GetKey();
    SND_Play(ssError);
  }
}

/*
//------------------------------------------------------------------------------
// EDIT_Password - enter password
//------------------------------------------------------------------------------
TBool EDIT_Password(char *title, char *tStr, TByte len)
  {
    //char tStr[10];
    //TWord val=0;
    TByte key;
    TByte curPos, initPos;
    
    //val=*value;
    ledBlinkPosYes = true;
    ledBlinkPos = LED_NUM_DIGITS*2 - len;
    initPos = len - LED_NUM_DIGITS;
    curPos = 0;
    for (;;)
      {
        //print title
        memcpy(&title[2], tStr, initPos);
        LED_Print(LED_DISPLAY_TEMP,false,title);
        //format and print value
        //sprintf(str,format,val);
        //LED_Print(LED_DISPLAY_RIGHT,false,str);
        LED_Print(LED_DISPLAY_SPEED,false,&tStr[initPos]);
        //process
        while(!globalDeviceStatus.kbdHit)
        {
            EXTERNAL_Process();
        }
        key=KBD_GetKey();

        switch(key)
          {
            case KBD_CODE_START:
              SND_Play(ssOk);
              ledBlinkPosYes=false;
              return true;
            case KBD_CODE_STOP:
              SND_Play(ssCancel);
              ledBlinkPosYes=false;
              return false;
            case KBD_CODE_TEMP_DOWN:
              tStr[curPos]--;
              SND_Play(ssSlowUp);
              break;
            case KBD_CODE_TEMP_UP:
              tStr[curPos]++;
              SND_Play(ssSlowUp);
              break;
            case KBD_CODE_SPEED_DOWN:
              if (!curPos) curPos=len-1;
                else curPos--;
              ledBlinkPos = curPos+LED_NUM_DIGITS;
              SND_Play(ssSlowDown);          
              break;
            case KBD_CODE_SPEED_UP:
              curPos++;
              if (curPos==len) curPos=0;
              ledBlinkPos = curPos+LED_NUM_DIGITS;
              SND_Play(ssSlowDown);          
              break;
            default:
              SND_Play(ssError);
              break;
          }       
          if (tStr[curPos]>0x39)
            tStr[curPos]=0x30;
          if (tStr[curPos]<0x30)
            tStr[curPos]=0x39;
      }
  }
*/

//------------------------------------------------------------------------------
// EDIT_Advanced - advanced edit (serial number and password)
//------------------------------------------------------------------------------
TBool EDIT_Advanced(char *title, char *tStr, TByte len, TBool sound)
{
  //char str[10];
  TByte key;
  TByte initPos, curPos, stPos;
    
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  //stPos = LED_NUM_DIGITS*2 - len;
  if(len > 4)
  {
    stPos = 8-len+LED_NUM_DIGITS;
  }
  else
  {
    stPos = LED_NUM_DIGITS - len; 				// 闪烁位置从左1位改变到右1位
  }
  
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
    //print title
    memcpy(&title[2], tStr, initPos);
    //LED_Print(LED_DISPLAY_TEMP,false,title);
    LED_Print(LED_DISPLAY_TIME,false,title); 			// 2017-11-9 星期四 14:11:02
    //format and print value
    //sprintf(str,format,val);
    //LED_Print(LED_DISPLAY_RIGHT,false,str);
    //LED_Print(LED_DISPLAY_SPEED,false,&tStr[initPos]);
    LED_Print(LED_DISPLAY_TEMP,false,&tStr[initPos]); 			// 2017-11-9 星期四 14:11:25
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        if (sound)
          SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        if (sound)
          SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
      	if(tStr[curPos] == '*')
      		tStr[curPos] = 0x30;
      	else
      		tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
      	if(tStr[curPos] == '*')
      		tStr[curPos] = 0x31;
      	else
      		tStr[curPos]++;
      	SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_TEMP_DOWN:
      case KBD_CODE_QDOWN:
        if (!curPos) 
        	curPos=len-1;
        else 
        	curPos--;
        
        ledBlinkPos=curPos+stPos;
        if(ledBlinkPos >= 8)
          ledBlinkPos -= 8;
        SND_Play(ssSlowDown);       
        break;
      //case KBD_CODE_TEMP_UP:
      case KBD_CODE_QUP:
        curPos++;
        if (curPos==len) 
        	curPos=0;
        
        ledBlinkPos=curPos+stPos;
        if(ledBlinkPos >= 8)
          ledBlinkPos -= 8;
        SND_Play(ssSlowDown);    
        break;
      default:
        SND_Play(ssError);
        break;
    }     
    
    if(tStr[curPos] != '*')
    {  
	    if (tStr[curPos]>0x39)
	      tStr[curPos]=0x30;
	    if (tStr[curPos]<0x30)
	      tStr[curPos]=0x39;
	  }
  }
}

//------------------------------------------------------------------------------
// EDIT_AdvancedPW - advanced edit (serial number and password)，v1.03，HHT，2024-06-14
//------------------------------------------------------------------------------
TBool EDIT_AdvancedPW(char *title, char *tStr, TByte len, TBool sound)
{
  TByte key;
  TByte initPos, curPos, stPos;
  char setStr[] = "****"; 							// Gavin，2019-10-10 星期四 11:19:39
	
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  //stPos = LED_NUM_DIGITS*2 - len;

  if(len > 4)
  {
    stPos = 8-len+LED_NUM_DIGITS;
  }
  else
  {
    stPos = LED_NUM_DIGITS - len; 				// 闪烁位置从左1位改变到右1位
  }  
  
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
  	setStr[0] = setStr[1] = setStr[2] = setStr[3] = '*'; 		// Gavin, 2019-10-10 星期四 11:31:56
  	setStr[curPos] = tStr[curPos];
  	
    //print title
    memcpy(&title[2], tStr, initPos);
    LED_Print(LED_DISPLAY_TIME,false,title);
    //print value
    //LED_Print(LED_DISPLAY_RIGHT,false,&tStr[initPos]);
    LED_Print(LED_DISPLAY_TEMP,false,setStr); 							// Gavin, 2019-10-10 星期四 11:32:42
    
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        if (sound)
          SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        if (sound)
          SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      case KBD_CODE_DOWN:
        tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_UP:
        tStr[curPos]++;
        SND_Play(ssSlowUp);
        break;
      case KBD_CODE_QDOWN:
//        if (!curPos) curPos=len-1;
//          else curPos--;
//        ledBlinkPos=curPos+stPos;
//        SND_Play(ssSlowDown);

        if (!curPos) 
        	curPos=len-1;
        else 
        	curPos--;
        
        ledBlinkPos=curPos+stPos;
        if(ledBlinkPos >= 8)
          ledBlinkPos -= 8;
        SND_Play(ssSlowDown);         
        break;
      case KBD_CODE_QUP:
//        curPos++;
//        if (curPos==len) curPos=0;
//          ledBlinkPos=curPos+stPos;
//        SND_Play(ssSlowDown);

        curPos++;
        if (curPos==len) 
        	curPos=0;
        
        ledBlinkPos=curPos+stPos;
        if(ledBlinkPos >= 8)
          ledBlinkPos -= 8;
        SND_Play(ssSlowDown);         
        break;
      default:
        SND_Play(ssError);
        break;
    }       
    if (tStr[curPos]>0x39)
      tStr[curPos]=0x30;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x39;
  }
}

//------------------------------------------------------------------------------
// EDIT_Hex - edit HEX-Value
//------------------------------------------------------------------------------
TBool EDIT_Hex(char *title, char *tStr, TByte len)
{
  //char str[10];
  TByte key;
  TByte initPos, curPos, stPos;
    
  ledBlinkPosYes=true;
  initPos = len - LED_NUM_DIGITS;
  stPos = LED_NUM_DIGITS*2 - len;
  ledBlinkPos = stPos;
  curPos=0;
  for (;;)
  {
    //print title
    memcpy(&title[stPos], tStr, initPos);
    //LED_Print(LED_DISPLAY_TEMP, false, title);
    LED_Print(LED_DISPLAY_TIME, false, title); 		// 2017-11-9 星期四 14:11:56
    //format and print value
    //LED_Print(LED_DISPLAY_SPEED, false, &tStr[initPos]);
    LED_Print(LED_DISPLAY_TEMP, false, &tStr[initPos]); 		// 2017-11-9 星期四 14:12:19
    //process
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();

    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        ledBlinkPosYes=false;
        return true;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        ledBlinkPosYes=false;
        return false;
      //case KBD_CODE_SPEED_DOWN:
      case KBD_CODE_DOWN:
        tStr[curPos]--;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_SPEED_UP:
      case KBD_CODE_UP:
        tStr[curPos]++;
        SND_Play(ssSlowUp);
        break;
      //case KBD_CODE_TEMP_DOWN:
      case KBD_CODE_QDOWN:
        if (!curPos) curPos=len-1;
          else curPos--;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);       
        break;
      //case KBD_CODE_TEMP_UP:
      case KBD_CODE_QUP:
        curPos++;
        if (curPos==len) curPos=0;
        ledBlinkPos=curPos+stPos;
        SND_Play(ssSlowDown);    
        break;
      default:
        SND_Play(ssError);
        break;
    }       
    if (tStr[curPos]>0x46)
      tStr[curPos]=0x30;
    if ((tStr[curPos]>0x39)&&(tStr[curPos]<0x3D))
      tStr[curPos]=0x41;
    if (tStr[curPos]<0x30)
      tStr[curPos]=0x46;
    if ((tStr[curPos]<0x41)&&(tStr[curPos]>0x3D))
      tStr[curPos]=0x39;
  }
}

//------------------------------------------------------------------------------
// EDIT_Start - show logo and device id
//------------------------------------------------------------------------------
void EDIT_Start(void)
{
  TByte ix, jx, kx;
  TByte l_len, d_len;
  char logo[10];
  char device[10];
  char str[LED_NUM_DIGITS];
  char str2[LED_NUM_DIGITS];
  
  //2014/6/18 18:15:57------------------------------------------------
  switch(EEPROM1.FirmLogo)
  {
  	case DEVICE_LOGO_ERWEKA:
  	{
  		switch(globalDeviceType)
	    {
	    	case DEVICE_ZT121:        // Gavin,171203-1100
	    		ix = 1;
	    		sprintf_P(device, devFmt, (int)ix);
	    	break;
	    	case DEVICE_ZT122:
	    		ix = 2;
	    		sprintf_P(device, devFmt, (int)ix);
	    	break;
	    	default: ;
	    }
  	}break;
  	// Gavin, 2019-3-26 15:10:29, A2.2
  	/*
  	case DEVICE_LOGO_CALEVA:
  	{
  		switch(globalDeviceType)
	    {
	    	case DEVICE_ZT121:
	    		ix = 1;
	    		sprintf_P(device, devFmtCaleva1, (int)ix);
	    	break;
	    	case DEVICE_ZT122:
	    		ix = 2;
	    		sprintf_P(device, devFmtCaleva2, (int)ix);
	    	break;
	    	default: ;
	    }
  	}break;
  	case DEVICE_LOGO_TABTEST:
  	{
  		switch(globalDeviceType)
	    {
	    	case DEVICE_ZT121:
	    		ix = 1;
	    		sprintf_P(device, devFmt1, (int)ix);
	    	break;
	    	case DEVICE_ZT122:
	    		ix = 2;
	    		sprintf_P(device, devFmt2, (int)ix);
	    	break;
	    	default: ;
	    }
  	}break;
  	case DEVICE_LOGO_QTEST:
  	{
  		switch(globalDeviceType)
	    {
	    	case DEVICE_ZT121:
	    		ix = 1;
	    		sprintf_P(device, devFmt1, (int)ix);
	    	break;
	    	case DEVICE_ZT122:
	    		ix = 2;
	    		sprintf_P(device, devFmt2, (int)ix);
	    	break;
	    	default: ;
	    }
  	}break;
    */
  	default: ;
  }
  //2014/6/18 18:15:57------------------------------------------------
  
  strcpy_P(logo, DEVICE_LOGO[EEPROM1.FirmLogo]);

  for (ix=0; ix<LED_NUM_DIGITS; ix++)
  {  
    str[ix]=' ';
    str2[ix]=' ';
  }
  l_len=strlen(logo);
  d_len=strlen(device);
  if (d_len>l_len)
    ix=d_len;
  else
    ix=l_len;
  ix+=LED_NUM_DIGITS;
  
  for (jx=0; jx<=ix; jx++)
  {
    //LED_Print(LED_DISPLAY_TEMP, false, str);
    //LED_Print(LED_DISPLAY_TIME, false, str2);
    LED_Print(LED_DISPLAY_TIME, false, str); 			// 2017-11-9 星期四 14:15:23
    LED_Print(LED_DISPLAY_TEMP, false, str2); 		// 2017-11-9 星期四 14:15:27
    
    TIMER_Start(TIMER_GENERAL, TO_200MSEC);
    while(!TIMER_TimeOut(TIMER_GENERAL));
    TIMER_Stop(TIMER_GENERAL);
    for (kx=1; kx<LED_NUM_DIGITS; kx++)
    {  
      str[kx-1]=str[kx];
      str2[kx-1]=str2[kx];
    }  
    if (jx<l_len)
      str[LED_NUM_DIGITS-1]=logo[jx];
    else
      str[LED_NUM_DIGITS-1]=' ';
    if (jx<d_len)
      str2[LED_NUM_DIGITS-1]=device[jx];
    else
      str2[LED_NUM_DIGITS-1]=' ';
  }  
}
