#include "dhc.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "motor.h"
#include "platform.h"
#include "sound.h"
#include "strings.h"
#include "syserror.h"

void ERROR_Check(void)
{
  TByte j;
  //TReal32 temp;
  
  if (globalDeviceStatus.motorFailure)
  {
    MOTOR_Stop();
    switch (globalDeviceStatus.motorFailure)
    {
      case MOTOR_ERROR_CURR:
        LED_Print_P(LED_DISPLAY_TEMP, true, errorMotorCurr);
        break;
      case MOTOR_ERROR_ENC:
        LED_Print_P(LED_DISPLAY_TEMP, true, errorMotorEnc);
        break;
      default:
        LED_Print_P(LED_DISPLAY_TEMP, true, errorMotorUnkn);
        break;
    }  
    LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);      
   
    MOTOR_Stop();
    globalTestStatus.State=TEST_MOTORSTOPPED;
    globalDeviceStatus.remoteMode=false;
    for(;;)
    {
      while(!KBD_KeyPressed())
        EXTERNAL_Process();
            
      j=KBD_GetKey();
         
      if (j==KBD_CODE_STOP)
      {
        globalDeviceStatus.motorFailure=MOTOR_ERROR_NO;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
    }
  }

  /*if (globalDeviceStatus.dhcFailure)
    {
      LED_Print_P(LED_DISPLAY_TEMP, true, errorDHC); 
      LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);
    
      MOTOR_Stop();
      globalTestStatus.State=TEST_STOPPED;
      for(;;)
        {
          while(!KBD_KeyPressed());        
          j=KBD_GetKey();
          if (j==KBD_CODE_STOP)
          {
            DHC_Init();
            SND_Play(ssStop);     
            break;
          }       
          SND_Play(ssError);     
       }
    }*/

  /*if (globalDeviceStatus.USBFailure)
  {
    LED_Print_P(LED_DISPLAY_TEMP, true, errorUSB); 
    LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);
    
    MOTOR_Stop();
    globalTestStatus.State=TEST_STOPPED;
    globalDeviceStatus.remoteMode=false;
    for(;;)
    {
      while(!KBD_KeyPressed());
      
      j=KBD_GetKey();
      if (j==KBD_CODE_STOP)
      {
        globalDeviceStatus.USBFailure=false;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
    }
  }*/

  if (globalDeviceStatus.dropFailure)
  {
    LED_Print_P(LED_DISPLAY_TEMP, true, errorDropper);
    LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);      
    
    globalTestStatus.State=TEST_STOPPED;
    globalDeviceStatus.remoteMode=false;
    for(;;)
    {
      while(!KBD_KeyPressed())        
        EXTERNAL_Process();

      j=KBD_GetKey();
         
      if (j==KBD_CODE_STOP)
      {
        globalDeviceStatus.dropFailure=false;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
     }
  }

  /*if (globalDeviceStatus.ovrHeating)
  {
    LED_Print_P(LED_DISPLAY_TEMP, true, errorOverHeat); 
    LED_Print_P(LED_DISPLAY_SPEED, true, errorStr);
    
    MOTOR_Stop();
    temp=0.0;
    DHC_SetTemp(&temp);
    globalTestStatus.State=TEST_STOPPED;
    globalDeviceStatus.remoteMode=false;
    for(;;)
    {
      while(!KBD_KeyPressed());        
      j=KBD_GetKey();
      if (j==KBD_CODE_STOP)
      {
        globalDeviceStatus.ovrHeating=false;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
    }
  }*/
    
}
