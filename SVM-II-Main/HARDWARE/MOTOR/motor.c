#include "motor.h"
#include "timer.h"
#include "pwm.h"

//status of the device
volatile TDeviceStatus globalStatus;

// Tick value for calculate speed
volatile uint16_t MOTOR_Tick_Rev=0; 		// 计算速度的脉冲数

volatile uint16_t MOTOR_Revolution; 		// 电机实际运转的圈数
volatile uint32_t MOTOR_Times; 					// 电机运转的时间

//volatile uint8_t MOTOR_CON=0;
volatile uint8_t Bit_MOTOR; 						// working bit 				// 工作位
volatile uint8_t Bit_Fast; 							// starting bit 			// 启动位
volatile uint8_t Bit_Slow; 							// stopping bit 			// 停止位
volatile uint8_t Bit_OneRev; 						// one rotation bit 	// 一个站
volatile uint8_t Bit_Stab; 							// stabilization bit 	// 转动
volatile uint8_t Bit_Pulse; 						// got pulse bit 			// 有脉冲
volatile uint8_t Bit_1sec; 							// 1 sec bit 					// 1秒
volatile uint8_t Bit_SnsrStop; 					// sensor stop bit 		// 编码器停止

float Speed_Buf[SPEED_BUF_LEN+1];

uint32_t MOTOR_Counter = 0;

//volatile TBool MOTOR_SpeedBlock=false;

uint16_t MotorSpeed;                   

uint8_t MOTOR_MaxStep;
uint8_t MOTOR_Prescaler;

uint16_t IntSpeedBuf[INT_SPEED_BUF_LEN];
uint16_t IntSpeedBuf2[INT_SPEED_BUF_LEN];
uint8_t IntSpeedBufIndex;
uint8_t IntSpeedBufLen;

uint16_t MOTOR_PulsPerRot;

volatile uint16_t MOTOR_Value;
uint16_t MOTOR_Exp_Value;
volatile uint16_t MOTOR_Cur_Value; 								// 电机编码器中断时的定时计数器中的数值

// Tick value for calculate speed
volatile uint16_t MOTOR_Tick1s=0;
volatile uint16_t MOTOR_Tick=0; 									// 电机编码器输出脉冲计数器

uint16_t MOTOR_CurrSpeed;

volatile uint16_t PWM_B;

// 使能时钟
// 初始化PA8为输出，电机的方向控制
// 初始化PC8为输入，电机的过流保护	    
void MOTOR_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 		// 使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 		// 使能GPIOC时钟

  // GPIOA8,电机方向，初始化设置 ----------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 						// 普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure); 										// 初始化
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 							// 普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure); 										// 初始化
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_8); 												// GPIOA8设置低，电机方向继电器不吸合
	
	//TIM8CH2_PWM_Init(1000-1,84-1); 														// PWM初始化,500uS
	TIM8CH2_PWM_Init(1000-1,12-1); 														// PWM初始化,500uS，时钟频率168MHz
	
	//TIM9_CH1_Cap_Init(65000-1, 6-1);  												// 电机编码器捕获
	//TIM9_CH1_Cap_Init(0xffff, 84-1);  												// 电机编码器捕获,时钟频率168MHz
	TIM9_CH1_Cap_Init(0xffff, 96-1);  												// 电机编码器捕获
	
	//MOTOR_PulsPerRot = EEPROM1.PulsesPerRotat;
	MOTOR_PulsPerRot = 120; 							// 每转脉冲数
	if(MOTOR_PulsPerRot < 50)
	{
		MOTOR_MaxStep = 32;
		MOTOR_Prescaler = 64;
		//TCCR3B |= (1<<CS30);
	}
	else
	if(MOTOR_PulsPerRot < 100)
	{
		MOTOR_MaxStep = 16;
		MOTOR_Prescaler = 8;
	}
	else
	{
		MOTOR_MaxStep = 8; 									// 最大步长
		MOTOR_Prescaler = 8; 								// 预分频器
	}
}

// 电机编码器捕捉: 	TIM9_CH1, 16位计数器，输入口：PE5
// -----------------------------------------------------------------------------
TIM_ICInitTypeDef  TIM9_ICInitStructure;

//定时器9通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM9_CH1_Cap_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure;
	NVIC_InitTypeDef 					NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE); 			// TIM9时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 		// 使能PORTA时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 								// GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 						// 下拉
	GPIO_Init(GPIOE,&GPIO_InitStructure); 										// 初始化PA0

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); 		// PE5复用位定时器9
  
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 													// 定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 				// 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; 														// 自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	
	//初始化TIM9输入捕获参数
	TIM9_ICInitStructure.TIM_Channel = TIM_Channel_1; 									// CC1S=01 	选择输入端 IC1映射到TI1上
  TIM9_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 				// 上升沿捕获
  TIM9_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 		// 映射到TI1上
  TIM9_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 							// 配置输入分频,不分频 
  TIM9_ICInitStructure.TIM_ICFilter = 0x00; 													// IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM9, &TIM9_ICInitStructure);
		
	TIM_ITConfig(TIM9,TIM_IT_Update|TIM_IT_CC1,ENABLE); 								// 允许更新中断 ,允许CC1IE捕获中断	
	
  TIM_Cmd(TIM9,ENABLE ); 																		// 使能定时器9
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 				// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure); 													// 根据指定的参数初始化VIC寄存器、
}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM9CH1_CAPTURE_STA = 0; 						// 输入捕获状态		    				
u32	TIM9CH1_CAPTURE_VAL; 								// 输入捕获值(TIM2/TIM5是32位)

//定时器9中断服务程序 ---------------------------------------
void TIM1_BRK_TIM9_IRQHandler(void)
{
	uint16_t CurPWM;
	uint32_t diff;
	
	if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET) 				// 溢出
	{	     
		if(Bit_Stab == 1)
	  {
	    TIM8->CCR2 += MOTOR_PWM_STEP;
	    if (TIM8->CCR2 > MOTOR_PWM_MAX)
	      TIM8->CCR2 = MOTOR_PWM_MAX;
	  }
	}
	
	if(TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET) 						// 捕获1发生捕获事件
	{	
		MOTOR_Cur_Value = TIM_GetCapture1(TIM9); 								// 获取当前的捕获值.
	  TIM_SetCounter(TIM9,0); 																// 清零Tim9计数器
	  
	  MOTOR_Tick++;
  	MOTOR_Tick_Rev++;
  	
  	MOTOR_Counter += MOTOR_Cur_Value;
    MOTOR_Tick1s++;
  	
  	if(Bit_Stab == 1)
		{
			CurPWM = TIM8->CCR2; 																	// 获取当前PWM比较值
			
			if(MOTOR_Cur_Value < MOTOR_Exp_Value) 								// 电机转快了
			{
				diff = (uint16_t)((((uint32_t)MOTOR_Exp_Value-MOTOR_Cur_Value)<<7)/MOTOR_Exp_Value);
				
				if(diff > MOTOR_MaxStep)
					diff = MOTOR_MaxStep;
				
				if(diff < CurPWM)
					CurPWM -= diff;
				else
					CurPWM = MOTOR_PWM_MIN;
			}
			else
	    if(MOTOR_Cur_Value > MOTOR_Exp_Value) 								// 电机转慢了
	    {
	    	diff = (uint16_t)((((uint32_t)MOTOR_Cur_Value-MOTOR_Exp_Value)<<7)/MOTOR_Exp_Value);
	    	
	    	if(diff > MOTOR_MaxStep)
	    		diff = MOTOR_MaxStep;
	    	
	    	CurPWM += diff;
	    	if(CurPWM > MOTOR_PWM_MAX) 
	    		CurPWM = MOTOR_PWM_MAX;
	    }
	    
	    TIM8->CCR2 = CurPWM;
  	}
  }
 	
	TIM_ClearITPendingBit(TIM9, TIM_IT_CC1|TIM_IT_Update); 		// 清除中断标志位
}

// stabilization
void MOTOR_IntHandler(void)
{
	if (Bit_Slow == 1) 
		MOTOR_Slow();
	
	if (Bit_Stab == 1)
	{
		if (clockCounter>=(TIMER_INT_RATE*SECONDS_COUNTER))
		{
			if (!MOTOR_Tick)
			{
				// motor error
				Bit_Stab = 0;
				globalStatus.motorFailure = true;
				TIM8->CCR2 = MOTOR_PWM_MIN;
				
				return;
			}
			
			MOTOR_Tick=0;
			clockCounter -= (TIMER_INT_RATE*SECONDS_COUNTER);
		}
		
		if (MOTOR_Tick_Rev>=MOTOR_PulsPerRot)
		{
			MOTOR_Tick_Rev-=(MOTOR_PulsPerRot);
			MOTOR_Revolution++;
		}
	}
}

// calculate PWM settings for current motor speed
// 计算当前电机转速的PWM设置
void MOTOR_SetSpeed(float speed)
{
	uint16_t CountPWM;
	
	MotorSpeed = (uint16_t)(speed*10);
	CountPWM = MOTOR_PWM_START; 					// 启动占空比值
	PWM_B = CountPWM;
	//MOTOR_Exp_Value = (uint16_t)(XTAL*60/(MOTOR_Prescaler*speed*MOTOR_PulsPerRot));
	MOTOR_Exp_Value = (uint16_t)(XTAL*60/(speed*MOTOR_PulsPerRot));
}

// initialize starting motor
// 电机启动 ------------------------------------------------
void MOTOR_Start(void)
{
	uint8_t ix;
	
	MOTOR_Value = 0;
	//TCNT1 = 0;
	TIM8->CCR2 = PWM_B;
	//TIM8->CCR2 = 250;
	
	MOTOR_Tick1s = 0;
	MOTOR_Tick = 0;
	MOTOR_Counter = 0;
	
	//MOTOR_PulsPerRot=EEPROM1.PulsesPerRotat;
	MOTOR_PulsPerRot = 120; 							// 每转脉冲数
	Bit_MOTOR = 1; 												// 电机工作标志置位
	Bit_Fast = 1; 												// 电机加速标志置位
	Bit_Stab = 1; 												// 电机稳定标志置位
	Bit_Slow = 0; 												// 电机减速标志复位
	
	clockCounter = 0;
	IntSpeedBufIndex = 0;
	IntSpeedBufLen = 0;
	for (ix=0; ix<INT_SPEED_BUF_LEN; ix++)
		IntSpeedBuf[ix] = 0;
}
  
// stop motor
// 电机停止 ------------------------------------------------
void MOTOR_Stop(void)
{
	Bit_Stab = 0; 												// 电机运转稳定标志复位
	Bit_Fast = 0; 												// 电机加速标志复位
	PWM_B = MOTOR_PWM_MIN; 								// 占空比最小值
	
	// set motor slow bit
	Bit_Slow = 1; 												// 电机减速标志置位
}

// starting motor
// 电机加速 ------------------------------------------------
void MOTOR_Fast(void)
{
	uint16_t TempOCR;
	
	// if motor is running
	if ((Bit_MOTOR==1)&&(Bit_Fast==1))
	{
		TempOCR = TIM8->CCR2;
		TempOCR++;
		if (TempOCR>=PWM_B)
		{
			// speed is reached
			TIM8->CCR2 = PWM_B;
			
			// clear starting bit
			Bit_Fast = 0;
			
			// set stabilization bit
			Bit_Stab = 1;
		}
		else
		{
			TIM8->CCR2 = TempOCR;
		}
	}
}

// stopping motor
// 电机减速 ------------------------------------------------
void MOTOR_Slow(void)
{
	//uint16_t TempOCR;
	
	// if stopping bit is set
	if (Bit_Slow == 1)
	{
		//TIM8CH2_SetCompare(PWM_B); 					// PWM占空比最小
		TIM8->CCR2 = PWM_B;
	
		// clear stopping bit
		Bit_Slow = 0;
		Bit_MOTOR = 0;
	}
}

// get current motor speed
// 获取电机转速
uint16_t MOTOR_GetSpeed(void)
{
	//uint8_t ix;
	uint32_t Summ;
	float tspeed;
	
	if (Bit_MOTOR == 0) 									// 判断电机接口变量，工作位
		return 0;
	
	//if (!MOTOR_Tick1s) 									// Gavin，获取转速精度低，速度快，2023-01-30 周一 10:28:58
	//	return 0;
	
	if (MOTOR_Tick1s < 25) 								// Gavin，获取转速精度高，速度慢，2023-01-30 周一 10:29:45，2022-11-04 周五 15:07:12
		return MOTOR_CurrSpeed;
	
	//MOTOR_SpeedBlock=true;
  __disable_irq() ; // 关闭总中断
  Summ = MOTOR_Counter/MOTOR_Tick1s;
  MOTOR_Tick1s=0;
  MOTOR_Counter=0;  
  //MOTOR_SpeedBlock=false;
  __enable_irq(); 	// 开启总中断
  //tspeed = ((float)XTAL/(MOTOR_Prescaler*Summ*MOTOR_PulsPerRot))*600+0.5;
  tspeed = ((float)XTAL/(Summ*MOTOR_PulsPerRot))*600+0.5f;
  MOTOR_CurrSpeed = (uint16_t)tspeed;
  
  return MOTOR_CurrSpeed;
}  

//get current motor speed without measurement
// 获取当前电机转速，不需要测量
uint16_t MOTOR_GetCurrSpeed(void)
{
	if (Bit_MOTOR == 0) 									// 判断电机接口变量，工作位
		return 0;
	
	return MOTOR_CurrSpeed;
}

// 外部中断5，电机电流超限报警 -----------------------------

//外部中断初始化程序
//初始化PE5为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 							// 使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5); 			// PE5 连接到中断线5
	
	/* 配置EXTI_Line54 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 								// 中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 						// 下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 													// 中断线使能
  EXTI_Init(&EXTI_InitStructure); 																		// 配置
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 									// 外部中断5
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 				// 抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 							// 子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 										// 使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); 																		// 配置
	   
}

//外部中断5服务程序
void EXTI5_IRQHandler(void)
{
	Bit_Stab = 0; 												// 电机接口变量，关闭转动位
	globalStatus.motorFailure = true; 		// 电流超限状态置位
	//TIM8CH2_SetCompare(MOTOR_PWM_MIN); 		// PWM占空比最小
	TIM8->CCR2 = MOTOR_PWM_MIN;
	
	EXTI_ClearITPendingBit(EXTI_Line5); 	// 清除LINE5上的中断标志位  
}

// get rotations
uint16_t MOTOR_GetRevolutions(void)
{
  return MOTOR_Revolution;
}

// reset rotations counter
// 重置旋转计数器 
void MOTOR_ResetRevolutions(void)
{
  MOTOR_Tick_Rev = 0;
  MOTOR_Revolution = 0;
  MOTOR_Times = 0;
}

// 电机编码器检查 ----------------------------------------
uint8_t MOTOR_SensorReady(void)
{
  //return macBitTest(PIN_SENSOR,pinSensor);
  return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5); 					// 电机编码器，PE5
}

// 电机转动方向控制 ----------------------------------------
// 参数On，0，继电器松开，1，继电器吸合
void MOTOR_SetReverse(uint8_t dir)
{
	if(dir)
		MOT_DIR1;
	else
		MOT_DIR0;
}
