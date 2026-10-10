#include "lan8720.h"
#include "stm32f4x7_eth.h"
#include "usart.h" 
//#include "delay.h"
#include "rtc.h"
#include "malloc.h" 

#include "lwip/netif.h"
#include "lwip_comm.h"

extern u8 DhcpSw; 											// DHCP开关
extern uint8_t CurntPage; 							// 页码
extern u8 cp_flag;											// 页面刷新标志

uint16_t link_status_new = 0;
uint16_t link_status_old = 0;
uint32_t EthInitStatus = 0;

__align(4) ETH_DMADESCTypeDef *DMARxDscrTab;	//以太网DMA接收描述符数据结构体指针
__align(4) ETH_DMADESCTypeDef *DMATxDscrTab;	//以太网DMA发送描述符数据结构体指针 
__align(4) uint8_t *Rx_Buff; 					//以太网底层驱动接收buffers指针 
__align(4) uint8_t *Tx_Buff; 					//以太网底层驱动发送buffers指针
  
static void ETHERNET_NVICConfiguration(void);
//LAN8720初始化
//返回值:0,成功;
//    其他,失败
u8 LAN8720_Init(void)
{
	u8 rval=0;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG , ENABLE);//使能GPIO时钟 RMII接口
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 		// 使能PD时钟，Gavin，2022-06-30 17:25
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //使能SYSCFG时钟
  
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC和PHY之间使用RMII接口
	
	/*网络引脚设置 RMII接口 
	  ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PG11
	  ETH_RMII_TXD0 --------------------> PG13
	  ETH_RMII_TXD1 --------------------> PG14
	  ETH_RESET-------------------------> PD3*/
	
	/*
	//配置PA1 PA2 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); //引脚复用到网络接口上
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	//配置PC1,PC4 and PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); //引脚复用到网络接口上
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
                                
	//配置PG11, PG14 and PG13 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
	
	//配置PD3为推完输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推完输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	*/
	
	/*网络引脚设置 RMII接口 
	  ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PB11
	  ETH_RMII_TXD0 --------------------> PB12
	  ETH_RMII_TXD1 --------------------> PB13
	  ETH_RESET-------------------------> PB14*/
	
	//配置PA1 PA2 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); //引脚复用到网络接口上
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);
	
	//配置PB11, PB12 and PB13 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);
	
	//配置PC1,PC4 and PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); //引脚复用到网络接口上
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
	
	//配置PB14为推完输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推完输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	LAN8720_RST=0;					//硬件复位LAN8720
	delay_ms(50);	
	LAN8720_RST=1;				 	//复位结束 
	ETHERNET_NVICConfiguration();
	rval=ETH_MACDMA_Config();
	
	return !rval;					//ETH的规则为:0,失败;1,成功;所以要取反一下
}

//以太网中断分组配置
void ETHERNET_NVICConfiguration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;  //以太网中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X00; //中断寄存器组2最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


//得到8720的速度模式
//返回值:
//001:10M半双工
//101:10M全双工
//010:100M半双工
//110:100M全双工
//其他:错误.
u8 LAN8720_Get_Speed(void)
{
	u8 speed;
	speed=((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); //从LAN8720的31号寄存器中读取网络速度和双工模式
	return speed;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//以下部分为STM32F407网卡配置/接口函数.

//初始化ETH MAC层及DMA配置
//返回值:ETH_ERROR,发送失败(0)
//		ETH_SUCCESS,发送成功(1)
u8 ETH_MACDMA_Config(void)
{
	u8 rval;
	ETH_InitTypeDef ETH_InitStructure; 
	
	//使能以太网时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
	ETH_DeInit();  								//AHB总线重启以太网
	ETH_SoftwareReset();  						//软件重启网络
	while (ETH_GetSoftwareResetStatus() == SET);//等待软件重启网络完成 
	ETH_StructInit(&ETH_InitStructure); 	 	//初始化网络为默认值  

	///网络MAC参数设置 
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//开启网络自适应功能
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//关闭反馈
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//关闭重传功能
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//关闭自动去除PDA/CRC功能 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//关闭接收所有的帧
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//允许接收所有广播帧
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//关闭混合模式的地址过滤  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//对于组播地址使用完美地址过滤   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//对单播地址使用完美地址过滤 
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//开启ipv4和TCP/UDP/ICMP的帧校验和卸载   
#endif
	//当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
	//这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //开启丢弃TCP/IP错误帧
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //开启接收数据的存储转发模式    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //开启发送数据的存储转发模式  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//禁止转发错误帧  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//不转发过小的好帧 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//打开处理第二帧功能
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//开启DMA传输的地址对齐功能
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//开启固定突发功能    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA发送的最大突发长度为32个节拍   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA接收的最大突发长度为32个节拍
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	
	rval=ETH_Init(&ETH_InitStructure,LAN8720_PHY_ADDRESS);		//配置ETH
	if(rval==ETH_SUCCESS)//配置成功
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//使能以太网接收中断	
	}
	
	return rval;
}

extern void lwip_pkt_handle(void);		//在lwip_comm.c里面定义
//以太网中断服务函数
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//检测是否收到数据包
	{ 
		lwip_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}  
//接收一个网卡数据包
//返回值:网络数据包帧结构体
FrameTypeDef ETH_Rx_Packet(void)
{ 
	u32 framelength=0;
	FrameTypeDef frame={0,0};   
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//清除ETH DMA的RBUS位  
			ETH->DMARPDR=0;//恢复DMA接收
		}
		return frame;//错误,OWN位被设置了
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//得到接收包帧长度(不包含4字节CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//得到包数据所在的位置
	}else framelength=ETH_ERROR;//错误  
	frame.length=framelength; 
	frame.descriptor=DMARxDescToGet;  
	//更新ETH DMA全局Rx描述符为下一个Rx描述符
	//为下一次buffer读取设置下一个DMA Rx描述符
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;  
}
//发送一个网卡数据包
//FrameLength:数据包长度
//返回值:ETH_ERROR,发送失败(0)
//		ETH_SUCCESS,发送成功(1)
u8 ETH_Tx_Packet(u16 FrameLength)
{   
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN)!=(u32)RESET)return ETH_ERROR;//错误,OWN位被设置了 
 	DMATxDescToSet->ControlBufferSize=(FrameLength&ETH_DMATxDesc_TBS1);//设置帧长度,bits[12:0]
	DMATxDescToSet->Status|=ETH_DMATxDesc_LS|ETH_DMATxDesc_FS;//设置最后一个和第一个位段置位(1个描述符传输一帧)
  	DMATxDescToSet->Status|=ETH_DMATxDesc_OWN;//设置Tx描述符的OWN位,buffer重归ETH DMA
	if((ETH->DMASR&ETH_DMASR_TBUS)!=(u32)RESET)//当Tx Buffer不可用位(TBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR=ETH_DMASR_TBUS;//重置ETH DMA TBUS位 
		ETH->DMATPDR=0;//恢复DMA发送
	} 
	//更新ETH DMA全局Tx描述符为下一个Tx描述符
	//为下一次buffer发送设置下一个DMA Tx描述符 
	DMATxDescToSet=(ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);    
	return ETH_SUCCESS;   
}
//得到当前描述符的Tx buffer地址
//返回值:Tx buffer地址
u32 ETH_GetCurrentTxBuffer(void)
{  
  return DMATxDescToSet->Buffer1Addr;//返回Tx buffer地址  
}

//为ETH底层驱动申请内存
//返回值:0,正常
//    其他,失败
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMIN,ETH_RXBUFNB*sizeof(ETH_DMADESCTypeDef));//申请内存
	DMATxDscrTab=mymalloc(SRAMIN,ETH_TXBUFNB*sizeof(ETH_DMADESCTypeDef));//申请内存  
	Rx_Buff=mymalloc(SRAMIN,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//申请内存
	Tx_Buff=mymalloc(SRAMIN,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//申请内存
	if(!DMARxDscrTab||!DMATxDscrTab||!Rx_Buff||!Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}

//释放ETH 底层驱动申请的内存
void ETH_Mem_Free(void)
{ 
	myfree(SRAMIN,DMARxDscrTab);//释放内存
	myfree(SRAMIN,DMATxDscrTab);//释放内存
	myfree(SRAMIN,Rx_Buff);		//释放内存
	myfree(SRAMIN,Tx_Buff);		//释放内存  
}




/*
//uint16_t link_status_new = 0;
//uint16_t link_status_old = 0;
//uint32_t EthInitStatus = 0;

void Eth_Link_ITHandler(void) //struct netif *netif) 			// 检查网线连接状态是否有变化
{
	// static u8 i=0;
	
	// 读PHY的状态寄存器，获取连接状态
	link_status_new = ((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
	if(EthInitStatus != ETH_SUCCESS) 								// 之前没有成功初始化过
	{
		if((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & PHY_Linked_Status)
		{
			//EthInitStatus = ETH_Reinit(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
			EthInitStatus = ETH_MACDMA_Config();
			//if(EthInitStatus == ETH_SUCCESS) 						// 配置成功
			//{
			//	ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);          // 使能以太网接收中断       
			//}
		}
		
		link_status_old = ((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
	}
	else 																						// 之前已经成功初始化过
	{
		if(link_status_new != link_status_old) 				// 自动协商状态或连接状态有变化（自动协商状态变化也是种网线重插）
		{
			link_status_old = link_status_new; 					// 记录连接状态的变化
			if(link_status_new) 												// 检测到网线连接
				netif_set_link_up(netif); 								// set link up for re link callbalk function
			else 																				// 网线断开
				netif_set_link_down(netif); 							// set link down for re link callbalk function       
		}
	}
}
*/

void Eth_Link_ITHandler(void) //struct netif *netif) 			// 检查网线连接状态是否有变化
{
	// static u8 i=0;
	u8 u8dat = 0;
	
	// 读PHY的状态寄存器，获取连接状态
	link_status_new = ((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
	if(EthInitStatus != ETH_SUCCESS) 								// 之前没有成功初始化过
	{
		if((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & PHY_Linked_Status)
		{
			//EthInitStatus = ETH_Reinit(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
			//EthInitStatus = ETH_MACDMA_Config();
			//if(EthInitStatus == ETH_SUCCESS) 						// 配置成功
			//{
			//	ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);          // 使能以太网接收中断       
			//}
			/*while(lwip_comm_init()!=0)
			{
				delay_ms(1200);
			}
			
			if(DhcpSw)
			{
				while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
				{
					lwip_periodic_handle();
				}
			}
			
			EthInitStatus = ETH_SUCCESS;*/
			
			u8dat = lwip_comm_init();
			if(u8dat == 0)
			{
				if(DhcpSw) 								// 自动获取IP
				{
					//LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
					//等待DHCP获取 
				 	//LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
					while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
					{
						lwip_periodic_handle();
					}
				}
				
				EthInitStatus = ETH_SUCCESS;
				if(CurntPage == 0) 													// 当前页面为第0页
				{
					cp_flag = 1; 															// 刷新页面标志置位
				}
			}
			else
			{
				if(u8dat != 1)
				{
					ETH_Mem_Free();
					lwip_comm_mem_free();
				}
			}
		}
		
		link_status_old = ((ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
	}
	else 																						// 之前已经成功初始化过
	{
		if(link_status_new != link_status_old) 				// 自动协商状态或连接状态有变化（自动协商状态变化也是种网线重插）
		{
			link_status_old = link_status_new; 					// 记录连接状态的变化
			if(link_status_new) 												// 检测到网线连接
				netif_set_link_up(&lwip_netif); 					// set link up for re link callbalk function
			else 																				// 网线断开
				netif_set_link_down(&lwip_netif); 				// set link down for re link callbalk function
			
			if(CurntPage == 0) 													// 当前页面为第0页
			{
				cp_flag = 1; 															// 刷新页面标志置位
			}
		}
		
		lwip_periodic_handle(); 											// LWIP轮询任务
	}
}

/*void Eth_Link_ITHandler(struct netif *netif) 			// 检查网线连接状态是否有变化
{
	// static u8 i=0;
	
	// 读PHY的状态寄存器，获取连接状态
	link_status_new = ((ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
	//printf("link_status_new is  %d\r\n",link_status_new);
	if(EthStatus != ETH_SUCCESS) 										// 之前没有成功初始化过
	{
		printf("EthStatus != ETH_SUCCESS \r\n");
		
		if((ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_BSR)) & PHY_Linked_Status)
		{       
			// Configure Ethernet 
			EthStatus = ETH_Init(&ETH_InitStructure, DP83848_PHY_ADDRESS);
			if(EthStatus==ETH_SUCCESS) 									// 配置成功
			{
				ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE); 					// 使能以太网接收中断
				printf("EthStatus == ETH_SUCCESS \r\n");
			}
		}                       
		// link_status_old = ((ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_BSR)) & (PHY_AutoNego_Complete | PHY_Linked_Status));
		// printf("link_status_new is  %d\r\n",link_status_new);
		// printf("link_status_old is  %d\r\n",link_status_old);
	}
	else 																						// 之前已经成功初始化过
	{
		if((link_status_new != 0)&&(link_status_old == 0))
		{
			link_status_old = link_status_new;
			
			netif_set_link_up(netif); 									// set link up for re link callbalk function
			printf("netif_set_link_up \r\n");
		}
		else if((link_status_new == 0)&&(link_status_old != 0))
		{
			link_status_old = link_status_new;
			netif_set_link_down(netif);//set link down for re link callbalk function   
			printf("netif_set_link_down \r\n");
		}
	
	}
}*/

/*
**************************************************************
*  函数名称：LAN8720_Get_State_link(void)
*  函数功能：获取网线的连接状态
*  返回参数：0或1
**************************************************************
*/
u8 LAN8720_Get_State_link(void)
{
	u8 state;
	
	state = ((ETH_ReadPHYRegister(0x00,1)&0x04)>>2); 	// 从LAN8720的1号寄存器中读取网络连接状态
	
	return state;
}

// 插入主函数之中，检测到网线即连接
/*void Net_Rest_Connect(void) 						// 插入网线连接函数
{
	if(state_link == 0x00 && flag_rest == 0 && net_run_flag == 1)
	{
		state_link = LAN8720_Get_State_link();
		if(state_link == 0x01 && flag_rest == 0 && net_run_flag == 1)
		{
			secon_net_run_flag = 0; 					// 初始化标志位
			
			if(lwip_comm_second_init() != 0)
			{
				secon_net_run_flag = 1; 				// 失败
			}
			
			if(secon_net_run_flag == 0)
			{
				TCP_server_init(); 							// tcp服务器初始化
				flag_rest = 1; 									// 网络已链接，TCP服务打开
				led_static_display();
			}
		}
	}
}*/














