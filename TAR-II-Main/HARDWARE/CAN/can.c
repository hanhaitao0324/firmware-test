#include "osObjects.h"                      // RTOS object definitions
#include "can.h"
#include "led.h"
//#include "delay.h"
#include "usart.h"
#include "system.h"

//volatile u8 CanBuf[256];
u8 CanRxBuf[32][8]; 							// CAN接收数据缓存
u8 CBRIp0 = 0; 									// CAN缓存数据处理指针
u8 CBRIp1 = 0; 									// CAN缓存数据接收指针

char CanTxBuf[256]; 						// CAN发送数据缓存

u16 CanTx_TimeOut = 0; 					// CAN发送超时
u16 Last_cmd = 0; 							// 保存指令名称
u8	Last_len = 0; 							// 保存指令长度

uint16_t cur_tpx; 							// 当前触摸x坐标
uint16_t cur_tpy; 							// 当前触摸y坐标

u8 TouchFlag = 0; 							// 触摸信号标志
u8 CanRxSn = 0; 								// can指令序号
u8 CanTxSn = 0;
u8 CanTx_BusyFlag = 0; 					// Can发送忙标志

void Can_Handler(void);

// CAN初始化
// tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
// tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
// tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
// brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
// 波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
// mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
// Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
// 则波特率为:42M/((6+7+1)*6)=500Kbps
// 返回值: 0,初始化OK;
// 				其他,初始化失败; 
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef 						GPIO_InitStructure; 
	CAN_InitTypeDef 						CAN_InitStructure;
	CAN_FilterInitTypeDef 			CAN_FilterInitStructure;
	
#if CAN1_RX0_INT_ENABLE 
	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
	
	// 使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 		// 使能PORTB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 			// 使能CAN1时钟	
	
	// 初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 							// 复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 						// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 				// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 							// 上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure); 										// 初始化PB8,PB9
	
	// 引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); 		// GPIOB8复用为CAN1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); 		// GPIOB9复用为CAN1
	
	// CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;											// 非时间触发通信模式
	CAN_InitStructure.CAN_ABOM = DISABLE;											// 软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM = DISABLE; 										// 睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART = ENABLE; 											// 禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM = DISABLE;											// 报文不锁定,新的覆盖旧的  
	//CAN_InitStructure.CAN_TXFP = DISABLE;											// 优先级由报文标识符决定 
	CAN_InitStructure.CAN_TXFP = ENABLE;											// 优先级为FIFO
	CAN_InitStructure.CAN_Mode = mode; 												// 模式设置 
	CAN_InitStructure.CAN_SJW = tsjw;													// 重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = tbs1; 												// Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = tbs2; 												// Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp; 										// 分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure); 											// 初始化CAN1 
	
	// 配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber = 0; 						// 过滤器0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 		// 32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000; 				// 32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; 		// 32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; // 过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 		// 激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure); 								// 滤波器初始化
	
#if CAN1_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE); 										// FIFO0消息挂号中断允许
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 				// 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	
	return 0;
}   

void CAN_ModeInit(void)
{
	// CAN初始化环回模式，波特率500Kbps -----------------------
	//CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
	
	// CAN初始化普通模式，波特率500Kbps -----------------------
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);
}

#if CAN1_RX0_INT_ENABLE																			// 使能RX0中断
// 中断服务函数			    
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	int i = 0;
	
	CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
	{
		CanRxBuf[CBRIp1][i] = RxMessage.Data[i];
	}
	
	CBRIp1++;
	if(CBRIp1 >= 32)
		CBRIp1 = 0;
	
	//osSignalSet(CommTask, 0x2); 					// 
}
#endif

// can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
// len:数据长度(最大为8)				     
// msg:数据指针,最大为8个字节.
// 返回值: 0,成功;
// 				其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{
	u8 mbox = 0x04;
	u32 i = 0;
	CanTxMsg TxMessage;
	
	TxMessage.StdId=0x12; 								// 标准标识符为0
	TxMessage.ExtId=0x12; 								// 设置扩展标示符（29位）
	TxMessage.IDE=0; 											// 使用扩展标识符
	TxMessage.RTR=0; 											// 消息类型为数据帧，一帧8位
	TxMessage.DLC=len; 										// 发送两帧信息
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i]; 						// 第一帧信息          
	
	i=0;
	do
	{
		mbox = CAN_Transmit(CAN1, &TxMessage); 	// 获取邮箱号
		i++;
		if(i>=0x0fff)
			return 1;
		
		//if(mbox > 2)
			osDelay(1);
	}while(mbox > 2);
	
	i=0;
	while(CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)
	{
		osDelay(1);
		i++; 																// 等待发送结束
		if(i>=0X0fff)
			return 1;
	}
	
	return 0;
}

void CAN1_SenDat(u8 * dat,u8 len)
{
	u8 i;
	u8 cb[8];
	
	do
	{
		for(i=0;i<len;i++)
		{
			cb[i] = dat[i];
		}
	}while(CAN1_Send_Msg(cb,len));
}

// can口接收数据查询
// buf:数据缓存区;	 
// 返回值:   0,无数据被收到;
// 				其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{
	u32 i;
	CanRxMsg RxMessage;
	
	if(CAN_MessagePending(CAN1,CAN_FIFO0)==0)
		return 0; 																							// 没有接收到数据,直接退出 
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage); 								// 读取数据
	for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];
	
	return RxMessage.DLC;	
}

u8 CAN1_RecDat(void)
{
	u8 cb[8];
	
	while(CAN1_Receive_Msg(cb)!=1);
	
	return cb[0];
}

// can发送一组数据
// cmd:指令代码
// len:参数长度(最大为200)				     
// msg:数据指针,最大为200个字节.
// 返回值:   0,成功;
// 				其他,失败;
u8 CAN1_Send_Dat(u16 cmd, u8 len, char* msg)
{
	u8 t_buf[8];
	u8 len_ip;
	//u8 redat;
	u16 i;

//CanReTx:
	
	Last_cmd = cmd; 											// 保存指令名称
	Last_len = len; 											// 保存指令长度
	
	t_buf[0] = 0xA5;
	t_buf[1] = 12345>>8;
	t_buf[2] = 12345&0x00ff;
	t_buf[3] = CanTxSn; 									// 校验指令序号，0~255循环
	
	t_buf[4] = cmd;
	t_buf[5] = 0;
	t_buf[6] = len;
	t_buf[7] = 0;
	
	// 发送数据头 ------------------------
	i = 0;
	while((CAN1_Send_Msg(t_buf,8)==1)&&(i < 0x0fff))
	{
		i++;
	}
	
	if(i >= 0x0fff)
		return 1;
	//osDelay(1);
	
	// 发送数据 --------------------------
	len_ip = 0;
	while(len_ip < len)
	{
		t_buf[len_ip%8] = msg[len_ip];
		len_ip++;
		if(len_ip%8 == 0)
		{
			//redat = CAN1_Send_Msg(t_buf,8);
			i = 0;
			while((CAN1_Send_Msg(t_buf,8)==1)&&(i<0xfff))
			{
				i++;
			}
			if(i>=0x0fff)
				return 1;
			//osDelay(1);
		}
	}
	
	// 发送尾数 --------------------------
	if(len_ip%8 != 0)
	{
		while(len_ip%8)
		{
			t_buf[len_ip%8] = 0;
			len_ip++;
		}
		
		//redat = CAN1_Send_Msg(t_buf,8);
		i=0;
		while((CAN1_Send_Msg(t_buf,8)!=0)&&(i<0x0fff))
		{
			i++;
		}
		if(i>=0x0fff)
			return 1;
	}
	
	CanTx_TimeOut = 1000; 								// 发送超时1000mS
	CanTxSn++;
	
	//osDelay(1);
	return 0;
}









