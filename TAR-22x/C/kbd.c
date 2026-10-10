#include "const.h"
#include "global.h"
#include "kbd.h"
#include "pins.h"

//current key combination
TByte kbdCurrentKeys;

//kbd buffer
TByte kbdBuffer[KBD_BUFFER_SIZE];
//
TByte curBut;
TByte kbdHead, kbdTail;
TByte oldBut;
TWord repeatCounter;
TBool repeatDelay;
TBool repeatEnabled;
TWord repeatReload;
TByte repeatButton;
TByte repeatedKeys;
TByte buttonDelay=0;
TBool kbdRepeatMode;

__flash char BUTTON_NAME[KBD_CODE_LAST][7]=
  {
    {"NO_KEY\x00"},
    {"START\x00"},
    {"STOP\x00"},
    {"S_UP\x00"},
    {"S_DOWN\x00"},
    {"F_UP\x00"},
    {"F_DOWN\x00"}
  };

//------------------------------------------------------------------------------
// KBD_Init - init module
//------------------------------------------------------------------------------
void KBD_Init(void)
{
  //reset
  KBD_Reset();
}

//------------------------------------------------------------------------------
// KBD_KeyPressed - check if key pressed
//------------------------------------------------------------------------------
TBool KBD_KeyPressed(void)
{
  return kbdTail!=kbdHead;
}

//------------------------------------------------------------------------------
// KBD_IsRepeated - check if key is repeated or not
//------------------------------------------------------------------------------
TBool KBD_IsRepeated(void)
{
  return kbdRepeatMode;
}

//------------------------------------------------------------------------------
// KBD_GetKey - get key
//------------------------------------------------------------------------------
TByte KBD_GetKey(void)
{
  TByte key;

  key=kbdBuffer[kbdTail++];
  if (kbdTail>=KBD_BUFFER_SIZE)
    kbdTail=0;
  //if (kbdTail==kbdHead)
  //  globalStatus.kbdHit=false;
  return key;
}

//------------------------------------------------------------------------------
// KBD_Reset - reset module
//------------------------------------------------------------------------------
void KBD_Reset(void)
{
  kbdHead=kbdTail=0;
  oldBut=0x00;
  repeatCounter=0x00;
  repeatDelay=false;
  repeatEnabled=true;
  repeatReload=0x00;
  repeatButton=0xFF;
  kbdRepeatMode=false;
  //globalStatus.kbdHit=false;
}

//------------------------------------------------------------------------------
// KBD_IntHandler - keypad interrupt handler
//------------------------------------------------------------------------------
void KBD_IntHandler(void)
  {
    //TByte curBut=0x00;
    TByte tmp;
    //TByte ix;
           
    if (buttonDelay)
    {
      buttonDelay--;
      return;
    }

    //check current state of the buttons - STOP button has the highest priority
    curBut=0x00;
    if (!macBitTest(PIN_KBD,pinKB1))
      curBut|=KBD_MASK_SLOW_DOWN;
    if (!macBitTest(PIN_KBD,pinKB2))
      curBut|=KBD_MASK_SLOW_UP;
    if (!macBitTest(PIN_KBD,pinKB4))
      curBut|=KBD_MASK_STOP;
    if (!macBitTest(PIN_KBD,pinKB3))
      curBut|=KBD_MASK_START;
    if (!macBitTest(PIN_KBD_5,pinKB5))
      curBut|=KBD_MASK_FAST_DOWN;
    if (!macBitTest(PIN_KBD,pinKB6))
      curBut|=KBD_MASK_FAST_UP;

    //current keys combination for monitoring hidden sequences
    kbdCurrentKeys=curBut;
    //key was released?
    if ((curBut<oldBut)||(!curBut))
      {
	//reset
        if (curBut<oldBut)
          buttonDelay=KBD_BUTTON_DELAY;
	oldBut=curBut;
        if (curBut)
          repeatEnabled=false;
        else
          repeatEnabled=true;
        return;
      }
    if (!repeatEnabled)
      return;
    tmp=oldBut;
    oldBut=curBut;
    //detect the new button
    curBut=curBut^tmp;

    if (curBut)
      {
        switch (curBut)
          {
            case KBD_MASK_START:     repeatButton=KBD_CODE_START; break;
            case KBD_MASK_STOP:      repeatButton=KBD_CODE_STOP; break;
            case KBD_MASK_SLOW_UP:   repeatButton=KBD_CODE_SLOW_UP; break;
            case KBD_MASK_SLOW_DOWN: repeatButton=KBD_CODE_SLOW_DOWN; break;
            case KBD_MASK_FAST_UP:   repeatButton=KBD_CODE_FAST_UP; break;
            case KBD_MASK_FAST_DOWN: repeatButton=KBD_CODE_FAST_DOWN; break;
            default:
              return;
          }
	//add to buffer
	kbdBuffer[kbdHead++]=repeatButton;
        kbdRepeatMode=false;
        lastPressedKey=repeatButton;
        //globalStatus.kbdHit=true;
	if (kbdHead>=KBD_BUFFER_SIZE)
	  kbdHead=0;
	//setup repeat counter
	repeatCounter=KBD_AUTOREPEAT;
	repeatDelay=true;
	if ((curBut&KBD_MASK_START)||(curBut&KBD_MASK_STOP))
	  repeatReload=0x00;
	if ((curBut&KBD_MASK_SLOW_UP)||(curBut&KBD_MASK_SLOW_DOWN))
	  repeatReload=KBD_SLOW_REPEAT;
	if ((curBut&KBD_MASK_FAST_UP)||(curBut&KBD_MASK_FAST_DOWN))
	  repeatReload=KBD_FAST_REPEAT;
	return;
      }
    //repeat counter done?
    if (repeatCounter)
      {
        repeatCounter--;
        return;
      }
    //button wants to be autorepeated
    if (!repeatDelay)
      {
        if (!repeatReload)
          return;
        
	//add to buffer
        if ((repeatedKeys==10)||(repeatedKeys==20)||(repeatedKeys==50)||(repeatedKeys==100))
          repeatReload>>=1;
        
	//for (;ix;ix--)
        //{
          kbdBuffer[kbdHead++]=repeatButton;
          kbdRepeatMode=true;
	  if (kbdHead>=KBD_BUFFER_SIZE)
	    kbdHead=0;
        //}
        //globalStatus.kbdHit=true;
	repeatCounter=repeatReload;
        if (repeatedKeys<101) repeatedKeys++; 
	return;
      }
    //setup repeat
    repeatDelay=false;
    repeatCounter=repeatReload;
    repeatedKeys=0;
  }
