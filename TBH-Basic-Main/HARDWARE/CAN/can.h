#ifndef __CAN_H
#define __CAN_H	 
//#include "sys.h"	    
#include "stm32f4xx.h"

	
//CAN1接收RX0中断使能
//#define CAN1_RX0_INT_ENABLE	0 					// 0,不使能;1,使能.								    
#define CAN1_RX0_INT_ENABLE	1 					// 0,不使能;1,使能.								    

//extern volatile u8 CanBuf[256]; 				// CAN接收数据缓存
extern u8 CanRxBuf[32][8]; 			// CAN接收数据缓存
extern u8 CBRIp0; 							// CAN缓存数据处理指针
extern u8 CBRIp1; 							// CAN缓存数据接收指针

extern char CanTxBuf[256]; 							// CAN发送数据缓存

extern u16 CanTx_TimeOut; 					// CAN发送超时
extern uint16_t cur_tpx; 							// 当前触摸x坐标
extern uint16_t cur_tpy; 							// 当前触摸y坐标
extern u8 TouchFlag; 											// 触摸信号标志
extern u8 CmdRxSn; 								// can指令序号
extern u8 CanTxSn;
extern u8 CanTx_BusyFlag; 					// Can发送忙标志

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
void CAN_ModeInit(void); 								// CAN初始化
 
u8 CAN1_Send_Msg(u8* msg,u8 len); 			// 发送数据
void CAN1_SenDat(u8 * dat,u8 len);
u8 CAN1_Receive_Msg(u8 *buf); 					// 接收数据
u8 CAN1_RecDat(void);

u8 CAN1_Send_Dat(u16 addr, u8 len, char* msg);
#endif

















