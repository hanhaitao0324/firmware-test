/*----------------------------------------------------------------------------
 *      Erweka - VDT
 *----------------------------------------------------------------------------
 *      Name:    eeprom.c
 *      Purpose: VDT eeprom Driver
 *----------------------------------------------------------------------------
 *      Copyright (c) 2015 erweka - All rights reserved.
 *---------------------------------------------------------------------------*/
#include <string.h>
#include <math.h>
#include "eeprom.h"
#include "crc32.h"
#include "const.h"
#include "m24xx.h"
#include "mcp3201.h"
#include "system.h"

extern int Vacuum,messZeitH,messZeitL,Penetration;
extern float VacM,VacB;
extern int tMode,NoLicense;
extern void InitLicManagement(void);

//__root __eeprom char PCBID[PCB_ID_MAX_LEN+1] @PCBID_ADDR;// = "0000000000";

/***********************************************************************/
/* AKC Wrapper functions 																						   */
/***********************************************************************/
unsigned char readeeprom(unsigned int address){
		unsigned char value;
		M24XX_ReadBuffer(address, sizeof(char), (unsigned char*)&value);
		return(value);
}
void readeepbuf(unsigned char *val, unsigned int address, unsigned int length){
		M24XX_ReadBuffer(address, length, val);
}
void writeepbuf(unsigned char *val, unsigned int address, unsigned int length){
		M24XX_BufferWrite(address, length, val);
}
void writeeeprom(unsigned char val, unsigned int address){
		M24XX_BufferWrite(address, sizeof(char), &val);
}

/***********************************************************************/
/*Integer aus EEPROM auslesen                                          */
/***********************************************************************/
unsigned int readinteep(unsigned int adr)
{
	unsigned int hil=0;

	readeepbuf((unsigned char *)&hil,adr, sizeof(int));
	return(hil);
}
/***********************************************************************/
/*Integer in EEPROM schreiben                                          */
/***********************************************************************/
void writinteep(unsigned int val,unsigned int adr){
		M24XX_BufferWrite(adr, sizeof(int), (unsigned char*)&val);
}
/*************************************************************************/
/* Float in in EEPROM schreiben                                          */
/*************************************************************************/
void savefloat(float *num,unsigned int adr)
{
	writeepbuf((unsigned char*)num,adr,sizeof(float));
}

/*************************************************************************/
/* float aus EEPROM laden                                                */
/*************************************************************************/
void loadfloat(float *num,unsigned int adr)
{
	readeepbuf((unsigned char *)num,adr,sizeof(float));
	if(!isfinite(*num)) *num=0.0;
}

/*************************************************************************/
/* Double in in EEPROM schreiben                                          */
/*************************************************************************/
//void savedouble(double *num,unsigned int adr)
//{
//	writeepbuf((unsigned char*)num,adr,sizeof(double));
//}

/*************************************************************************/
/* Double aus EEPROM laden                                                */
/*************************************************************************/
//void loaddouble(double *num,unsigned int adr)
//{	
////	readeepbuf((unsigned char *)num, adr, sizeof(double));
//	M24XXX_ReadBuffer(adr, sizeof(double), (unsigned char*)num);
//	
//	if(!isfinite(*num)) *num=0.0;
//}

/*******************************************************************************
* Function Name  : InitEEPROM
* Description    : Initialize the EEPROM values for the VDT system 
* Input          : unsigned char mode
* Output         : None
* Return         : None
*******************************************************************************/
void InitEEPROM(unsigned char mode)
{
	writeeeprom(SIGNATUR, ADRSIGNATUR);
	Platform_Data->Language = LANGUAGE_MODE_GER;
	writeeeprom(Platform_Data->Language, ADR_SPRACHE);
	messZeitH = 10;
	writinteep(messZeitH, ADRMESSZEITH);
	messZeitL = 0;
	writinteep(messZeitL, ADRMESSZEITL);
	Vacuum=500;
	writinteep(Vacuum, ADRVACUUM);
	Penetration=0;
	writinteep(Penetration, ADRPENETRATION);
	VacM=0.3;
	VacB=-160.0;
	savefloat(&VacM, ADRVACM);
	savefloat(&VacB, ADRVACB);
	Platform_Data->Exisccator_size = 150,					//default 150mm Exsiccator
	writinteep(Platform_Data->Exisccator_size, ADREXSICCATOR_SIZE);//Behälter Größe speichern
	Platform_Data->Penetration_inverse = 0;	// 0=None inverse
	writinteep(Platform_Data->Penetration_inverse, ADRPENETRATION_INVERSE);//Invertierung der Penetration Phase speichern
  //	writinteep(1, ADRAUTOPRINT);// bug penetration overwritten V2.02
	writeeeprom(1, ADRAUTOPRINT);

	tMode=0;
	writinteep(tMode, ADRTMODE);
	InitLicManagement();
}

void LoadDate(DTIM *dt,unsigned int adr)		
{
	readeepbuf((unsigned char *)dt,adr,sizeof(DTIM));
}
void WriteDate(DTIM *dt,unsigned int adr)
{
	writeepbuf((unsigned char*)dt,adr,sizeof(DTIM));
}


