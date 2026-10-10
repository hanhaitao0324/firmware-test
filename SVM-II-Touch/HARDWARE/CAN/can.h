#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

//CAN1接收RX0中断使能
//#define CAN1_RX0_INT_ENABLE	0		//0,不使能;1,使能.
#define CAN1_RX0_INT_ENABLE	1		//0,不使能;1,使能.

extern volatile u8 CanRxBuf[64][8]; 		// CAN接收数据缓存
extern volatile u8 CBRIp0; 							// CAN缓存数据处理指针
extern volatile u8 CBRIp1; 							// CAN缓存数据接收指针
extern volatile u8 CanRxSn; 						// CAN接收序列号

extern u8 CanTxBuf[8]; 			// CAN发送数据缓存

u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode);//CAN初始化
u8 CAN1_Send_Msg(u8* msg,u8 len);						//发送数据
//u8 CAN1_Receive_Msg(u8 *buf);							//接收数据

//u8 CAN1_Receive_Dat(void);
u8 CAN1_Send_Dat(u16 addr, u8 len, u8* msg);
#endif
