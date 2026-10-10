#ifndef _RS485_H
#define _RS485_H

#include "defines.h"

enum RS485_IO_MODES
{
  RS485_RECEPTION,
  RS485_TRANSMISSION
};

enum RS485_STATUS
{
  RS485_STATUS_FREE,
  RS485_STATUS_SENDING,
  RS485_STATUS_WAITING,
  RS485_STATUS_READY
};

//RS-485 header
#define PKG_HEAD0	0xAA
#define PKG_HEAD1	0x55
#define PKG_HEAD2	0xA5

//package structure
enum PACKAGE_STRUCTURE
{
  PKG_OFS_ADDRESS,
  PKG_OFS_CMD_STS,
  PKG_OFS_DATA_LEN,
  PKG_OFS_DATA
};

//RS-485 master
#define RS485_ADDR_MASTER	0x00
//LED&KBD device group mask
#define RS485_ADDR_LEDKBD	0xF0	
//PRN&RTC
#define RS485_ADDR_PRNRTC	0xE0
//Ekey board
#define RS485_ADDR_EKEY         0xD8
//bootloader - main (PC is connected directly to this bootloader)
//#define RS485_ADDR_BOOTLOADER	0xC0

#define RS485_MAX_TRY           3

#define RS485_BUFFER_O_SIZE	(96)
#define RS485_BUFFER_I_SIZE	(32)

#define RS485_BAUDRATE	57600L

#define RS485_BaudRate(baud)	(XTAL/16L/baud-1)

//I/O buffer
//I/O buffers
extern TByte iBuffer[RS485_BUFFER_I_SIZE];
extern TByte oBuffer[RS485_BUFFER_O_SIZE];

//device address
extern TByte RS485_Address;

// set bus status
extern void RS485_SetStatus(TByte status);
// get bus status
extern TByte RS485_GetStatus(void);
// RS485_Init
extern void RS485_Init(void);
// RS485 stop
extern void RS485_Stop(void);
// get a byte from RS-485 if any presents
extern TBool RS485_GetByte(TByte *data);
// put a byte on RS-485
extern void RS485_PutByte2(TByte data);
// put package
//extern void RS485_PutPackage(TByte address,TByte command, TByte len,TByte *buffer);
// build package and start transmitting
extern void RS485_SendPackage(TByte address, TByte command, TByte len, TByte *buffer);

extern TByte RS485_WaitPackage(TWord timeOut);

#endif
