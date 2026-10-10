#include "platform.h"
#include "syserror.h"
#include "global.h"
#include "led.h"
#include "strings.h"
#include "sound.h"
#include "strings.h"
#include "motor.h"
#include "kbd.h"

void SYSERROR_Check(void)
{
  TByte j;
 
  if (globalStatus.motorFailure)
  {
    MOTOR_Stop();
    LED_Print_P(LED_DISPLAY_LEFT, true, errorMotor);
    LED_Print_P(LED_DISPLAY_RIGHT, true, errorStr);      
    globalStatus.testStart=false;
    for(;;)
    {
      while(!KBD_KeyPressed());        
      j=KBD_GetKey();
         
      if (j==KBD_CODE_STOP)
      {
        globalStatus.motorFailure=false;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
    }
  }

  if (globalStatus.usbFailure)
  {
    LED_Print_P(LED_DISPLAY_LEFT, true, errorUSB); 
    LED_Print_P(LED_DISPLAY_RIGHT, true, errorStr);
    MOTOR_Stop();
    globalStatus.testStart=false;
    for(;;)
    {
      while(!KBD_KeyPressed());        
      j=KBD_GetKey();
      if (j==KBD_CODE_STOP)
      {
        globalStatus.usbFailure=false;
        SND_Play(ssStop);     
        break;
      }       
      SND_Play(ssError);     
    }
  }
}
