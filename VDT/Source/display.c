/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    beeper.c
 *      Purpose: Piezo beeper program
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "display.h"

unsigned char dp1,dp2,dp3,dp4,dp10,dp20,dp30,dp40;
unsigned char anzpoi;

void anzrefresh(void)
{
	//spi_send(anzdigh[anzpoi]);
	//spi_send(anzdigl[anzpoi]);
	//spi_send(anzdat[anzpoi]);
	
//	ANZ_LE=ANZ_LE_H;
//	ANZ_LE=0;
	
	if(anzpoi++>14) anzpoi=0;	

}

void anzout1(char *str)
{
	unsigned char i;
	
	for(i=0;i<4;i++) {
		if(str[i]<0x30) {
			anzdat[2*i]=0x00;
			anzdat[(2*i)+1]=0x00;
		}
		else {	
			anzdat[2*i]=font[(str[i] - 0x30)] >> 8;
			if((i==0 && dp10) || (i==1 && dp20) || (i==2 && dp30) || (i==3 && dp40)) 
				anzdat[(2*i)+1] = (font[(str[i] - 0x30)]) | 0x80;
			else
				anzdat[(2*i)+1]=font[(str[i] - 0x30)];
		}	
	}
}
void anzout2(char *str)
{
	unsigned char i;
	
	for(i=0;i<4;i++) {
		if(str[i]<0x30) {
			anzdat[(2*i)+8]=0x00;
			anzdat[(2*i)+9]=0x00;
		}
		else {	
			anzdat[(2*i)+8]=font[(str[i] - 0x30)] >> 8;
			if((i==0 && dp1) || (i==1 && dp2) || (i==2 && dp3) || (i==3 && dp4)) 
				anzdat[(2*i)+9] = (font[(str[i] - 0x30)]) | 0x80;
			else
				anzdat[(2*i)+9]=font[(str[i] - 0x30)];
		}
	}
}
