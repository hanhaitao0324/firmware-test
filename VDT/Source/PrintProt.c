/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    PrintProt.c
 *      Purpose: VDT Print 
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/

#include "osObjects.h"                      // RTOS object definitions
#include "utils.h"

extern char buf[];
	
//#ifndef PRINT40
//		if(ch==0x04) {
//			printhead();
//			sprintf(buf,"          Nominal:  %3d mbar      Testtime: %2bdh %2bdmin %2bdsec\r\n\n",dru*10,ustu,umin,usek);
//			centout(buf);
//			centout(    "          ------------------------------------------------------------------\r\n\n");
//		}
//		if(ch==0x06){
//			centout("          Run time \r\n");	
//			sprintf(buf,"            %2bd:%02bd:%02bd      %3umbar \n\r",stut,mint,sekt,dru);
//			centout(buf);
//			centout("          ------------------------------------------------------------------\r\n\n");
//			centout("          Test time\n\r");
//			stus=mins=seks=0;
//			
//		}
//		if(ch==0x08){
//			sprintf(buf,"            %2bd:%02bd:%02bd      %3umbar \n\r",stus,mins,seks,dru);
//			centout(buf);
//			sprintf(buf,"\r\n          Test completed  (Total run time %2bd:%02bd:%02bd)\n\r",stut,mint,sekt);
//			centout(buf);
//		}
//		if(ch==0x03){
//			sprintf(buf,"            %2bd:%02bd:%02bd      %3umbar \n\r",stus,mins,seks,dru);
//			centout(buf);
//		}
//		if(ch==0x05) {
//			sprintf(buf,"\r\n                          Test aborted at %2bd:%02bd:%02bd \n\r",stut,mint,sekt);
//			centout(buf);
//		} 
//#else
//		if(ch==0x04) {
//			printhead();
//			sprintf(buf,"Nominal:%3dmbar Testtime:%2bdh %2bdmin %2bdsec\r\n\n",dru*10,ustu,umin,usek);
//			centout(buf);
//			centout(    "----------------------------------------\r\n\n");
//		}
//		if(ch==0x06){
//			centout("Run time \r\n");	
//			sprintf(buf,"%2bd:%02bd:%02bd      %3umbar \n\r",stut,mint,sekt,dru);
//			centout(buf);
//			centout(    "-----------------------------------------\r\n\n");
//			centout("Test time\n\r");
//			stus=mins=seks=0;
//			
//		}
//		if(ch==0x08){
//			sprintf(buf,"%2bd:%02bd:%02bd      %3umbar \n\r",stus,mins,seks,dru);
//			centout(buf);
//			sprintf(buf,"\r\nTest completed(Total run time %2bd:%02bd:%02bd)\n\r",stut,mint,sekt);
//			centout(buf);
//		}
//		if(ch==0x03){
//			sprintf(buf,"%2bd:%02bd:%02bd      %3umbar \n\r",stus,mins,seks,dru);
//			centout(buf);
//		}
//		if(ch==0x05) {
//			sprintf(buf,"\r\nTest aborted at %2bd:%02bd:%02bd \n\r",stut,mint,sekt);
//			centout(buf);
//		} 
//#endif

//		if(ch==0x01) {
//			hil=rs232in();
//			dru=hil*256;
//		}
//		if(ch==0x02) {
//			hil=rs232in();
//			dru+=hil;
//		}
//		if(ch==0x07) {
//			ustu=rs232in();
//			umin=rs232in();
//			usek=rs232in();
//		}
//		if(ch==0x09) {
//			stu=rs232in();
//			min=rs232in();
//			tag=rs232in();
//			mon=rs232in();
//			jah=rs232in();
//			settime();
//		}

#include "stdio.h"
void printhead(void){
	unsigned char tim[10],dat[10];

	gettimstr((char *) tim);
	getdatstr((char *) dat);
#ifndef PRINT40
	sprintf(buf,"              ERWEKA VDT    Testprotocol %s %s \r\n",dat,tim);
	centout(buf);
	centout(    "              -----------------------------------------\r\n\n");
#else
	sprintf(buf,"ERWEKA VDT Protocol %s %s \r\n",dat,tim);
	centout(buf);
	centout(    "----------------------------------------\r\n\n");
#endif
	timcou=0;
	sekt=0;
	mint=0;
	stut=0;
}

