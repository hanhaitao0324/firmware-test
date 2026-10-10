#ifndef __MYIIC_H
#define __MYIIC_H
#include "system.h" 
#include "stm32f4xx_conf.h"

//IO方向设置
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

//#define SDA_IN()  {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=0<<3*2;} 	// PC3输入模式
//#define SDA_OUT() {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=1<<3*2;} 	// PC3输出模式

//IO操作函数	 
//#define IIC_SCL    PBout(8) //SCL
//#define IIC_SDA    PBout(9) //SDA	 
//#define READ_SDA   PBin(9)  //输入SDA 

//#define IIC_SCL    PBout(2) 						// SCL
//#define IIC_SDA    PBout(3) 						// SDA	 
//#define READ_SDA   PBin(3) 							// 输入SDA 

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

//IIC所有操作函数
void IIC_Init(void); 										// 初始化IIC的IO口				 
void IIC_Start(void); 									// 发送IIC开始信号
void IIC_Stop(void); 										// 发送IIC停止信号
void IIC_SendByte(u8 txd); 							// IIC发送一个字节
u8 IIC_ReadByte(void); 									// IIC读取一个字节
u8 IIC_WaitAck(void); 									// IIC等待ACK信号
void IIC_Ack(void); 										// IIC发送ACK信号
void IIC_NAck(void); 										// IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















