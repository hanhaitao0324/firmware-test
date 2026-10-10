/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    vinculum.c
 *      Purpose: Vinculum Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x_conf.h"   							// Device header
#include "defines.h"
#include "system.h"
#include "eeprom.h"
#include "uart.h"
#include "string.h"
#include "ymodem.h"
#include "vinculum.h"
#include <stdio.h>
#include <stdlib.h>
#include "beeper.h"

unsigned int eepadr,kraftanst;
char buf[1500],id[25],buf1[50];
bool RemoteFlg = false;
bool BusyFlg = false;
bool LicFree = false;

extern unsigned int DaysLeft,FullDays,Periods,AktPeriod;
extern int YModemAktiv;
extern char RS232_RXBuffer[];
extern bool MartelMode;
extern char RS232_RXBuffer[];

extern void InitLicManagement(void);
extern long FCode(char *ID,int art);

/*******************************************************************************
* Function Name  : Communicate_Init
* Description    : Initialize RS232 Port Communication
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Communicate_Init(void) {	
	com_baudrate(COMMUNICATION_UART, COMMUNICATION_BAUD, HARDWARE_FLOW_CONTROL);
}
/*******************************************************************************
* Function Name  : Communicate_Handler
* Description    : Communication thread
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Communicate_Handler(void)
{
	int i,j,pos;

		BusyFlg=true;
		if(strcmp(RS232_RXBuffer,"UPFW")==0) {            /*Firmware update*/
			BEEPER_PlayTones(tones_1khz);	
		  osDelay(500);			
			StartBootloader();
			return;
		}
		if(strcmp(RS232_RXBuffer,"ST")==0) {              /*Abfrage Messmethode*/
			RS232_SendString("ST 00\r");
			return;
		}
		if(strcmp(RS232_RXBuffer,"HVS")==0) {
			for(i=0;i<5;i++)
				buf1[i]=DEVICE_VERSION[i];
			buf1[5]=0;
			sprintf(buf,"HVS: %4s%s\r", buf1, DEVICE_BETAVERSION);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"GI")==0) {
			getid(id);
			sprintf(buf,"GI: %s\r",id);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"VINCPROG")==0) {
			Ymodem_Receive((unsigned char *)buf);
			VINC_Reset();
			BEEPER_PlayTones(tones_1khz);	
			sprintf(buf,"VINCPROG\r");
			RS232_SendString(buf);
			return;
		}			
		if(strcmp(RS232_RXBuffer,"VINCRESET")==0) {
			VINC_Reset();
			RS232_SendString("VINCRESET\r");
			return;
		}
		if(strncmp(RS232_RXBuffer,"VS ",3)==0) {
			Vinc_Send(&RS232_RXBuffer[3]);
			osDelay(100);
			Vinc_GetBuf(buf);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"VINCEXT")==0) {
			Vinc_Extended();
			RS232_SendString("VINCEXT\r");
			return;
		}
		if(strncmp(RS232_RXBuffer,"VINCPORT ",9)==0) {
			sprintf(buf,"VINCPORT: %02X\r",Vinc_Query(RS232_RXBuffer[9] & 0x0F));
			RS232_SendString(buf);
			return;
		}
		if(strncmp(RS232_RXBuffer,"VINCVERS",9)==0) {
			Vinc_Vers(buf1);
			sprintf(buf,"VINCVERS: %s\r",buf1);
			RS232_SendString(buf);
			return;
		}
		if(strncmp(RS232_RXBuffer,"VINCDIR",9)==0) {
			Vinc_Dir(buf1);
			sprintf(buf,"VINCDIR: %s\r",buf1);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"VINCQD")==0) {
			for(j=0;j<16;j++) {
				Vinc_QD(j,buf1);
				sprintf(buf,"VINCQD: %02X",buf1[0]);
				RS232_SendString(buf);
				for(i=1;i<20;i++){
					sprintf(buf," %02X",buf1[i]);				
					RS232_SendString(buf);
				}
				RS232_SendString("\r");
				osDelay(20);
			}
			return;
		}
		if(strcmp(RS232_RXBuffer,"HSNR")==0) {		/* Holen Seriennummer */
			for(i=0;i<6;i++)	buf1[i]=readeeprom(ADRSERNUM+i);
			buf1[6]=0;
		  sprintf(buf, "HSNR: %s\r", buf1);
			RS232_SendString(buf);
			return;
		}
		if(strncmp(RS232_RXBuffer,"SSNR ",5)==0) {	/* Schreiben Seriennummer */
			strncpy(buf,&RS232_RXBuffer[5], SERIAL_NUM_MAX_LEN);
			pos = strlen(buf);
			if (pos>SERIAL_NUM_MAX_LEN)	pos = SERIAL_NUM_MAX_LEN;
			buf[pos] = '\0';
			for(i=0;i<6;i++) writeeeprom(buf[i],ADRSERNUM+i);
			RS232_SendString(RS232_RXBuffer);
			return;
		}

		if(strcmp(RS232_RXBuffer,"EP")==0) {	          			/*FW Version senden*/
			sprintf(buf,"EPROM %c.%c%c.%c%s\r",DEVICE_VERSION[0], DEVICE_VERSION[1], DEVICE_VERSION[2], DEVICE_VERSION[3], DEVICE_BETAVERSION);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"EPSONDER")==0) {          	/*FW Sonder Version senden*/
			for(i=0;i<3;i++) buf1[i]=DEVICE_SONDERVERSION[i];
			buf1[3]=0;
			sprintf(buf,"EPSONDER %s\r",buf1);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"EPDATUM")==0) {	          	/*FW Datum senden*/
			for(i=0;i<8;i++) buf1[i]=DEVICE_FWDATE[i];
			buf1[8]=0;
			sprintf(buf,"EPDATUM %s\r",buf1);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"HUPDDAT")==0) {          	/*Update Datum senden*/
			for(i=0;i<8;i++) buf1[i]=readeeprom(ADRLASTUPDATE+i);
			buf1[8]=0;
			sprintf(buf,"HUPDDAT %s\r",buf1);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"HUPDID")==0) {          	/*Update ID senden*/
			RS232_SendString("HUPDID: 0001234567\r");
			return;
		}
		if(strcmp(RS232_RXBuffer,"HVAC")==0) {          	/*HVAC senden*/
			sprintf(buf,"HVAC: %d\r",getadavrg());
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RXBuffer,"VNULL")==0) {          	/*VNULL senden*/
			RS232_SendString("VNULL\r");
			return;
		}
		if(strncmp(RS232_RXBuffer,"LICFREE",7)==0) {			/* Freigabe License Manager */
			if(RS232_RXBuffer[7]==id[13] && RS232_RXBuffer[8]==id[12] && RS232_RXBuffer[9]==id[11] && RS232_RXBuffer[10]==id[10]) {
				LicFree=1;
				RS232_SendString("OK\r");
			}
			else {
				LicFree=0;
				RS232_SendString("LICFREE\r");
			}
			return;
		}
		if(LicFree==1) {
			if(strcmp(RS232_RXBuffer,"HPERIODS")==0) {          	/*Periods senden*/
				sprintf(buf,"HPERIODS: %d\r",Periods);
				RS232_SendString(buf);
			return;
			}
			if(strcmp(RS232_RXBuffer,"HFULLDAYS")==0) {          	/*FullDays senden*/
				sprintf(buf,"HFULLDAYS: %d\r",FullDays);
				RS232_SendString(buf);
			return;
			}
			if(strncmp(RS232_RXBuffer,"SPERIODS ",9)==0) {	/* write Periods */
				Periods=atoi(&RS232_RXBuffer[9]);
				writinteep(Periods,ADRPERIODS);
				sprintf(buf,"SPERIODS: %d\r",Periods);
				RS232_SendString(buf);
			return;
			}
			if(strncmp(RS232_RXBuffer,"SFULLDAYS ",9)==0) {	/* write FullDays */
				FullDays=atoi(&RS232_RXBuffer[10]);
				writinteep(FullDays,ADRFULLDAYS);
				sprintf(buf,"SFULLDAYS: %d\r",FullDays);
				RS232_SendString(buf);
			return;
			}
			if(strcmp(RS232_RXBuffer,"LICINIT")==0) {
				InitLicManagement();
				RS232_SendString("LICINIT\r");
			return;
			}
			if(strncmp(RS232_RXBuffer,"GCOD ",5)==0) {
				i=atoi(&RS232_RXBuffer[5]);
				sprintf(buf,"GCOD: %6lu\r",FCode(id,i));
				RS232_SendString(buf);
			return;
			}
		}
		if(strcmp(RS232_RXBuffer,"PRINTTEST")==0) {
			if(StartPrinter()) {
                if(!MartelMode) {
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                    SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
                }
                else {
//      				      com_put2ww(0x1B);
//                    com_put2ww(0x21);
//                    com_put2ww(0x00);
//                    druckergebnisMart();
                    
//                    SendPrinter("Dies ist ein Test!\r\n");
//                    SendPrinter("Dies ist ein Test!\r\n");
//                    SendPrinter("Dies ist ein Test!\r\n");
//                    SendPrinter("Dies ist ein Test!\r\n");
//                    SendPrinter("Dies ist ein Test!\r\n");
//                    SendPrinter("Dies ist ein Test!\r\n");
                }
				StopPrinter();
				}
			return;
		}
		RS232_SendString("NA\r");
}

