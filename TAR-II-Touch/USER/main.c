#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "tftlcd.h"
#include "timer.h"
#include "sdram.h"
#include "malloc.h"
#include "touch.h"
#include "rtc.h"
#include "GUI.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "ftl.h" 
#include "exfuns.h"
#include "w25qxx.h"
#include "fontupd.h"
#include "WM.h"
#include "can.h"
#include "image2lcd.h"
#include "string.h"
//#include "ttffontcreate.h"
#include "DIALOG.h"
#include "24cxx.h"
#include "version.h"

/************************************************
 SVM-LCM
 QIHUAN
 GAVIN
************************************************/

#define RAM_KEY_ADDR        (0x20002000)
#define BOOTLOADER_KEY1     (0x12345678)
#define BOOTLOADER_KEY2     (0x43211234)

// 触摸坐标 ------------------------------------------------
u16 old_tpx; 														
u16 old_tpy;
u16 cur_tpx;
u16 cur_tpy;

u8 CdStep = 0; 													// CAN接收数据处理步骤

// 变量定义 -------------------------------------------------
u16 rxd_x1,rxd_x2,rxd_w;
u16 rxd_y1,rxd_y2,rxd_h;
u16 rxd_color1,rxd_color2;  						// 前景色，背景色
u8 rxd_mode,rxd_size,rxd_nblen,rxd_icon;
u8 rxd_imgnb;
u16 rxd_u16dat;
u8 rxd_str[128];

u8 KeyState = 0; 												// 显示键盘状态，0不显示键盘，1数字键盘，2小写字母键盘，3大写键盘
u8 CmdNb; 															// 接收指令代码；
u8 PmtLen;  														// 接收参数长度；
u8 PmtWip; 															// 接收参数指针；

u8 powon_flag = 0; 											// 初始上电标志，发送硬件版本号和软件版本号

u8 DeviceMode = 0; 											// 设备模式，10：SVM-II；11：TAR-II；

//extern GUI_CONST_STORAGE GUI_BITMAP bm0DTBasicHomeScreenpng;
//extern GUI_CONST_STORAGE GUI_BITMAP bmErweka;

#define RGB888_RED      0x00ff0000
#define RGB888_GREEN    0x0000ff00
#define RGB888_BLUE     0x000000ff

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

u32 RGB888ToRGB565(u32 n888Color)
{
	u32 n565Color = 0;

	// 获取RGB单色，并截取高位
	unsigned char cRed   = (n888Color & RGB888_RED)   >> 19;
	unsigned char cGreen = (n888Color & RGB888_GREEN) >> 10;
	unsigned char cBlue  = (n888Color & RGB888_BLUE)  >> 3;

	// 连接
	n565Color = (cRed << 11) + (cGreen << 5) + (cBlue << 0);
	return n565Color;
}

u32 RGB565ToRGB888(u16 n565Color)
{
	u32 n888Color = 0;
	
	if(n565Color == 0xffff)
		n888Color = 0xffffffff;
	else
	{
		// 获取RGB单色，并填充低位
		unsigned char cBlue   = (n565Color & RGB565_RED)   >> 8;
		unsigned char cGreen = (n565Color & RGB565_GREEN)  >> 3;
		unsigned char cRed  = (n565Color & RGB565_BLUE)    << 3;

		// 连接
		n888Color = (cRed << 16) + (cGreen << 8) + (cBlue << 0);
	}
	
	return n888Color;
}

// 函数声明 -------------------------------------------------
void KeyPadDisplay(u8 kps);
void LCD_ClearSE(u32 color);
void StartBootloader(void);

// 声明字体 ------------------------------------------------
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoMedium20;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoMedium22;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoMedium24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoRegular27;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoBold28;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRobotoBold34;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueBold48;

//extern GUI_CONST_STORAGE GUI_BITMAP bmErweka;

extern GUI_CONST_STORAGE GUI_BITMAP bmKP_BackSpace;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_Clear;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_Empty;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_Enter;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_ShiftL;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_ShiftR;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_Space;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_To123;
extern GUI_CONST_STORAGE GUI_BITMAP bmKP_ToABC;

//extern GUI_CONST_STORAGE GUI_BITMAP bmLiangTong;
extern GUI_CONST_STORAGE GUI_BITMAP bmGGB_035;
extern GUI_CONST_STORAGE GUI_BITMAP bmSCB_039;

extern GUI_CONST_STORAGE GUI_BITMAP bmlogo_grey_235_48; 							// “ERWEKA”图标
//extern GUI_CONST_STORAGE GUI_BITMAP bmerwekalightpositiv5cm; 					// 开机画面
extern GUI_CONST_STORAGE GUI_BITMAP bmerwekapositiv5cm; 							// 开机画面

int main(void)
{
	u8 lcd_id[12];
	u8 i = 0;
	//char c1;
	u8 tbuf[50];
	GUI_RECT Rect;
	
	u8 cp_flag = 1;
	u8 u8Dat;
	
	// 串口1使用的变量 -------------------
	u8 len;
	char buf1[50];
	
	
	SCB->VTOR = FLASH_BASE | 0x10000; 		// 设置中断向量的偏移量
    
	HAL_Init();                     			// 初始化HAL库   
	
	Stm32_Clock_Init(360,25,2,8); 				// 设置时钟,180Mhz
	delay_init(180); 											// 初始化延时函数
	
	uart_init(115200); 										// 初始化USART
	LED_Init(); 													// 初始化LED 
	
	AT24CXX_Init(); 											// 初始化24C256，并读出硬件版本号
	AT24CXX_Read(HTVER_ADDR,(u8 *)TOUCH_HARDWARE_VERSION,HVER_MAX_LEN);
	if((TOUCH_HARDWARE_VERSION[0] < 0x30)||
		 (TOUCH_HARDWARE_VERSION[0] > 0x39))
	{
		strcpy(TOUCH_HARDWARE_VERSION,"1.10.0");
		AT24CXX_Write(HTVER_ADDR,(u8 *)TOUCH_HARDWARE_VERSION,HVER_MAX_LEN);
	}
	
	SDRAM_Init(); 												// 初始化SDRAM
	TFTLCD_Init(); 												// 初始化LCD
	
	TIM3_Init(999,89); 										// 1KHZ，定时器3设置为1ms
	//TIM4_Init(999,899); 									// 触摸屏扫描速度,100HZ.
	
	my_mem_init(SRAMIN); 									// 初始化内部内存池
	my_mem_init(SRAMEX); 									// 初始化外部内存池
	my_mem_init(SRAMCCM); 								// 初始化CCM内存池
	
	exfuns_init(); 												// 为fatfs相关变量申请内存				 
	f_mount(fs[0],"0:",1); 								// 挂载SD卡 
	f_mount(fs[1],"1:",1); 								// 挂载SPI FLASH
	f_mount(fs[2],"2:",1); 								// 挂载NAND FLASH.
	
	/*
	while(font_init())		//初始化字库
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!",0);
		while(SD_Init())	//检测SD卡
		{
			LCD_ShowString(30,90,200,16,16,"SD Card Failed!",0);
			delay_ms(200);
			LCD_Fill(30,90,200+30,70+16,WHITE);
			delay_ms(200);
		}
		
		update_font(30,90,16,"0:");	//如果字库不存在就更新字库
		delay_ms(2000);
		LCD_Clear(WHITE);	//清屏
		break;
	}
	*/
	
	__HAL_RCC_CRC_CLK_ENABLE(); 					// 使能CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV); 			// 开启STemWin存储设备
	
	GUI_Init();
	WM_MULTIBUF_Enable(1); 								// 开启STemWin多缓冲，RGB屏可能会用到
	
	RTC_Init(); 													// 初始化RTC
	RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); 				// 配置WAKE UP中断,1秒钟中断一次  
	
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id); // 将LCD ID打印到lcd_id数组。	
	
	tp_dev.init();				    						//触摸屏初始化 
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_8TQ,6,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	
	//定时器5初始化，定时器时钟为90M，分频系数为9000-1，
	//所以定时器5的频率为90M/9000=10K，自动重装载为5000-1，那么定时器周期就是500ms
	//TIM5_Init(5000-1,9000-1);
	//所以定时器5的频率为90M/9000=10K，自动重装载为10-1，那么定时器周期就是1ms
	TIM5_Init(20-1,9000-1);
	BeepTime = 25;
	
	POINT_COLOR = WHITE;
	BACK_COLOR = BLACK;
	//LCD_ShowIcon(250,250,468,88,53,0); 		// 开机画面
	//GUI_DrawBitmap(&bmerwekalightpositiv5cm, 250, 250); 			// 开机画面
	//GUI_DrawBitmap(&bmerwekapositiv5cm, 305, 250); 						// 开机画面		240124-1600
	GUI_DrawBitmapEx(&bmerwekapositiv5cm, 305, 250, 0, 0, 1000, 1000);		//240124-1600 HHT 暂时解决了LOGO丢失像素的问题
	
	POINT_COLOR = WHITE;
	BACK_COLOR = BLACK;
	sprintf((char*)tbuf,"Touch Hardware Version: %s",TOUCH_HARDWARE_VERSION); 		// 将硬件版本号打印到tbuf数组。
	LCD_ShowString(0,575,500,24,24,tbuf,0); 																			// 显示硬件版本号
	
	while(1)
	{
		// 触摸控制 ------------------------
		if((tp_dev.scan(0) != 0)&& 					// 有触摸信号
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
					cur_tpx = old_tpx;
					cur_tpy = old_tpy;
					
					//cur_tpx = ((u32)old_tpx*95)/100;
					//cur_tpy = ((u32)old_tpy*95)/100;
					
					//BeepTime = 25; 							// 触摸按键音
					
					//POINT_COLOR = WHITE;
					//BACK_COLOR = BLACK;
					//sprintf((char*)tbuf,"x,y: %04d,%04d; ",cur_tpx,cur_tpy); 		// 将触摸坐标打印到tbuf数组。	
					//LCD_ShowString(30,0,200,24,24,tbuf,0); 										// 提示触摸坐标数值
					
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
				}
				
				tp_ktime = 0;
				tp_onf = 0;
			}
		}
		
		// can接收数据 -------------------------------
		while(CBRIp0 != CBRIp1) 						// 接收到新数据
		{
			if(CanRx_TimeOut == 0)
				CdStep = 0;
			
			CanRx_TimeOut = 100; 							// mS
			
			switch(CdStep) 										// 数据处理进程
			{
				case 0: 												// 帧头+地址+长度+验证
				{
					if(CanRxBuf[CBRIp0][0] == 0xA5) 														// 校验帧头
					{
						if((CanRxBuf[CBRIp0][1] == (uint8_t)(12345>>8))&&
							 (CanRxBuf[CBRIp0][2] == (uint8_t)(12345&0x00ff))) 			// 校验密码
						{
							//CanRxSn = CanRxBuf[CBRIp0][3]; 		// 接收信息序列号
							
							if(CanRxSn == CanRxBuf[CBRIp0][3])
							{
								//CanRxSn++;
								
								CmdNb = CanRxBuf[CBRIp0][4]; 			// 接收指令代码
								PmtLen = CanRxBuf[CBRIp0][6]; 		// 接收参数长度
								
								if((CmdNb != 0)&&(PmtLen != 0))
								{
									PmtWip = 0; 											// 参数指针清零
									CdStep++; 												// 帧解析状态机++
								}
								else 															// 8字节以内指令
								{
									// 心跳指令，不操作 --------------------
									CanRxSn++;
									;
								}
							}
							else 																// 指令序列号错
							{
								// CAN TX ----------------------------------------
								CanTxBuf[0] = 0xA5;
								CanTxBuf[1] = (u8)(12345>>8);
								CanTxBuf[2] = (u8)(12345&0x00ff);
								CanTxBuf[3] = 0x0A;
								
								CanTxBuf[4] = CanRxSn;
								CanTxBuf[5] = 0;
								CanTxBuf[6] = 0;
								CanTxBuf[7] = 0;
								
								CAN1_Send_Msg(CanTxBuf,8);
							}
						}
						else 																// UpDataFlash
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
						CanRxSn++;
						
						switch(CmdNb) 							// 指令代码
						{
							case 10: 									// 清屏
							{
								rxd_color1 = rxd_str[0];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[1];
								
								LCD_Clear(rxd_color1);
							}break;
							case 11: 									// 清屏+SVM+ERWEKA
							{
								rxd_color1 = rxd_str[0];
								rxd_color1 <<= 8;
								rxd_color1 += rxd_str[1];
								
								DeviceMode = rxd_str[2];
								
								LCD_ClearSE(RGB565ToRGB888(rxd_color1));
								CdStep = 0; 												// 帧解析状态机复位
								
								//rxd_color1 = 0; //0xffffffff;
								//rxd_x1 = 300;
								//rxd_y1 = 0;
								//rxd_x2 = 1024;
								//rxd_y2 = 80;
								//GUI_SetColor(RGB565ToRGB888(rxd_color1));
								//GUI_FillRect(rxd_x1,rxd_y1,rxd_x2,rxd_y2);
							}break;
							case 20: 									// 单色矩形填充
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
								
								//LCD_Fill(rxd_x1,rxd_y1,rxd_x2,rxd_y2,rxd_color1);
								GUI_SetColor(RGB565ToRGB888(rxd_color1));
								GUI_FillRect(rxd_x1,rxd_y1,rxd_x2,rxd_y2);
							}break;
							case 30: 									// 图标显示
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
								
								//POINT_COLOR = rxd_color1;
								//BACK_COLOR = rxd_color2;
								
								//LCD_ShowSpStr(rxd_x1,rxd_y1,rxd_w,rxd_h,rxd_size,rxd_str+14,rxd_mode); 	// 字符串显示
								
								GUI_SetColor(RGB565ToRGB888(rxd_color1));
								GUI_SetBkColor(RGB565ToRGB888(rxd_color2));
								
								Rect.x0 = rxd_x1;
								Rect.y0 = rxd_y1;
								Rect.x1 = rxd_x1+rxd_w;
								Rect.y1 = rxd_y1+rxd_h;
								
								// 文本绘制模式 ----------------------------
								// 正常文本 GUI_TEXTMODE_NORMAL
								// 反转文本 GUI_TEXTMODE_REV
								// 透明文本 GUI_TEXTMODE_TRANS
								// 异或文本 GUI_TEXTMODE_XOR
								u8Dat = rxd_mode&0x0f;
								//u8Dat = 0;
								switch(u8Dat)
								{
									case 0: GUI_SetTextMode(GUI_TEXTMODE_NORMAL); break;
									case 1: GUI_SetTextMode(GUI_TEXTMODE_REV); break;
									case 2: GUI_SetTextMode(GUI_TEXTMODE_TRANS); break;
									case 3: GUI_SetTextMode(GUI_TEXTMODE_XOR); break;
									default: break;
								}
								
								// 文本对齐模式 ----------------------------
								// 水平和垂直标记可以通过 “ OR”操作进行组合
								// GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
								// 水平对齐
								// GUI_TA_LEFT X 轴方向左对齐 （默认）。
								// GUI_TA_HCENTER X 轴方向居中。
								// GUI_TA_RIGHT X 轴方向右对齐。
								// 垂直对齐
								// GUI_TA_TOP 在字符 Y 轴方向顶部对齐 （默认）。
								// GUI_TA_VCENTER Y 轴方向居中。
								// GUI_TA_BOTTOM 在字体 Y 轴底部像素线对齐。
								u8Dat = rxd_mode>>4;
								switch(u8Dat)
								{
									case 0: GUI_SetTextAlign(GUI_TA_LEFT); break;
									case 1: GUI_SetTextAlign(GUI_TA_HCENTER); break;
									case 2: GUI_SetTextAlign(GUI_TA_RIGHT); break;
									case 3: GUI_SetTextAlign(GUI_TA_TOP); break;
									case 4: GUI_SetTextAlign(GUI_TA_VCENTER); break;
									case 5: GUI_SetTextAlign(GUI_TA_BOTTOM); break;
									default: break;
								}
								
								switch(rxd_size)
								{
									case 20: GUI_SetFont(&GUI_FontRobotoMedium20); break;
									case 22: GUI_SetFont(&GUI_FontRobotoMedium22); break;
									case 24: GUI_SetFont(&GUI_FontRobotoMedium24); break;
									case 27: GUI_SetFont(&GUI_FontRobotoRegular27); break;
									case 28: GUI_SetFont(&GUI_FontRobotoBold28); break;
									case 34: GUI_SetFont(&GUI_FontRobotoBold34); break;
									case 48: GUI_SetFont(&GUI_FontHelveticaNeueBold48); break;
									default: GUI_SetFont(&GUI_Font24B_1); break;
								}
								
								GUI_DispStringInRectWrap((const char *)(rxd_str+14),&Rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
								//GUI_DispStringAtCEOL((const char *)(rxd_str+14), rxd_x1, rxd_y1);
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
								
								//POINT_COLOR = rxd_color1;
								//BACK_COLOR = rxd_color2;
								
								GUI_SetColor(RGB565ToRGB888(rxd_color2));
								GUI_FillRect(rxd_x1,rxd_y1,rxd_x1+(rxd_size*rxd_nblen)/2,rxd_y1+rxd_size);
								
								GUI_SetColor(RGB565ToRGB888(rxd_color1));
								GUI_SetBkColor(RGB565ToRGB888(rxd_color2));
								
								GUI_SetTextAlign(GUI_TA_LEFT);
								
								//GUI_SetTextMode(GUI_TM_TRANS);
								GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
								switch(rxd_size)
								{
									case 20: GUI_SetFont(&GUI_FontRobotoMedium20); break;
									case 22: GUI_SetFont(&GUI_FontRobotoMedium22); break;
									case 24: GUI_SetFont(&GUI_FontRobotoMedium24); break;
									case 27: GUI_SetFont(&GUI_FontRobotoRegular27); break;
									case 28: GUI_SetFont(&GUI_FontRobotoBold28); break;
									case 34: GUI_SetFont(&GUI_FontRobotoBold34); break;
									case 48: GUI_SetFont(&GUI_FontHelveticaNeueBold48); break;
									default: GUI_SetFont(&GUI_Font24B_1); break;
								}
								
								//LCD_ShowSpNum(rxd_x1,rxd_y1,rxd_u16dat,rxd_nblen,rxd_size); 	// 数字显示，不显示前置0
								GUI_GotoXY(rxd_x1,rxd_y1);
								GUI_DispDecSpace(rxd_u16dat,rxd_nblen);
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
								
								//POINT_COLOR = rxd_color1;
								//BACK_COLOR = rxd_color2;
								
								GUI_SetColor(RGB565ToRGB888(rxd_color1));
								GUI_SetBkColor(RGB565ToRGB888(rxd_color2));
								
								//GUI_SetTextMode(GUI_TM_TRANS);
								GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
								switch(rxd_size)
								{
									case 20: GUI_SetFont(&GUI_FontRobotoMedium20); break;
									case 22: GUI_SetFont(&GUI_FontRobotoMedium22); break;
									case 24: GUI_SetFont(&GUI_FontRobotoMedium24); break;
									case 27: GUI_SetFont(&GUI_FontRobotoRegular27); break;
									case 28: GUI_SetFont(&GUI_FontRobotoBold28); break;
									case 34: GUI_SetFont(&GUI_FontRobotoBold34); break;
									case 48: GUI_SetFont(&GUI_FontHelveticaNeueBold48); break;
									default: GUI_SetFont(&GUI_Font24B_1); break;
								}
								
								//LCD_ShowxSpNum(rxd_x1,rxd_y1,rxd_u16dat,rxd_nblen,rxd_size,rxd_mode); 	// 数字显示，显示前置0
								GUI_DispDecAt(rxd_u16dat,rxd_x1,rxd_y1,rxd_nblen);
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
								
								//image_disbynb(rxd_x1,rxd_y1,rxd_imgnb);
								switch(rxd_imgnb)
								{
									//case 0: GUI_DrawBitmap(&bmLiangTong, rxd_x1, rxd_y1); break;
									case 1: GUI_DrawBitmap(&bmGGB_035, rxd_x1, rxd_y1); break;
									case 2: GUI_DrawBitmap(&bmSCB_039, rxd_x1, rxd_y1); break;
									default: ; break;
								} 
							}break;
							case 70: 									// 键盘显示
							{
								KeyState = rxd_str[0];
								
								KeyPadDisplay(KeyState);
							}break;
							case 250: 								// 查询硬件版本号
							{
								// CAN TX 硬件版本号----------------------------------------
								CanTxBuf[0] = 0xA5;
								CanTxBuf[1] = (u8)(12345>>8);
								CanTxBuf[2] = (u8)(12345&0x00ff);
								CanTxBuf[3] = 0xB0;
								
								CanTxBuf[4] = TOUCH_HARDWARE_VERSION[0];
								CanTxBuf[5] = TOUCH_HARDWARE_VERSION[2];
								CanTxBuf[6] = TOUCH_HARDWARE_VERSION[3];
								CanTxBuf[7] = TOUCH_HARDWARE_VERSION[5];
								
								CAN1_Send_Msg(CanTxBuf,8);
							}break;
							case 251: 								// 查询软件版本号
							{
								// CAN TX 软件版本号----------------------------------------
								CanTxBuf[0] = 0xA5;
								CanTxBuf[1] = (u8)(12345>>8);
								CanTxBuf[2] = (u8)(12345&0x00ff);
								CanTxBuf[3] = 0xB1;
								
								CanTxBuf[4] = TOUCH_FIRMWARE_VERSION[0];
								CanTxBuf[5] = TOUCH_FIRMWARE_VERSION[1];
								CanTxBuf[6] = TOUCH_FIRMWARE_VERSION[3];
								CanTxBuf[7] = TOUCH_FIRMWARE_VERSION[4];
								
								CAN1_Send_Msg(CanTxBuf,8);
							}break;
							case 255: 								// 软件升级
							{
								if(strncmp((char *)rxd_str,(char *)"IBFW",4) == 0)
									StartBootloader();
							}break;
							default:
							{
								CdStep = 0;
							}break;
						}
						
						CdStep = 0; 								// 帧解析状态机复位
						
						if(powon_flag == 0) 				// 初始上电标志，发送触摸板的硬件版本号和软件版本号
						{
							powon_flag = 1;
							
							// CAN TX 硬件版本号----------------------------------------
							CanTxBuf[0] = 0xA5;
							CanTxBuf[1] = (u8)(12345>>8);
							CanTxBuf[2] = (u8)(12345&0x00ff);
							CanTxBuf[3] = 0xB0;
							
							CanTxBuf[4] = TOUCH_HARDWARE_VERSION[0];
							CanTxBuf[5] = TOUCH_HARDWARE_VERSION[2];
							CanTxBuf[6] = TOUCH_HARDWARE_VERSION[3];
							CanTxBuf[7] = TOUCH_HARDWARE_VERSION[5];
							
							CAN1_Send_Msg(CanTxBuf,8);
							
							// CAN TX 软件版本号----------------------------------------
							CanTxBuf[0] = 0xA5;
							CanTxBuf[1] = (u8)(12345>>8);
							CanTxBuf[2] = (u8)(12345&0x00ff);
							CanTxBuf[3] = 0xB1;
							
							CanTxBuf[4] = TOUCH_FIRMWARE_VERSION[0];
							CanTxBuf[5] = TOUCH_FIRMWARE_VERSION[1];
							CanTxBuf[6] = TOUCH_FIRMWARE_VERSION[3];
							CanTxBuf[7] = TOUCH_FIRMWARE_VERSION[4];
							
							CAN1_Send_Msg(CanTxBuf,8);
						}
					}
				}break;
				default:
				{
					CdStep = 0;
				}break;
			}
			
			/*for(i=0;i<8;i++) 								// 测试，上传can接收的数据，上传过程会影响can接收
			{
				c1 = CanRxBuf[CBRIp0][i];
				printf("CAN[%x][%x]:%x,%d,%c;\r\n",(u16)CBRIp0,(u16)i,(u16)c1,(u16)c1,c1);
			}*/
			
			CBRIp0++;
			if(CBRIp0 > 63)
				CBRIp0 = 0;
		}
		
		if((CdStep != 0)&& 									// 正在接收CAN数据中，还未完成
			 (CanRx_TimeOut == 0)) 						// 接收超时
		{
			CdStep = 0; 										// 复位状态步数
			
			// CAN TX ----------------------------------------
			CanTxBuf[0] = 0xA5;
			CanTxBuf[1] = (u8)(12345>>8);
			CanTxBuf[2] = (u8)(12345&0x00ff);
			CanTxBuf[3] = 0x0A;
			
			CanTxBuf[4] = CanRxSn;
			CanTxBuf[5] = 0;
			CanTxBuf[6] = 0;
			CanTxBuf[7] = 0;
			
			CAN1_Send_Msg(CanTxBuf,8);
		}
		
		// 串口接收数据 ------------------------------
		if(USART_RX_STA&0x8000)
		{
			//len = USART_RX_STA&0x3fff; 				// 获取接收到的数据长度
			//printf("\r\n您发送的消息为：\r\n");
			//HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000); 				// 发送接收到的数据
			//while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET); 						// 等待发送结束
			//printf("\r\n\r\n"); 							// 插入换行
			//USART_RX_STA = 0;
			
			// 处理接收到的指令 ----------------------------------
			USART_RX_BUF[USART_RX_STA&0x3FFF] = '\0';
			if(strcmp(USART_RX_BUF,"GHTVER")==0) 				// 获取触摸板硬件版本号
			{
				//AT24CXX_Read(HTVER_ADDR,(u8 *)buf1,VER_MAX_LEN);
				//buf1[VER_MAX_LEN]=0;
				
				//sprintf(buf, "HTVER: %s\r", buf1);
				printf("GHTVER: ");
				printf(TOUCH_HARDWARE_VERSION);
				printf("\r\n\r\n"); 											// 插入换行
			}
			else
			if(strncmp(USART_RX_BUF,"SHTVER ",7)==0) 		// 设置触摸板硬件版本号
			{
				//strncpy(buf1,&USART_RX_BUF[7], VER_MAX_LEN);
				strcpy(buf1,&USART_RX_BUF[7]);
				
				len = strlen(buf1);
				if(len>HVER_MAX_LEN)
					len = HVER_MAX_LEN;
				buf1[len] = '\0';
				
				AT24CXX_Write(HTVER_ADDR,(u8 *)buf1,len);
				strcpy(TOUCH_HARDWARE_VERSION,buf1);
				
				printf("SHTVER: ");
				printf(buf1);
				printf("\r\n\r\n"); 											// 插入换行
			}
			
			USART_RX_STA = 0;
		}
		
		///*
		i = 0;
		switch(i)
		{
			case 0: 													// 首页
			{
				if(cp_flag) 										// 页面刷新
				{
					cp_flag = 0;
					/*
					//LCD_Clear(BLACK);
					GUI_Clear();
					
					GUI_SetBkColor(GUI_BLACK);
					
					GUI_SetColor(GUI_WHITE);
					GUI_FillRect(10,20,60,30);
					GUI_FillRect(10,40,60,50);
					GUI_FillRect(10,60,60,70);
					
					GUI_SetColor(GUI_GRAY);
					GUI_FillRect(32,112,832,160);
					GUI_SetColor(GUI_WHITE);
					GUI_FillRect(32,160,832,488);
					
					//LCD_Fill(32,112,32+800,112+376,WHITE);
					//LCD_Fill(32,112,32+800,112+48,GRAY);
					
					GUI_SetColor(GUI_WHITE);
					GUI_SetBkColor(GUI_BLACK);
					
					Rect.x0 = 384;
					Rect.y0 = 30;
					Rect.x1 = 384+192;
					Rect.y1 = 30+48;
					
					GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
					GUI_DispStringInRectWrap("SVM+",&Rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
					
					POINT_COLOR = WHITE;
					LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
					
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_WHITE);
					*/
					
					//GUI_DrawBitmap(&bmErweka,770,30);
					//GUI_DrawBitmapEx(&bmErweka,770,30,100,24,600,600);
					
					/*
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
					*/
					
					//image_display(395,161,(u8*)gImage_LiangTong);
					//GUI_DrawBitmap(&bm0DTBasicHomeScreenpng,395,161);
				}
				
				/*
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
				}*/
			}break;
			/*case 1: 													// 测试方法选择页面
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
				
			}break;*/
			default:
			break;
		}
	}
}

#define KEYPAD_FONT 27
const GUI_POINT aPoint_back[] = 
{
	{  0,   0},
	{  9,  -9},
	{ 27,  -9},
	{ 27,   9},
	{  9,   9},
};

const GUI_POINT aPoint_Shift[] = 
{
	{  0,   0},
	{ 10,  10},
	{  5,  10},
	{  5,  20},
	{ -5,  20},
	{ -5,  10},
	{-10,  10}
};

void PutKey(u16 x0,u16 y0,u16 sw,u32 color,const char * ch)
{
	GUI_SetColor(color);
	GUI_FillRoundedRect(x0,y0,x0+sw,y0+78,3);
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(x0+3, y0+76, x0+sw-3, y0+77);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetBkColor(color);
	
	if((x0 == 920)&&(y0 == 250))
	{
		GUI_SetPenSize(2);
		x0 += 24; y0 += 40;
		GUI_DrawPolygon(aPoint_back, 5, x0, y0);
		x0 += 12; y0 -= 4;
		GUI_DrawLine(x0, y0, x0+9, y0+9);
		GUI_DrawLine(x0+9, y0, x0, y0+9);
	}
	else
	if((x0 == 20)&&(y0 == 420))
	{
		GUI_SetPenSize(2);
		x0 += 38; y0 += 30;
		GUI_DrawPolygon(aPoint_Shift, 7, x0, y0);
	}
	else
	if((x0 == 911)&&(y0 == 420))
	{
		GUI_SetPenSize(2);
		x0 += 38; y0 += 30;
		GUI_DrawPolygon(aPoint_Shift, 7, x0, y0);
	}
	else
	{
		//GUI_SetFont(&GUI_FontRobotoMedium24);
		GUI_SetFont(&GUI_FontRobotoBold34);
		GUI_DispStringAt(ch, x0+sw/2, y0+39);
	}
}

void KeyPadDisplay(u8 kps)
{
	GUI_SetColor(0xff686868);
	GUI_FillRect(154,92,868,166);
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(160,98,862,160);
	//LCD_Fill(154,92,868,166,GRAY2);
	//LCD_Fill(160,98,862,160,WHITE); 								// 白色数据输入框
	//POINT_COLOR = GRAY1;
	//LCD_ShowSpStr(174,118,200,28,28,"Input here...",1);
	
	GUI_SetColor(0xff656565);
	GUI_FillRect(0,224,1023,599);
	//LCD_Fill(0,224,1023,599,GRAY2); 								// 绘制键盘背景
	
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER); 	// 字符水平居中/垂直居中
	if(kps == 1) 													// 数字键盘
	{
		PutKey( 20,250,78,0xff8e8e8e,"1");
		PutKey(110,250,78,0xff8e8e8e,"2");
		PutKey(200,250,78,0xff8e8e8e,"3");
		PutKey(290,250,78,0xff8e8e8e,"4");
		PutKey(380,250,78,0xff8e8e8e,"5");
		PutKey(470,250,78,0xff8e8e8e,"6");
		PutKey(560,250,78,0xff8e8e8e,"7");
		PutKey(650,250,78,0xff8e8e8e,"8");
		PutKey(740,250,78,0xff8e8e8e,"9");
		PutKey(830,250,78,0xff8e8e8e,"0");
		PutKey(920,250,78,0xffa8a8a8,"back");
		
		PutKey( 50,335,78,0xff8e8e8e,"'");
		PutKey(140,335,78,0xff8e8e8e,"|");
		PutKey(230,335,78,0xff8e8e8e,"{");
		PutKey(320,335,78,0xff8e8e8e,"}");
		PutKey(410,335,78,0xff8e8e8e,"%");
		PutKey(500,335,78,0xff8e8e8e,"^");
		PutKey(590,335,78,0xff8e8e8e,"*");
		PutKey(680,335,78,0xff8e8e8e,"/");
		PutKey(770,335,78,0xff8e8e8e,"\"");
		PutKey(860,335,140,0xffcfa52e,"Enter");
		
		PutKey(288,420,78,0xff8e8e8e,"$");
		PutKey(377,420,78,0xff8e8e8e,"&");
		PutKey(466,420,78,0xff8e8e8e,"~");
		PutKey(555,420,78,0xff8e8e8e,"#");
		PutKey(644,420,78,0xff8e8e8e,"=");
		PutKey(733,420,78,0xff8e8e8e,"+");
		PutKey(822,420,78,0xff8e8e8e,".");
		PutKey(912,420,78,0xff8e8e8e,",");
		
		PutKey( 20,505,78,0xffa8a8a8,"ABcd");
		PutKey(110,505,168,0xff8e8e8e,"clear");
		PutKey(288,505,446,0xff8e8e8e," ");
		PutKey(744,505,78,0xff8e8e8e,"_");
		PutKey(833,505,78,0xff8e8e8e,"-");
	}
	else 																	// 字母键盘
	{
		POINT_COLOR = WHITE;
		
		if(kps == 2) 												// 小写字母键盘
		{
			PutKey( 20,250,78,0xff8e8e8e,"q");
			PutKey(110,250,78,0xff8e8e8e,"w");
			PutKey(200,250,78,0xff8e8e8e,"e");
			PutKey(290,250,78,0xff8e8e8e,"r");
			PutKey(380,250,78,0xff8e8e8e,"t");
			PutKey(470,250,78,0xff8e8e8e,"y");
			PutKey(560,250,78,0xff8e8e8e,"u");
			PutKey(650,250,78,0xff8e8e8e,"i");
			PutKey(740,250,78,0xff8e8e8e,"o");
			PutKey(830,250,78,0xff8e8e8e,"p");
			PutKey(920,250,78,0xffa8a8a8,"back");
			
			PutKey( 50,335,78,0xff8e8e8e,"a");
			PutKey(140,335,78,0xff8e8e8e,"s");
			PutKey(230,335,78,0xff8e8e8e,"d");
			PutKey(320,335,78,0xff8e8e8e,"f");
			PutKey(410,335,78,0xff8e8e8e,"g");
			PutKey(500,335,78,0xff8e8e8e,"h");
			PutKey(590,335,78,0xff8e8e8e,"j");
			PutKey(680,335,78,0xff8e8e8e,"k");
			PutKey(770,335,78,0xff8e8e8e,"l");
			PutKey(860,335,140,0xffcfa52e,"Enter");
			
			PutKey( 20,420,78,0xffa8a8a8,"Shift");
			PutKey(110,420,78,0xff8e8e8e,"z");
			PutKey(200,420,78,0xff8e8e8e,"x");
			PutKey(288,420,78,0xff8e8e8e,"c");
			PutKey(377,420,78,0xff8e8e8e,"v");
			PutKey(466,420,78,0xff8e8e8e,"b");
			PutKey(555,420,78,0xff8e8e8e,"n");
			PutKey(644,420,78,0xff8e8e8e,"m");
			PutKey(733,420,78,0xff8e8e8e,"@");
			PutKey(822,420,78,0xff8e8e8e,".");
			PutKey(911,420,78,0xffa8a8a8,"Shift");
			
		}
		else
		if(kps == 3) 													// 大写字母键盘
		{
			PutKey( 20,250,78,0xff8e8e8e,"Q");
			PutKey(110,250,78,0xff8e8e8e,"W");
			PutKey(200,250,78,0xff8e8e8e,"E");
			PutKey(290,250,78,0xff8e8e8e,"R");
			PutKey(380,250,78,0xff8e8e8e,"T");
			PutKey(470,250,78,0xff8e8e8e,"Y");
			PutKey(560,250,78,0xff8e8e8e,"U");
			PutKey(650,250,78,0xff8e8e8e,"I");
			PutKey(740,250,78,0xff8e8e8e,"O");
			PutKey(830,250,78,0xff8e8e8e,"P");
			PutKey(920,250,78,0xffa8a8a8,"back");
			
			PutKey( 50,335,78,0xff8e8e8e,"A");
			PutKey(140,335,78,0xff8e8e8e,"S");
			PutKey(230,335,78,0xff8e8e8e,"D");
			PutKey(320,335,78,0xff8e8e8e,"F");
			PutKey(410,335,78,0xff8e8e8e,"G");
			PutKey(500,335,78,0xff8e8e8e,"H");
			PutKey(590,335,78,0xff8e8e8e,"J");
			PutKey(680,335,78,0xff8e8e8e,"K");
			PutKey(770,335,78,0xff8e8e8e,"L");
			PutKey(860,335,140,0xffcfa52e,"Enter");
			
			PutKey( 20,420,78,0xffa8a8a8,"Shift");
			PutKey(110,420,78,0xff8e8e8e,"Z");
			PutKey(200,420,78,0xff8e8e8e,"X");
			PutKey(288,420,78,0xff8e8e8e,"C");
			PutKey(377,420,78,0xff8e8e8e,"V");
			PutKey(466,420,78,0xff8e8e8e,"B");
			PutKey(555,420,78,0xff8e8e8e,"N");
			PutKey(644,420,78,0xff8e8e8e,"M");
			PutKey(733,420,78,0xff8e8e8e,"@");
			PutKey(822,420,78,0xff8e8e8e,".");
			PutKey(911,420,78,0xffa8a8a8,"Shift");
			
		}
		
		PutKey( 20,505,78,0xffa8a8a8,".?123");
		PutKey(110,505,168,0xff8e8e8e,"clear");
		PutKey(288,505,446,0xff8e8e8e," ");
		PutKey(744,505,78,0xff8e8e8e,"_");
		PutKey(833,505,78,0xff8e8e8e,"-");
	}
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP); 				// 字符水平左对齐/垂直顶部对齐
}

void LCD_ClearSE(u32 color)
{
	GUI_RECT Rect;
	
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(color);
	
	//LCD_Clear(color);
	GUI_Clear();
	
	POINT_COLOR = WHITE;
	//LCD_ShowSpStr(384,30,192,48,40,"SVM+",1);
	
	//GUI_SetColor(GUI_BLACK);
	//GUI_SetBkColor(GUI_WHITE);
	
	Rect.x0 = 384;
	Rect.y0 = 30;
	Rect.x1 = 384+192;
	Rect.y1 = 30+55;
	
	//GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
	//GUI_SetFont(&GUI_Font32B_1);
	GUI_SetFont(&GUI_FontHelveticaNeueBold48);
	switch(DeviceMode)
	{
		case 10:
			GUI_DispStringInRectWrap("SVM II",&Rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
			break;
		case 20:
			GUI_DispStringInRectWrap("TAR II",&Rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
			break;
		default:
			GUI_DispStringInRectWrap("SVM II",&Rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
			break;
	}
	
	//LCD_ShowIcon(770,30,235,48,10,1); 			// ERWEKA
	
	//GUI_DrawBitmap(&bmErweka, 770, 30);
	//GUI_BMP_Draw(&bmErweka, 770, 30);
	//GUI_DrawBitmap(&bmlogo_grey_235_48, 770, 30); 			// ERWEKA		240124-1400
	//GUI_DrawBitmap(&bmlogo_grey_235_48, 770, 30); 			// ERWEKA
	GUI_DrawBitmapEx(&bmlogo_grey_235_48, 770, 30, 0, 0, 1000, 1000);		//240124-1400 HHT 暂时解决了LOGO丢失像素的问题
}

/*******************************************************************************
* Function Name  : StartBootloader
* Description    : Initialize the FW update, reboot to bootloader 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StartBootloader(void)
{
	unsigned int * ptr;
		
	ptr =(unsigned int *)RAM_KEY_ADDR;
	*ptr = BOOTLOADER_KEY1;
	ptr++;
	*ptr = BOOTLOADER_KEY2;
	
	__disable_irq();

//	RCC_LSICmd(ENABLE);
//	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//	IWDG_SetPrescaler(IWDG_Prescaler_32);
//	IWDG_SetReload(1);
//	IWDG_ReloadCounter();
//	IWDG_Enable();
	
	NVIC_SystemReset();
	while(1);
}




