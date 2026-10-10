#ifndef _PROGRAMM_H
#define _PROGRAMM_H

#include "defines.h"
#include "sys.h"

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   

#define CMD_ECHO		  'E'
#define CMD_ERASE		  '0'
#define CMD_WRITE		  '1'
//#define CMD_READ    '2'
#define CMD_RESET		  'R'
#define CMD_VER 		  'V'
#define CMD_DEV     	'D'
#define CMD_ID      	'I'
#define CMD_STOP      'S'

#define ANSW_ERROR    '-'
#define ANSW_OK			  '!'
#define ANSW_UNKNOWN  '?'
#define ANSW_ECHO		  'E'

#define COMM_CONF1      0x75
#define COMM_CONF2      0x55

#define SIGNATURE_BYTE_1  	0x22	//Controller signature
#define SIGNATURE_BYTE_2  	0x01	//Controller signature
#define SIGNATURE_BYTE_3  	0x01	//Controller signature
//#define DEVICE_ID      		  34    //VDT board
#define DEVICE_ID 					40 		//SVM-MB

#define RAM_START_ADDR      0x20002000
#define BLOCK_SIZE			    256

//#define FLASH_KEY1          ((uint32_t)0x45670123)
//#define FLASH_KEY2          ((uint32_t)0xCDEF89AB)

#define CMD_ERROR     0 
#define CMD_SUCCESS   1 

#define FLASH_PAGE_SIZE     (1024)

#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//#define USER_FLASH_START    (0x08001000)
#define USER_FLASH_START    (0x08010000)
#define USER_FLASH_END      (0x08080000)

#define USER_FLASH_ADD      (0x08000000)

//#define CHECKSUM_START      (0x08000C00)
#define CHECKSUM_START      (0x0800C000)

#define BOOTLOADER_KEY1     (0x12345678)
#define BOOTLOADER_KEY2     (0x43211234)

extern void PROGRAMM_EraseFlash(void);
extern int PROGRAMM_WriteFlash(void* Src, void* Dst, int Len);
extern int PROGRAMM_CalcChecksum(void);
extern int PROGRAMM_CodeChecksum(void);
extern void PROGRAMM_ExecuteCode(void);
extern void iap_load_app(u32 appxaddr);

#endif
