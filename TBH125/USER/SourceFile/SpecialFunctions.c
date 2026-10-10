#include "tbh125.h"

int OWTouchReset(void);
void OWWriteBit(int);
int OWReadBit(void);
int OWReadByte(void);
void OWWriteByte(int);
void tickDelay(int);
int Calc_CRC(unsigned char *,int);


unsigned char snbu[20];
int A=15,B=161,C=151,D=25,E=23,F=138,G=0,H=1206,I=176,J=1030;

void tickDelay(int val)  //1.13us
{
	int i,j;
	for(i=0;i<val;i++) {
		for(j=0;j<42;j++);
	}
}


int OWTouchReset(void)
{
	int result;
	OW_PIN_OUT();
	tickDelay(G);
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_RESET);

	tickDelay(600);
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);

	OW_PIN_IN();

	tickDelay(50);

	result = GPIO_ReadInputDataBit(GPIOC,DS2401_DAT);
  OW_PIN_OUT();
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);
	tickDelay(500);
	return(result);
}

void OWWriteBit(int bit)
{

	if(bit){
		GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_RESET);
		tickDelay(10);
		GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);
		tickDelay(85);
	}
	else {
		GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_RESET);
		tickDelay(85);
		GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);
		tickDelay(13);
	}
}

int OWReadBit(void)
{
	int result;
	
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_RESET);	
	tickDelay(10);
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);
	OW_PIN_IN();
	tickDelay(1);
	result = GPIO_ReadInputDataBit(GPIOC,DS2401_DAT);
	OW_PIN_OUT();
	GPIO_WriteBit(GPIOC, DS2401_DAT, Bit_SET);
	tickDelay(85);
	return(result);
}	

void OWWriteByte(int data)
{
	int loop;

	for(loop=0;loop<8;loop++) {
		OWWriteBit(data & 0x01);
		data >>= 1;
	}
}

int OWReadByte(void)
{
	int loop,result=0;

	for(loop=0;loop<8;loop++)
	{
		result>>=1;
		if(OWReadBit()) 
			result |= 0x80;
	}
	return(result);
}

void getid(volatile char *buf)
{
	unsigned char i,n;
	unsigned long erg;
	n=0;
	strcpy((char *)buf,"000000000000");
	tsk_lock();

	do {
		if(!OWTouchReset()) {
			OWWriteByte(0x33);
			for(i=0;i<8;i++)
				snbu[i]=OWReadByte();

			erg=0;
			if(snbu[7]==Calc_CRC(snbu,7)) 
			{
				for(i=0;i<6;i++) 
				{
					erg+=snbu[i+1] * pow(2,i*8);
				}
				sprintf((char *)buf,"%014ld",erg);
				break;
			}
		}
		n++;
	}while(n<10);
	tsk_unlock();
}

	
int Calc_CRC(unsigned char *buff,int num_vals)
{
   unsigned char Shift_Reg, SR_lsb, Data_Bit, v;
   unsigned char FB_bit;
   int i, j; 

   Shift_Reg = 0;

   for(i = 0;i< num_vals;i++) 
	 {
			v = buff[i];
			for(j=0;j<8;j++) 
			{
         Data_Bit = v &  0x01;     							
         SR_lsb = Shift_Reg & 0x01;
         FB_bit =(Data_Bit ^ SR_lsb) & 0x01;  				
         Shift_Reg >>= 1;			 						
         if (FB_bit == 1) 
					Shift_Reg = Shift_Reg ^ 0x8C;
         v >>= 1;											
			}     
   }
   return(Shift_Reg);

}
