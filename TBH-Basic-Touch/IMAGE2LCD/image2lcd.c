#include "sys.h"
#include "tftlcd.h"
#include "image2lcd.h"
#include "ltdc.h"

//从8位数据获得16位颜色
//mode:0,低位在前,高位在后.
//     1,高位在前,低位在后.
//str:数据
u16 image_getcolor(u8 mode,u8 *str)
{
	u16 color;
	if(mode)
	{
		color=((u16)*str++)<<8;
		color|=*str;
	}else
	{
		color=*str++;
		color|=((u16)*str)<<8;
	}
	return color;	
}
//在液晶上画图（仅支持：从左到右，从上到下 or 从上到下，从左到右 的扫描方式！）	   
//xsta,ysta,width,height:画图区域
//scan:见image2lcd V2.9的说明.
//*p:图像数据
void image_show(u16 xsta,u16 ysta,u16 width,u16 height,u8 scan,u8 *p)
{  
    u32 i,j,k;
    u32 len=0;
    if(lcdltdc.pwidth!=0)//如果是RGB屏
    {
    for(j=0;j<height;j++)         //针对扫描方式为从左往右，从上往下的情况
     {
      for(k=0;k<width;k++)
      {
        LTDC_Draw_Point(xsta+k,ysta+j,image_getcolor(scan&(1<<4),p));
        p+=2;
      }
     }
		return;
    }
    if(lcddev.id!=0X1963)
    {
      if((scan&0x03)==0)//水平扫描
      {
        LCD_Scan_Dir(L2R_U2D);//从左到右,从上到下
        LCD_Set_Window(xsta,ysta,width,height);
        LCD_SetCursor(xsta,ysta);//设置光标位置 
				
      }else  //垂直扫描
      {  
        LCD_Scan_Dir(U2D_L2R);//从上到下,从左到右
        LCD_Set_Window(xsta,ysta,width,height);
        LCD_SetCursor(xsta,ysta);//设置光标位置  
      }
    }else
    {
			LCD_Scan_Dir(L2R_U2D);//从左到右,从上到下
			LCD_Set_Window(xsta,ysta,width,height);
    }
    LCD_WriteRAM_Prepare();   	//开始写入GRAM
    len=width*height;			//写入的数据长度
    for(i=0;i<len;i++)
    {
      LCD_WR_DATA(image_getcolor(scan&(1<<4),p)); 
      p+=2;
    } 	  
    LCD_Set_Window(0,0,lcddev.width,lcddev.height);  
}  

//在指定的位置显示一个图片
//此函数可以显示image2lcd软件生成的任意16位真彩色图片.
//限制:1,尺寸不能超过屏幕的区域.
//     2,生成数据时不能勾选:高位在前(MSB First)
//     3,必须包含图片信息头数据
//x,y:指定位置
//imgx:图片数据(必须包含图片信息头,"4096色/16位真彩色/18位真彩色/24位真彩色/32位真彩色”的图像数据头)
//注意:针对STM32,不能选择image2lcd的"高位在前(MSB First)"选项,否则imginfo的数据将不正确!!
void image_display(u16 x,u16 y,u8 * imgx)
{
	HEADCOLOR *imginfo;
 	u8 ifosize=sizeof(HEADCOLOR);//得到HEADCOLOR结构体的大小
	imginfo=(HEADCOLOR*)imgx;
 	image_show(x,y,imginfo->w,imginfo->h,imginfo->scan,imgx+ifosize);		
}

// 根据图片编号显示图片 -------------------------------------

void image_disbynb(u16 x,u16 y,u8 imgnb)
{
	switch(imgnb)
	{
		case 0: image_display(x,y,(u8*)gImage_LiangTong); 			break;
		case 1: image_display(x,y,(u8*)gImage_035); 						break;
		case 2: image_display(x,y,(u8*)gImage_039); 						break;
		case 3: image_display(x,y,(u8*)gImage_BackSpace); 			break;
		case 4: image_display(x,y,(u8*)gImage_Empty0); 					break;
		case 5: image_display(x,y,(u8*)gImage_To123); 					break;
		case 6: image_display(x,y,(u8*)gImage_Empty1); 					break;
		case 7: image_display(x,y,(u8*)gImage_Space); 					break;
		case 8: image_display(x,y,(u8*)gImage_Empty2); 					break;
		case 9: image_display(x,y,(u8*)gImage_ShiftL0); 				break;
		case 10: image_display(x,y,(u8*)gImage_ShiftR0); 				break;
		case 11: image_display(x,y,(u8*)gImage_Toabc); 					break;
		case 12: image_display(x,y,(u8*)gImage_Clear); 					break;
		case 13: image_display(x,y,(u8*)gImage_Empty3); 				break;
		case 14: image_display(x,y,(u8*)gImage_ShiftL1); 				break;
		case 15: image_display(x,y,(u8*)gImage_ShiftR1); 				break;
		case 16: image_display(x,y,(u8*)gImage_Enter); 					break;
		case 17: image_display(x,y,(u8*)gImage_Return); 				break;
		//case 18: image_display(x,y,(u8*)); 				break;
		default: break;
	}
}
















