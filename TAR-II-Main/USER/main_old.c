#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "touch.h"
#include "rtc.h"
#include "timer.h"
#include "can.h"
#include "image2lcd.h"
#include "string.h"

/************************************************
 SVM-LCM
 QIHUAN
 GAVIN
************************************************/

#define TOUCHADDR 0x0000

u16 old_tpx;
u16 old_tpy;
u16 cur_tpx;
u16 cur_tpy;

u8 CurntPage=0; 												// 页码

u8 tcf = 0; 														// 收到触摸标志
//u8 TCNT_step = 0; 											// 计数测试步骤
//u8 TIME_step = 0; 											// 计时测试步骤
//u8 UEP1_step = 0; 											// 压缩率测试
//u8 UEP2_step = 0; 											// 

u8 TestType = 0; 												// 测试类型
#define QTF 1 													// Quick Test
#define EUTF 2 													// EP/USP Test
u8 tt_flag = 0; 												// 更新标志

u8 TD_Method = 0; 											// 测试方法，0未选；1方法一；2方法二；
u8 tdm_flag = 0; 												// 更新标志

u8 TestMode = 0; 												// 快速测试模式，0未选；1计数；2计时；
u8 tm_flag = 0; 												// 更新标志

u16 NumberOfTaps; 									// 计数模式的设置次数；
u8 nbt_flag = 0; 												// 更新标志

u8 TimeOfHour; 										// 计时模式的设置小时；
u8 TimeOfMin; 											// 计时模式的设置分钟；
u8 TimeOfSec; 											// 计时模式的设置秒；

u8 Test_Start = 0; 								// 测试运转标志
//u8 KeyState = 0; 									// 显示键盘状态，0不显示键盘，1数字键盘，2小写字母键盘，3大写键盘
u8 ks_flag = 0; 												// 更新标志

uint8_t PowOnFlag = 0;

u8 ProductNameSta1[100] = "ERWEKA";
u8 pns1_flag = 0; 												// 更新标志
u8 VolumeTypeSta1[100] = "150";
u8 vts1_flag = 0; 												// 更新标志
u8 CylinderNumberSta1[100] = "abcdef";
u8 cns1_flag = 0; 												// 更新标志
u8 BatchNumberSta1[100] = "abcdef";
u8 bns1_flag = 0; 												// 更新标志
u8 InformationSta1[100] = "cdefghijklm HKAD;SKA;DJDHFDJFH15 65456465JGRSjfg-abcdefghijklm HKAD;SKA;DJDHFDJFH15 65456465JGRSjfg";
u8 infors1_flag = 0; 												// 更新标志

u8 ProductNameSta2[100] = "ERWEKA";
u8 pns2_flag = 0; 												// 更新标志
u8 VolumeTypeSta2[100] = "150";
u8 vts2_flag = 0; 												// 更新标志
u8 CylinderNumberSta2[100] = "abcdef";
u8 cns2_flag = 0; 												// 更新标志
u8 BatchNumberSta2[100] = "abcdef";
u8 bns2_flag = 0; 												// 更新标志
u8 InformationSta2[100] = "abcdefghijklmn";
u8 infors2_flag = 0; 												// 更新标志

u8 KeyInStr[100]; 						// 键盘字符串
u8 KIS_ip = 0; 								// 键盘字符串的字符指针
u8 KIS_cd = 0; 								// 键盘字符串的变量指向
u8 keybc_flag = 0;

u8 CdStep = 0; 										// CAN接收数据处理步骤
u8 VarAddrH = 0; 									// 变量地址高位
u8 VarAddrL = 0; 									// 变量地址低位
u8 VarWriteIp = 0; 								// 变量写入指针
u8 VarLen = 0; 										// 变量长度

u16 MOTOR_Revolution = 0; 			// 电机实际运转圈数
u8 mrev_flag = 0; 							// 圈数改变标志

u8 menu_flag = 0; 						// 菜单刷新编制

// 变量定义 -------------------------------------------------
u16 rxd_x1,rxd_x2,rxd_w;
u16 rxd_y1,rxd_y2,rxd_h;
u16 rxd_color1,rxd_color2;  						// 前景色，背景色
u8 rxd_mode,rxd_size,rxd_nblen,rxd_icon;
u8 rxd_imgnb;
u16 rxd_u16dat;
u8 rxd_str[128];

u8 KeyState = 0; 												// 显示键盘状态，0不显示键盘，1数字键盘，2小写字母键盘，3大写键盘
u8 CmdRxSn = 0; 												// 接收指令序号，0~255循环
u8 CmdNb; 															// 接收指令代码；
u8 PmtLen;  														// 接收参数长度；
u8 PmtWip; 															// 接收参数指针；

// 函数声明 -------------------------------------------------
void KeyPadDisplay(u8 kps);
void LCD_ClearSE(u32 color);

int main(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	u8 tbuf[40];
	u8 lcd_id[12];
	u8 cp_flag = 1; 											// 页码切换标志
	u8 i=0;
	u8 t=0;
	//u16 cnt=3000;
	//u8 canbuf[8];
	//u8 res;
	//u8 dir = 0;
	
	HAL_Init();                     			// 初始化HAL库   
	
	Stm32_Clock_Init(360,25,2,8); 				// 设置时钟,180Mhz
	delay_init(180); 											// 初始化延时函数
	
	uart_init(115200); 										// 初始化USART
	LED_Init(); 													// 初始化LED 
	//KEY_Init(); 													// 初始化按键
	SDRAM_Init(); 												// 初始化SDRAM
	LCD_Init(); 													// 初始化LCD
	RTC_Init(); 													// 初始化RTC
	RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); 				// 配置WAKE UP中断,1秒钟中断一次  
	
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id); // 将LCD ID打印到lcd_id数组。	
	
	tp_dev.init();				    //触摸屏初始化 
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_8TQ,6,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	//CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_8TQ,6,CAN_MODE_LOOPBACK); //CAN初始化,波特率500Kbps      
	//delay_ms(1500);
 	//Load_Drow_Dialog();	 	
	
	//定时器5初始化，定时器时钟为90M，分频系数为9000-1，
	//所以定时器5的频率为90M/9000=10K，自动重装载为5000-1，那么定时器周期就是500ms
	//TIM5_Init(5000-1,9000-1);
	//所以定时器5的频率为90M/9000=10K，自动重装载为10-1，那么定时器周期就是1ms
	TIM5_Init(20-1,9000-1);
	BeepTime = 25;
	
	while(1)
	{
		// 触摸控制 ------------------------
		if((tp_dev.scan(0) != 0)&& 						// 有触摸信号
			 (tp_dev.sta&0xc1))
		{
			tp_onf = 1;
			if((tp_dev.x[0] < old_tpx-10)||
				 (tp_dev.x[0] > old_tpx+10))
			{
				old_tpx = tp_dev.x[0];
				tp_ktime = 0;
			}
			
			if((tp_dev.y[0] < old_tpy-10)||
				 (tp_dev.y[0] > old_tpy+10))
			{
				old_tpy = tp_dev.y[0];
				tp_ktime = 0;
			}
		}
		else
		{
			if(tp_dev.sta == 0x0040)
			{
				if(tp_ktime >= 5)
				{
					//cur_tpx = old_tpx;
					//cur_tpy = old_tpy;
					
					cur_tpx = ((u32)old_tpx*95)/100;
					cur_tpy = ((u32)old_tpy*95)/100;
					
					tcf = 1; 											// 收到触摸信号标志
					BeepTime = 25;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					
					sprintf((char*)tbuf,"x,y: %04d,%04d; ",cur_tpx,cur_tpy); 		// 将触摸坐标打印到tbuf数组。	
					LCD_ShowString(30,0,200,24,24,tbuf,0); 										// 提示触摸坐标数值
					
					// CAN TX ----------------------------------------
					CanTxBuf[0] = 0xA5;
					CanTxBuf[1] = (u8)(12345>>8);
					CanTxBuf[2] = (u8)(12345&0x00ff);
					CanTxBuf[3] = 0xA0;
					
					CanTxBuf[4] = cur_tpx>>8;
					CanTxBuf[5] = cur_tpx;
					CanTxBuf[6] = cur_tpy>>8;
					CanTxBuf[7] = cur_tpy;
					
					CAN1_Send_Msg(CanTxBuf,8);
					//CAN1_Send_Dat((u16)TOUCHADDR, 8, CanTxBuf);
				}
				
				tp_ktime = 0;
				tp_onf = 0;
			}
		}
		
		//can接收数据 --------------------------
		while(CBRIp0 != CBRIp1) 							// 接收到新数据
		{
			switch(CdStep) 										// 数据处理进程
			{
				case 0: 												// 帧头+地址+长度+验证
				{
					if(CanRxBuf[CBRIp0][0] == 0xA5) 														// 校验帧头
					{
						if((CanRxBuf[CBRIp0][1] == (uint8_t)(12345>>8))&&
							 (CanRxBuf[CBRIp0][2] == (uint8_t)(12345&0x00ff))) 			// 校验密码
						{
							if(CanRxBuf[CBRIp0][3] == CmdRxSn) 											// 校验指令序号，0~255循环
							{
								CmdRxSn++; 												// 指令序号累加
								CmdNb = CanRxBuf[CBRIp0][4]; 			// 接收指令代码
								PmtLen = CanRxBuf[CBRIp0][6]; 		// 接收参数长度
								
								PmtWip = 0; 											// 参数指针清零
								CdStep++; 												// 帧解析状态机++
							}
						}
						else
						if((CanRxBuf[CBRIp0][1] == (uint8_t)(54321>>8))&&
							 (CanRxBuf[CBRIp0][2] == (uint8_t)(54321&0x00ff))) 			// 校验密码
						{
							NVIC_SystemReset(); 																		// 软件复位
						}
					}
				}break;
				case 1: 												// 接收数据
				{
					for(i=0;i<8;i++)
					{
						if(PmtWip < PmtLen)
							rxd_str[PmtWip] = CanRxBuf[CBRIp0][i];
						else
							rxd_str[PmtWip] = 0;
						
						PmtWip++;
					}
					
					if(PmtWip >= PmtLen) 					// 参数接完成
					{
						switch(CmdNb) 							// 指令代码
						{
							case 10: 										// 清屏
							{
								rxd_color1 = rxd_str[0];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[1];
								
								LCD_Clear(rxd_color1);
							}break;
							case 11: 										// 清屏+SVM+ERWEKA
							{
								rxd_color1 = rxd_str[0];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[1];
								
								LCD_ClearSE(rxd_color1);
								CdStep = 0; 												// 帧解析状态机复位
							}break;
							case 20: 										// 单色矩形填充
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_x2 = rxd_str[4];
								rxd_x2 <<= 8;
								rxd_x2 += rxd_str[5];
								
								rxd_y2 = rxd_str[6];
								rxd_y2 <<= 8;
								rxd_y2 += rxd_str[7];
								
								rxd_color1 = rxd_str[8];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[9];
								
								LCD_Fill(rxd_x1,rxd_y1,rxd_x2,rxd_y2,rxd_color1);
							}break;
							case 30: 										// 图标显示
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_w = rxd_str[4];
								rxd_w <<= 8;
								rxd_w += rxd_str[5];
								
								rxd_h = rxd_str[6];
								rxd_h <<= 8;
								rxd_h += rxd_str[7];
								
								rxd_color1 = rxd_str[8];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[9];
								
								rxd_color2 = rxd_str[10];
								rxd_color2 <<= 8;
								rxd_color2 += rxd_str[11];
								
								rxd_mode = rxd_str[12];
								rxd_icon = rxd_str[13];
								
								POINT_COLOR = rxd_color1;
								BACK_COLOR = rxd_color2;
								LCD_ShowIcon(rxd_x1,rxd_y1,rxd_w,rxd_h,rxd_icon,rxd_mode); 			// 图标显示
							}break;
							case 40: 									// 字符串显示
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_w = rxd_str[4];
								rxd_w <<= 8;
								rxd_w += rxd_str[5];
								
								rxd_h = rxd_str[6];
								rxd_h <<= 8;
								rxd_h += rxd_str[7];
								
								rxd_color1 = rxd_str[8];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[9];
								
								rxd_color2 = rxd_str[10];
								rxd_color2 <<= 8;
								rxd_color2 += rxd_str[11];
								
								rxd_mode = rxd_str[12];
								rxd_size = rxd_str[13];
								
								POINT_COLOR = rxd_color1;
								BACK_COLOR = rxd_color2;
								
								LCD_ShowYanStr(rxd_x1,rxd_y1,rxd_w,rxd_h,rxd_size,rxd_str+14,rxd_mode); 	// 字符串显示
							}break;
							case 50: 									// 数字显示，不显示前置0
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_color1 = rxd_str[4];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[5];
								
								rxd_color2 = rxd_str[6];
								rxd_color2 <<= 8;
								rxd_color2 += rxd_str[7];
								
								rxd_nblen = rxd_str[8];
								rxd_size = rxd_str[9];
								
								//rxd_u16dat = rxd_str[10];
								//rxd_u16dat <<= 24;
								//rxd_u16dat += rxd_str[11];
								rxd_u16dat = rxd_str[12];
								rxd_u16dat <<= 8;
								rxd_u16dat += rxd_str[13];
								
								POINT_COLOR = rxd_color1;
								BACK_COLOR = rxd_color2;
								
								LCD_ShowYanNum(rxd_x1,rxd_y1,rxd_u16dat,rxd_nblen,rxd_size); 	// 数字显示，不显示前置0
							}break;
							case 51: 									// 数字显示，显示前置0
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_color1 = rxd_str[4];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[5];
								
								rxd_color2 = rxd_str[6];
								rxd_color2 <<= 8;
								rxd_color2 += rxd_str[7];
								
								rxd_nblen = rxd_str[8];
								rxd_size = rxd_str[9];
								rxd_mode = rxd_str[10];
								
								//rxd_u16dat = rxd_str[11];
								//rxd_u16dat <<= 8;
								//rxd_u16dat += rxd_str[12];
								rxd_u16dat = rxd_str[13];
								rxd_u16dat <<= 8;
								rxd_u16dat += rxd_str[14];
								
								POINT_COLOR = rxd_color1;
								BACK_COLOR = rxd_color2;
								
								LCD_ShowxYanNum(rxd_x1,rxd_y1,rxd_u16dat,rxd_nblen,rxd_size,rxd_mode); 	// 数字显示，显示前置0
							}break;
							case 60: 									// 图片显示，彩色
							{
								rxd_x1 = rxd_str[0];
								rxd_x1 <<= 8;
								rxd_x1 += rxd_str[1];
								
								rxd_y1 = rxd_str[2];
								rxd_y1 <<= 8;
								rxd_y1 += rxd_str[3];
								
								rxd_imgnb = rxd_str[4];
								
								image_disbynb(rxd_x1,rxd_y1,rxd_imgnb);
							}break;
							case 70: 									// 键盘显示
							{
								KeyState = rxd_str[0];
								
								KeyPadDisplay(KeyState);
							}break;
							default:
							{
								CdStep = 0;
							}break;
						}
						
						CdStep = 0; 								// 帧解析状态机复位
					}
				}break;
				default:
				{
					CdStep = 0;
				}break;
			}
			
			if(CBRIp0 < 31)
				CBRIp0++;
			else
				CBRIp0 = 0;
		}
		
		///*
		switch(CurntPage)
		{
			case 0: 													// 首页
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					LCD_Fill(32,112,32+800,112+376,WHITE);
					LCD_Fill(32,112,32+800,112+48,GRAY);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0);
					
					POINT_COLOR = WHITE;
					LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,112,112,128,11,0);
					LCD_ShowIcon(880,272,112,128,12,0);
					LCD_ShowIcon(928,512,64,64,13,0);
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					LCD_ShowYanStr(46,124,1020,36,28,"Tapped Density Testing with SVM",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(75,192,290,24,22,"Systemstatus: ",1); 	
					LCD_ShowYanStr(75,220,800,24,22,"Teststations: 2",1);
					LCD_ShowYanStr(75,248,800,24,22,"Test Apparatus: USP 1",1);
					
					POINT_COLOR = GREEN;
					LCD_ShowYanStr(210,192,290,24,22,"OK",1); 	
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(890,206,100,24,22,"Quick Test",1);	
					LCD_ShowYanStr(900,352,70,24,22,"EP/USP",1);
					LCD_ShowYanStr(915,375,60,24,22,"Test",1);
					
					image_display(395,161,(u8*)gImage_LiangTong);
				}
				
				if(dtFlash == 0) 								// 日期时间刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(menu_flag)
				{
					menu_flag = 0;
					LCD_Fill(0,0,320,600,GRAY);
				}
			}break;
			case 1: 													// 测试方法选择页面
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横标
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1); 				// SVM+
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA标
					
					//POINT_COLOR = BLACK;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					//LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方，Abort
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上小灰横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"USP/EP Test - Step 1",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(222,176,650,24,22,"Please select testing method according to ",1); 	
					LCD_ShowYanStr(222,200,650,24,22,"installed hardware.",1); 	
					
					//LCD_Fill(224,256,224+232,256+192,GRAY1); 					// 中间灰框
					//LCD_Fill(488,256,488+232,256+192,GRAY1); 					// 中间灰框
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(756,256,44,48,15,0); 								// 圆圈？
					
					//POINT_COLOR = GRAY1;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(376,464,80,80,26,0); 								// 对勾，选中
					//LCD_ShowIcon(640,464,80,80,26,0); 								// 对勾，选中
					
					POINT_COLOR = BLACK;
					//LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
					//LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
					//LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
					//LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1); 	
					
					//LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1); 	
					//LCD_ShowYanStr(507,300,650,24,22,"Method 2",1); 	
					//LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1); 	
					//LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1); 	
					
					LCD_ShowYanStr(75,350,650,24,22,"Stroke height",1); 	
					LCD_ShowYanStr(75,400,650,24,22,"Taps per min.",1); 	
					
					//image_display(395,161,(u8*)gImage_039);
					
					tdm_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(tdm_flag)
				{
					tdm_flag = 0;
					
					switch(TD_Method)
					{
						case 0: 								// 没有选中
						{
							LCD_Fill(224,256,224+232,256+192,GRAY1); 			// 左灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1);
							
							LCD_Fill(488,256,488+232,256+192,GRAY1); 			// 右灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(880,112,112,128,14,0); 				// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						case 1: 								// 方法一
						{
							LCD_Fill(224,256,224+232,256+192,0x2D3A); 		// 中间灰框
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = WHITE;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1); 	
							
							LCD_Fill(488,256,488+232,256+192,GRAY1); 			// 中间灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 					// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						case 2: 								// 方法二
						{
							LCD_Fill(488,256,488+232,256+192,0x2D3A); 		// 中间灰框
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = WHITE;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							LCD_Fill(224,256,224+232,256+192,GRAY1); 			// 中间灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 				// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						default:
						break;
					}
				}
				
			}break;
			case 2: 													// 计数/计时测试模式选择页面
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					LCD_Fill(32,112,32+800,112+464,WHITE);
					LCD_Fill(32,112,32+800,112+48,GRAY);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0);
					
					//POINT_COLOR = BLACK;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0);
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(212,280,44,48,15,0); 								// 圆圈？
					LCD_ShowIcon(616,280,44,48,15,0);
					
					//POINT_COLOR = GRAY1;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(280,240,136,136,16,0); 							// 双循环箭头大
					//LCD_ShowIcon(456,240,136,136,17,0); 							// 手表大
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
										
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 1",1);
					
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(196,483,650,24,22,"Enter either number of taps or time value for testing.",1); 	
					
					POINT_COLOR = BLACK;
					//LCD_ShowYanStr(304,334,100,24,22,"Tap count",1);
					//LCD_ShowYanStr(502,334,50,24,22,"Time",1);
					//LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					//image_display(395,161,(u8*)gImage_039);
					tm_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(tm_flag)
				{
					tm_flag = 0;
					
					switch(TestMode) 							// 计数/计时参数
					{
						case 0: 								// 未选中
						{
							POINT_COLOR = BLACK;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(880,112,112,128,14,0); 		// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
							
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(280,240,136,136,16,0); 		// 上下双箭头循环
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(304,334,100,24,22,"Tap count",1);
							
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(456,240,136,136,17,0); 		// 手表大
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(502,334,100,24,22,"Time",1);
							
						}break;
						case 1: 								// 计数模式
						{
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(456,240,136,136,17,0); 		// 手表大
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(502,334,100,24,22,"Time",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 		// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
							
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(280,240,136,136,16,0); 		// 上下双箭头循环
							LCD_ShowYanStr(304,334,100,24,22,"Tap count",1);
						}break;
						case 2: 								// 计时模式
						{
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(280,240,136,136,16,0); 		// 上下双箭头循环
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(304,334,100,24,22,"Tap count",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 		// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
							
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(456,240,136,136,17,0); 		// 手表大
							LCD_ShowYanStr(502,334,100,24,22,"Time",1);
						}break;
						default:
						break;
					}
				}
			}break;
			case 3: 													// 计数测试模式页面
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					LCD_Fill(32,112,32+800,112+464,WHITE);
					LCD_Fill(32,112,32+800,112+48,GRAY);
					LCD_Fill(64,208,64+576+80,208+80,GRAY);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 					// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(880,112,112,128,14,0); 		// 右箭头
					LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,272,112,128,20,0); 		// 左箭头
					LCD_ShowYanStr(916,364,50,24,22,"Back",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0);
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(756,223,44,48,15,0);
					//LCD_ShowIcon(616,280,44,48,15,0);
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = GRAY;
					LCD_ShowIcon(80,223,44,48,19,0); 		// 上下双箭头循环
					
					//POINT_COLOR = GRAY1;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(456,240,136,136,17,0);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
										
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 1",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(63,174,650,24,22,"Number of taps",1); 	
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GRAY;
					LCD_ShowNum(577,247,NumberOfTaps,4,32); 					// 显示测试次数
					
					//POINT_COLOR = WHITE;
					//LCD_ShowYanStr(304,334,100,24,22,"Tap count",1);
					//POINT_COLOR = BLACK;
					//LCD_ShowYanStr(502,334,50,24,22,"Time",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(ks_flag)
				{
					ks_flag = 0;
					
					KeyPad(KeyState);
					nbt_flag = 1;
				}
				
				if(nbt_flag)
				{
					nbt_flag = 0;
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowNum(513,127,NumberOfTaps,4,32);
				}
				
			}break;
			case 4: 													// 计时测试模式页面
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					LCD_Fill(32,112,32+800,112+464,WHITE);
					LCD_Fill(32,112,32+800,112+48,GRAY);
					
					LCD_Fill(64,208,64+208,208+80,GRAY); 							// 小时显示框
					LCD_Fill(288,243,288+4,243+4,GRAY); 							// ：
					LCD_Fill(288,261,288+4,261+4,GRAY); 							// ：
					LCD_Fill(304,208,304+192,208+80,GRAY); 						// 分钟显示框
					LCD_Fill(511,243,511+4,243+4,GRAY); 							// ：
					LCD_Fill(511,261,511+4,261+4,GRAY); 							// ：
					LCD_Fill(528,208,528+192,208+80,GRAY); 						// 秒钟显示框
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 1",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(63,174,650,24,22,"Time",1); 	
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,272,112,128,20,0); 							// 左箭头
					LCD_ShowYanStr(916,364,50,24,22,"Back",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = GRAY;
					LCD_ShowIcon(80,224,44,48,21,0); 									// 手表小
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(144,240,60,24,22,"hh",1);
					LCD_ShowYanStr(450,240,60,24,22,"mm",1);
					LCD_ShowYanStr(685,240,60,24,22,"ss",1);
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(756,223,44,48,15,0); 								// 圆圈？
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
			}break;
			case 5: 													// 快速测试，step2，测试站1
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 2-Test Station 1",1);
					
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(56,174,650,24,22,"Product Name (Station 1)",1); 	
					//LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(328,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(415,174,650,24,22,"Batch Number (Station 1)",1); 	
					//LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,220,650,24,22,"Batch Number",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,270,650,24,22,"Volume (Station 1)",1); 	
					//LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,310,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(414,271,650,24,22,"Information:",1); 	
					//LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,308,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,366,650,24,22,"Cylinder number",1); 	
					//LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,406,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,272,112,128,20,0); 							// 左箭头
					LCD_ShowYanStr(916,364,50,24,22,"Back",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
					
					pns1_flag = 1;
					vts1_flag = 1;
					cns1_flag = 1;
					bns1_flag = 1;
					infors1_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(ks_flag) 										// 键盘显示刷新
				{
					ks_flag = 0;
					
					KeyPad(KeyState);
					
					if(KeyState != 0)
						keybc_flag = 1; 							// 刷新键盘输入框
				}
				
				if(pns1_flag) 									// 产品名称更新
				{
					pns1_flag = 0;
					
					LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,219,650,24,28,ProductNameSta1,1);
					
				}
				
				if(vts1_flag) 									// 体积更新
				{
					vts1_flag = 0;
					
					LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(180,315,650,24,28,VolumeTypeSta1,1);
				}
				
				if(cns1_flag) 									// 柱面更新
				{
					cns1_flag = 0;
					
					LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,410,650,24,28,CylinderNumberSta1,1);
				}
				
				if(bns1_flag) 									// 批号更新
				{
					bns1_flag = 0;
					
					LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,220,650,24,28,BatchNumberSta1,1);
				}
				
				if(infors1_flag) 								// 信息更新
				{
					infors1_flag = 0;
					
					LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,335,220,200,28,InformationSta1,1);
				}
				
				if(keybc_flag)
				{
					keybc_flag = 0;
					
					LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(140,90,460,150,28,KeyInStr,1);
				}
			}break;
			case 6: 													// 快速测试，step3，测试站2
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 3-Test Station 2",1);
					
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(56,174,650,24,22,"Product Name (Station 2)",1); 	
					//LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(328,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(415,174,650,24,22,"Batch Number (Station 2)",1); 	
					//LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,220,650,24,22,"Batch Number",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,270,650,24,22,"Volume (Station 2)",1); 	
					//LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,310,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(414,271,650,24,22,"Information:",1); 	
					//LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,308,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,366,650,24,22,"Cylinder number",1); 	
					//LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,406,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,272,112,128,20,0); 							// 左箭头
					LCD_ShowYanStr(916,364,50,24,22,"Back",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
					
					pns2_flag = 1;
					vts2_flag = 1;
					cns2_flag = 1;
					bns2_flag = 1;
					infors2_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(ks_flag) 										// 键盘显示刷新
				{
					ks_flag = 0;
					
					KeyPad(KeyState);
					
					if(KeyState != 0)
						keybc_flag = 1; 							// 刷新键盘输入框
				}
				
				if(pns2_flag) 									// 产品名称更新
				{
					pns2_flag = 0;
					
					LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,219,650,24,28,ProductNameSta2,1);
					
				}
				
				if(vts2_flag) 									// 体积更新
				{
					vts2_flag = 0;
					
					LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(180,315,650,24,28,VolumeTypeSta2,1);
				}
				
				if(cns2_flag) 									// 柱面更新
				{
					cns2_flag = 0;
					
					LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,410,650,24,28,CylinderNumberSta2,1);
				}
				
				if(bns2_flag) 									// 批号更新
				{
					bns2_flag = 0;
					
					LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,220,650,24,22,"Batch Number",1);
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,220,650,24,28,BatchNumberSta2,1);
				}
				
				if(infors2_flag) 								// 信息更新
				{
					infors2_flag = 0;
					
					LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,335,220,200,28,InformationSta2,1);
				}
				
				if(keybc_flag)
				{
					keybc_flag = 0;
					
					LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(140,90,460,150,28,KeyInStr,1);
				}
			}break; 
			case 7: 													// Quick Test – Ready
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,112,112,128,22,0); 							// 大圆右三角，START
					LCD_ShowYanStr(916,204,50,24,22,"Start",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test - Ready",1);
					
					// 工作站1信息 ------------------------------------
					LCD_Fill(352,192,352+256,192+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,200,150,30,22,ProductNameSta1,1);
					LCD_ShowYanStr(374,225,150,30,22,BatchNumberSta1,1);
					LCD_ShowYanStr(374,250,150,30,22,"Glass 1",1);
					LCD_ShowYanStr(374,275,150,30,22,"Hopper 1",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,200,60,60,40,"1",1);
					
					// 工作站2信息 ------------------------------------
					LCD_Fill(352,384,352+256,384+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,393,150,108,22,ProductNameSta2,1);
					LCD_ShowYanStr(374,418,150,108,22,BatchNumberSta2,1);
					LCD_ShowYanStr(374,443,150,108,22,"Glass 2",1);
					LCD_ShowYanStr(374,468,150,108,22,"Hopper 2",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,392,60,60,40,"2",1);
					
					if(TestMode == 1)
					{
						// 计数模式 ---------------------------------------
						LCD_Fill(64,192,64+256,192+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						//LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowNum(79,200,NumberOfTaps,4,32);
						LCD_ShowYanStr(176,242,650,24,40,"--",1);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					else
					{
						// 计数模式 ---------------------------------------
						LCD_Fill(64,192,64+256,192+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowYanStr(176,242,650,24,40,"--",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					
					if(TestMode == 2)
					{
						// 计时模式 ---------------------------------------
						LCD_Fill(64,384,64+256,384+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowYanStr(107,451,171,62,40,"00:00:00",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,514,96+192,514+15,GRAY1); 						// 进度条
					}
					else
					{
						// 计时模式 ---------------------------------------
						LCD_Fill(64,384,64+256,384+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowYanStr(107,451,171,62,40,"00:00:00",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,514,96+192,514+15,GRAY1); 						// 进度条
					}
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
			}break;
			case 8: 													// Quick Test – Starting
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test - Starting",1);
					
					// 工作站1信息 -------------------------------------
					LCD_Fill(352,192,352+256,192+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,200,150,30,22,ProductNameSta1,1);
					LCD_ShowYanStr(374,225,150,30,22,BatchNumberSta1,1);
					LCD_ShowYanStr(374,250,150,30,22,"Glass 1",1);
					LCD_ShowYanStr(374,275,150,30,22,"Hopper 1",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,200,60,60,40,"1",1);
					
					// 工作站2信息 -------------------------------------
					LCD_Fill(352,384,352+256,384+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,393,150,108,22,ProductNameSta2,1);
					LCD_ShowYanStr(374,418,150,108,22,BatchNumberSta2,1);
					LCD_ShowYanStr(374,443,150,108,22,"Glass 2",1);
					LCD_ShowYanStr(374,468,150,108,22,"Hopper 2",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,392,60,60,40,"2",1);
					
					if(TestMode == 1)
					{
						LCD_Fill(64,192,64+256,192+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						//LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowNum(79,200,NumberOfTaps,4,32);
						LCD_ShowYanStr(176,242,650,24,40,"--",1);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					else
					{
						LCD_Fill(64,192,64+256,192+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowYanStr(176,242,650,24,40,"--",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					
					if(TestMode == 2)
					{
						LCD_Fill(64,384,64+256,384+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowYanStr(168,451,171,62,40,"10",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,512,96+192,512+16,GRAY1); 						// 进度条
						LCD_Fill(96,512,96+183,512+16,RED); 							// 进度条
					}
					else
					{
						LCD_Fill(64,384,64+256,384+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowYanStr(168,451,171,62,40,"10",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,512,96+192,512+16,GRAY); 						// 进度条
						LCD_Fill(96,512,96+183,512+16,RED); 							// 进度条
					}
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(mrev_flag)
				{
					mrev_flag = 0;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GRAY;
					
					LCD_ShowNum(150,250,MOTOR_Revolution,4,32);
					//LCD_ShowYanStr(176,242,650,24,40,"--",1);
				}
			}break;
			case 9: 													// Quick Test – Running
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(736,112,256,255,23,0); 							// 大图标量具
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					LCD_Fill(32,112,32+656,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+656,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test - Running",1);
					
					// 工作站1信息 --------------------------------------
					LCD_Fill(352,192,352+256,192+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,200,150,30,22,ProductNameSta1,1);
					LCD_ShowYanStr(374,225,150,30,22,BatchNumberSta1,1);
					LCD_ShowYanStr(374,250,150,30,22,"Glass 1",1);
					LCD_ShowYanStr(374,275,150,30,22,"Hopper 1",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,200,60,60,40,"1",1);
					
					// 工作站2信息 -------------------------------------
					LCD_Fill(352,384,352+256,384+160,GRAY1); 					// 显示框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(374,393,150,108,22,ProductNameSta2,1);
					LCD_ShowYanStr(374,418,150,108,22,BatchNumberSta2,1);
					LCD_ShowYanStr(374,443,150,108,22,"Glass 2",1);
					LCD_ShowYanStr(374,468,150,108,22,"Hopper 2",1);
					POINT_COLOR = GRAY;
					LCD_ShowYanStr(578,392,60,60,40,"2",1);
					
					if(TestMode == 1)
					{
						LCD_Fill(64,192,64+256,192+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						//LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowNum(79,200,NumberOfTaps,4,32);
						//LCD_ShowYanStr(128,242,650,24,40,"149.9",1);
						LCD_ShowNum(128,242,MOTOR_Revolution,4,32);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					else
					{
						LCD_Fill(64,192,64+256,192+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(79,200,650,24,22,"150",1);
						LCD_ShowYanStr(128,242,650,24,40,"149.9",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowIcon(268,222,44,48,19,0); 								// 循环双箭头小
					}
					
					if(TestMode == 2)
					{
						LCD_Fill(64,384,64+256,384+160,GRAY); 						// 显示框
						POINT_COLOR = WHITE;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = WHITE;
						BACK_COLOR = GRAY;
						LCD_ShowYanStr(107,451,171,62,40,"00:10:35",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,509,96+192,509+15,GRAY1); 						// 进度条
						LCD_Fill(96,509,96+112,509+15,BLACK); 						// 进度条
					}
					else
					{
						LCD_Fill(64,384,64+256,384+160,GRAY1); 						// 显示框
						POINT_COLOR = BLACK;
						LCD_ShowYanStr(80,393,78,29,22,"00:15:00",1);
						POINT_COLOR = BLACK;
						BACK_COLOR = GRAY1;
						LCD_ShowYanStr(107,451,171,62,40,"00:10:35",1);
						LCD_ShowIcon(268,395,44,48,21,0); 								// 手表小
						LCD_Fill(96,509,96+192,509+15,GRAY); 						// 进度条
						LCD_Fill(96,509,96+112,509+15,BLACK); 						// 进度条
					}
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
			}break;
			case 10: 													// Quick Test – Finished – Summary
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,112,112,128,24,0); 							// 打印机图标
					LCD_ShowYanStr(916,206,60,24,22,"Print",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,25,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(911,523,60,24,22,"Close",1);
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					LCD_Fill(32,208,32+800,208+96,GRAY1); 						// 中间灰色横条
					LCD_Fill(32,352,32+800,352+192,GRAY1); 						// 中间灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test - Finished - Summary",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowYanStr(45,174,650,24,28,"1  Status: Test completed.",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = GRAY1;
					LCD_ShowYanStr(45,220,650,24,28,"2  RPM: 150 ",1);
					LCD_ShowYanStr(172,266,650,24,28,"Min. 149.8 / Max. 150.1 ",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowYanStr(45,314,650,24,28,"3  Time: 00:10:00 (hh:mm:ss)",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = GRAY1;
					LCD_ShowYanStr(45,364,650,24,28,"4  Station 1:",1);
					LCD_ShowYanStr(172,410,650,24,28,ProductNameSta1,1);
					LCD_ShowYanStr(172,435,650,24,28,BatchNumberSta1,1);
					LCD_ShowYanStr(172,460,650,24,28,"Glass 1",1);
					LCD_ShowYanStr(172,485,650,24,28,"Hopper 1",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowYanStr(45,554,650,24,28,"4  Station 1:",1);
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
			}break;
			case 11:
			{
				if(cp_flag)
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					//image_display(0,0,(u8*)gImage_KeyPad2);
				}
			}break;
			case 20: 													// EP/USP Test - Step 1
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横标
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1); 				// SVM+
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA标
					
					//POINT_COLOR = BLACK;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					//LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方，Abort
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上小灰横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"USP/EP Test - Step 1",1);
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(222,176,650,24,22,"Please select testing method according to ",1); 	
					LCD_ShowYanStr(222,200,650,24,22,"installed hardware.",1); 	
					
					//LCD_Fill(224,256,224+232,256+192,GRAY1); 					// 中间灰框
					//LCD_Fill(488,256,488+232,256+192,GRAY1); 					// 中间灰框
					
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(756,256,44,48,15,0); 								// 圆圈？
					
					//POINT_COLOR = GRAY1;
					//BACK_COLOR = GRAY;
					//LCD_ShowIcon(376,464,80,80,26,0); 								// 对勾，选中
					//LCD_ShowIcon(640,464,80,80,26,0); 								// 对勾，选中
					
					POINT_COLOR = BLACK;
					//LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
					//LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
					//LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
					//LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1); 	
					
					//LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1); 	
					//LCD_ShowYanStr(507,300,650,24,22,"Method 2",1); 	
					//LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1); 	
					//LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1); 	
					
					LCD_ShowYanStr(75,350,650,24,22,"Stroke height",1); 	
					LCD_ShowYanStr(75,400,650,24,22,"Taps per min.",1); 	
					
					//image_display(395,161,(u8*)gImage_039);
					
					tdm_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(tdm_flag)
				{
					tdm_flag = 0;
					
					switch(TD_Method)
					{
						case 0: 								// 没有选中
						{
							LCD_Fill(224,256,224+232,256+192,GRAY1); 			// 左灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1);
							
							LCD_Fill(488,256,488+232,256+192,GRAY1); 			// 右灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(880,112,112,128,14,0); 				// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						case 1: 								// 方法一
						{
							LCD_Fill(224,256,224+232,256+192,0x2D3A); 		// 中间灰框
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = WHITE;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1); 	
							
							LCD_Fill(488,256,488+232,256+192,GRAY1); 			// 中间灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 					// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						case 2: 								// 方法二
						{
							LCD_Fill(488,256,488+232,256+192,0x2D3A); 		// 中间灰框
							POINT_COLOR = WHITE;
							BACK_COLOR = 0x2D3A;
							LCD_ShowIcon(640,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = WHITE;
							LCD_ShowYanStr(507,271,650,24,22,"Tapped Density",1);
							LCD_ShowYanStr(507,300,650,24,22,"Method 2",1);
							LCD_ShowYanStr(507,350,650,24,22,"3 +/- 0.2 mm",1);
							LCD_ShowYanStr(507,400,650,24,22,"250 +/- 15 taps/min",1);
							
							LCD_Fill(224,256,224+232,256+192,GRAY1); 			// 中间灰框
							POINT_COLOR = GRAY1;
							BACK_COLOR = GRAY;
							LCD_ShowIcon(376,464,80,80,26,0); 						// 对勾，选中
							
							POINT_COLOR = BLACK;
							LCD_ShowYanStr(243,271,650,24,22,"Tapped Density",1); 	
							LCD_ShowYanStr(243,300,650,24,22,"Method 1",1); 	
							LCD_ShowYanStr(243,350,650,24,22,"14 +/- 2 mm",1); 	
							LCD_ShowYanStr(243,400,650,24,22,"300 +/- 15 taps/min",1);
							
							POINT_COLOR = BLACK;
							BACK_COLOR = WHITE;
							LCD_ShowIcon(880,112,112,128,14,0); 				// 右箭头
							LCD_ShowYanStr(916,204,50,24,22,"Next",1);
						}break;
						default:
						break;
					}
				}
				
				tm_flag = 1; 										// 测试模式刷新标志
			}break;
			case 21: 													// USP/EP Test - Step 2
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					
					LCD_Clear(BLACK);
					
					POINT_COLOR = GRAY;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(32,30,64,48,9,0); 										// 三横
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					LCD_ShowIcon(770,30,235,48,10,1); 								// ERWEKA
					
					LCD_Fill(32,112,32+800,112+464,WHITE); 						// 大白框
					LCD_Fill(32,112,32+800,112+48,GRAY); 							// 上灰色横条
					
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(47,124,1020,36,28,"Quick Test-Step 2-Test Station 1",1);
					
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(56,174,650,24,22,"Product Name (Station 1)",1); 	
					//LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(328,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(415,174,650,24,22,"Batch Number (Station 1)",1); 	
					//LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,220,650,24,22,"Batch Number",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,215,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,270,650,24,22,"Volume (Station 1)",1); 	
					//LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,310,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(414,271,650,24,22,"Information:",1); 	
					//LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					//POINT_COLOR = GRAY1;
					//LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(689,308,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(57,366,650,24,22,"Cylinder number",1); 	
					//LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = GRAY1;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(329,406,44,48,15,0); 								// 圆圈？
					
					POINT_COLOR = WHITE;
					BACK_COLOR = GREEN;
					LCD_ShowIcon(880,112,112,128,14,0); 							// 右箭头
					LCD_ShowYanStr(916,204,50,24,22,"Next",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,272,112,128,20,0); 							// 左箭头
					LCD_ShowYanStr(916,364,50,24,22,"Back",1);
					
					POINT_COLOR = BLACK;
					BACK_COLOR = WHITE;
					LCD_ShowIcon(880,432,112,128,18,0); 							// 铜钱，大圆小方
					LCD_ShowYanStr(912,524,60,24,22,"Abort",1);
					
					//image_display(500,250,(u8*)gImage_image1); 						// 在指定地址显示图片
					
					pns1_flag = 1;
					vts1_flag = 1;
					cns1_flag = 1;
					bns1_flag = 1;
					infors1_flag = 1;
				}
				
				if(dtFlash == 0) 								// 日期时间的刷新
				{
					dtFlash = 250;
					
					POINT_COLOR = WHITE;
					BACK_COLOR = BLACK;
					HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
					LCD_ShowYanStr(127,35,210,24,22,tbuf,0);
					
					HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
					sprintf((char*)tbuf,"%02d.%02d.20%02d",RTC_DateStruct.Date,RTC_DateStruct.Month,RTC_DateStruct.Year); 
					LCD_ShowYanStr(127,56,250,24,22,tbuf,0);	
				}
				
				if(ks_flag) 										// 键盘显示刷新
				{
					ks_flag = 0;
					
					KeyPad(KeyState);
					
					if(KeyState != 0)
						keybc_flag = 1; 							// 刷新键盘输入框
				}
				
				if(pns1_flag) 									// 产品名称更新
				{
					pns1_flag = 0;
					
					LCD_Fill(56,208,56+256,208+48,GRAY); 							// Name显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(72,219,650,24,22,"Name",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,219,650,24,28,ProductNameSta1,1);
					
				}
				
				if(vts1_flag) 									// 体积更新
				{
					vts1_flag = 0;
					
					LCD_Fill(57,304,57+256,304+48,GRAY); 							// ml显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(283,315,650,24,22,"ml",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(180,315,650,24,28,VolumeTypeSta1,1);
				}
				
				if(cns1_flag) 									// 柱面更新
				{
					cns1_flag = 0;
					
					LCD_Fill(57,400,57+256,400+48,GRAY); 							// 显示框
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(150,410,650,24,28,CylinderNumberSta1,1);
				}
				
				if(bns1_flag) 									// 批号更新
				{
					bns1_flag = 0;
					
					LCD_Fill(416,208,416+256,208+48,GRAY); 						// Batch Number显示框
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,220,650,24,28,BatchNumberSta1,1);
				}
				
				if(infors1_flag) 								// 信息更新
				{
					infors1_flag = 0;
					
					LCD_Fill(416,304,416+256,304+224,GRAY); 					// General Information...显示框
					POINT_COLOR = GRAY1;
					LCD_ShowYanStr(432,310,650,24,22,"General Information...",1);
					POINT_COLOR = WHITE;
					LCD_ShowYanStr(432,335,220,200,28,InformationSta1,1);
				}
				
				if(keybc_flag)
				{
					keybc_flag = 0;
					
					LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
					POINT_COLOR = BLACK;
					LCD_ShowYanStr(140,90,460,150,28,KeyInStr,1);
				}
			}break;
			case 22: 													// USP/EP Test - Step 3 - Ready
			{
				
			}break;
			case 23: 													// USP/EP Test - Step 3 - Running
			{
				
			}break;
			case 40: 													// Help info, Service
			{
				
			}break;
			case 41: 													// Contact
			{
				
			}break;
			case 42: 													// Menu RPM
			{
				
			}break;
			case 60: 													// Manage menu, Qualification
			{
				
			}break;
			case 61: 													// Control Panel
			{
				
			}break;
			default:
			break;
		}
		*/
	}
}

void KeyPadDisplay(u8 kps)
{
	if(kps == 1) 													// 数字键盘
	{
		LCD_Clear(BLACK); 												// 黑色刷屏
		LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
		LCD_Fill(642,88,863,168,GREEN); 					// 绿色确认键
		LCD_ShowYanStr(705,110,100,40,40,"Enter",1);
		
		image_display(300,228,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(390,228,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(480,228,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(570,228,(u8*)gImage_BackSpace123); 		// 在指定地址显示图片
		LCD_ShowYanChar(325,235,'7',40,1);
		LCD_ShowYanChar(415,235,'8',40,1);
		LCD_ShowYanChar(505,235,'9',40,1);
		
		image_display(300,313,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(390,313,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(480,313,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(570,313,(u8*)gImage_Space123); 				// 在指定地址显示图片
		LCD_ShowYanChar(325,320,'4',40,1);
		LCD_ShowYanChar(415,320,'5',40,1);
		LCD_ShowYanChar(505,320,'6',40,1);
		
		image_display(300,400,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(390,400,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(480,400,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(570,400,(u8*)gImage_KeyP123); 				// 在指定地址显示图片
		LCD_ShowYanChar(325,407,'1',40,1);
		LCD_ShowYanChar(415,407,'2',40,1);
		LCD_ShowYanChar(505,407,'3',40,1);
		
		image_display(300,484,(u8*)gImage_Toabc); 					// 在指定地址显示图片
		image_display(390,484,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(480,484,(u8*)gImage_Empty123); 				// 在指定地址显示图片
		image_display(570,484,(u8*)gImage_Enter123); 				// 在指定地址显示图片
		LCD_ShowYanChar(415,491,'0',40,1);
		LCD_ShowYanChar(505,491,'.',40,1);
	}
	else
	if(kps == 2) 													// 小写字母键盘
	{
		LCD_Clear(BLACK); 												// 黑色刷屏
		LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
		LCD_Fill(642,88,863,168,GREEN); 					// 绿色确认键
		LCD_ShowYanStr(705,110,100,40,40,"Enter",1);
		
		image_display(60,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(144,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(226,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(310,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(393,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(476,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(558,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(641,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(724,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(807,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(890,265,(u8*)gImage_BackSpace); 		// 在指定地址显示图片
		LCD_ShowYanChar(90,275,'q',40,1);
		LCD_ShowYanChar(174,275,'w',40,1);
		LCD_ShowYanChar(256,275,'e',40,1);
		LCD_ShowYanChar(340,275,'r',40,1);
		LCD_ShowYanChar(423,275,'t',40,1);
		LCD_ShowYanChar(506,275,'y',40,1);
		LCD_ShowYanChar(588,275,'u',40,1);
		LCD_ShowYanChar(671,275,'i',40,1);
		LCD_ShowYanChar(754,275,'o',40,1);
		LCD_ShowYanChar(837,275,'p',40,1);
		
		image_display(88,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(171,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(254,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(337,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(420,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(503,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(587,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(669,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(752,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(840,338,(u8*)gImage_Enter); 				// 在指定地址显示图片
		LCD_ShowYanChar(118,348,'a',40,1);
		LCD_ShowYanChar(201,348,'s',40,1);
		LCD_ShowYanChar(284,348,'d',40,1);
		LCD_ShowYanChar(367,348,'f',40,1);
		LCD_ShowYanChar(450,348,'g',40,1);
		LCD_ShowYanChar(533,348,'h',40,1);
		LCD_ShowYanChar(617,348,'j',40,1);
		LCD_ShowYanChar(699,348,'k',40,1);
		LCD_ShowYanChar(782,348,'l',40,1);
		
		image_display(60,414,(u8*)gImage_LShift); 				// 在指定地址显示图片
		image_display(130,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(213,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(296,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(379,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(462,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(545,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(628,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(711,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(794,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(882,414,(u8*)gImage_RShift); 				// 在指定地址显示图片
		LCD_ShowYanChar(160,424,'z',40,1);
		LCD_ShowYanChar(243,424,'x',40,1);
		LCD_ShowYanChar(326,424,'c',40,1);
		LCD_ShowYanChar(409,424,'v',40,1);
		LCD_ShowYanChar(492,424,'b',40,1);
		LCD_ShowYanChar(575,424,'n',40,1);
		LCD_ShowYanChar(658,424,'m',40,1);
		LCD_ShowYanChar(741,424,',',40,1);
		LCD_ShowYanChar(824,424,'.',40,1);
		
		image_display(60,488,(u8*)gImage_To123); 					// 在指定地址显示图片
		image_display(189,488,(u8*)gImage_EarthK); 				// 在指定地址显示图片
		image_display(279,488,(u8*)gImage_Space); 				// 在指定地址显示图片
		image_display(666,488,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(756,488,(u8*)gImage_Smile); 				// 在指定地址显示图片
		image_display(844,488,(u8*)gImage_KeyP); 					// 在指定地址显示图片
		LCD_ShowYanChar(696,498,'\'',40,1);
		
	}
	else
	if(kps == 3) 													// 大写字母键盘
	{
		LCD_Clear(BLACK); 												// 黑色刷屏
		LCD_Fill(132,88,607,168,WHITE); 					// 白色数据输入框
		LCD_Fill(642,88,863,168,GREEN); 					// 绿色确认键
		LCD_ShowYanStr(705,110,100,40,40,"Enter",1);
		
		image_display(60,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(144,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(226,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(310,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(393,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(476,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(558,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(641,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(724,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(807,265,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(890,265,(u8*)gImage_BackSpace); 		// 在指定地址显示图片
		LCD_ShowYanChar(90,275,'Q',40,1);
		LCD_ShowYanChar(174,275,'W',40,1);
		LCD_ShowYanChar(256,275,'E',40,1);
		LCD_ShowYanChar(340,275,'R',40,1);
		LCD_ShowYanChar(423,275,'T',40,1);
		LCD_ShowYanChar(506,275,'Y',40,1);
		LCD_ShowYanChar(588,275,'U',40,1);
		LCD_ShowYanChar(671,275,'I',40,1);
		LCD_ShowYanChar(754,275,'O',40,1);
		LCD_ShowYanChar(837,275,'P',40,1);
		
		image_display(88,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(171,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(254,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(337,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(420,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(503,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(587,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(669,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(752,338,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(840,338,(u8*)gImage_Enter); 				// 在指定地址显示图片
		LCD_ShowYanChar(118,348,'A',40,1);
		LCD_ShowYanChar(201,348,'S',40,1);
		LCD_ShowYanChar(284,348,'D',40,1);
		LCD_ShowYanChar(367,348,'F',40,1);
		LCD_ShowYanChar(450,348,'G',40,1);
		LCD_ShowYanChar(533,348,'H',40,1);
		LCD_ShowYanChar(617,348,'J',40,1);
		LCD_ShowYanChar(699,348,'K',40,1);
		LCD_ShowYanChar(782,348,'L',40,1);
		
		image_display(60,414,(u8*)gImage_LShift); 				// 在指定地址显示图片
		image_display(130,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(213,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(296,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(379,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(462,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(545,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(628,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(711,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(794,414,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(882,414,(u8*)gImage_RShift); 				// 在指定地址显示图片
		LCD_ShowYanChar(160,424,'Z',40,1);
		LCD_ShowYanChar(243,424,'X',40,1);
		LCD_ShowYanChar(326,424,'C',40,1);
		LCD_ShowYanChar(409,424,'V',40,1);
		LCD_ShowYanChar(492,424,'B',40,1);
		LCD_ShowYanChar(575,424,'N',40,1);
		LCD_ShowYanChar(658,424,'M',40,1);
		LCD_ShowYanChar(741,424,',',40,1);
		LCD_ShowYanChar(824,424,'.',40,1);
		
		image_display(60,488,(u8*)gImage_To123); 					// 在指定地址显示图片
		image_display(189,488,(u8*)gImage_EarthK); 				// 在指定地址显示图片
		image_display(279,488,(u8*)gImage_Space); 				// 在指定地址显示图片
		image_display(666,488,(u8*)gImage_Empty); 				// 在指定地址显示图片
		image_display(756,488,(u8*)gImage_Smile); 				// 在指定地址显示图片
		image_display(844,488,(u8*)gImage_KeyP); 					// 在指定地址显示图片
		LCD_ShowYanChar(696,498,'\'',40,1);
	}
	else
	{
		;
	}
}

void LCD_ClearSE(u32 color)
{
	LCD_Clear(color);
	
	POINT_COLOR = WHITE;
	LCD_ShowYanStr(384,30,192,48,40,"SVM+",1);
	LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
}


