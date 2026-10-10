#ifndef _AD7190_H_
#define _AD7190_H_

#include "rtc.h"
//#include "sys.h"
#include "weight_manage.h"

#define V_Ref 5 		// ²Î¿¼µçÑ¹


extern long int DisordersData;
extern u8 ADC_Gain;
////0
////1			PBout(3)		SPI_SCK
////2			PBout(5)		SPI_SDO			DIN
////3			PBin(4)			SPI_DOUT		DOUT
////4			PBout(6)		SPI_CS
/*#define SPI_DOUT 		PBin(4)
#define GP0DAT 			PBin(4)
#define SPI_SCK  		PBout(3)
#define SPI_CS 			PBout(6)
#define SPI_SDO 		PBout(5)

#define SET_CS()		SPI_CS=1	//P4.3->/CS
#define CLR_CS()		SPI_CS=0

#define	SET_SCL()		SPI_SCK=1	//P4.4->SCLK
#define	CLR_SCL()		SPI_SCK=0

#define SET_SDO()		SPI_SDO=1	//P4.5->DIN
#define CLR_SDO()		SPI_SDO=0
*/

#define SPI_DOUT 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define GP0DAT 			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)

#define SET_CS()		GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define CLR_CS()		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define	SET_SCL()		GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define	CLR_SCL()		GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define SET_SDO()		GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define CLR_SDO()		GPIO_ResetBits(GPIOB,GPIO_Pin_5)


/*
COMMUNICATIONS REGISTER :
CR7   	CR6   	CR5   		CR4   	CR3   	CR2   		CR1   CR0  
WEN(0)   R/W(0)   RS2(0)   RS1(0)   RS0(0)   CREAD(0)   0(0)   0(0)  


RS2   RS1   RS0   Register   							Register 
0   	0   0   	Communications register during a write operation   	8 bits  
0   	0   0   	Status register during a read operation   		8 bits 
0   	0   1  	 	Mode register   					24 bits  
0   	1   0   	Configuration register   				24 bits  
0   	1   1   	Data register/data register plus status information  	24 bits/32
1   	0   0   	ID register   						8 bits  
1   	0   1   	GPOCON register   					8 bits  
1   	1   0   	Offset register   					24 bits  
1   	1   1   	Full-scale register   					24 bits  

STATUS REGISTER :

SR7   		SR6   	SR5   		SR4   	SR3   	SR2   	SR1   		SR0  
RDY(1)   ERR(0)   NOREF(0)   Parity(0)   0(0)  CHD2(0)  CHD1(0)   CHD0(0)  
*/
/*----------------------------------------------------------------------
  File Name          : AD7190_IO.h 
  Author             : Neil Zhao - CAST
  Version            : V1.0
  Date               : 20/2/2009
  Description        : AD7190 Driver Declaration

  The AD7190 is a low noise, complete analog front end for high
  precision measurement applications.

  China Applications Support Team.
  china.support@analog.com

----------------------------------------------------------------------

  The present firmware which is for guidance only aims at providing
  customers with coding information regarding their products in order
  for them to save time.  As a result, Analog Devices shall not be
  held liable for any direct, indirect or consequential damages with
  respect to any claims arising from the content of such firmware and/or
  the use made by customers of the coding information contained herein
  in connection with their products.

----------------------------------------------------------------------*/

//---------------------------------
//void WriteToAD7190(unsigned char count,unsigned char *buf);
//---------------------------------
//Function that writes to the AD7190 via the SPI port. 
//--------------------------------------------------------------------------------
//void WaitDataRDY(void);
u8 WaitDataRDY(void);
void WriteToAD7190(unsigned char count, unsigned char *buf);

//---------------------------------
//void ReadFromAD7190(unsigned char count,unsigned char *buf)
//---------------------------------
//Function that reads from the AD7190 via the SPI port. 
//--------------------------------------------------------------------------------
void ReadFromAD7190(unsigned char count, unsigned char *buf);
unsigned long GET_AD7190(void);
void ad7190_Init(void);
//u32 zhongliang(void);
float ADC_Votage(int n);
u32 ADC_Num(void);
u32 Get_ADC_AvrgNum(int n);
extern u8 ADC_Channel;

#endif


