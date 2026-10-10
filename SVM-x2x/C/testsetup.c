#include "const.h"
#include "eeprom.h"
#include "ekey.h"
#include "global.h"
#include "kbd.h"
#include "led.h"
#include "sound.h"
#include "test.h"
#include "testsetup.h"
#include "pins.h"
#include "external.h"
#include "confmenu.h"
#include "menu.h"

//------------------------------------------------------------------------------
//inc integer
//------------------------------------------------------------------------------
TBool TESTSETUP_IncInt(signed char step,TWord *value,TWord lo,TWord hi)
{
  signed short val;
    
  //try to increment
  val=*value+step;
  if ((TWord)val>hi)
    return false;
  if ((TWord)val<lo)
    return false;
  *value+=step;
  return true;
}

//------------------------------------------------------------------------------
//show speed
//------------------------------------------------------------------------------
void TESTSETUP_ShowSpeed(TWord speed,TBool blink)
{
  static __flash char format[]="%04d";
    
  sprintf_P(globalStr,format,speed);
  LED_Print(LED_DISPLAY_RIGHT,blink,globalStr);
}

//------------------------------------------------------------------------------
//setup test
//------------------------------------------------------------------------------
void TESTSETUP_Run(void)
{
  TWord val;
  TByte key;
  //TByte mode;
  TWord min,max;
  //TByte step;
  signed char step; 										// v1.30, Gavin, 2019-08-19 星期一 16:03:57
  TByte DevMode;
  
  TByte setACAL=0; 											// V1.30, Gavin, 2019-08-17 星期六 15:43:48
	TWord val02; 													// 2号参数
	
  globalStatus.valueUpdate=false;
  
  DevMode=EEPROM1.DevMode;
  //if (EEPROM1.DevType==DEVICE_TYPE_TDT)   !!!!!
  //  DevMode=DevMode*2+1;
  if (DevMode>DEVICE_MODE_223)
    DevMode=DEVICE_MODE_223;

  step = SVM_CHANGE_STEP;
  //allow editing both strokes(revolutions) and time
  if (DevMode==DEVICE_MODE_223)
    globalTestStatus.Mode=TIME_UNIT_TIME;
  else
    globalTestStatus.Mode=EEPROM1.TimeUnits;
	
	/*
  if (globalTestStatus.Mode==TIME_UNIT_TIME)
  {
    val=EEPROM1.TestTime;
    min=SVM_TIME_MIN;
    max=SVM_TIME_MAX;
  } 
  else
  {
    val=EEPROM1.TestRevolutions;
    min=SVM_REV_MIN;
    max=SVM_REV_MAX;
  }
  */
  // v1.30, Gavin, 2019-08-18 星期日 11:01:35
  switch(globalTestStatus.Mode)
  {
  	case TIME_UNIT_TIME:
  	{
  		val = EEPROM1.TestTime;
	    min = SVM_TIME_MIN;
	    max = SVM_TIME_MAX;
  	}break;
  	case TIME_UNIT_REVOLUTIONS:
  	{
  		val = EEPROM1.TestRevolutions;
	    min = SVM_REV_MIN;
	    max = SVM_REV_MAX;
  	}break;
  	case TIME_UNIT_ACAL:
  	{
  		val = EEPROM1.TestWeih01;
  		val02 = EEPROM1.TestWeih02;
  		min = SVM_WEIH_MIN;
	    max = SVM_WEIH_MAX;
	    step = SVM_CHANGE_STEP;
	    
	    if(val > max) val = max;
	    if(val < min) val = min;
	    
	    if(val02 > max) val02 = max;
	    if(val02 < min) val02 = min;
	  }break;
  	default:
  	break;
  }
    
  if (EEPROM1.Ekey)
  {
    if (globalStatus.ekeyStatus==EKEY_STATUS_BLOCKED)
      EKEY_Check(false);
    else
      globalStatus.ekeyStatus=EKEY_Check(true);
  }  

  //run
  // v1.30, Gavin, 工作模式设置，工作参数设置，2019-08-16 星期五 12:03:36
  for (;;)
  {
    //show revolutions or time
    /*
    if (TIME_UNIT_REVOLUTIONS==globalTestStatus.Mode)
    {
      TEST_ShowRev(val,false);
      LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[globalTestStatus.Mode]);
      step = SVM_CHANGE_STEP;
    } 
    else
    {
      TEST_ShowTime(val,false);
      LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[globalTestStatus.Mode]);
      if (val>=SVM_TIME_CONVERT)
        step = SVM_CHANGE_STEP_TIME;
      else
        step = SVM_CHANGE_STEP;
    }
    */
    
    // v1.30，Gavin，数码管显示增加ACAL，2019-08-16 星期五 12:04:15
    switch(globalTestStatus.Mode)
    {
    	case TIME_UNIT_TIME:
    	{
    		TEST_ShowTime(val,false);
	      LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[TIME_UNIT_TIME]);
	      if (val>=SVM_TIME_CONVERT)
	        step = SVM_CHANGE_STEP_TIME;
	      else
	        step = SVM_CHANGE_STEP;
    	}break;
    	case TIME_UNIT_REVOLUTIONS:
    	{
    		TEST_ShowRev(val,false);
	      LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[TIME_UNIT_REVOLUTIONS]);
	      step = SVM_CHANGE_STEP;
    	}break;
    	case TIME_UNIT_ACAL:
    	{
    		switch(setACAL)
    		{
    			case 0:
    			{
		    		LED_Print_P(LED_DISPLAY_LEFT,false,CHOOSE_LIST[TIME_UNIT_ACAL]);
		    		LED_Print(LED_DISPLAY_RIGHT,false,"    ");
		    	}break;
		    	case 1: 											// "TEST", "CYL1"
		    	{
		    		LED_Print(LED_DISPLAY_LEFT,false,"TEST");
		    		LED_Print(LED_DISPLAY_RIGHT,false,"CYL1");
		    	}break;
		    	case 2: 											// 测试量筒的选择，500ml/250ml/100ml/NA
		    	{
		    		if(val == 0)
		    			LED_Print(LED_DISPLAY_LEFT,false,"  NA");
		    		else
		    			TEST_ShowVolt(val,false,LEDNB_LEFT);
		    		
		    		if(val02 == 0)
		    			LED_Print(LED_DISPLAY_RIGHT,false,"  NA");
		    		else
		    			TEST_ShowVolt(val02,false,LEDNB_RIGHT);
		    	}break;
		    	case 3: 											// "SAMP", "WEIH"
		    	{
		    		LED_Print(LED_DISPLAY_LEFT,false,"SAMP");
		    		LED_Print(LED_DISPLAY_RIGHT,false,"WEIH");
		    	}break;
		    	case 4: 											// 显示初始质量
		    	{
						if(val == 0)
							LED_Print(LED_DISPLAY_LEFT,false,"  NA");
						else
							TEST_ShowWeih(val,false,LEDNB_LEFT); 					// 左显示
						
						if(val02 == 0)
							LED_Print(LED_DISPLAY_RIGHT,false,"  NA");
						else
							TEST_ShowWeih(val02,false,LEDNB_RIGHT); 			// 右显示
		    	}break;
		    	case 5: 											// "VOL.T", "INIT"
		    	{
		    		LED_Print(LED_DISPLAY_LEFT,false,"VOL.T");
		    		LED_Print(LED_DISPLAY_RIGHT,false,"INIT");
		    	}break;
		    	case 6: 											// 显示初始体积
		    	{
		    		if(val == 0)
		    			LED_Print(LED_DISPLAY_LEFT,false,"  NA");
		    		else
		    			TEST_ShowVolt(val,false,LEDNB_LEFT);
		    		
		    		if(val02 == 0)
		    			LED_Print(LED_DISPLAY_RIGHT,false,"  NA");
		    		else
		    			TEST_ShowVolt(val02,false,LEDNB_RIGHT);
		    	}break;
		    	default:
		    	break;
				}
    	}break;
    	default:
    	break;
    }
    
    //wait for key press
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
      if (globalStatus.valueUpdate)
      {
        globalTestStatus.Mode=EEPROM1.TimeUnits;
        if (globalTestStatus.Mode==TIME_UNIT_TIME)
          val=EEPROM1.TestTime;
        else
          val=EEPROM1.TestRevolutions;
        globalStatus.valueUpdate=false;
        break;
      }
      //if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_LOCKED))
        if (globalStatus.testStart)
          return;
    }
          
    if (KBD_KeyPressed())
    {  
      key=KBD_GetKey();

      if ((!EEPROM1.Ekey)||(globalStatus.ekeyStatus<EKEY_STATUS_LOCKED))
      {  
        switch(key)
        {
          case KBD_CODE_START: 					// 中下键
          {
          	/*
            //start the test - save presets
            if (TIME_UNIT_REVOLUTIONS==globalTestStatus.Mode)
            {
              if (EEPROM1.TestRevolutions!=val)
                EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestRevolutions, (TByte *)&val, sizeof(EEPROM1.TestRevolutions));
            } 
            else
            {
              if (EEPROM1.TestTime!=val)
                EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestTime, (TByte *)&val, sizeof(EEPROM1.TestTime));
            }
            */
            
            // v1.30，Gavin，2019-08-17 星期六 14:00:04
						switch(globalTestStatus.Mode)
						{
							case TIME_UNIT_TIME:
							{
								//eepromTimeUnits=mode;
            		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&globalTestStatus.Mode, sizeof(EEPROM1.TimeUnits));
								if (EEPROM1.TestTime!=val)
                	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestTime, (TByte *)&val, sizeof(EEPROM1.TestTime));
                
		            globalStatus.testStart = true;
		            SND_Play(ssOk);
		            return;
							}break;
							case TIME_UNIT_REVOLUTIONS:
							{
								//eepromTimeUnits=mode;
            		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&globalTestStatus.Mode, sizeof(EEPROM1.TimeUnits));
								if (EEPROM1.TestRevolutions!=val)
                	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestRevolutions, (TByte *)&val, sizeof(EEPROM1.TestRevolutions));
							  
		            globalStatus.testStart = true;
		            SND_Play(ssOk);
		            return;
							}break;
							case TIME_UNIT_ACAL:
							{
								setACAL++;
								switch(setACAL)
								{
									case 1: 							// "TEST CYL1", 量筒选择标题
									{
										SND_Play(ssOk);
									}break;
									case 2: 							// " 500  100", 量筒选择输入
									{
										min = SVM_MCYL_MIN;
								    max = SVM_MCYL_MAX;
								    step = SVM_CHANGE_STEP;
								    
										val = EEPROM1.TestMCyl01;
							  		if(val > max) val = max;
								    if(val < min) val = min;
								    
								    val02 = EEPROM1.TestMCyl02;
							  		if(val02 > max) val02 = max;
								    if(val02 < min) val02 = min;
								    
										switch(DevMode)
										{
											case DEVICE_MODE_121:
											case DEVICE_MODE_122:
											{
												if(val02 != 0)
												{
													val02 = 0;
													EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl02, (TByte *)&val02, sizeof(EEPROM1.TestMCyl02));
												}
											}break;
											case DEVICE_MODE_221:
											case DEVICE_MODE_222:
											//case DEVICE_MODE_223:
											{
												;
											}break;
											default:
											break;
										}
										
								    SND_Play(ssOk);
									}break;
									case 3: 							// 量筒选择输入数据检查，"SAMP WEIH", 质量输入标题
									{
										if((DevMode == DEVICE_MODE_121)||
											 (DevMode == DEVICE_MODE_122))
										{
											val02 = 0;
										}
										
										if((val == 0)&&(val02 == 0))
										{
											setACAL--;
											SND_Play(ssError);
										}
										else
										{
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl01, (TByte *)&val, sizeof(EEPROM1.TestMCyl01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl02, (TByte *)&val02, sizeof(EEPROM1.TestMCyl02));
											SND_Play(ssOk);
										}
									}break;
									case 4: 							// "", 质量输入
									{
										min = SVM_WEIH_MIN;
								    max = SVM_WEIH_MAX;
								    step = SVM_CHANGE_STEP;
								    
								    val = EEPROM1.TestWeih01;
							  		if(val > max) val = max;
								    if(val < min) val = min;
								    
								    val02 = EEPROM1.TestWeih02;
							  		if(val02 > max) val02 = max;
								    if(val02 < min) val02 = min;
								    
										if(EEPROM1.TestMCyl01 == 0)
											val = 0;
										
										if(EEPROM1.TestMCyl02 == 0)
											val02 = 0;
										
								    SND_Play(ssOk);
									}break;
									case 5: 							// 质量输入数据检查，"VOL.T INIT", 体积输入标题
									{
										if((DevMode == DEVICE_MODE_121)||
											 (DevMode == DEVICE_MODE_122))
										{
											val02 = 0;
										}
										
										if((val == 0)&&(val02 == 0))
										{
											setACAL--;
											SND_Play(ssError);
										}
										else
										{
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih01, (TByte *)&val, sizeof(EEPROM1.TestWeih01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih02, (TByte *)&val02, sizeof(EEPROM1.TestWeih02));
											SND_Play(ssOk);
										}
									}break;
									case 6: 							// "", 初始体积输入
									{
										step = SVM_CHANGE_STEP;
								    min = SVM_VOLT_MIN;
								    max = SVM_VOLT_MAX;
								    
								    val = EEPROM1.TestVolt01;
										if(val > max) val = max;
								    if(val < min) val = min;
								    
								    val02 = EEPROM1.TestVolt02;
							  		if(val02 > max) val02 = max;
								    if(val02 < min) val02 = min;
								    
								    if(EEPROM1.TestWeih01 == 0)
											val = 0;
										
										if(EEPROM1.TestWeih02 == 0)
											val02 = 0;
										
								    SND_Play(ssOk);
									}break;
									case 7: 							// 体积输入数据检查，
									{
										if((val == 0)&&(val02 == 0))
										{
											setACAL--;
											SND_Play(ssError);
										}
										else
										{
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt01, (TByte *)&val, sizeof(EEPROM1.TestVolt01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt02, (TByte *)&val02, sizeof(EEPROM1.TestVolt02));
											
											//eepromTimeUnits=mode;
			            		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TimeUnits, (TByte *)&globalTestStatus.Mode, sizeof(EEPROM1.TimeUnits));
											globalStatus.testStart = true;
					            SND_Play(ssOk);
					            return;
					          }
									}break;
									default:
									break;
								}
							}break;
							default:
							break;
						}
					}break;
          case KBD_CODE_SLOW_UP: 				// 右上键
          {
            //increment time/strokes
            /*
            if(globalTestStatus.Mode < TIME_UNIT_ACAL)
            {
	            if (TESTSETUP_IncInt(+step,&val,min,max))
	              SND_Play(ssOk);
	            else
	              SND_Play(ssError);
	          }
	          else
	          	SND_Play(ssError);
	          */
	          
						switch(globalTestStatus.Mode)
						{
							case TIME_UNIT_TIME:
							{
								if (TESTSETUP_IncInt(+step,&val,min,max))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}break;
							case TIME_UNIT_REVOLUTIONS:
							{
								if (TESTSETUP_IncInt(+step,&val,min,max))
								  SND_Play(ssOk);
								else
								  SND_Play(ssError);
							}break;
							case TIME_UNIT_ACAL:
							{
								switch(setACAL)
								{
									case 0: 							// A.CAL 
									case 1: 							// TEST CYL1
									case 3: 							// SAMP WEIH
									case 5: 							// VOL.T INIT
									{
										SND_Play(ssError);
									}break;
									case 2: 							// 量筒选择输入
									{
										switch(DevMode)
						    		{
						    			case DEVICE_MODE_121: 			// 1个头，左边，转速1
						    			case DEVICE_MODE_122: 			// 1个头，左边，转速2
						    			{
						    				SND_Play(ssError);
						    			}break;
						    			case DEVICE_MODE_221: 			// 2个头，两边，转速1
						    			case DEVICE_MODE_222: 			// 2个头，两边，转速2
						    			{
						    				switch(val02)
						    				{
						    					case 0: 
						    						val02 = 100; 
						    						SND_Play(ssOk);
						    						break;
						    					case 100: 
						    						val02 = 250; 
						    						SND_Play(ssOk);
						    						break;
						    					case 250: 
						    						val02 = 500; 
						    						SND_Play(ssOk);
						    						break;
						    					case 500: 
						    						SND_Play(ssError);
						    						break;
						    					default: 
						    						val02 = 0;
						    						SND_Play(ssError);
						    						break;
						    				}
						    				
						    			}break;
						    			case DEVICE_MODE_223: 			// 只有时间测试时用到此型号
						    			{
						    				;
						    				/*
						    				switch(val02)
						    				{
						    					case 0: 
						    						val02 = 100; 
						    						SND_Play(ssOk);
						    						break;
						    					case 100: 
						    						val02 = 250; 
						    						SND_Play(ssOk);
						    						break;
						    					case 250: 
						    						val02 = 500; 
						    						SND_Play(ssOk);
						    						break;
						    					case 500: 
						    						SND_Play(ssError);
						    						break;
						    					default: 
						    						val02 = 0;
						    						SND_Play(ssError);
						    						break;
						    				}
						    				
												if(val != 0)
												{
													val = 0;
													EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl01, (TByte *)&val, sizeof(EEPROM1.TestMCyl01));
												}
												*/
											}break;
						    			default:
						    			break;
						    		}
									}break;
									case 4: 							// 右站初始质量
									{
										if(EEPROM1.TestMCyl02 != 0)
										{
											if (TESTSETUP_IncInt(+step,&val02,min,max))
											  SND_Play(ssOk);
											else
											  SND_Play(ssError);
										}
										else
											SND_Play(ssError);
										
									}break;
									case 6: 							// 右站初始体积
									{
										if(EEPROM1.TestWeih02 != 0)
										{
											if (TESTSETUP_IncInt(+step,&val02,min,max))
											  SND_Play(ssOk);
											else
											  SND_Play(ssError);
										}
										else
											SND_Play(ssError);
									}break;
									default:
									break;
								}
							}break;
							default:
							break;
						}
          }break;
          case KBD_CODE_SLOW_DOWN: 			// 右下键
          {
            //increment time/strokes
            /*
            if(globalTestStatus.Mode < TIME_UNIT_ACAL)
            {
	            if (TESTSETUP_IncInt(-step,&val,min,max))
	              SND_Play(ssOk);
	            else
	              SND_Play(ssError);
	          }
	          else
	          	SND_Play(ssError);
	          */
	          
						switch(globalTestStatus.Mode)
						{
							case TIME_UNIT_TIME:
							{
								if (TESTSETUP_IncInt(-step,&val,min,max))
		              SND_Play(ssOk);
		            else
		              SND_Play(ssError);
							}break;
							case TIME_UNIT_REVOLUTIONS:
							{
								if (TESTSETUP_IncInt(-step,&val,min,max))
		              SND_Play(ssOk);
		            else
		              SND_Play(ssError);
							}break;
							case TIME_UNIT_ACAL:
							{
								switch(setACAL)
								{
									case 0: 							// A.CAL 
									case 1: 							// TEST CYL1
									case 3: 							// SAMP WEIH
									case 5: 							// VOL.T INIT
									{
										SND_Play(ssError);
									}break;
									case 2: 							// 量筒选择输入
									{
										switch(DevMode)
						    		{
						    			case DEVICE_MODE_121:
						    			case DEVICE_MODE_122:
						    			{
						    				SND_Play(ssError);
						    			}break;
						    			case DEVICE_MODE_221:
						    			case DEVICE_MODE_222:
						    			{
						    				switch(val02)
						    				{
						    					case 500: 
						    						val02 = 250; 
						    						SND_Play(ssOk);
						    						break;
						    					case 250: 
						    						val02 = 100; 
						    						SND_Play(ssOk);
						    						break;
						    					case 100: 
						    						val02 = 0; 
						    						SND_Play(ssOk);
						    						break;
						    					case 0: 
						    						SND_Play(ssError);
						    						break;
						    					default: 
						    						val02 = 500;
						    						SND_Play(ssError);
						    						break;
						    				}
						    				
						    			}break;
						    			case DEVICE_MODE_223:
						    			{
						    				;
						    				/*
												switch(val02)
						    				{
						    					case 500: 
						    						val02 = 250; 
						    						SND_Play(ssOk);
						    						break;
						    					case 250: 
						    						val02 = 100; 
						    						SND_Play(ssOk);
						    						break;
						    					case 100: 
						    						val02 = 0; 
						    						SND_Play(ssOk);
						    						break;
						    					case 0: 
						    						SND_Play(ssError);
						    						break;
						    					default: 
						    						val02 = 500;
						    						SND_Play(ssError);
						    						break;
						    				}
						    				
												if(val != 0)
												{
													val = 0;
													EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl01, (TByte *)&val, sizeof(EEPROM1.TestMCyl01));
												}
												*/
						    			}break;
						    			default:
						    			break;
						    		}
									}break;
									case 4: 							// 右站初始质量
									{
										if(EEPROM1.TestMCyl02 != 0)
										{
											if (TESTSETUP_IncInt(-step,&val02,min,max))
												SND_Play(ssOk);
											else
												SND_Play(ssError);
										}
										else
											SND_Play(ssError);
										
									}break;
									case 6: 							// 右站初始体积
									{
										if(EEPROM1.TestWeih02 != 0)
										{
											if (TESTSETUP_IncInt(-step,&val02,min,max))
											  SND_Play(ssOk);
											else
											  SND_Play(ssError);
										}
										else
											SND_Play(ssError);
									}break;
									default:
									break;
								}
							}break;
							default:
							break;
						}
          }break;
          case KBD_CODE_FAST_UP: 				// 左上键
          {
          	/*
            if ((globalTestStatus.Mode==TIME_UNIT_REVOLUTIONS)&&(DevMode!=DEVICE_MODE_223))
            {
              globalTestStatus.Mode=TIME_UNIT_TIME;
              val=EEPROM1.TestTime;
              min=SVM_REV_MIN;
              max=SVM_REV_MAX;
              SND_Play(ssOk);
            } 
            else 
            	SND_Play(ssError);
            */
            
            // v1.30，Gavin，2019-08-16 星期五 11:17:40
            switch(globalTestStatus.Mode)
						{
							case TIME_UNIT_TIME:
							{
								SND_Play(ssError);
							}break;
							case TIME_UNIT_REVOLUTIONS:
							{
								if(DevMode!=DEVICE_MODE_223)
								{
									globalTestStatus.Mode = TIME_UNIT_TIME;
									val=EEPROM1.TestTime;
						  		min=SVM_TIME_MIN;
						  		max=SVM_TIME_MAX;
									SND_Play(ssOk);
								}
								else
									SND_Play(ssError);
							}break;
							case TIME_UNIT_ACAL:
							{
								switch(setACAL)
								{
									case 0: 							// A.CAL 
									{
										if(DevMode!=DEVICE_MODE_223)
						  			{
						    			globalTestStatus.Mode = TIME_UNIT_REVOLUTIONS;
											val=EEPROM1.TestRevolutions;
						          min=SVM_REV_MIN;
						          max=SVM_REV_MAX;
						          
						          if(val > max) val = max;
									    if(val < min) val = min;
									    
						          SND_Play(ssOk);
						        }
						        else
						        	SND_Play(ssError);
									}break;
									case 1: 							// TEST CYL1
									case 3: 							// SAMP WEIH
									case 5: 							// VOL.T INIT
									{
										SND_Play(ssError);
									}break;
									case 2: 							// 量筒选择数据修改
									{
										switch(val)
										{
											case 0:
												val = 100;
												SND_Play(ssOk);
												break;
											case 100:
												val = 250;
												SND_Play(ssOk);
												break;
											case 250:
												val = 500;
												SND_Play(ssOk);
												break;
											case 500:
												SND_Play(ssError);
												break;
											default:
												val = 0;
												SND_Play(ssError);
										}
										
									}break;
									case 4: 							// 左站初始质量
									{
										if(EEPROM1.TestMCyl01 != 0)
										{
											if (TESTSETUP_IncInt(+step,&val,min,max))
												SND_Play(ssOk);
											else
												SND_Play(ssError);
										}
										else
											SND_Play(ssError);
						    		
						    		/*
						    		if(DevMode == DEVICE_MODE_223)
						    		{
						    			if(val02 != 0)
						    			{
						    				val02 = 0;
						    				EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih02, (TByte *)&val02, sizeof(EEPROM1.TestWeih02));
						    			}
						    		}
						    		*/
									}break;
									case 6: 							// 左站初始体积
									{
										if(EEPROM1.TestWeih01 != 0)
										{
											if (TESTSETUP_IncInt(+step,&val,min,max))
											  SND_Play(ssOk);
											else
											  SND_Play(ssError);
										}
										else
											SND_Play(ssError);
									}break;
									default:
									break;
								}
							}break;
							default:
							break;
						}
          }break;
          case KBD_CODE_FAST_DOWN: 			// 左下键
          {
            //decrement time/revs
            /*
            if ((globalTestStatus.Mode==TIME_UNIT_TIME)&&(DevMode!=DEVICE_MODE_223))
            {
              globalTestStatus.Mode=TIME_UNIT_REVOLUTIONS;
              val=EEPROM1.TestRevolutions;
              min=SVM_TIME_MIN;
              max=SVM_TIME_MAX;
              SND_Play(ssOk);
            } 
            else 
            	SND_Play(ssError);
            */
            
            // v1.30，Gavin，2019-08-16 星期五 11:17:40
            switch(globalTestStatus.Mode)
						{
							case TIME_UNIT_TIME:
							{
								if(DevMode!=DEVICE_MODE_223)
								{
									globalTestStatus.Mode = TIME_UNIT_REVOLUTIONS;
									val=EEPROM1.TestRevolutions;
						      min=SVM_REV_MIN;
						      max=SVM_REV_MAX;
						      SND_Play(ssOk);
						    }
						    else
						    	SND_Play(ssError);
							}break;
							case TIME_UNIT_REVOLUTIONS:
							{
								globalTestStatus.Mode = TIME_UNIT_ACAL;
								SND_Play(ssOk);
							}break;
							case TIME_UNIT_ACAL:
							{
								switch(setACAL)
								{
									case 0: 							// A.CAL 
									case 1: 							// TEST CYL1
									case 3: 							// SAMP WEIH
									case 5: 							// VOL.T INIT
									{
										SND_Play(ssError);
									}break;
									case 2: 							// 量筒选择数据输入
									{
										switch(val)
										{
											case 500:
												val = 250;
												SND_Play(ssOk);
												break;
											case 250:
												val = 100;
												SND_Play(ssOk);
												break;
											case 100:
												val = 0;
												SND_Play(ssOk);
												break;
											case 0:
												SND_Play(ssError);
												break;
											default:
												val = 500;
												SND_Play(ssError);
												break;
										}
										
									}break;
									case 4: 							// 左站初始质量
									{
										if(EEPROM1.TestMCyl01 != 0)
										{
											if (TESTSETUP_IncInt(-step,&val,min,max))
										  	SND_Play(ssOk);
											else
										  	SND_Play(ssError);
										}
										else
											SND_Play(ssError);
										
									}break;
									case 6: 							// 左站初始体积
									{
										if(EEPROM1.TestWeih01 != 0)
										{
											if (TESTSETUP_IncInt(-step,&val,min,max))
											  SND_Play(ssOk);
											else
											  SND_Play(ssError);
										}
										else
											SND_Play(ssError);
									}break;
									default:
									break;
								}
							}break;
							default:
							break;
						}
          }break;
          case KBD_CODE_STOP: 					// 中上键
          {
          	switch(globalTestStatus.Mode)
          	{
          		case TIME_UNIT_TIME:
          		{
          			SND_Play(ssOk);
            		CONFIG_Menu();
          		}break;
          		case TIME_UNIT_REVOLUTIONS:
          		{
          			SND_Play(ssOk);
            		CONFIG_Menu();
          		}break;
          		case TIME_UNIT_ACAL:
          		{
          			SND_Play(ssOk);
          			if(setACAL != 0)
          			{
          				setACAL--;
          				switch(setACAL)
          				{
          					case 5: 						// 体积标题
          						EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt01, (TByte *)&val, sizeof(EEPROM1.TestVolt01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt02, (TByte *)&val02, sizeof(EEPROM1.TestVolt02));
          						break;
          					case 4: 						// 质量输入
          						val = EEPROM1.TestWeih01;
								  		val02 = EEPROM1.TestWeih02;
								  		min = SVM_WEIH_MIN;
									    max = SVM_WEIH_MAX;
									    step = SVM_CHANGE_STEP;
          						break;
          					case 3: 						// 质量标题
          						EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih01, (TByte *)&val, sizeof(EEPROM1.TestWeih01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih02, (TByte *)&val02, sizeof(EEPROM1.TestWeih02));
          						break;
          					case 2: 						// 量筒选择
          						val = EEPROM1.TestMCyl01;
								  		val02 = EEPROM1.TestMCyl02;
								  		min = SVM_MCYL_MIN;
									    max = SVM_MCYL_MAX;
									    step = SVM_CHANGE_STEP;
          						break;
          					case 1: 						// 量筒标题
          						EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl01, (TByte *)&val, sizeof(EEPROM1.TestMCyl01));
											EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestMCyl02, (TByte *)&val02, sizeof(EEPROM1.TestMCyl02));
          						break;
          					default:
          					break;
          				}
          				/*
          				if(setACAL == 3)
          				{
          					EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt01, (TByte *)&val, sizeof(EEPROM1.TestVolt01));
										EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestVolt02, (TByte *)&val02, sizeof(EEPROM1.TestVolt02));
          				}
          				else
          				if(setACAL == 2)
          				{
          					val = EEPROM1.TestWeih01;
							  		val02 = EEPROM1.TestWeih02;
							  		min = SVM_WEIH_MIN;
								    max = SVM_WEIH_MAX;
								    step = SVM_CHANGE_STEP;
          				}
          				else
          				{
          					EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih01, (TByte *)&val, sizeof(EEPROM1.TestWeih01));
										EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestWeih02, (TByte *)&val02, sizeof(EEPROM1.TestWeih02));
          				}
          				*/
          			}
          			else
          			{
          				CONFIG_Menu();
          			}
          		}break;
          		default:
          		break;
          	}
            
          }break;
          default:
            //SND_Play(ssError);
            break;
        }   
      } 
      else
      {
        if (globalStatus.ekeyStatus!=EKEY_STATUS_BLOCKED)
        {  
          if (key==KBD_CODE_STOP)
          {
            SND_Play(ssOk);
            CONFIG_Menu();
          } 
          else
          {
            SND_Play(ssError);
          }
        } else
        {
          SND_Play(ssError);
        }  
      }  
    }
  }
}                          
