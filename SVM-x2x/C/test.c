#include "confmenu.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "menu.h"
#include "motor.h"
#include "sound.h"
#include "strings.h"
#include "syserror.h"
#include "testsetup.h"
#include "test.h"

TReal32 minSpeed,avrgSpeed,maxSpeed,prevSpeed;

TBool acalOver1,acalOver2; 							// 测试完成标志
TBool acalNa1,acalNa2; 									// 工作站未使用标志

//------------------------------------------------------------------------------
// TEST_Stat - statistics on speed
//------------------------------------------------------------------------------
void TEST_Stat(TReal32 current)
{
  TWord Speed=(TWord)(current*10);
  if (globalTestStatus.MinSpeed>Speed)
    globalTestStatus.MinSpeed=Speed;
  if (globalTestStatus.MaxSpeed<Speed)
    globalTestStatus.MaxSpeed=Speed;
  avrgSpeed=0.3*current+0.7*prevSpeed;
  prevSpeed=avrgSpeed;
}

//------------------------------------------------------------------------------
//show speed
//------------------------------------------------------------------------------
void TEST_ShowSpeed(TBool Led, TReal32 speed, TBool blink)
{
  sprintf_P(globalStr,speedFmt,speed);
  if (!Led)
    LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
  else
    LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
}

//------------------------------------------------------------------------------
//显示体积, rl=0,为左显示；rl=1，为右显示
//------------------------------------------------------------------------------
void TEST_ShowVolt(TWord rev,TBool blink,TBool rl)
{
  if(rev<=SVM_VOLT_MAX)
    sprintf_P(globalStr,voltFmt,rev);
  else
    strcpy_P(globalStr,voltOver);
	
	if(rl)
  	LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
  else
  	LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
}

//------------------------------------------------------------------------------
//显示质量, rl=0,为左显示；rl=1，为右显示
//------------------------------------------------------------------------------
void TEST_ShowWeih(TWord rev,TBool blink,TBool rl)
{
  if(rev<=SVM_WEIH_MAX)
    sprintf_P(globalStr,weihFmt,(TReal32)rev/10);
  else
    strcpy_P(globalStr,weihOver);  
  
	if(rl)
  	LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
  else
  	LED_Print(LED_DISPLAY_LEFT,blink,globalStr);
}

//------------------------------------------------------------------------------
//show strokes
//------------------------------------------------------------------------------
void TEST_ShowRev(TWord rev,TBool blink)
{
  if(rev<=SVM_REV_MAX)
    sprintf_P(globalStr,revFmt,rev);
  else
    strcpy_P(globalStr,revOver);  
  LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
}

//------------------------------------------------------------------------------
//show time
//------------------------------------------------------------------------------
void TEST_ShowTime(TWord time,TBool blink)
{
  static __flash char format2[]="h%01d.%02d";
  TByte sec,min,hour;

  if (time>SVM_TIME_MAX)
  {
    strcpy_P(globalStr,timeOver);
    LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
    return;
  }
  sec=time%60;
  min=(time/60)%60;
  hour=(time/(60*60));
  if (time<SVM_TIME_CONVERT)
    sprintf_P(globalStr,timeFmt,min,sec);
  else
    sprintf_P(globalStr,format2,hour,min);
  LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
}

//------------------------------------------------------------------------------
//process key press during the test
//------------------------------------------------------------------------------
TBool TEST_ProcessKey(TBool *paused)
{
  TByte key;
  TByte DevMode;
    
  DevMode=EEPROM1.DevMode;
  //if (EEPROM1.DevType==DEVICE_TYPE_TDT)   !!!!!
  //  DevMode=DevMode*2+1;
  if (DevMode>DEVICE_MODE_223)
    DevMode=DEVICE_MODE_223;

  key=KBD_GetKey();
  if (!key) return false;
  if (KBD_CODE_START==key)
  {
    if (*paused)
    {
      *paused=false;
      SND_Play(ssOk);
      //MOTOR_GoForward(forward); !!!
      if ((DevMode==DEVICE_MODE_121)||(DevMode==DEVICE_MODE_221)||(DevMode==DEVICE_MODE_223))
      {  
        MOTOR_SetSpeed(SVM_SPEED_MODE1);
      } else
      {
        MOTOR_SetSpeed(SVM_SPEED_MODE2);
      }
      MOTOR_Start();
      //globalStatus.timeTick=false;
      TIMER_ResetTick();
      while (!TIMER_TimeTick());
      //globalStatus.timeTick=false;
      TIMER_ResetTick();
    }
    //else
    //  SND_Play(ssError);
  }
  else if (KBD_CODE_STOP==key)
  {
    MOTOR_Stop();
    SND_Play(ssOk);
    if (*paused)
      return true;
    *paused=true;
  } 
  else 
  	SND_Play(ssError);
  
  return false;
}

//------------------------------------------------------------------------------
//run test
//------------------------------------------------------------------------------
TBool TEST_Run(void)
{
  TBool paused,ledsUpdated; //testRev; Gavin, 2019-08-17 星期六 14:47:51
  TReal32 speed;
  //number of revolutions when stat is disabled
  //TByte statDisabled=SVM_START_MEASURE;
  TWord statDisabled = SVM_START_MEASURE; 				// v1.30, Gavin, 2019-08-24 星期六 12:37:16
  
  TWord numRevs=0;
  //TWord SpeedBuf_Period;
  //TWord SpeedBuf_Value;
  //TByte SpeedBuf_Index;
  TWord BeepTime;
  TByte DevMode;
  
  TByte testMode; 						// v1.30, Gavin, 2019-08-17 星期六 14:46:22
  TByte acalState; 						// v1.30, Gavin, 2019-08-18 星期日 22:56:08
	TByte key; 									// v1.30, Gavin, 2019-08-18 星期日 15:10:08
	
  DevMode=EEPROM1.DevMode;
  //if (EEPROM1.DevType==DEVICE_TYPE_TDT)   !!!!!
  //  DevMode=DevMode*2+1;
  if (DevMode>DEVICE_MODE_223)
    DevMode=DEVICE_MODE_223;

  globalTestStatus.State = TEST_RUNNING;
	globalTestStatus.Unlimited = false;
  
	testMode = EEPROM1.TimeUnits;
	switch(testMode)
  {
  	case TIME_UNIT_TIME: 								// 计时模式测试
  	{
  		if (0==EEPROM1.TestTime) 											// 计时模式参数为0，即加法连续测试
	    {
	      globalTestStatus.Unlimited=true;
	      globalTestStatus.Value=0;
	    } 
	    else 
	    	globalTestStatus.Value=EEPROM1.TestTime;
  	}break;
  	case TIME_UNIT_REVOLUTIONS: 				// 次数模式测试
  	{
  		if (0==EEPROM1.TestRevolutions) 							// 次数模式参数为0，即加法连续测试
	    {
	      globalTestStatus.Unlimited=true;
	      globalTestStatus.Value=0;
	    }
	    else
	    	globalTestStatus.Value=EEPROM1.TestRevolutions;
  	}break;
  	case TIME_UNIT_ACAL: 								// 固定次数程序测试
  	{
  		acalStep = 0;
  		acalStep1 = 0;
  		acalStep2 = 0;
  		acalState = 0;
  		acalOver1 = false;
  		acalOver2 = false;
  		acalRevTotal_1 = 0;
  		acalRevTotal_2 = 0;
  		
  		globalTestStatus.Value = acalRev[acalStep];
  		testVolt[acalStep][0] = EEPROM1.TestVolt01;
  		testVolt[acalStep][1] = EEPROM1.TestVolt02;
  		
  		if(testVolt[acalStep][0] == 0)
  			acalNa1 = true;
  		else
  			acalNa1 = false;
  		
  		if(testVolt[acalStep][1] == 0)
  			acalNa2 = true;
  		else
  			acalNa2 = false;
  	}break;
  	default:
  	break;
  }
  
  // initialize Speed buffer
  /*for (SpeedBuf_Index=0; SpeedBuf_Index<SPEED_BUF_LEN; SpeedBuf_Index++)
    Speed_Buf[SpeedBuf_Index]=0.0;
  SpeedBuf_Index=0;
  if (testRev)
    SpeedBuf_Period=EEPROM1.TestRevolutions/10;
  else
    SpeedBuf_Period=EEPROM1.TestTime/10;
  SpeedBuf_Value=globalTestStatus.Value-SpeedBuf_Period;*/
  globalTestStatus.MinSpeed=SVM_MAX_SPEED*10;
  globalTestStatus.MaxSpeed=0;
  //in pause mode update LED's only one time - cont update prevents from blinking
  MOTOR_Stop();
  if ((DevMode==DEVICE_MODE_121)||(DevMode==DEVICE_MODE_221)||(DevMode==DEVICE_MODE_223))
  {  
    MOTOR_SetSpeed(SVM_SPEED_MODE1);
    BeepTime=SVM_SPEED_MODE1*SVM_TIME_BEEP/60; 
  } 
  else
  {
    MOTOR_SetSpeed(SVM_SPEED_MODE2);
    BeepTime=SVM_SPEED_MODE1*SVM_TIME_BEEP/60; 
  }
  
  MOTOR_ResetRevolutions();
  MOTOR_Start();
  paused=false;
  
  //testValue=0;
  //sync
  //globalStatus.timeTick=false;
  //while (!globalStatus.timeTick);
  //globalStatus.timeTick=false;
  TIMER_ResetTick();
  KBD_Reset();
  globalTestStatus.Ticker=0;
  speed=(TReal32)MOTOR_GetSpeed()/10;
  if (DevMode!=DEVICE_MODE_223)
  {
  	if(testMode != TIME_UNIT_ACAL)
  		TEST_ShowSpeed(false, speed, paused);
  	else
  	{
  		LED_Print(LED_DISPLAY_LEFT,false,"STRK");
  		TEST_ShowRev(globalTestStatus.Value,false);
  	}
  }
  
  globalTestStatus.NumStatRecords=0;

  //run
  for (;;)
  {
  	//wait for key press
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      SYSERROR_Check();
      if (!globalStatus.testStart)
        goto TestDone;

      if (numRevs<=SVM_REV_MAX) 
      	numRevs=MOTOR_GetRevolutions();
      
      // 数码管显示刷新, 时间/次数---------------------------
			if (!ledsUpdated)
			{
			  switch(testMode)
				{
					case TIME_UNIT_TIME: 					// 计时模式测试
					{
						TEST_ShowTime(globalTestStatus.Value,paused);
					}break;
					case TIME_UNIT_REVOLUTIONS: 	// 次数模式测试
					{
						if (globalTestStatus.Unlimited)
				    {  
				      globalTestStatus.Value=numRevs;
				    } 
				    else
				    {
				      if (EEPROM1.TestRevolutions>numRevs)
				        globalTestStatus.Value = EEPROM1.TestRevolutions-numRevs;
				      else
				        globalTestStatus.Value = 0;
				    }
				    
				    TEST_ShowRev(globalTestStatus.Value,paused);
					}break;
					case TIME_UNIT_ACAL: 					// 固定次数程序测试
					{
						switch(acalState) 					// 测试状态
						{
							case 0: 									// 显示测试次数
							{
								if(acalStep < 3)
									globalTestStatus.Value = acalRev[acalStep];
								else
									globalTestStatus.Value = acalRev[2];
								
			          TEST_ShowRev(globalTestStatus.Value,false);
							}break;
							case 1: 									// 显示运行剩余次数
							{
								if(acalStep < 3)
									globalTestStatus.Value = acalRev[acalStep]-numRevs;
								else
									globalTestStatus.Value = acalRev[2]-numRevs;
								
					      TEST_ShowRev(globalTestStatus.Value,paused);
							}break;
							case 2: 									// 显示体积次数
							{
								if(acalStep < 3)
									globalTestStatus.Value = acalRev[acalStep];
								else
									globalTestStatus.Value = acalRev[2];
								
			          TEST_ShowRev(globalTestStatus.Value,false);
							}break;
							case 3: 									// 显示输入体积数值
							{
								if(acalNa2)
								{
									LED_Print(LED_DISPLAY_RIGHT,false,"  NA");
								}
								else
								{
									if(acalOver2)
									{
										LED_Print(LED_DISPLAY_RIGHT,false,"DONE");
									}
									else
									{
										TEST_ShowVolt(testVolt[acalStep][1],false,LEDNB_RIGHT);
									}
								}
							}break;
							case 4: 									// 根据比对结果静态显示完成或继续
							{
								if(acalStep == acalTestMax-1)
								{
									LED_Print(LED_DISPLAY_RIGHT,false,"RUN ");
								}
								else
								{
									if(acalNa2)
									{
										LED_Print(LED_DISPLAY_RIGHT,false,"  NA");
									}
									else
									{
										if(acalOver2)
										{
											LED_Print(LED_DISPLAY_RIGHT,false,"DONE");
										}
										else
										{
											LED_Print(LED_DISPLAY_RIGHT,false,"C.TAP");
										}
									}
								}
							}break;
							default:
							break;
						}
					}break;
					default:
					break;
				}
			}
			
      //time tick? 时间刻度
      if (TIMER_TimeTick())
      {
        //show test parameters，显示测试参数
        if (!ledsUpdated)
        {
          if (!paused)
          {
            speed=(TReal32)MOTOR_GetSpeed()/10;
            if (globalTestStatus.Ticker>=statDisabled)
            {  
              TEST_Stat(speed);
            } 
            else
            {  
              avrgSpeed=prevSpeed=speed;
              prevSpeed=EEPROM1.Speed;
            }
          } 
          else
          {
            avrgSpeed=EEPROM1.Speed;
            statDisabled = globalTestStatus.Ticker + 2; 		// v1.30, Gavin, 2019-08-24 星期六 12:38:31
          }

          /*speed=(TReal32)MOTOR_GetSpeed()/10;
          if (numRevs>=statDisabled)
            TEST_Stat(speed);
          else
            avrgSpeed=prevSpeed=speed;
                
          if (testRev)
          {
            if (globalTestStatus.Unlimited)
              testValue = numRevs;
            else
              //testValue = eepromTestRevolutions - numRevs;
              testValue = EEPROM1.TestRevolutions - numRevs;
            TEST_ShowRev(testValue,paused);
          } else
          {
            TEST_ShowTime(testValue,paused);
          }*/
          
          if (DevMode==DEVICE_MODE_223)
            LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[EEPROM1.TimeUnits]);
          else 													// v1.30, Gavin, 2019-08-18 星期日 14:07:48
          {
          	if(testMode != TIME_UNIT_ACAL)
          		TEST_ShowSpeed(false, speed, paused);
          	else
          	{
          		switch(acalState) 					// 测试状态
							{
								case 0: 									// 显示测试次数
								{
									LED_Print(LED_DISPLAY_LEFT,false,"STRK");
								}break;
								case 1: 									// 显示运行剩余次数
								{
									//LED_Print(LED_DISPLAY_LEFT,false,"STRK");
	            		TEST_ShowSpeed(false, speed, paused);
								}break;
								case 2: 									// 显示体积次数
								{
									LED_Print(LED_DISPLAY_LEFT,false,"VOL.T");
								}break;
								case 3: 									// 显示输入体积数值
								{
									if(acalNa1)
									{
										LED_Print(LED_DISPLAY_LEFT,false,"  NA");
									}
									else
									{
										if(acalOver1)
										{
											LED_Print(LED_DISPLAY_LEFT,false,"DONE");
										}
										else
										{
											TEST_ShowVolt(testVolt[acalStep][0],false,LEDNB_LEFT);
										}
									}
								}break;
								case 4:
								{
									if(acalStep == acalTestMax-1)
									{
										LED_Print(LED_DISPLAY_LEFT,false,"OVER");
									}
									else
									{
										if(acalNa1)
										{
											LED_Print(LED_DISPLAY_LEFT,false,"  NA");
										}
										else
										{
											if(acalOver1)
											{
												LED_Print(LED_DISPLAY_LEFT,false,"DONE");
											}
											else
											{
												LED_Print(LED_DISPLAY_LEFT,false,"C.TAP");
											}
										}
									}
								}break;
								default:
								break;
							}
						}
          }
        }

        if (paused)
        {
          ledsUpdated=true;
          if(testMode != TIME_UNIT_ACAL) 					// v1.30, Gavin, 2019-08-18 星期日 16:08:17
          	LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[EEPROM1.TimeUnits]);
          else
          {
          	if(acalState == 1)
          	{
          		LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[EEPROM1.TimeUnits]);
          	}
          	else
          		ledsUpdated=false;
          }
          
          continue;
        } 
        else 
        	ledsUpdated=false;

        if (globalTestStatus.Ticker<SVM_TIME_MAX) 
        	globalTestStatus.Ticker++;
        //globalStatus.timeTick=false;
        TIMER_ResetTick();
        if (globalTestStatus.Unlimited)
        {
          if (testMode == TIME_UNIT_TIME)
            if (globalTestStatus.Value<=SVM_TIME_MAX) 
            	globalTestStatus.Value++;
        } 
        else
        {
					switch(testMode)
          {
          	case TIME_UNIT_TIME:
          	{
          		globalTestStatus.Value--;
          		if (globalTestStatus.Value<SVM_TIME_BEEP)
              	SND_Play(ssWarning);
          	}break;
          	case TIME_UNIT_REVOLUTIONS:
          	{
          		if (globalTestStatus.Value<BeepTime)
              	SND_Play(ssWarning);
          	}break;
          	case TIME_UNIT_ACAL:
          	{
          		if(acalState == 1)
          		{
	          		if (globalTestStatus.Value<BeepTime)
	              	SND_Play(ssWarning);
	            }
          	}break;
          	default:
          	break;
          }
        }
      }

      //check if test complete? 检查测试是否完成
      if (!globalStatus.testStart) 
      	goto TestDone;

      //if (numRevs<=SVM_REV_MAX) numRevs=MOTOR_GetRevolutions();
            
      if (globalTestStatus.Unlimited)
        continue;
			
			switch(testMode)
			{
				case TIME_UNIT_TIME:
				{
					if (!globalTestStatus.Value)
          	goto TestDone;
				}break;
				case TIME_UNIT_REVOLUTIONS:
				{
					if (numRevs>=EEPROM1.TestRevolutions)
	        {
	          goto TestDone;
	        }
				}break;
				case TIME_UNIT_ACAL:
				{
					if(acalState == 1)
          {
          	if(acalStep < 3)
          	{
	          	if (numRevs>=acalRev[acalStep])
			        {
			          //goto TestDone;
			          
			          //acalStep++;
			          acalState++;
			          MOTOR_Stop();
								paused = true;
	  						SND_Play(ssOk);
			        }
			      }
			      else
			      {
			      	if (numRevs>=acalRev[2])
			        {
			          //goto TestDone;
			          
			          //acalStep++;
			          acalState++;
			          MOTOR_Stop();
								paused = true;
	  						SND_Play(ssOk);
			        }
			      }
					}
				}break;
				default:
				break;
			}
    }
    
    // v1.30, Gavin, 2019-08-18 星期日 14:53:48
    if(testMode == TIME_UNIT_ACAL)
    {
			if (KBD_KeyPressed())
			{
				key=KBD_GetKey();
      	
      	switch(key)
      	{
      		case KBD_CODE_START: 					// 中下键
      		{
      			switch(acalState)
      			{
      				case 0: 									// 显示测试次数
      				{
      					if(!paused)
								{
									MOTOR_Stop();
									paused = true;
	    						SND_Play(ssOk);
	    					}
	    					else
	    					{
	    						acalState++;
	    						
	    						MOTOR_ResetRevolutions();
	    						if(acalStep < 3)
    								globalTestStatus.Value = acalRev[acalStep];
    							else
    								globalTestStatus.Value = acalRev[2];
    							
    							if(acalStep == 0)
    							{
    								testVolt[acalStep][0] = EEPROM1.TestVolt01;
  									testVolt[acalStep][1] = EEPROM1.TestVolt02;
    							}
    							else
    							{
	  								testVolt[acalStep][0] = testVolt[acalStep-1][0];
	  								testVolt[acalStep][1] = testVolt[acalStep-1][1];
	  							}
  								
	    						paused=false;
									SND_Play(ssOk);
									
									//MOTOR_GoForward(forward); !!!
									if ((DevMode==DEVICE_MODE_121)||(DevMode==DEVICE_MODE_221)||(DevMode==DEVICE_MODE_223))
									{  
									  MOTOR_SetSpeed(SVM_SPEED_MODE1);
									} 
									else
									{
									  MOTOR_SetSpeed(SVM_SPEED_MODE2);
									}
									MOTOR_Start();
									
									//TIMER_ResetTick();
									//while (!TIMER_TimeTick());
									
									//TIMER_ResetTick();
								}
      				}break;
      				case 1: 									// 显示测试剩余次数
      				{
      					if(paused)
								{
									paused=false;
									SND_Play(ssOk);
									//MOTOR_GoForward(forward); !!!
									if ((DevMode==DEVICE_MODE_121)||(DevMode==DEVICE_MODE_221)||(DevMode==DEVICE_MODE_223))
									{  
									  MOTOR_SetSpeed(SVM_SPEED_MODE1);
									} 
									else
									{
									  MOTOR_SetSpeed(SVM_SPEED_MODE2);
									}
									MOTOR_Start();
									
									TIMER_ResetTick();
									while (!TIMER_TimeTick());
									
									TIMER_ResetTick();
								}
								else
									SND_Play(ssError);
      				}break;
      				case 2: 									// 显示体积次数
      				{
      					if(acalStep < 2)
      					{
	      					acalRevTotal_1 += acalRev[acalStep1];
	      					acalRevTotal_2 += acalRev[acalStep2];
	      				}
	      				else
	      				{
	      					if(!acalOver1)
	      						acalRevTotal_1 += acalRev[2];
	      					
	      					if(!acalOver2)
	      						acalRevTotal_2 += acalRev[2];
	      				}
	      				
      					acalState++;
      					SND_Play(ssOk);
      				}break;
      				case 3: 									// 显示体积
      				{
      					// 检查测试结果-------------------
      					if(acalStep >= 2)
      					{
      						if(!acalOver1)
      						{
      							if((EEPROM1.TestMCyl01 == 250)|| 				// 量筒为250ml
      								 (EEPROM1.TestMCyl01 == 500)) 				// 量筒为500ml
      							{
			      					if((testVolt[acalStep-1][0] - testVolt[acalStep][0]) <= 2)
			      					{
			      						acalOver1 = true;
			      						//acalStep1 = acalStep;
			      					}
			      				}
			      				else
			      				if(EEPROM1.TestMCyl01 == 100) 					// 量筒为100ml
			      				{
			      					if((testVolt[acalStep-1][0] - testVolt[acalStep][0]) <= 1)
			      					{
			      						acalOver1 = true;
			      					}
			      				}
			      				else
			      					acalOver1 = true;
		      				}
	      					
	      					if(!acalOver2)
	      					{
	      						if((EEPROM1.TestMCyl02 == 250)|| 				// 量筒为250ml
      								 (EEPROM1.TestMCyl02 == 500)) 				// 量筒为500ml
      							{
			      					if((testVolt[acalStep-1][1] - testVolt[acalStep][1]) <= 2)
			      					{
			      						acalOver2 = true;
			      						//acalStep2 = acalStep;
			      					}
			      				}
			      				else
			      				if(EEPROM1.TestMCyl02 == 100) 					// 量筒为100ml
			      				{
			      					if((testVolt[acalStep-1][1] - testVolt[acalStep][1]) <= 1)
			      					{
			      						acalOver2 = true;
			      					}
			      				}
			      				else
			      					acalOver2 = true;
		      				}
	      					
	      					acalState = 4;
	      				}
      					else
      					{
      						acalStep++;
      						
	      					acalStep1 = acalStep;
	      					acalStep2 = acalStep;
	      					
      						acalState = 0;
      					}
      					
      					SND_Play(ssOk);
      				}break;
      				case 4:
      				{
      					acalState = 0;
      					SND_Play(ssOk);
      					
      					if((acalOver1)&&(acalOver2))
      					{
      						globalTestStatus.Value = 0; 		// Gavin, 测试完成，2019-09-17 星期二 18:02:48
	      					goto TestDone;
	      				}
	      				
      					acalStep++;
      					if(acalStep >= acalTestMax)
      					{
      						MOTOR_ResetRevolutions();
						      globalStatus.testStart=false;
						      globalTestStatus.State=TEST_STOPPED;
	      					
	      					//wait a second
	      					__delay_cycles(XTAL/2);
	      					return true;
      					}
      					
      					if(!acalOver1)
      						acalStep1 = acalStep;
      					
      					if(!acalOver2)
      						acalStep2 = acalStep;
	      			}break;
      				default:
      				break;
      			}
      		}break;
      		case KBD_CODE_STOP: 					// 中上键
      		{
      			if(paused) 									// 暂停状态
      			{
      				MOTOR_Stop();
					    SND_Play(ssOk);
					    
					    MOTOR_ResetRevolutions();
				      globalStatus.testStart=false;
				      globalTestStatus.State=TEST_STOPPED;
				      
      				if(!acalOver1)
      				{
		      			acalStep1 = acalStep; 	// Gavin,2019-10-10 星期四 16:20:21
		      			
		      			if(acalStep2 < 2)
		      				acalRevTotal_1 += acalRev[acalStep1]-globalTestStatus.Value;
		      			else
		      				acalRevTotal_1 += acalRev[2]-globalTestStatus.Value;
		      		}
		      		
		      		if(!acalOver2)
		      		{
		      			acalStep2 = acalStep; 	// Gavin,2019-10-10 星期四 16:20:21
		      			
		      			if(acalStep2 < 2)
		      				acalRevTotal_2 += acalRev[acalStep2]-globalTestStatus.Value;
		      			else
		      				acalRevTotal_2 += acalRev[2]-globalTestStatus.Value;
		      		}
		      		
				      //wait a second
				      __delay_cycles(XTAL/2);
				      return true;
      			}
      			else
      			{
	      			if(acalState == 1)
	      			{
	      				MOTOR_Stop();
					    	SND_Play(ssOk);
					    	
					    	paused = true;
					    }
      			}
      		}break;
      		case KBD_CODE_FAST_UP: 				// 左上键
      		{
      			if(acalState == 3)
      			{
      				if((acalOver1)||(acalNa1))
      					SND_Play(ssError);
      				else
      				{
	      				if (TESTSETUP_IncInt(1,&testVolt[acalStep][0],0,9999))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}
      			}
      			else
      				SND_Play(ssError);
      		}break;
      		case KBD_CODE_FAST_DOWN: 			// 左下键
      		{
      			if(acalState == 3)
      			{
      				if((acalOver1)||(acalNa1))
      					SND_Play(ssError);
      				else
      				{
	      				if (TESTSETUP_IncInt(-1,&testVolt[acalStep][0],0,9999))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}
      			}
      			else
      				SND_Play(ssError);
      		}break;
      		case KBD_CODE_SLOW_UP: 				// 右上键
      		{
      			if(acalState == 3)
      			{
      				if((acalOver2)||(acalNa2))
      					SND_Play(ssError);
      				else
      				{
	      				if (TESTSETUP_IncInt(1,&testVolt[acalStep][1],0,9999))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}
      			}
      			else
      				SND_Play(ssError);
      		}break;
      		case KBD_CODE_SLOW_DOWN: 			// 右下键
      		{
      			if(acalState == 3)
      			{
      				if((acalOver2)||(acalNa2))
      					SND_Play(ssError);
      				else
      				{
	      				if (TESTSETUP_IncInt(-1,&testVolt[acalStep][1],0,9999))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}
      			}
      			else
      				SND_Play(ssError);
      		}break;
      		default:
      			SND_Play(ssError);
      		break;
      	}
			}
    }
    else
    {
	    if (TEST_ProcessKey(&paused)) 			// 暂停状态
	    {
	      MOTOR_ResetRevolutions();
	      globalStatus.testStart=false;
	      globalTestStatus.State=TEST_STOPPED;
	      
	      //wait a second
	      __delay_cycles(XTAL/2);
	      if (globalTestStatus.Unlimited)
	        return true;
	      return true;
	    }
	  }
  }

TestDone:
  MOTOR_Stop();
  globalStatus.testStart=false;
  globalTestStatus.State=TEST_STOPPED;
  
  //update screen
  /*
  if (TIME_UNIT_REVOLUTIONS==EEPROM1.TimeUnits)
    TEST_ShowRev(globalTestStatus.Value, false);
  else
    TEST_ShowTime(globalTestStatus.Value, false);
  */
  
  // v1.30, Gavin, 2019-08-18 星期日 21:38:43
  switch(testMode)
  {
  	case TIME_UNIT_TIME: 								// 计时模式测试
  	{
  		TEST_ShowTime(globalTestStatus.Value, false);
  	}break;
  	case TIME_UNIT_REVOLUTIONS: 				// 次数模式测试
  	{
  		TEST_ShowRev(globalTestStatus.Value, false);
  	}break;
  	case TIME_UNIT_ACAL: 								// 固定次数程序测试
  	{
  		LED_Print(LED_DISPLAY_RIGHT,false,"    ");
  	}break;
  	default:
  	break;
  }
  
  LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[EEPROM1.TimeUnits]);
  SND_Play(ssOn);
    
  return true;
}

