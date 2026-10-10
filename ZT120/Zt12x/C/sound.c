#include "eeprom.h"
#include "pins.h"
#include "platform.h"
#include "kbd.h"
#include "sound.h"

TWord SNDDelay=0;
TByte SNDPos=0;
TSoundStream __flash *SNDStream;

TBool flgSND_Off=false;

// 声音流-----------------------------------------
__flash TSoundStream ssOn[]= 						// 打开
  {
     {MAX_FREQ/(NOTE_C*3), (NOTE_C*3)/32},
     {MAX_FREQ/(NOTE_E*3), (NOTE_E*3)/32},
     {MAX_FREQ/(NOTE_A*2), (NOTE_A*2)/16},
     {0,0}
  };

__flash TSoundStream ssStart[]= 				// 开始
  {
     {MAX_FREQ/(NOTE_A*2), (NOTE_A*2)/32},
     {MAX_FREQ/(NOTE_C*3), (NOTE_C*2)/64},
     {MAX_FREQ/(NOTE_E*4), (NOTE_E*4)/32},
     {0,0}
  };

__flash TSoundStream ssStop[]= 					// 停止
  {
     {MAX_FREQ/(NOTE_F*4), (NOTE_F*4)/32},
     {0,0}
  };

__flash TSoundStream ssSlowUp[]= 				// 慢加速
  {
     {MAX_FREQ/(NOTE_A*5), (NOTE_A*5)/32},
     {0,0}
  };

__flash TSoundStream ssSlowDown[]= 			// 慢减速
  {
     {MAX_FREQ/(NOTE_A*5), (NOTE_A*5)/32},
     {0,0}
  };

__flash TSoundStream ssFastUp[]= 				// 快加速
  {
     {MAX_FREQ/(NOTE_A*5), (NOTE_A*5)/32},
     {0,0}
  };

__flash TSoundStream ssFastDown[]= 			// 快减速
  {
     {MAX_FREQ/(NOTE_A*5), (NOTE_A*5)/32},
     {0,0}
  };


__flash TSoundStream ssFastBeep[]= 			// 快速鸣响
  {
     {MAX_FREQ/(NOTE_D*3), (NOTE_D*3)/8},
     {0,0}
  };

__flash TSoundStream ssSlowBeep[]= 			// 慢速鸣响
  {
     {MAX_FREQ/(NOTE_D*3), (NOTE_D*3)/8},
     {0,0}
  };

__flash TSoundStream ssError[]= 				// 错误
  {
     {MAX_FREQ/(NOTE_E*5), (NOTE_E*5)/64},
     //{MAX_FREQ/(NOTE_D*4), (NOTE_D*4)/32},
     {PAUSE, (PAUSE_LEN)/32},
     {MAX_FREQ/(NOTE_D*4), (NOTE_D*4)/32},
     {0,0}
  };

__flash TSoundStream ssOk[]= 						// OK
  {
     {MAX_FREQ/(NOTE_E*5), (NOTE_E*5)/64},
     {PAUSE, (PAUSE_LEN)/32},
     {MAX_FREQ/(NOTE_E*5), (NOTE_E*5)/64},
     //{PAUSE, (PAUSE_LEN)/32},
     //{MAX_FREQ/(NOTE_E*5), (NOTE_E*5)/64},
     {0,0}
  };

__flash TSoundStream ssWarning[]= 			// 警告
  {
     {MAX_FREQ/(NOTE_C*3), (NOTE_C*3)/32},
     //{MAX_FREQ/(NOTE_A*2), (NOTE_A*2)/8},
     {0,0}
   };

__flash TSoundStream ssCancel[]= 				// 取消
  {
     {MAX_FREQ/(NOTE_E*5), (NOTE_E*5)/16},
     {0,0}
  };

__flash TSoundStream ssAlarm[]= 				// 报警
  {
     {MAX_FREQ/(NOTE_A*8), (NOTE_A*3)/16},
     {0,0}
   };

//------------------------------------------------------------------------------
// SND_Init - init module
//------------------------------------------------------------------------------
void SND_Init(void)
{
  TCCR2|=(1<<WGM21);
  TCCR2|=(1<<COM20);
  TCNT2=0;
  OCR2=0x00;
  if (EEPROM1.Sound)
    SND_On();
  else
    SND_Off();
}

//------------------------------------------------------------------------------
// SND_On - turn sound on
//------------------------------------------------------------------------------
void SND_On(void)
{
  flgSND_Off=false;
}
  
//------------------------------------------------------------------------------
// SND_Off - turn sound off
//------------------------------------------------------------------------------
void SND_Off(void)
{
  flgSND_Off=true;
  OCR2=0x00;    
}
  
//------------------------------------------------------------------------------
// SND_Play - play a sound
//------------------------------------------------------------------------------
void SND_Play(TSoundStream __flash *Stream)
{
  if (flgSND_Off)
    return;
  if (KBD_IsRepeated())
    return;
  SNDPos=1;
  SNDStream=Stream;
  TCNT2=0x00;
  OCR2=Stream[0].freq;
  TCCR2|=((1<<CS21)|(1<<CS20));
  SNDDelay=Stream[0].time;
}
