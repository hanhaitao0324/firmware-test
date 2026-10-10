// FRAM Memory
#ifndef	_FRAM_H
#define	_FRAM_H

#include "defines.h"
#include "const.h"
#include "timedate.h"

//------------------------------------------------------------------------------
#define FRAM_WRITE_EN		(0x06)
#define FRAM_WRITE		(0x02)
#define FRAM_READ		(0x03)

//------------------------------------------------------------------------------
enum OPERATION_TYPES
{
  MEM_NOOP,
  MEM_READ,
  MEM_WRITE,
  MEM_READ_HEADER,
  MEM_WRITE_HEADER
};

typedef struct 
{
  TWord Number;        	// Number of records
  TWord Interval;         // Report interval in seconds
  TWord Speed;            // Test speed
  TReal32 Temp;           // Test temperature
  TDWord Time;            // Test time
  TWord MinSpeed;         // Minimal speed during the test
  TWord MaxSpeed;         // Maximal speed during the test
  TWord MinTemp;          // Minimal temperature during the test
  TWord MaxTemp;          // Maximal temperature during the test
  TByte Method;           // Test method
  TByte Volume;           // Test volume
  TByte Head;             // Head mode
  TsWord TempOffset;      // Temperature offset for current test
  TDWord RunTime;         // Total test runtime
  TByte Status;           // Status of report
  //TByte Multiplex;        // Multiplex to become more results
  TTimeDate StartTime;    // Starting time
} TFHeader;

typedef struct 
{
  TWord MinTemp;		// Temperature*10
  TWord MaxTemp;
  TWord MinSpeed;		// Rotation speed*10
  TWord MaxSpeed;
  //TWord SampT[MAX_SENSORS_NUM];
} TFReport;

typedef struct 
{
  TByte Type;
  TWord Number;
} TFOperation;

extern TFHeader FHeader;
extern TFReport FReport;

//------------------------------------------------------------------------------
#define FRAM_LENGTH		(0x8000)	// FRAM memory length
#define FRAM_ADDR               (0x0000)	// FRAM starting address

#define MAX_REPORT_NUMBER	(4000L)		// maximal number of reports

#define FRAM_HEADER_SIZE	(sizeof(TFHeader))  //header structure size

#define FRAM_REPORT_SIZE	(sizeof(TFReport))  //single-point report structure size

//------------------------------------------------------------------------------

extern volatile TFOperation FRAM_Operate;

//FRAM memory initialization
extern TBool FRAM_Init();        			
//FRAM interrupt for actions
extern void FRAM_IntHandler(void);
//do something with FRAM
extern void FRAM_Do(TByte Type, TWord Number);
extern TBool FRAM_GetRecord(TWord Number);

#endif
