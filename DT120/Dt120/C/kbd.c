#include "pins.h"
#include "global.h"
#include "kbd.h"

//kbd buffer
TByte kbdBuffer[KBD_BUFFER_SIZE];

//current key combination
TWord kbdCurrentKeys;
TWord curBut;
TWord oldBut;
TByte repeatButton;

TByte kbdHead,kbdTail;
TWord repeatReload;
TWord repeatCounter;
TBool repeatDelay;
TBool repeatEnabled;
TByte repeatedKeys;
TByte buttonDelay=0;
TBool kbdRepeatMode;

void KBD_Delay(void)
{
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
}

//------------------------------------------------------------------------------
// KBD_Init - init module
//------------------------------------------------------------------------------
void KBD_Init(void)
{
  //reset
  KBD_Reset();
}

//------------------------------------------------------------------------------
// KBD_KeyPressed - check if key pressed（检查按键按下）
//------------------------------------------------------------------------------
TBool KBD_KeyPressed(void)
{
  return kbdTail!=kbdHead;
}

//------------------------------------------------------------------------------
// KBD_IsRepeated - check if key is repeated or not（检查按键是否重复）
//------------------------------------------------------------------------------
TBool KBD_IsRepeated(void)
{
  return kbdRepeatMode;
}

//------------------------------------------------------------------------------
// KBD_GetKey - get key（获取按键码）
//------------------------------------------------------------------------------
TByte KBD_GetKey(void)
{
  TByte key;

  key=kbdBuffer[kbdTail++];
  if (globalDeviceStatus.remoteMode)
    key=KBD_CODE_BLOCKED;
  if (kbdTail>=KBD_BUFFER_SIZE)
    kbdTail=0;
  if (kbdTail==kbdHead)
    globalDeviceStatus.kbdHit=false;
  return key;
}

//------------------------------------------------------------------------------
// KBD_Reset - reset module（复位模块）
//------------------------------------------------------------------------------
void KBD_Reset(void)
{
  kbdHead=kbdTail=0;
  oldBut=0x0000;
  repeatCounter=0x00;
  repeatDelay=false;
  repeatEnabled=true;
  repeatReload=0x00;
  repeatButton=0xFF;
  globalDeviceStatus.kbdHit=false;
  kbdRepeatMode=false;
}

//------------------------------------------------------------------------------
// KBD_IntHandler - keypad interrupt handler（键盘中断处理）
//------------------------------------------------------------------------------
void KBD_IntHandler(void)
{
	TWord tmp;
	
	if (buttonDelay)  										// 一个字节，延时
	{
		buttonDelay--;
		return;
	}
	
	curBut = 0x00;
	
	//check current state of the buttons（检查按钮的当前状态）
	macBitOn(PORT_KBD, pinKBIn3);
	KBD_Delay();
	
	if (macBitTest(PIN_KBD,pinKBOut1))
		curBut |= KBD_MASK_START;
	if (macBitTest(PIN_KBD,pinKBOut2))
		curBut |= KBD_MASK_STOP;
	if (macBitTest(PIN_KBD,pinKBOut3))
		curBut |= KBD_MASK_MENU;
	
	macBitOff(PORT_KBD, pinKBIn3);
	
	macBitOn(PORT_KBD, pinKBIn2);
	KBD_Delay();
	
	if (macBitTest(PIN_KBD,pinKBOut1))
		curBut |= KBD_MASK_TEMP_UP;
	if (macBitTest(PIN_KBD,pinKBOut2))
		curBut |= KBD_MASK_TEMP_DOWN;
	if (macBitTest(PIN_KBD,pinKBOut3))
		curBut |= KBD_MASK_SPEED_UP;
	if (macBitTest(PIN_KBD,pinKBOut4))
		curBut |= KBD_MASK_SPEED_DOWN;
	
	macBitOff(PORT_KBD, pinKBIn2);
	
	macBitOn(PORT_KBD, pinKBIn1);
	KBD_Delay();
	
	if (macBitTest(PIN_KBD,pinKBOut1))
		curBut |= KBD_MASK_HOUR_UP;
	if (macBitTest(PIN_KBD,pinKBOut2))
		curBut |= KBD_MASK_HOUR_DOWN;
	if (macBitTest(PIN_KBD,pinKBOut3))
		curBut |= KBD_MASK_MIN_UP;
	if (macBitTest(PIN_KBD,pinKBOut4))
		curBut |= KBD_MASK_MIN_DOWN;
	
	macBitOff(PORT_KBD, pinKBIn1);
	
	//current keys combination for monitoring hidden sequences（保护当前键码）
	kbdCurrentKeys = curBut;
	
	//key was released?（按键已经释放？）
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
		
		return; 														// 键码有改变返回
	}
	
	if (!repeatEnabled)
		return; 														// 按钮未松开返回
	
	tmp=oldBut;
	oldBut=curBut;
	
	//detect the new button（检测新按钮）
	curBut=curBut^tmp;
	if (curBut)
	{
		switch (curBut)
		{
			case KBD_MASK_START:      repeatButton=KBD_CODE_START; 					break;
			case KBD_MASK_STOP:       repeatButton=KBD_CODE_STOP; 					break;
			case KBD_MASK_MENU:       repeatButton=KBD_CODE_MENU; 					break;
			case KBD_MASK_HOUR_UP:    repeatButton=KBD_CODE_HOUR_UP; 				break;
			case KBD_MASK_HOUR_DOWN:  repeatButton=KBD_CODE_HOUR_DOWN; 			break;
			case KBD_MASK_MIN_UP:     repeatButton=KBD_CODE_MIN_UP; 				break;
			case KBD_MASK_MIN_DOWN:   repeatButton=KBD_CODE_MIN_DOWN; 			break;
			case KBD_MASK_TEMP_UP:    repeatButton=KBD_CODE_TEMP_UP; 				break;
			case KBD_MASK_TEMP_DOWN:  repeatButton=KBD_CODE_TEMP_DOWN; 			break;
			case KBD_MASK_SPEED_UP:   repeatButton=KBD_CODE_SPEED_UP; 			break;
			case KBD_MASK_SPEED_DOWN: repeatButton=KBD_CODE_SPEED_DOWN; 		break;
			default: return;
		}
		
		//add to buffer（添加到缓冲区）
		kbdBuffer[kbdHead++]=repeatButton;
		kbdRepeatMode=false; 								// 非重复按键，有新键码产生
		lastPressedKey=repeatButton; 				// 当前按键码设置为最后一个按键码
		globalDeviceStatus.kbdHit=true; 		// ？
		
		if (kbdHead>=KBD_BUFFER_SIZE) 			// 缓存地址长度管理
			kbdHead=0;
		
		//setup repeat counter（设置重复计数器）
		repeatCounter=KBD_AUTOREPEAT;
		repeatDelay=true;
		if ((curBut&KBD_MASK_START)||(curBut&KBD_MASK_STOP)||(curBut&KBD_MASK_MENU))
			repeatReload=0x00; 								// 没有重复键码
		if ((curBut&KBD_MASK_HOUR_DOWN)||(curBut&KBD_MASK_MIN_DOWN)||(curBut&KBD_MASK_HOUR_UP)||(curBut&KBD_MASK_MIN_UP))
			repeatReload=KBD_SLOW_REPEAT; 		// 低速重复键码
		if ((curBut&KBD_MASK_TEMP_DOWN)||(curBut&KBD_MASK_SPEED_DOWN)||(curBut&KBD_MASK_TEMP_UP)||(curBut&KBD_MASK_SPEED_UP))
			repeatReload=KBD_FAST_REPEAT; 		// 高速重复键码
		
		return; 														// 检测到新按键码返回
	}
	
	//repeat counter done?（重复计数器完成？）
	if (repeatCounter)
	{
		repeatCounter--;
		return; 														// 初始延时未到返回
	}
	
	//button wants to be autorepeated（按钮自动重复）
	if (!repeatDelay)
	{
		if (!repeatReload)
			return;
		
		//add to buffer（添加到缓冲区）
		if ((repeatedKeys==10)||(repeatedKeys==20)||(repeatedKeys==50))
			repeatReload>>=1; 								// 在连续出现10、20或50个键码时，速度加快一倍
		
		kbdBuffer[kbdHead++]=repeatButton; 	// 键码写入缓存
		kbdRepeatMode=true; 								// 重复键模式置位
		if (kbdHead>=KBD_BUFFER_SIZE) 			// 缓存地址空间管理
			kbdHead=0;
		globalDeviceStatus.kbdHit=true;
		repeatCounter=repeatReload; 				// 延时赋值
		
		if (repeatedKeys<51) 								// 重复键次数上限
			repeatedKeys++; 									// 超过50个键码后速度不再改变
		
		return; 														// 重复键码返回
	}
	
	//setup repeat（设置重复）
	repeatDelay=false; 										// 重复延时标识复位
	repeatCounter=repeatReload; 					// 重复计数器=重复重加载
	repeatedKeys=0; 											// 重复键次数清零
}
