#ifndef _SOUND_H
  #define _SOUND_H

#include "defines.h"

// prescaler 64
#define MAX_FREQ	XTAL/64/2

// only octav more 1 ---------
#define NOTE_C          262
#define NOTE_CD         277
#define NOTE_D          294
#define NOTE_DD         311
#define NOTE_E          330
#define NOTE_F		349
#define NOTE_FD	        370
#define NOTE_G		392
#define NOTE_GD	        415
#define NOTE_A		440
#define NOTE_AD	        466
#define NOTE_H		494

#define PAUSE           0xFF
#define PAUSE_LEN       MAX_FREQ*2/256 

typedef struct
  {
    TByte freq;
    TWord time;       
  }TSoundStream;

extern TWord SNDDelay;
extern TByte SNDPos;
extern TSoundStream __flash *SNDStream;

extern __flash TSoundStream ssOn[];
extern __flash TSoundStream ssStart[];
extern __flash TSoundStream ssStop[];
extern __flash TSoundStream ssSlowUp[];
extern __flash TSoundStream ssSlowDown[];
extern __flash TSoundStream ssFastUp[];
extern __flash TSoundStream ssFastDown[];
extern __flash TSoundStream ssOk[];
extern __flash TSoundStream ssCancel[];
extern __flash TSoundStream ssFastBeep[];
extern __flash TSoundStream ssSlowBeep[];
extern __flash TSoundStream ssCancel[];
extern __flash TSoundStream ssWarning[];
extern __flash TSoundStream ssError[];

extern TBool flgSND_Off;
extern void SND_Init(void);
extern void SND_Play(TSoundStream __flash *Stream);
//extern void SND_Play(TWord stime);
extern void SND_On(void); 
extern void SND_Off(void);

#endif
