#ifndef _EEPROM_H
#define _EEPROM_H

#include "defines.h"
#include "const.h"
#include "mcp3201.h"
#include "system.h"
#include "timedate.h"

extern bool EEPROM_WriteVar(void);
extern void EEPROM_Restore(void);
extern void EEPROM_LoadDefault(void);
extern bool EEPROM_CheckIntegrity(void);
extern void EEPROM_Check(void);
unsigned char readeeprom(unsigned int address);
void writinteep(unsigned int val,unsigned int adr);
void InitEEPROM(unsigned char mode);
unsigned int readinteep(unsigned int adr);
void writeeeprom(unsigned char val, unsigned int address);
//void loaddouble(double *num,unsigned int adr);
//void savedouble(double *num,unsigned int adr);
void LoadDate(DTIM *dt,unsigned int adr);
void WriteDate(DTIM *dt,unsigned int adr);
void loadfloat(float *num,unsigned int adr);
void savefloat(float *num,unsigned int adr);
void writeepbuf(unsigned char *val, unsigned int address,unsigned int length);
void readeepbuf(unsigned char *val, unsigned int address, unsigned int length);
#endif
