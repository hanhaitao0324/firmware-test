#include "tftlcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	 
#include "ltdc.h"

SRAM_HandleTypeDef SRAM_Handler;    //SRAM句柄(用于控制LCD)

// LCD的画笔颜色和背景色	   
u32 POINT_COLOR = 0xFF000000;		//画笔颜色
u32 BACK_COLOR = 0xFFFFFFFF;  	//背景色 
  
// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;
	 
// 写寄存器函数
// regval:寄存器值
void LCD_WR_REG(vu16 regval)
{   
	regval=regval;		//使用-O2优化的时候,必须插入的延时
	LCD->LCD_REG=regval;//写入要写的寄存器序号	 
}

// 写LCD数据
// data:要写入的值
void LCD_WR_DATA(vu16 data)
{	  
	data=data;			//使用-O2优化的时候,必须插入的延时
	LCD->LCD_RAM=data;		 
}

// 读LCD数据
// 返回值:读到的值
u16 LCD_RD_DATA(void)
{
	vu16 ram;			//防止被优化
	ram=LCD->LCD_RAM;	
	return ram;	 
}

// 写寄存器
// LCD_Reg:寄存器地址
// LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}

// 读寄存器
// LCD_Reg:寄存器地址
// 返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);		  
	return LCD_RD_DATA();		//返回读到的值
}

// 开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}

// LCD写GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}

// 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
// 通过该函数转换
// c:GBR格式的颜色值
// 返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}

// 当mdk -O1时间优化时需要设置
// 延时i
void opt_delay(u8 i)
{
	while(i--);
}

// 读取个某点的颜色值	 
// x,y:坐标
// 返回值:此点的颜色
u32 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回
	if(lcdltdc.pwidth!=0)							//如果是RGB屏
	{
		return LTDC_Read_Point(x,y);
	}
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X2E);//9341/3510/1963 发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
 	r=LCD_RD_DATA();								//dummy Read	   
	if(lcddev.id==0X1963)return r;					//1963直接读就可以 
	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						//实际坐标颜色
	//9341/NT35310/NT35510要分2次读出 
	opt_delay(2);	  
	b=LCD_RD_DATA(); 
	g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
	g<<=8; 
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));	//ILI9341/NT35310/NT35510需要公式转换一下
}

// LCD开启显示
void LCD_DisplayOn(void)
{					   
	if(lcdltdc.pwidth!=0)LTDC_Switch(1);//开启LCD
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X29);	//开启显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);	//开启显示
}

// LCD关闭显示
void LCD_DisplayOff(void)
{	   
	if(lcdltdc.pwidth!=0)LTDC_Switch(0);//关闭LCD
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X28);	//关闭显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);	//关闭显示
}

// 设置光标位置(对RGB屏无效)
// Xpos:横坐标
// Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
	}else if(lcddev.id==0X1963)
	{  			 		
		if(lcddev.dir==0)//x坐标需要变换
		{
			Xpos=lcddev.width-1-Xpos;
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0); 		
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF);		 	 
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 		
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);		 	 			
		}	
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF); 			 		
		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(Xpos>>8); 		
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(Xpos&0XFF);			 
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(Ypos>>8);  		
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(Ypos&0XFF);			
	} 
} 		 
// 设置LCD的自动扫描方向(对RGB屏无效)
// 注意:其他函数可能会受到此函数设置的影响(尤其是9341),
// 所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
// dir:0~7,代表8个方向(具体定义见lcd.h)
// 9341/5310/5510/1963等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	if((lcddev.dir==1&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//横屏时，对1963不改变扫描方向！竖屏时1963改变方向
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	if(lcddev.id==0x9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//9341/5310/5510/1963,特殊处理
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))regval|=0X08;//5310/5510/1963不需要BGR   
 		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963不做坐标处理
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)//交换X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  	} 
}

// 画点
// x,y:坐标
// POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	if(lcdltdc.pwidth!=0)//如果是RGB屏
	{
		LTDC_Draw_Point(x,y,POINT_COLOR);
	}
	else
	{
		LCD_SetCursor(x,y);		//设置光标位置 
		LCD_WriteRAM_Prepare();	//开始写入GRAM
		LCD->LCD_RAM=POINT_COLOR; 
	}
}

// 快速画点
// x,y:坐标
// color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u32 color)
{	   
	if(lcdltdc.pwidth!=0)//如果是RGB屏
	{
		LTDC_Draw_Point(x,y,color);
		return;
	}else if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);  			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		 	 
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);	  
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF); 
	}else if(lcddev.id==0X1963)
	{
		if(lcddev.dir==0)x=lcddev.width-1-x;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
	}		 
	LCD->LCD_REG=lcddev.wramcmd; 
	LCD->LCD_RAM=color; 
}

// SSD1963 背光设置
// pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(u8 pwm)
{	
	LCD_WR_REG(0xBE);	//配置PWM输出
	LCD_WR_DATA(0x05);	//1设置PWM频率
	LCD_WR_DATA(pwm*2.55);//2设置PWM占空比
	LCD_WR_DATA(0x01);	//3设置C
	LCD_WR_DATA(0xFF);	//4设置D
	LCD_WR_DATA(0x00);	//5设置E
	LCD_WR_DATA(0x00);	//6设置F
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	lcddev.dir=dir; 											// 横屏/竖屏
	if(lcdltdc.pwidth!=0) 								// 如果是RGB屏
	{
		LTDC_Display_Dir(dir);
		lcddev.width=lcdltdc.width;
		lcddev.height=lcdltdc.height;
		return;
	}
	
	if(dir==0)			//竖屏
	{
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}else if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2B;	//设置写X坐标指令
			lcddev.setycmd=0X2A;	//设置写Y坐标指令
			lcddev.width=480;		//设置宽度480
			lcddev.height=800;		//设置高度800  
		}
	}
	else 				//横屏
	{	  				
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}else if(lcddev.id==0X1963)
		{
			lcddev.wramcmd=0X2C;	//设置写入GRAM的指令 
			lcddev.setxcmd=0X2A;	//设置写X坐标指令
			lcddev.setycmd=0X2B;	//设置写Y坐标指令
			lcddev.width=800;		//设置宽度800
			lcddev.height=480;		//设置高度480  
		}
		if(lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	}
	
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}

// 设置窗口(对RGB屏无效),并自动设置画点坐标到窗口左上角(sx,sy).
// sx,sy:窗口起始坐标(左上角)
// width,height:窗口宽度和高度,必须大于0!!
// 窗体大小:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{     
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||(lcddev.dir==1&&lcddev.id==0X1963))
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 
	}else if(lcddev.id==0X1963)//1963竖屏特殊处理
	{
		sx=lcddev.width-width-sx; 
		height=sy+height-1; 
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA((sx+width-1)>>8); 
		LCD_WR_DATA((sx+width-1)&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(height>>8); 
		LCD_WR_DATA(height&0XFF); 		
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(twidth>>8);   
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(twidth&0XFF);   
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(theight>>8);   
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(theight&0XFF);  
	} 
}

//SRAM底层驱动，时钟使能，引脚分配
//此函数会被HAL_SRAM_Init()调用
//hsram:SRAM句柄
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_FMC_CLK_ENABLE();				//使能FMC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//使能GPIOD时钟
	__HAL_RCC_GPIOE_CLK_ENABLE();			//使能GPIOE时钟
	
	//初始化PD0,1,4,5,7,8,9,10,13,14,15
	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8|\
                     GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_Initure.Mode=GPIO_MODE_AF_PP; 		//复用
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF12_FMC;	//复用为FMC
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	//初始化PE7,8,9,10,11,12,13,14,15
	GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}
        
// 初始化lcd
// 该初始化函数可以初始化各种型号的LCD(详见本.c文件最前面的描述)
void TFTLCD_Init(void)
{
	//GPIO_InitTypeDef GPIO_Initure;
	//FMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
	//FMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
	
	lcddev.id=LTDC_PanelID_Read(); 				// 检查是否有RGB屏接入
	if(lcddev.id!=0)
	{
		LTDC_Init(); 												// ID非零,说明有RGB屏接入.
	}
	
	LCD_Display_Dir(1); 									// 0为竖屏，1为横屏；默认为横屏
	//LCD_UD = 1;
	//LCD_LR = 0;
	LCD_UD = 0;
	LCD_LR = 1;
	
	LCD_LED = 1; 													// 点亮背光
	
	//LCD_Clear(GRAY);
	
	//POINT_COLOR = BLACK;
	//BACK_COLOR = GRAY;
	//LCD_ShowIcon(250,250,468,88,53,0);
	
	LCD_Clear(BLACK);
	
	//POINT_COLOR = GRAY;
	//POINT_COLOR = WHITE;
	//BACK_COLOR = BLACK;
	//LCD_ShowIcon(250,250,468,88,53,0);
	
}

// 清屏函数
// color:要清屏的填充色
void LCD_Clear(u32 color)
{
	u32 index = 0;
	u32 totalpoint = lcddev.width;
	
	if(lcdltdc.pwidth != 0) 							// 如果是RGB屏
	{
		LTDC_Clear(color);
	}
	else
	{
		totalpoint*=lcddev.height; 					// 得到总点数
		LCD_SetCursor(0x00,0x0000); 				// 设置光标位置
		LCD_WriteRAM_Prepare(); 						// 开始写入GRAM
		
		for(index=0;index<totalpoint;index++)
		{
			LCD->LCD_RAM=color;	
		}
	}
}

// 在指定区域内填充单个颜色
// (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
// color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{          
	u16 i,j;
	u16 xlen = 0;
	
	if(lcdltdc.pwidth!=0) 								// 如果是RGB屏
	{
		LTDC_Fill(sx,sy,ex,ey,color);
	}
	else
	{	
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
			LCD_SetCursor(sx,i); 							// 设置光标位置 
			LCD_WriteRAM_Prepare();     			// 开始写入GRAM	  
			for(j=0;j<xlen;j++)LCD->LCD_RAM=color; 			// 显示颜色 	    
		} 
	}
}

// 在指定区域内填充指定颜色块			 
// (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
// color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	
	if(lcdltdc.pwidth!=0) 								// 如果是RGB屏
	{
		LTDC_Color_Fill(sx,sy,ex,ey,color);
	}
	else
	{
		width=ex-sx+1; 											// 得到填充的宽度
		height=ey-sy+1; 										// 高度
		
		for(i=0;i<height;i++)
		{
			LCD_SetCursor(sx,sy+i); 					// 设置光标位置 
			LCD_WriteRAM_Prepare(); 					// 开始写入GRAM
			for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j]; 		// 写入数据 
		}
	}	
}

// 画线
// x1,y1:起点坐标
// x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol;
	
	delta_x=x2-x1; 												// 计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1;
	
	if(delta_x>0)
		incx=1; 														// 设置单步方向 
	else
	if(delta_x==0)
		incx=0; 														// 垂直线 
	else
	{
		incx=-1;
		delta_x=-delta_x;
	}
	
	if(delta_y>0)
		incy=1; 
	else
	if(delta_y==0)
		incy=0; 														// 水平线 
	else
	{
		incy=-1;
		delta_y=-delta_y;
	}
	
	if( delta_x>delta_y)
		distance=delta_x; 									// 选取基本增量坐标轴 
	else
		distance=delta_y;
	
	for(t=0;t<=distance+1;t++ ) 					// 画线输出 
	{  
		LCD_DrawPoint(uRow,uCol); 					// 画点 
		xerr += delta_x; 
		yerr += delta_y;
		
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		}
		
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

// 画矩形	  
// (x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

// 在指定位置画一个指定大小的圆
// (x,y):中心点
// r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	
	a=0;b=r;	  
	di=3-(r<<1); 													// 判断下个点位置的标志
	
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b); 					// 5
 		LCD_DrawPoint(x0+b,y0-a); 					// 0
		LCD_DrawPoint(x0+b,y0+a); 					// 4
		LCD_DrawPoint(x0+a,y0+b); 					// 6
		LCD_DrawPoint(x0-a,y0+b); 					// 1
 		LCD_DrawPoint(x0-b,y0+a);
		LCD_DrawPoint(x0-a,y0-b); 					// 2
		LCD_DrawPoint(x0-b,y0-a); 					// 7
		a++;
		
		// 使用Bresenham算法画圆     
		if(di<0)
			di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

// 在指定位置显示一个字符
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24/32
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
	//u8 temp,t1,t;
	u8 temp;
	u16 t1,t;
	u16 y0=y;
	//u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	u16 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{
		switch(size)
		{
			case 12: temp=asc2_1206[num][t]; break; 	 	//调用1206字体
			case 16: temp=asc2_1608[num][t]; break; 		//调用1608字体
			case 22: temp=asc2_22[num][t]; break;
			case 24: temp=asc2_2412[num][t]; break; 		//调用2412字体
			case 32: temp=asc2_3216[num][t]; break; 		//调用3216字体
			//case 40: temp=asc2Black_4020[num][t]; break;
			//case 48: temp=asc2_4020[num][t]; break; 		//调用4020字体
			//case 80: temp=asc2Helvetica_4020[num][t]; break; 		//调用4020字体
			default: return;  										//没有的字库
		}
		
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}

// m^n函数
// 返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

// 显示数字,高位为0,则不显示
// x,y :起点坐标	 
// len :数字的位数
// size:字体大小
// color:颜色 
// num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
}

// 显示数字,高位为0,还是显示
// x,y:起点坐标
// num:数值(0~999999999);	 
// len:长度(即要显示的位数)
// size:字体大小
// mode:
// [7]:0,不填充;1,填充0.
// [6:1]:保留
// [0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
}

// 显示数字,高位为0,则不显示
// x,y :起点坐标	 
// len :数字的位数
// size:字体大小
// color:颜色 
// num:数值(0~4294967295);	 
/*
void LCD_ShowSpNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 t,temp;
	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp = (num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(size > 100)
					LCD_ShowSpChar(x+(size/10/2)*t,y,' ',size,0);
				else
					LCD_ShowSpChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}
			else
				enshow=1;
		}
		
		if(size > 100)
			LCD_ShowSpChar(x+(size/10/2)*t,y,temp+'0',size,0);
		else
			LCD_ShowSpChar(x+(size/2)*t,y,temp+'0',size,0);
	}
}
*/

// 显示数字,高位为0,还是显示
// x,y:起点坐标
// num:数值(0~999999999);	 
// len:长度(即要显示的位数)
// size:字体大小
// mode:
// [7]:0,不填充;1,填充0.
// [6:1]:保留
// [0]:0,非叠加显示;1,叠加显示.
/*
void LCD_ShowxSpNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{
	u8 t,temp;
	u8 enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp = (num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)
				{
					if(size > 100)
						LCD_ShowSpChar(x+(size/10/2)*t,y,'0',size,mode&0X01);  
					else
						LCD_ShowSpChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				}
				else
				{
					if(size > 100)
						LCD_ShowSpChar(x+(size/10/2)*t,y,' ',size,mode&0X01);  
					else
						LCD_ShowSpChar(x+(size/2)*t,y,' ',size,mode&0X01);  
				}
				
				continue;
			}
			else
				enshow = 1;
		}
		
		if(size > 100)
			LCD_ShowSpChar(x+(size/10/2)*t,y,temp+'0',size,mode&0X01); 
		else
			LCD_ShowSpChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
}
*/

// 显示字符串
// x,y:起点坐标
// width,height:区域大小  
// size:字体大小
// *p:字符串起始地址		  
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p,u8 mode)
{         
	u8 x0=x;
	
	width+=x;
	height+=y;
	while((*p<='~')&&(*p>=' ')) 					//判断是不是非法字符!
	{       
		if(x>=width){x=x0;y+=size;}
		if(y>=height)break;//退出
		//LCD_ShowChar(x,y,*p,size,0);
		LCD_ShowChar(x,y,*p,size,mode);
		x+=size/2;
		p++;
	}
}

// 在指定位置显示一个字符，专用字体
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24/32
// mode:叠加方式(1)还是非叠加方式(0)
/*
void LCD_ShowSpChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
	//u8 temp,t1,t;
	u8 temp;
	u16 t1,t;
	u16 y0=y;
	//u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	u16 csize=(size/8+((size%8)?1:0))*(size/4);		//得到字体一个字符对应点阵集所占的字节数	
	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
 	switch(size)
 	{
 		case 181: csize = 48; 	break; 			// Roboto medium, 18px，ASCII字符集点阵
		case 200: csize = 48; 	break; 			// Roboto Regular, 20px，ASCII字符集点阵
		case 201: csize = 48; 	break; 			// Roboto medium, 20px，ASCII字符集点阵
		case 202: csize = 48; 	break; 			// Roboto Bold, 20px，ASCII字符集点阵
		case 220: csize = 54; 	break; 			// Roboto Regular, 22px，ASCII字符集点阵
		case 221: csize = 54; 	break; 			// Roboto medium, 22px，ASCII字符集点阵
		case 231: csize = 54; 	break; 			// Roboto medium, 23px，ASCII字符集点阵
		case 242: csize = 57; 	break; 			// Roboto Bold, 24px，ASCII字符集点阵
		case 26: 	csize = 88; 	break; 			// Roboto Regular, 26px，ASCII字符集点阵
		case 27: 	csize = 88; 	break; 			// Roboto Regular, 28px，ASCII字符集点阵
		case 28:  csize = 88; 	break; 			// Roboto Bold, 28px，ASCII字符集点阵
		case 44:  csize = 216; 	break; 			// Roboto medium, 44px，ASCII字符集点阵
		case 48:  csize = 240; 	break; 			// Roboto medium, 48px，ASCII字符集点阵
		case 40:  csize = 180; 	break; 			// Helvetica Neue, Bold, 40px，ASCII字符集点阵
		default: break;
 	}
 	
	for(t=0;t<csize;t++)
	{
		switch(size)
		{
			case 181: temp = asc2_18Roboto_medium16x24[num][t]; 			break; // Roboto medium, 18px，ASCII字符集点阵
			case 200: temp = asc2_20Roboto_Regular16x24[num][t]; 			break; // Roboto Regular, 20px，ASCII字符集点阵
			case 201: temp = asc2_20Roboto_medium16x24[num][t]; 			break; // Roboto medium, 20px，ASCII字符集点阵
			case 202: temp = asc2_20Roboto_Bold16x24[num][t]; 				break; // Roboto Bold, 20px，ASCII字符集点阵
			case 220: temp = asc2_22Roboto_Regular18x24[num][t]; 			break; // Roboto Regular, 22px，ASCII字符集点阵
			case 221: temp = asc2_22Roboto_medium18x24[num][t]; 			break; // Roboto medium, 22px，ASCII字符集点阵
			case 231: temp = asc2_23Roboto_medium18x24[num][t]; 			break; // Roboto medium, 23px，ASCII字符集点阵
			case 242: temp = asc2_24Roboto_Bold19x24[num][t]; 				break; // Roboto Bold, 24px，ASCII字符集点阵
			case 26: 	temp = asc2_26Roboto_Regular22x32[num][t]; 			break; // Roboto Regular, 26px，ASCII字符集点阵
			case 27: 	temp = asc2_28Roboto_Regular22x32[num][t]; 			break; // Roboto Regular, 28px，ASCII字符集点阵
			case 28:  temp = asc2_28Roboto_Bold22x32[num][t]; 				break; // Roboto Bold, 28px，ASCII字符集点阵
			case 44:  temp = asc2_44Roboto_medium36x48[num][t]; 			break; // Roboto medium, 44px，ASCII字符集点阵
			case 48:  temp = asc2_48Roboto_medium40x48[num][t]; 			break; // Roboto medium, 48px，ASCII字符集点阵
			case 40:  temp = asc2_40HelveticaNeue_Bold_36x40[num][t]; break; // Helvetica Neue, Bold, 40px，ASCII字符集点阵
			default: break;
		}
		
		
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else
			if(mode==0)
				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			
			temp<<=1;
			y++;
			if(y>=lcddev.height)
				return; 												// 超区域了
			
			switch(size)
			{
				case 181:
				case 200:
				case 201:
				case 202:
				case 220:
				case 221:
				case 231:
				case 242:
				{
					if((y-y0)==24)
					{
						y=y0;
						x++;
						if(x>=lcddev.width)
							return;										// 超区域了
					}
				}break;
				case 26:
				case 27:
				case 28:
				{
					if((y-y0)==32)
					{
						y=y0;
						x++;
						if(x>=lcddev.width)
							return;										// 超区域了
					}
				}break;
				case 44:
				case 48:
				{
					if((y-y0)==48)
					{
						y=y0;
						x++;
						if(x>=lcddev.width)
							return;										// 超区域了
					}
				}break;
				case 40:
				{
					if((y-y0)==40)
					{
						y=y0;
						x++;
						if(x>=lcddev.width)
							return;										// 超区域了
					}
				}break;
				default: break;
			}
		}  	 
	}  	    	   	 	  
}
*/

// 显示特殊字体字符串
// x,y:起点坐标
// width,height:区域大小  
// size:字体大小
// *p:字符串起始地址		  
// mode:叠加方式(1)还是非叠加方式(0)
/*
void LCD_ShowSpStr(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p,u8 mode)
{         
	u16 x0=x;
	
	width+=x;
	height+=y;
	while((*p<='~')&&(*p>=' ')) 					// 判断是不是非法字符!
	{       
		if(x>=width)
		{
			x=x0;
			if(size >100)
				y+= size/10;
			else
				y+=size;
		}
		
		if(y>=height)
			break; 														// 退出
		
		LCD_ShowSpChar(x,y,*p,size,mode);
		switch(size)
		{
			case 181:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=5; break;
					case 'm': x+=16; break;
					case 'w': x+=15; break;
					case '@':
					case 'M':
					case 'W': x+=18; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=9; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=11; 
						else
							x+=10;
					}break;
				}
			}break;
			case 200:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=6; break;
					case 'm': x+=17; break;
					case 'w': x+=15; break;
					case '@':
					case 'M':
					case 'W': x+=19; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=10; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=12; 
						else
							x+=11;
					}break;
				}
			}break;
			case 201:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 202:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 220:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 221:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 231:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 242:
			{
				switch(*p)
				{
					case ':': 
					case '.':
					case ' ': 
					case 'l':
					case 'i':
					case 'f': 
					case 'r':
					case 't':
					case 'I': x+=7; break;
					case 'm': x+=18; break;
					case 'w': x+=16; break;
					case '@':
					case 'M':
					case 'W': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=11; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=13; 
						else
							x+=12;
					}break;
				}
			}break;
			case 26:
			case 27:
			case 28:
			{
				switch(*p)
				{
					case ' ': x+=6; break;
					case 'l':
					case 'i':
					case 'I':
					case 'j':
					case 'f':
					case 'r': x+=8; break;
					case 't': x+=10; break;
					case 'm':
					case 'w':
					case 'M':
					case 'W':
					case '@': x+=20; break;
					default: 
					{
						if((*p >= 'a')&&(*p <= 'z')) x+=15; 
						else
						if((*p >= 'A')&&(*p <= 'Z')) x+=17; 
						else
							x+=14;
					}break;
				}
			}break;
			case 40:
			{
				switch(*p)
				{
					case '@':
					case 'W':
					case 'M': x+=32; break;
					default: x+=24; break;
				}
				
			}break;
			case 44:
			{
				switch(*p)
				{
					case '@':
					case 'W':
					case 'M': x+=36; break;
					default: x+=28; break;
				}
			}break;
			case 48:
			{
				switch(*p)
				{
					case '@':
					case 'W':
					case 'M': x+=40; break;
					default: x+=32; break;
				}
			}break;
			default: break;
		}
		
		p++;
	}
}
*/

// 在指定位置显示一个图标
// x,y:起始坐标
// width,height:区域大小  
// *p:图标起始地址		  
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowIcon(u16 x,u16 y,u16 width,u16 height,u8 p,u8 mode)
{
	u16 y0=y;
	
	u8 temp;
	u16 t1,t;
	u16 csize=(width*height)/8;		//得到字体一个字符对应点阵集所占的字节数	
	
 	//num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{
		switch(p)
		{
			case 9:  temp = ErwekaIcon64_48A[t]; 		break; 				// 三横
			case 10: temp = ErwekaIcon235_48A[t]; 	break; 				// ERWEKA
			case 11: temp = ErwekaIcon112_128A[t]; 	break; 				// Quick Test
			case 12: temp = ErwekaIcon112_128B[t]; 	break; 				// EPUSP Test
			case 13: temp = ErwekaIcon64_64A[t]; 		break; 				// 圆圈i
			case 14: temp = ErwekaIcon112_128C[t]; 	break; 				// 右箭头,Next
			case 15: temp = ErwekaIcon44_48A[t]; 		break; 				// 圆圈?,Help
			case 16: temp = ErwekaIcon136_136A[t]; 	break; 				// 循环双箭头大,Tap count
			case 17: temp = ErwekaIcon136_136B[t]; 	break; 				// 手表大,Time
			case 18: temp = ErwekaIcon112_128D[t]; 	break; 				// 铜钱，大圆小方,Abort
			case 19: temp = ErwekaIcon44_48B[t]; 		break; 				// 循环双箭头小
			case 20: temp = ErwekaIcon112_128E[t]; 	break; 				// 左箭头,Back
			case 21: temp = ErwekaIcon44_48C[t]; 		break; 				// 手表小，计时
			case 22: temp = ErwekaIcon112_128F[t]; 	break; 				// 大圆右三角，START
			case 23: temp = ErwekaIcon256_256A[t]; 	break; 				// 大图标量具
			case 24: temp = ErwekaIcon112_128J[t]; 	break; 				// 打印机图标, Test Page
			case 25: temp = ErwekaIcon112_128G[t]; 	break; 				// 方叉关闭图标，Close
			case 26: temp = ErwekaIcon80_80A[t]; 		break; 				// 对勾，选中
			case 27: temp = ErwekaIcon112_128H[t]; 	break; 				// Enter
			case 28: temp = ErwekaIcon112_128I[t]; 	break; 				// Save
			case 29: temp = ErwekaIcon24_24A[t]; 		break; 				// 小三角上拉
			case 30: temp = ErwekaIcon112_128K[t]; 	break; 				// Start
			
			case 31: temp = ErwekaIcon64_64[t]; 		break; 				// 小房子
			case 32: temp = ErwekaIcon48_48[t]; 		break; 				// 小X
			case 33: temp = ErwekaIcon32_24A[t]; 		break; 				// 半表针
			case 34: temp = ErwekaIcon16_32[t]; 		break; 				// 上下双箭头
			case 35: temp = ErwekaIcon32_32A[t]; 		break; 				// 逆时针
			case 36: temp = ErwekaIcon32_24B[t]; 		break; 				// 三角波
			case 37: temp = ErwekaIcon24_32[t]; 		break; 				// 小秒表
			case 38: temp = ErwekaIcon32_32B[t]; 		break; 				// 小天平
			case 39: temp = ErwekaIcon24_24B[t]; 		break; 				// 起子扳手
			case 40: temp = ErwekaIcon32_32C[t]; 		break; 				// 小表盘
			case 41: temp = ErwekaIcon32_32D[t]; 		break; 				// 小打印机
			case 42: temp = ErwekaIcon24_24C[t]; 		break; 				// 小喇叭
			case 43: temp = ErwekaIcon32_32E[t]; 		break; 				// 小苗
			case 44: temp = ErwekaIcon32_32F[t]; 		break; 				// 大表盘
			case 45: temp = ErwekaIcon32_16[t]; 		break; 				// 小三角下拉
			case 46: temp = ErwekaIcon24_24D[t]; 		break; 				// 小电话
			case 47: temp = ErwekaIcon32_32G[t]; 		break; 				// 上下循环双箭头
			case 48: temp = ErwekaIcon32_64[t]; 		break; 				// 小温度计
			case 49: temp = ErwekaIcon56_56[t]; 		break; 				// 大X
			case 50: temp = ErwekaIcon144_168[t]; 	break; 				// 大秒表
			case 51: temp = ErwekaIcon204_168[t]; 	break; 				// 大天平
			case 52: temp = ErwekaIcon152_184[t]; 	break; 				// 二维码
			case 53: temp = ErwekaIcon468_88[t]; 		break; 				// 开机画面1
			
			case 54: temp = ErwekaIcon112_128L[t]; 	break; 				// 上下双循环箭头，Start/Stop
			case 55: temp = ErwekaIcon112_128M[t]; 	break; 				// 温度计，Start
			
			case 56: temp = Erweka24Icon32X32[t]; 	break; 				// 小叉
			case 57: temp = Erweka25Icon64X64[t]; 	break; 				// 小上箭头
			case 58: temp = Erweka26Icon64X64[t]; 	break; 				// 小下箭头
			
			case 59: temp = Erweka27Icon24X24[t]; 	break; 				// 水平双箭头
			case 60: temp = ErwekaIcon32_32H[t]; 		break; 				// 'X'编辑标
			
			case 61: temp = Erweka28Icon24X24[t]; 	break; 				// 打印管理图标
			case 62: temp = Erweka51Icon112X128[t]; break; 				// 单页打印图标
			case 63: temp = Erweka52Icon112X128[t]; break; 				// 全部打印图标
			case 64: temp = Erweka29Icon24X24[t]; 	break; 				// 压缩率参考体积
			case 65: temp = Erweka30Icon24X24[t]; 	break; 				// OQ指示灯
			
			case 66: temp = Erweka31Icon48X48[t]; 	break; 				// 量筒运行
			case 67: temp = Erweka32Icon48X48[t]; 	break; 				// 量筒暂停
			case 68: temp = Erweka18bIcon64X64[t]; 	break; 				// 循环双箭头大号
				
			case 69: temp = Erweka69bIcon42X42[t];	break;				// 解锁						2021/10/29 星期五 9:00:22
			case 70: temp = Erweka70bIcon112X128[t];break;				// Export(大)
			case 71: temp = Erweka71Icon42X42[t];		break;				// Export(小)
			
			case 72: temp = Erweka39Icon112X128[t]; break; 				// 左侧上下双箭头，右侧三横，去皮图标Tare
			
			case 73: temp = Erweka01cIcon112_128N[t]; break; 			// 测试助手，TestAssist
			case 74: temp = Erweka03aIcon64_64B[t]; break; 				// 小圈i
			case 75: temp = ErwekaXunhuan64x64[t]; break; 				// 上下循环双圆弧箭头
			case 76: temp = ErwekaShizhong64x64[t]; break; 				// 时钟，4点
			case 77: temp = ErwekaM0Mf64x64[t]; break; 						// M0Mf
			case 78: temp = ErwekaDuigou48x48[t]; break; 					// 小对勾
			case 79: temp = ErwekaDrum64x64[t]; break; 						// 小鼓Drum，Move up
			case 80: temp = ErwekaBigDrum256x256[t]; break; 			// 大鼓Drum，运行绿色图标
			
			case 81: temp = ErwekaSmallDrum64x40[t]; break; 			// 小鼓Drum，
			case 82: temp = ErwekaMidDrum144x160[t]; break; 			// 中鼓Drum；
			
			case 83: temp = ErwekaLouDou80x104[t]; break; 				// 漏斗定时器
			case 84: temp = ErwekaJiaoNang88x96[t]; break; 				// 胶囊10/6.5g
			case 85: temp = ErwekaTianPing80x64[t]; break; 				// 中号天平
			
			case 86: temp = ErwekaJiaoNang48x40[t]; break; 				// 小胶囊
			case 87: temp = ErwekaMSDrum80x88[t]; break; 					// 中小鼓
			case 88: temp = ErwekaBrush80x88[t]; break; 					// 斜刷子
			
			case 89: temp = ErwekaSensor32x32[t]; break; 					// 位置传感器
			case 90: temp = ErwekaLouDou80x104A[t]; break; 				// 漏斗定时器，上下颠倒
			
			case 91: temp = ErwekaLAN48X48A[t]; break; 						// 网络断开
			case 92: temp = ErwekaLAN48X48B[t]; break; 						// 网络连接
			case 93: temp = ErwekaLAN48X48C[t]; break; 						// 网络断开，黑块
			case 94: temp = ErwekaLAN48X48D[t]; break; 						// 网络连接，客户提供的新图标
			case 95: temp = ErwekaUDisk48X48[t]; break; 					// U盘连接图标
			
			case 96: temp = ErwekaLAN32X32[t]; break; 						// 网络图标
			
			case 97: temp = SvmUsp3_56X72[t]; break; 							// SVM USP3图标
			
			default: break;
		}
		
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else 
			if(mode==0)
				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==height)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   





























