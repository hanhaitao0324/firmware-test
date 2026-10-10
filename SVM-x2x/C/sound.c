#include "global.h"
#include "eeprom.h"
#include "pins.h"
#include "platform.h"
#include "sound.h"
#include "kbd.h"

TWord SNDDelay=0;
TByte SNDPos=0;
TSoundStream __flash *SNDStream;

TBool flgSND_Off=false;

__flash TSoundStream ssOn[]=
  {
     {MAX_FREQ/(NOTE_C*3), 50},
     {MAX_FREQ/(NOTE_E*3), 25},
     {MAX_FREQ/(NOTE_A*2), 50},
     {0,0}
  };

__flash TSoundStream ssStart[]=
  {
     {MAX_FREQ/(NOTE_A*2), 50},
     {MAX_FREQ/(NOTE_C*3), 25},
     {MAX_FREQ/(NOTE_E*4), 50},
     {0,0}
  };


__flash TSoundStream ssStop[]=
  {
     {MAX_FREQ/(NOTE_F*2), 75},
     {0,0}
  };

__flash TSoundStream ssSlowUp[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };

__flash TSoundStream ssSlowDown[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };

__flash TSoundStream ssFastUp[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };


__flash TSoundStream ssFastDown[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };


__flash TSoundStream ssFastBeep[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };

__flash TSoundStream ssSlowBeep[]=
  {
     {MAX_FREQ/(NOTE_A*5), 50},
     {0,0}
  };


__flash TSoundStream ssError[]=
  {
     {MAX_FREQ/(NOTE_D*4), 25},
     {PAUSE,               25},
     {MAX_FREQ/(NOTE_D*4), 25},
     {0,0}
  };



__flash TSoundStream ssOk[]=
  {
     {MAX_FREQ/(NOTE_E*3), 50},
     {0,0}
  };


__flash TSoundStream ssWarning[]=
  {
     {MAX_FREQ/(NOTE_C*3), 50},
     {MAX_FREQ/(NOTE_A*2), 50},
     {0,0}
   };

__flash TSoundStream ssCancel[]=
  {
     {MAX_FREQ/(NOTE_E*5), 50},
     {0,0}
  };


//------------------------------------------------------------------------------
// SND_Init - init module
//------------------------------------------------------------------------------
void SND_Init(void)
{
  TCCR2|=(1<<WGM21);
  //TCCR2|=(1<<COM20);
  TCNT2=0;
  OCR2=0x00;
  //if (eepromSound)
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
  TCCR2&=~(1<<COM20); //added!!!
  //macPinOff(PORT_BEEPER, pinBeeper);
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
  TCCR2|=(1<<COM20); //added!!!
  SNDPos=1;
  SNDStream=Stream;
  TCNT2=0x00;
  OCR2=Stream[0].freq;
  TCCR2|=((1<<CS21)|(1<<CS20));
  SNDDelay=Stream[0].time;
}

//------------------------------------------------------------------------------
// interrupt handler
//------------------------------------------------------------------------------
/*
#pragma vector=TIMER2_COMP_vect
__interrupt void SND_Int(void)
  {
    static TByte tick=false;
    
    if (!tick)
      {
        tick=true;
        macBitOff(PORT_BEEPER,pinBeeper);
        if (SNDDelay)
          {
            SNDDelay--;
            return;
          }      
        SNDDelay=SNDStream[SNDPos].time;
        OCR2=SNDStream[SNDPos].freq;
        if ((OCR2==0)&&(SNDDelay==0))
          {
             macBitOff(TIMSK,OCIE2);
             return;
          }
        SNDPos++;
        return;           
      }
    if(OCR2!=0xFF)
      macBitOn(PORT_BEEPER,pinBeeper);
    tick=false;
  }
*/
