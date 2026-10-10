#ifndef _UPDATER_H
#define _UPDATER_H

#include "Platform.h"
#include "defines.h"

#define CMD_ECHO		'E'
#define CMD_ERASE		'0'
#define CMD_WRITE		'1'
//#define CMD_READ        	'2'
#define CMD_RESET		'R'
#define CMD_VER 		'V'
#define CMD_DEV     		'D'
#define CMD_ID      		'I'
#define CMD_SELECT      	'S'
#define CMD_EXIT		'X'

#define ANSW_ERROR      	'-'
#define ANSW_OK			'!'
#define ANSW_UNKNOWN    	'?'
#define ANSW_ECHO		'E'

#define COMM_CONF1      	0x75
#define COMM_CONF2      	0x55

#define SIGNATURE_BYTE_1  	0x20
#define SIGNATURE_BYTE_2  	0x01
#define SIGNATURE_BYTE_3  	0x05

#define DEVICE_ID_TFT      	17	//TFT-Console
#define DEVICE_ID_DTAS		19	//DT Autosampler
#define DEVICE_ID_DTLIFTER	20	//DT Lifter
#define DEVICE_ID_PISTONPUMP    22      //Piston Pump
#define DEVICE_ID_CLEANER	24	//Cleaner (SP Pump)
#define DEVICE_ID_DHQ		23	//DHQ
#define DEVICE_ID_PRINTER       29      //Printer board

#define DEVICE_CODE_TFT		1

#define BLOCK_SIZE		256

#define CMD_SUCCESS 		0
#define CMD_ERROR		1

#define CONFIRM_CODE		0x12345678

enum CAN_COMMANDS_2
{
  CAN_COMMAND_EMPTY,
  CAN_COMMAND_ERASE,
  CAN_COMMAND_ADDRESS,
  CAN_COMMAND_WRITE,
  CAN_COMMAND_FLASH,
  CAN_COMMAND_SIGNATURE,
  CAN_COMMAND_VERSION,
  CAN_COMMAND_RESET
};

extern void UPDATER_BusRun(int Device);
extern TBool UPDATER_GetBusy(void);

#endif
