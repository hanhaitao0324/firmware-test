/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    mcp3201.c
 *      Purpose: ADC Microchip
 *
 *	The device is capable of sample rates of up to
 *	100 ksps at a clock rate of 1.6 MHz.
 *  100 ksps Maximum Sampling Rate at VDD = 5V
 *  50 ksps Maximum Sampling Rate at VDD = 2.7V
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include <string.h>
#include "MCP3201.h"
#include "stm32f10x_conf.h"   							// Device header
#include "pins.h"
#include "spi.h"
#include "eeprom.h"


const MCP3201_Data_s MCP3201_Default = 
{
  0.3,  			//VacM
  -160.0,  		//VacB
	500, 				//VACUUM,
	
};

MCP3201_Data_s MCP3201;

extern osThreadId ADCTask;													/* assigned task id of task:  ADCTask */

volatile bool mcp3201_RXNE = false;
static uint16_t ADC_RXBuffer;
float VacM,VacB;
unsigned int getadavrg(void);

/*******************************************************************************
* Function Name  : set_MCP3201_default
* Description    : set the default value 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void set_MCP3201_default(void) {
  memcpy((uint8_t*)&MCP3201, (uint8_t*)&MCP3201_Default, sizeof(MCP3201_Data_s));
}

/*******************************************************************************
* Function Name  : set_VacM
* Description    : set the VacM value 
* Input          : float val
* Output         : None
* Return         : None
*******************************************************************************/
void set_VacM(float val) {
	VacM = val;
}
	/*******************************************************************************
* Function Name  : set_VacB
* Description    : set the VacB value 
* Input          : float val
* Output         : None
* Return         : None
*******************************************************************************/
void set_VacB(float val) {
		VacB = val;
}
/*******************************************************************************
* Function Name  : MCP3201_Init
* Description    : Initialize 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MCP3201_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, 	ENABLE);	/* Enable GPIO bank clock */

	// GPIO Configuration
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin   = MCP3201_CLK | MCP3201_CS ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MCP3201_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin   = MCP3201_DOUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MCP3201_PORT, &GPIO_InitStructure);
  
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CS, Bit_SET);//CS high MCP3201
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CLK, Bit_RESET);//clk lo MCP3201
	mcp3201_RXNE = false;
}
void ADC_Handler(void){
//	getadavrg();
}
/*******************************************************************************
* Function Name  : ADC_read
* Description    : Read ADC conversion
* Input          : None
* Output         : None
* Return         : uint16_t Value
*******************************************************************************/
uint16_t ADC_get_val(void)
{ 
	return ADC_RXBuffer;
}

/*******************************************************************************
* Function Name  : Clk_out
* Description    : generate one clk on GPIO Pin 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Clk_out(void){
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CLK, Bit_SET);//CLK MCP3201
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CLK, Bit_RESET);//CLK MCP3201
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
}

/*******************************************************************************
* Function Name  : MCP3201_read
* Description    : Read the ADC value 
* Input          : None
* Output         : None
* Return         : ADC value 12 Bit
*******************************************************************************/
uint16_t MCP3201_read(void)
{
	uint16_t i;
	unsigned int res;
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CS, Bit_RESET);//select MCP3201
	__nop();
	__nop();
	
	Clk_out();//sync bit
	Clk_out();//sync bit
	Clk_out();//NULL bit

	res=0;
	for(i=0;i<12;i++){
		res*=2;
		if (GPIO_ReadInputDataBit(MCP3201_PORT,MCP3201_DOUT)) res++;	
		Clk_out();
	}
	
	GPIO_WriteBit(MCP3201_PORT, MCP3201_CS, Bit_SET);//deselect MCP3201
	ADC_RXBuffer = res & 0x0FFF;									 //12Bit

	mcp3201_RXNE = true;
  return ADC_RXBuffer;
}

/*******************************************************************************
* Function Name  : getadavrg
* Description    : Read the ADC value average
* Input          : None
* Output         : None
* Return         : ADC value 12 Bit
*******************************************************************************/
unsigned int getadavrg(void)
{
	unsigned int i;
	unsigned long sum=0;

	for(i=0;i<1000;i++) {
		sum += MCP3201_read();
		osThreadYield();
	}
	sum /= 1000;
	i=(int)sum;
	return(i);
}


//float GetUein(void)
//{
//	float ret;
//	unsigned int i;
//	
//	i=ADC->DR1 & 0x03FF;
//	ret=(float) i;
//	ret /= 28.2093;
//	return(ret);

//}

unsigned int GetVac(void)
{
	unsigned int i;
	double hil;

	i=MCP3201_read();
	hil=(double)i;

	hil = (hil*VacM) + VacB;
	i=(unsigned int)hil;
	return(i);
}

unsigned int GetVacAvrg(void)
{
	unsigned int i;
	unsigned long sum=0;
	double hil;

	for(i=0;i<1000;i++) {
		sum += MCP3201_read();
		osThreadYield();
	}
	sum /= 1000;
	i=(int)sum;
	hil = (double)i;
	if(hil > 3350)
		hil = (hil*VacM) + (VacB+12);//Anpassung der Sensor Kurve über 850mBar
	else
		hil = (hil*VacM) + VacB;
	i=(unsigned int)hil;
	return(i);
}


unsigned int GetVacAvrgN(int n)
{
	unsigned int i;
	unsigned long sum=0;

	for(i=0;i<n;i++) { 
		sum += GetVac();
		osThreadYield();
	}
	sum /= n;
	i=(int)sum;
	return(i);
}
