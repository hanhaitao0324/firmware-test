/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    vinculum.c
 *      Purpose: Vinculum Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx_conf.h"   							// Device header
#include "defines.h"
#include "system.h"
#include "eeprom.h"
#include "w25qxx.h"
#include "usart.h"
#include "string.h"
#include "ymodem.h"
#include "vinculum.h"
#include <stdio.h>
#include <stdlib.h>
#include "beeper.h"
#include "updater.h"
#include "can.h"
#include "version.h"
#include "motor.h"

char ClearBuf[20];
unsigned int eepadr,kraftanst;
char buf[1500],id[25],buf1[50];
bool RemoteFlg = false;
bool BusyFlg = false;
bool LicFree = false;

//extern unsigned int DaysLeft,FullDays,Periods,AktPeriod;
unsigned int DaysLeft,FullDays,Periods,AktPeriod;
extern int YModemAktiv;
extern char RS232_RxBuffer[];
extern bool MartelMode;

//extern void InitLicManagement(void);
extern long FCode(char *ID,int art);

extern u8 UsbInUse; 										// USB占用标志, Gavin, 2023-08-10 周四 15:08:54

extern int MDIANUM;
extern int MHARNUM;
extern u8 PopWindow;

void LCD_ShowSpStr(u16 x,u16 y,u16 width,u16 height,u16 pcolor,u16 bcolor,u8 size,char *p,u8 mode);
void LCD_ShowIcon(u16 x,u16 y,u16 width,u16 height,u16 pcolor,u16 bcolor,u8 p,u8 mode);
void LCD_Clear(uint16_t color);

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
	float tempf;
	
	BusyFlg=true;
	if(strcmp(RS232_RxBuffer,"UPFW")==0) 						// 主控板软件刷新
	{            /*Firmware update*/
		BEEPER_PlayTones(tones_1khz);	
	  	osDelay(500);
	
		PopWindow = 10;
		LCD_Clear(0xffff);
		delay_ms(100);
		LCD_ShowSpStr(430,100,300,50,0x8430,0xffff,48,"Update",0);
		delay_ms(100);
		
		sprintf(buf1,"Current Version  v%s",MAIN_FIRMWARE_VERSION);
		LCD_ShowSpStr(390,240,500,40,0x0000,0xffff,24,buf1,0);
		delay_ms(10);
		
		//LCD_ShowSpStr(390,270,500,40,0x0000,0xffff,24,"Target Version   v3.0.0",0);
		//delay_ms(10);
		LCD_ShowSpStr(350,340,500,40,0x253a,0xffff,24,"Update in progress... Please wait...",0);
		delay_ms(10);
		LCD_ShowIcon(20,535,235,48,0x8430,0xffff,10,0);

		StartBootloader();
		return;
	}
	if(strcmp(RS232_RxBuffer,"IBFW")==0) 						// 显示触控板软件刷新
	{
		if(CurntPage == 0) 									// 只有在首页时才能升级液晶屏程序
		{
			UpdataFlag = 1;
			
			BEEPER_PlayTones(tones_1khz);	
			CAN1_Send_Dat(255,4,"IBFW");
		 	osDelay(500);
			UPDATER_BusRun(DEVICE_ID_TFT);
			return;
		}
	}
	if(strcmp(RS232_RxBuffer,"ST")==0) 
	{              /*Abfrage Messmethode*/
		RS232_SendString("ST 00\r");
		BEEPER_PlayTones(tones_error);	
		return;
	}
	if(strcmp(RS232_RxBuffer,"HVS")==0) 
	{
		for(i=0;i<5;i++)
			buf1[i]=DEVICE_VERSION[i];
		buf1[5]=0;
		sprintf(buf,"HVS: %4s%s\r", buf1, DEVICE_BETAVERSION);
		RS232_SendString(buf);
		BEEPER_PlayTones(tones_3beep);	
		return;
	}
	if(strcmp(RS232_RxBuffer,"CEETEST")==0) 					// 清除Flash中的测试报告
	{
		RS232_SendString("CEETEST: wait...\r");
		for(i = 2048;i<2048+88;i++)
		{
			W25QXX_Erase_Sector(i);
		}

		RS232_SendString("CEETEST: Clear report finished.\r");
		BEEPER_PlayTones(tones_3beep);
		return;
	}
	if(strcmp(RS232_RxBuffer,"CEEADJU")==0) 					// 清除Flash中的调整和校正报告
	{
		RS232_SendString("CEEADJU: wait...\r");
		for(i = 2144;i<2144+148;i++)
		{
			W25QXX_Erase_Sector(i);
		}

		RS232_SendString("CEEADJU: Clear report finished.\r");
		BEEPER_PlayTones(tones_3beep);
		return;
	}	
	if(strcmp(RS232_RxBuffer,"CEEPR")==0) 					// 清除Flash中的报告
	{
		RS232_SendString("CEEPR: wait...\r");
		for(i = 2048;i<2048+244;i++)
		{
			W25QXX_Erase_Sector(i);
		}

		RS232_SendString("CEEPR: Clear report finished.\r");
		BEEPER_PlayTones(tones_3beep);
		return;
	}	
	if(strcmp(RS232_RxBuffer,"CEEUSER")==0) 					// 清除用户列表
	{
		sprintf(buf1,"%d",User_Num);
		RS232_SendString(buf1);
		RS232_SendString("CEEUSER: wait...\r");
		memset(&User_datatemp1,0,sizeof(User_datatemp1));
		for(i = 0;i<103;i++)
		{
			writeepbuf((unsigned char *)&User_datatemp1, ADR_User_admin+i*UserData_LongAct, UserData_Long);
		}
		
		User_Num = 3;
		writeeeprom(User_Num, ADRUserNum);

		RS232_SendString("CEEUSER: Clear user finished.\r");
		BEEPER_PlayTones(tones_3beep);
		return;
	}
	if(strcmp(RS232_RxBuffer,"GID")==0) 						// 获取激活码的ID
	{
		sprintf(buf, "GID: %010ld\r", M407ID);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"GHTVER")==0) 					// 获取触摸板硬件版本号
	{
		strcpy(buf1,TOUCH_HARDWARE_VERSION);
		sprintf(buf, "GHTVER: %s\r", buf1);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SHTVER ",7)==0) 			// 设置触摸板硬件版本号
	{
		strcpy(buf1,&RS232_RxBuffer[7]);
		pos = strlen(buf1);
		if (pos>HVER_MAX_LEN)
			pos = HVER_MAX_LEN;
		buf[pos] = '\0';
		
		CAN1_Send_Dat(252,strlen(buf1)+1,buf1);
		
		strcpy(TOUCH_HARDWARE_VERSION,buf1);
		sprintf(buf, "SHTVER: %s\r", buf1);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"GHMVER")==0) 					// 获取主控板硬件版本号
	{
		strcpy(buf1,MAIN_HARDWARE_VERSION);
		sprintf(buf, "GHMVER: %s\r", buf1);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SHMVER ",7)==0) 			// 设置主控板硬件版本号
	{
		strcpy(buf1,&RS232_RxBuffer[7]);
		pos = strlen(buf1);
		if (pos>HVER_MAX_LEN)
			pos = HVER_MAX_LEN;
		buf[pos] = '\0';
		
		writeepbuf((unsigned char *)buf1, ADR_HTVER, HVER_MAX_LEN);
		strcpy(MAIN_HARDWARE_VERSION,buf1);
		sprintf(buf, "SHMVER: %s\r", buf1);
		RS232_SendString(buf);
		return;
	}
	if((strncmp(RS232_RxBuffer,"G     ",6)==0)|| 							// 电子称
		 (strncmp(RS232_RxBuffer,"N     ",6)==0))
	{
		EBalanceRxCnt++; 															// 电子称接收计数器
		
		BalanceWeight = atof(&RS232_RxBuffer[7]);
		if(RS232_RxBuffer[6] == '-')
			BalanceWeight = 0 - BalanceWeight;
		
		return;
	}
	if(strncmp(RS232_RxBuffer,"S S    ",7) == 0)
	{
		EBalanceRxCnt++; 															// 电子称接收计数器
		
		BalanceWeight = atof(&RS232_RxBuffer[7]);
		
		return;
	}
	if(strncmp(RS232_RxBuffer,"SerNo.    ",10)==0)
	{
		strncpy(BalanceSn,&RS232_RxBuffer[10],10);
		BalanceSn[10] = '\0';
		return;
	}
	if(strcmp(RS232_RxBuffer,"GI")==0) 
	{
		//getid(id);
		sprintf(buf,"GI: %s\r",id);
		RS232_SendString(buf);
		BEEPER_PlayTones(tones_startup);	
		return;
	}
	if(strcmp(RS232_RxBuffer,"VINCPROG")==0) 
	{
		UsbInUse = 1; 											// USB占用标志置位，Gavin，2023-08-10 周四 15:30:34
		Ymodem_Receive((unsigned char *)buf);
		VINC_Reset();
		osDelay(3000);
		BEEPER_PlayTones(tones_1khz);	
		sprintf(buf,"VINCPROG\r");
		RS232_SendString(buf);
		UsbInUse = 0; 											// USB占用标志复位，Gavin，2023-08-10 周四 15:30:16
		return;
	}			
	if(strcmp(RS232_RxBuffer,"VINCRESET")==0) 
	{
		VINC_Reset();
		RS232_SendString("VINCRESET\r");
		return;
	}
	if(strncmp(RS232_RxBuffer,"VS ",3)==0) 
	{
		Vinc_Send(&RS232_RxBuffer[3]);
		osDelay(100);
		Vinc_GetBuf(buf);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"VINCEXT")==0) 
	{
		Vinc_Extended();
		RS232_SendString("VINCEXT\r");
		return;
	}
	if(strncmp(RS232_RxBuffer,"VINCPORT ",9)==0) 
	{
		sprintf(buf,"VINCPORT: %02X\r",Vinc_Query(RS232_RxBuffer[9] & 0x0F));
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"VINCVERS",9)==0) 
	{
		Vinc_Vers(buf1);
		sprintf(buf,"VINCVERS: %s\r",buf1);
		RS232_SendString(buf);
		BEEPER_PlayTones(tones_1khz);	
	  return;
	}
	if(strncmp(RS232_RxBuffer,"VINCDIR",9)==0) 
	{
		Vinc_Dir(buf1);
		sprintf(buf,"VINCDIR: %s\r",buf1);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"VINCQD")==0) 
	{
		for(j=0;j<16;j++) {
			Vinc_QD_Old(j,buf1);
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
	if(strcmp(RS232_RxBuffer,"HSNR")==0) 						// Holen Seriennummer
	{
		readeepbuf((unsigned char *)DeviceSerialNb, ADRSERNUM, 6); 				// 设???序列号
		DeviceSerialNb[6] = '\0';
	  	sprintf(buf1, "HSNR: %s\r", DeviceSerialNb);
		RS232_SendString(buf1);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SSNR ",5)==0) 				// Schreiben Seriennummer
	{
		strncpy(buf1,&RS232_RxBuffer[5], SERIAL_NUM_MAX_LEN);
		pos = strlen(buf1);
		if (pos>SERIAL_NUM_MAX_LEN)	pos = SERIAL_NUM_MAX_LEN;
		buf1[pos] = '\0';
		strcpy(DeviceSerialNb,buf1);
		for(i=0;i<6;i++) writeeeprom(buf1[i],ADRSERNUM+i);
		RS232_SendString(RS232_RxBuffer);
		return;
	}
	if(strcmp(RS232_RxBuffer,"EP")==0) 
	{	          			/*FW Version senden*/
		sprintf(buf,"EPROM %c.%c%c.%c%s\r",DEVICE_VERSION[0], DEVICE_VERSION[1], DEVICE_VERSION[2], DEVICE_VERSION[3], DEVICE_BETAVERSION);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"EPSONDER")==0) 
	{          	/*FW Sonder Version senden*/
		for(i=0;i<3;i++) buf1[i]=DEVICE_SONDERVERSION[i];
		buf1[3]=0;
		sprintf(buf,"EPSONDER %s\r",buf1);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"EPDATUM")==0) 
	{	          	/*FW Datum senden*/
		for(i=0;i<8;i++) buf1[i]=DEVICE_FWDATE[i];
		buf1[8]=0;
		sprintf(buf,"EPDATUM %s\r",buf1);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"HUPDDAT")==0) 
	{          	/*Update Datum senden*/
		//for(i=0;i<8;i++) buf1[i]=readeeprom(ADRLASTUPDATE+i);
		buf1[8]=0;
		sprintf(buf,"HUPDDAT %s\r",buf1);
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"HUPDID")==0) 
	{          	/*Update ID senden*/
		RS232_SendString("HUPDID: 0001234567\r");
		return;
	}
	if(strcmp(RS232_RxBuffer,"HVAC")==0) 
	{          	/*HVAC senden*/
		//sprintf(buf,"HVAC: %d\r",getadavrg());
		RS232_SendString(buf);
		return;
	}
	if(strcmp(RS232_RxBuffer,"VNULL")==0) 
	{          	/*VNULL senden*/
		RS232_SendString("VNULL\r");
		return;
	}
	if(strncmp(RS232_RxBuffer,"LICFREE",7)==0) 
	{			/* Freigabe License Manager */
		if(RS232_RxBuffer[7]==id[13] && RS232_RxBuffer[8]==id[12] && RS232_RxBuffer[9]==id[11] && RS232_RxBuffer[10]==id[10]) 
		{
			LicFree=1;
			RS232_SendString("OK\r");
		}
		else 
		{
			LicFree=0;
			RS232_SendString("LICFREE\r");
		}
		return;
	}
	if(LicFree==1) 
	{
		if(strcmp(RS232_RxBuffer,"HPERIODS")==0) 
		{          	/*Periods senden*/
			sprintf(buf,"HPERIODS: %d\r",Periods);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RxBuffer,"HFULLDAYS")==0) 
		{          	/*FullDays senden*/
			sprintf(buf,"HFULLDAYS: %d\r",FullDays);
			RS232_SendString(buf);
			return;
		}
		if(strncmp(RS232_RxBuffer,"SPERIODS ",9)==0) 
		{	/* write Periods */
			Periods=atoi(&RS232_RxBuffer[9]);
			//writinteep(Periods,ADRPERIODS);
			sprintf(buf,"SPERIODS: %d\r",Periods);
			RS232_SendString(buf);
			return;
		}
		if(strncmp(RS232_RxBuffer,"SFULLDAYS ",9)==0) 
		{	/* write FullDays */
			FullDays=atoi(&RS232_RxBuffer[10]);
			//writinteep(FullDays,ADRFULLDAYS);
			sprintf(buf,"SFULLDAYS: %d\r",FullDays);
			RS232_SendString(buf);
			return;
		}
		if(strcmp(RS232_RxBuffer,"LICINIT")==0) 
		{
			//InitLicManagement();
			RS232_SendString("LICINIT\r");
			return;
		}
		if(strncmp(RS232_RxBuffer,"GCOD ",5)==0) 
		{
			i=atoi(&RS232_RxBuffer[5]);
			//sprintf(buf,"GCOD: %6lu\r",FCode(id,i));
			RS232_SendString(buf);
			return;
		}
	}
	if(strcmp(RS232_RxBuffer,"PRINTTEST")==0) 
	{
		if(StartPrinter()) 
		{
			if(!MartelMode) 
			{
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
				SendPrinter("Dies ist ein Test! Dies ist ein Test!\r\n");
			}
			else 
			{
				
	//      				      com_put3ww(0x1B);
	//                    com_put3ww(0x21);
	//                    com_put3ww(0x00);
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
	if(strcmp(RS232_RxBuffer,"CERSC")==0)	//2021/11/1 星期一 16:15:40 清除SVMII设备里面保存的密码重置码
	{
		writeepbuf((unsigned char *)&ClearBuf, Password_COD_ADDR, 16);
		RS232_SendString("CERSC: Clear password reset code data finished.");
		return ;	
	}

	if(strcmp(RS232_RxBuffer,"GHDL")==0) 				// 获取灵敏度大小
	{
		sprintf(buf, "GHDL: %d\r\n", Sensitivity);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SHDL ",5)==0) 			// 设置灵敏度大小
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i < 3 || i > 20)
			return;

		Sensitivity = i;
		writinteep(Sensitivity,ADRSENSIBILITY);
		sprintf(buf, "SHDL: %d\r\n", Sensitivity);
		RS232_SendString(buf);
		return;
	}

	if(strcmp(RS232_RxBuffer,"GLTF")==0) 				// 获取接触力大小
	{
		sprintf(buf, "GLTF: %d\r\n", ContactForce);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SLTF ",5)==0) 			// 设置接触力大小
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i < 3 || i > 10)
			return;

		ContactForce = i;
		writinteep(ContactForce,ADRCONTACTFORCE);
		sprintf(buf, "SLTF: %d\r\n", ContactForce);
		RS232_SendString(buf);
		return;
	}	

	if(strcmp(RS232_RxBuffer,"GFSP")==0) 				// 获取测试座快速移动速度
	{
		sprintf(buf, "GFSP: %.2f\r\n", FastMoveSP);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SFSP ",5)==0) 			// 设置测试座快速移动速度
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		tempf = atof(buf1);
		if(tempf < 3.0f || tempf > 45.0f)
			return;

		FastMoveSP = tempf;
		savefloat(&FastMoveSP,ADRFASTMOVESP);
		sprintf(buf, "SFSP: %.2f\r\n", FastMoveSP);
		RS232_SendString(buf);
		return;
	}

	if(strncmp(RS232_RxBuffer,"MDIA ",5)==0) 			// 自动测量长度
	{
		if(CurntPage != 42)
			return;
					
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i < 0 || i > 999)
			return;

		MDIANUM = i;
		if(MDIANUM != 0)
			sprintf(buf, "MDIA START: %d\r\n", MDIANUM);
		else
			sprintf(buf, "MDIA STOP: %d\r\n", MDIANUM);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"MHAR ",5)==0) 			// 自动测量硬度
	{
		if(CurntPage != 43)
			return;

		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i < 0 || i > 999)
			return;

		MHARNUM = i;
		if(MHARNUM != 0)
			sprintf(buf, "MHAR START: %d\r\n", MHARNUM);
		else
			sprintf(buf, "MHAR STOP: %d\r\n", MHARNUM);
		RS232_SendString(buf);
		return;
	}	

	if(strcmp(RS232_RxBuffer,"GDMS")==0) 				// 获取直径测量速度
	{
		sprintf(buf, "GDMS: %.2f\r\n", DiaMeasSpeed);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SDMS ",5)==0) 			// 设置直径测量速度
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		tempf = atof(buf1);
		if(tempf < 1.0f || tempf > 3.0f)
			return;

		DiaMeasSpeed = tempf;
		savefloat(&DiaMeasSpeed,ADRDIAMEASSPEED);
		sprintf(buf, "SDMS: %.2f\r\n", DiaMeasSpeed);
		RS232_SendString(buf);
		return;
	}

	if(strcmp(RS232_RxBuffer,"GHML")==0) 				// 获取力传感器类型
	{
		sprintf(buf, "GHML: %ukg\r\n", ForceSenserType);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SHML ",5)==0) 			// 设置力传感器类型
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i != 50 && i != 100)
			return;

		if(i == 50 && LoadCellMeaRange == 1)
		{
			LoadCellMeaRange = 0;
			SenMeaRange = 500;
			writeeeprom(LoadCellMeaRange,ADRLoadCellMeaRange);
		}

		ForceSenserType = i;
		writeeeprom(ForceSenserType,ADRSenserType);
		sprintf(buf, "SHML: %ukg\r\n", ForceSenserType);
		RS232_SendString(buf);
		return;
	}	

	if(strcmp(RS232_RxBuffer,"GHMR")==0) 				// 获取传感器量程
	{
		sprintf(buf, "GHMR: %d N\r\n", SenMeaRange);
		RS232_SendString(buf);
		return;
	}
	if(strncmp(RS232_RxBuffer,"SHMR ",5)==0) 			// 设置传感器量程
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		if(i < 100 || i > ForceSenserType*10)
			return;

		SenMeaRange = i;
		//writeeeprom(ForceSenserType,ADRSenserType);
		sprintf(buf, "SHMR: %d N\r\n", SenMeaRange);
		RS232_SendString(buf);
		return;
	}

	if(strncmp(RS232_RxBuffer,"SRTC ",5)==0) 			// 设置RTC偏移量
	{
		strcpy(buf1,&RS232_RxBuffer[5]);

		i = atoi(buf1);
		RTC_ApplySmoothCalib(i);
		sprintf(buf, "SRTC: %d ppm\r\n", i);
		RS232_SendString(buf);
		return;
	}

	RS232_SendString("NA\r");
}

void RS485_Handler(void)
{
	;
}

void Balance_Handler(void)
{
	// if((strncmp(Balance_RxBuffer,"SerNo.",6)==0)||
	// 	(strncmp(Balance_RxBuffer,"I4 A ",5)==0)) 				// 电子称序列号获取
	// {
	// 	EBalanceRxCnt++; 															// 电子称接收计数器
	// 	return;
	// }
	if((strncmp(Balance_RxBuffer,"G     ",6)==0)|| 							// 电子称
		 (strncmp(Balance_RxBuffer,"N     ",6)==0))
	{
		EBalanceRxCnt++; 															// 电子称接收计数器
		
		BalanceWeight = atof(&Balance_RxBuffer[7]);
		if(Balance_RxBuffer[6] == '-')
			BalanceWeight = 0 - BalanceWeight;
		
		if(strncmp(&Balance_RxBuffer[17],"g ",2)==0)
		{
			BalanceUnit = 0;
		}
		else
		if(strncmp(&Balance_RxBuffer[17],"mg",2)==0)
		{
			BalanceUnit = 1;
		}

		return;
	}
	if(strncmp(Balance_RxBuffer,"S S    ",7) == 0)
	{
		EBalanceRxCnt++; 															// 电子称接收计数器
		
		BalanceWeight = atof(&Balance_RxBuffer[7]);
		BalanceUnit = 0;
		
		return;
	}
}


