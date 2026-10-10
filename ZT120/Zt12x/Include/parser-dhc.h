#ifndef _PARSER_H
#define _PARSER_H

//parsed item structure
typedef struct
  {
    //string
    char __flash *itemStr;
    //format
    char __flash *inFmt;
    //output message format
    char __flash *outFmt;
  } TParsedItem;

typedef TParsedItem TParsedItems[];

enum TComm_Status
  {
    STS_OK,
    STS_WRONG_COMMAND=2,
    STS_NOT_FIRST_POINT,
    STS_ERROR_STORE_CONFIG,
    STS_WRONG_CHKSUM,
    STS_SENSOR_NOT_CONNECTED
  };

extern void PARSER_Comm(void);

#endif

