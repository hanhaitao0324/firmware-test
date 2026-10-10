#include "motor.h"
#include "timer.h"
#include "pwm.h"
#include "stdlib.h"
#include "math.h"
#include "INP.h"
#include "ad7190.h"
#include "beeper.h"
#include "osObjects.h"

u8 MotSpiStatus = 0;
int nullwert;	// 空载ad值
extern u8 KeyState;
extern u8 PopWindow;

extern u8 CBRIp0,CBRIp1;
extern u8 CanRxBuf[32][8];
extern u16 cur_tpx,cur_tpy;
extern u8 TouchFlag;
extern int MDIANUM;
extern int MHARNUM;
extern u8 PauseFlag;

void LCD_ShowIcon(u16 x,u16 y,u16 width,u16 height,u16 pcolor,u16 bcolor,u8 p,u8 mode);

// 使能时钟
// 初始化PA8为输出，电机的方向控制
// 初始化PC8为输入，电机的过流保护	    
void MOTOR_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//SPI_InitTypeDef 	SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 		// 使能GPIOE时钟
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 		// 使能GPIOC时钟

	// I/O 输出设置 ----------------------------------------------------
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 						// 普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
  
	// GPIOE8,ST_ENA1，初始化设置
	// 设备端描述：使能输入；当该引脚被驱动到高电平时， 
	// 功率 MOSFET 输出关闭(所有电机输出为悬浮（FLOATING） 状态)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// ST_ENA1，初始化
	
	// GPIOE9,REFL_BRUCH
	// 设备端描述：SD_MODE = 0: 左参考输入(用于内部斜坡发生器)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// REFL_BRUCH，初始化
	
	// GPIOE11,SPI_ST_MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// SPI_ST_MOSI，初始化
	
	// GPIOE12,SPI_ST_SCK，
	// 设备端描述：第一个上升沿锁SDI，第二个上升沿输出SDO，不传输时常低电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// SPI_ST_SCK，初始化
	
	// GPIOE13,SPI_ST_SS1，片选，低电平有效
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// SPI_ST_SS1，初始化
	
	// I/O输入设置 -----------------------------------------------------
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 							// 普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	
  // GPIOE10,SPI_ST_MISO，初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 										// SPI_ST_MISO，初始化
	
	// 端口初始值设置 --------------------------------------------------
	GPIO_SetBits(GPIOE,GPIO_Pin_8); 													// ST_ENA1置高，MOSFET输出关闭
	GPIO_SetBits(GPIOE,GPIO_Pin_13); 													// SPI_ST_SS1置高，SPI片选禁止
	GPIO_ResetBits(GPIOE,GPIO_Pin_12); 												// SPI_ST_SCK置低，SPI时钟常态
}

// SPI读写一字节数据
u8 mySPI_ReadWriteByte(u8 TxData) 			// 这个没有测试应该是可以的
{
	u8 i;
	u8 temp=0;
	
	for(i=0;i<8;i++)
	{
		//SCLK=0;
		if(TxData&0x80)
			//MOSI=1;
			SPI_ST_MOSI_H();
		else
			//MOSI=0;
			SPI_ST_MOSI_L();
		
		TxData<<=1;
		//SCLK=1;
		SPI_ST_SCK_H();
		
		delay_us(SPI_SPEED);
		temp<<=1;
		
		//if(MISO)
		if(SPI_ST_MISO())
			temp++;
		
		//SCLK=0;
		SPI_ST_SCK_L();
		delay_us(SPI_SPEED);
	}
	
	return temp;
}

void sendMotDat(u8 r, u32 dat)
{
	u8 td;
	
	SPI_ST_SS1_L();
	delay_us(SPI_SPEED);
	
	td = r|0x80;
	mySPI_ReadWriteByte(td);
	
	td = (u8)((dat>>24)&0x000000ff);
	mySPI_ReadWriteByte(td);
	
	td = (u8)((dat>>16)&0x000000ff);
	mySPI_ReadWriteByte(td);
	
	td = (u8)((dat>>8)&0x000000ff);
	mySPI_ReadWriteByte(td);
	
	td = (u8)(dat&0x000000ff);
	mySPI_ReadWriteByte(td);
	
	SPI_ST_SS1_H();
}

u32 receiveMotDat(u8 r, u8 * ss)
{
	u8 td;
	u32 dat;
	
	SPI_ST_SS1_L();
	delay_us(SPI_SPEED);
	
	//td = r|0x80;
	td = r&(~0x80);
	*ss = mySPI_ReadWriteByte(td);
	
	dat <<= 8;
	td = mySPI_ReadWriteByte(0);
	dat += td;
	
	dat <<= 8;
	td = mySPI_ReadWriteByte(0);
	dat += td;
	
	dat <<= 8;
	td = mySPI_ReadWriteByte(0);
	dat += td;
	
	dat <<= 8;
	td = mySPI_ReadWriteByte(0);
	dat += td;
	
	SPI_ST_SS1_H();
	
	return dat;
}

u32 readMotDat(u8 r, u8 * ss)
{
	receiveMotDat(r, ss);
	
	return receiveMotDat(r, ss);
}

void motor_set_vmax(u32 velocity) 
{
	float driver_factor = 1.3981f;
	
	float velocity_f = (float)velocity * driver_factor;
	if(velocity) velocity_f += 0.5f;

	if (velocity_f > 0x7ffe00) 
	{
		// 2^23-512 is the maximum value for VMAX
		velocity_f = 0x7ffe00;
	}

	sendMotDat(TMC5160_VMAX , velocity_f);
}

void motor_stop(void) 
{
	motor_set_vmax(0);
}

void motor_moveTo(int target, u32 speed)
{
	motor_set_vmax(speed);
	sendMotDat(TMC5160_XTARGET , (u32)target);
	sendMotDat(TMC5160_RAMPMODE , 0x00000000);
}

void motor_rotate(int velocity)
{
	motor_set_vmax(abs(velocity));
	sendMotDat(TMC5160_RAMPMODE , (velocity >= 0)? 1 : 2);
}

void backreset(void)		// 电机回到零位，限位开关位置
{
	if(!INP1)
		return;
	motor_rotate(-51200*3);
	while(INP1);
	motor_stop();
}

void tbhnull(void)
{
	unsigned int i;
	unsigned long sum=0;

	for(i=0;i<100;i++)
	{
		ad7190Result = ADC_Num();
		sum += ad7190Result;
	}
	sum /= 100;
	i=(int)sum;
	nullwert=i;	
}

unsigned char dmmess(void)
{
	u8 rammel = 0;
	u8 ueberl = 0;
	int smot;

	tbhnull();

	step = (int)((NomiLength + DiamOffset)*mmfactor);
	motor_moveTo(-step,(FastMoveSP/2)*51200);

	while(ueberl<1)
	{
		if(Dis_Flash == 0 && PopWindow == 0 && (CurntPage==42 || CurntPage==43))
		{
			Dis_Flash = 500;
			if(INP1)
				LCD_ShowIcon(57,208,48,48,GRAY_878787,GRAY_EDEDED,110,0); 		// 小"X"
			else
				LCD_ShowIcon(57,208,48,48,GREEN_3AAA35,WHITE,78,0); 		// 小"√"
		}

		// CAN通信处理 -------------------------------
		CanDataHandle();

		if(TouchFlag == 1) 							// 有触摸标志
		{
			TouchFlag = 0;
			BEEPER_PlayTones(tones_1khz);
			
			if(((CurntPage == 42) && (MDIANUM == 0)) || ((CurntPage == 43) && (MHARNUM == 0)))
			{
				if((cur_tpx > 880)&&(cur_tpx < 880+112)&&(cur_tpy > 280)&&(cur_tpy < 280+128))	// stop
				{
					BEEPER_PlayTones(tones_1khz);
					motor_stop();
					return(1);
				}	
			}
			AbortHandle();
		}

		smot = (int)readMotDat(TMC5160_XACTUAL, &MotSpiStatus);
		if(smot == (-step))
		{
			return(rammel);
		}

		if((CurntPage == 6 || CurntPage == 25) && ConLidSensor == 1)
		{
			if(GetIni(1) == 0)
			{
				motor_stop();
				while(GetIni(1) == 0);
				PauseFlag = 1;
				return(1);
			}
		}

		ad7190Result = Get_ADC_AvrgNum(1);
		if(abs(ad7190Result-nullwert) > NwtFak*ContactForce)
			ueberl++;
		else
			ueberl=0;
	}

	motor_stop();
	rammel = 1;
	// error
	return(rammel);
}

float GetNomiLength(void)
{
	float nominal;
	int smot;

	tbhnull();

	motor_moveTo(0,51200*(DiaMeasSpeed/2));

	while(abs(Get_ADC_AvrgNum(1)-nullwert) < NwtFak*ContactForce)
	{
		smot = (int)readMotDat(TMC5160_XACTUAL, &MotSpiStatus);
		if(smot == 0)
		{
			break;
		}
	}
	motor_stop();
	osDelay(50);

	nominal = abs((int)readMotDat(TMC5160_XACTUAL, &MotSpiStatus))/mmfactor;

	motor_moveTo(-blops,(FastMoveSP/2)*51200);
	do
	{
		smot = (int)readMotDat(TMC5160_XACTUAL, &MotSpiStatus);
	}while(smot != (-blops));
	
	CBRIp0 = 0;
	CBRIp1 = 0;
	
	return(nominal);
}

u8 Encoder_deviation_detected(void)
{
	uint32_t gstat=0;
	u8 error=0;
    uint32_t enc_status = readMotDat(TMC5160_ENC_STATUS, &MotSpiStatus);

	if(MotSpiStatus&0x03)
	{
		gstat = readMotDat(TMC5160_GSTAT, &MotSpiStatus);
		error = gstat&0x000000ff;
	}
    
    if (enc_status & 2) 
	{
        error |= 0x08;
    }

	return error;
}

void Encoder_clear_deviation_warning(void)
{
    sendMotDat(TMC5160_ENC_STATUS, 2);
	sendMotDat(TMC5160_GSTAT, 0x07);
}






