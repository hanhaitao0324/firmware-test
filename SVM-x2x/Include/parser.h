#ifndef _PARSER_H
  #define _PARSER_H
  
#include "defines.h"

//parsed item structure
typedef struct
  {
    //string
    char __flash *itemStr;
    //format
    char __flash *inFmt;
    //output message format
    char __flash *outFmt;
    //help string
    char __flash *help;
    //needs authorization
    TBool secure;
    TBool svm223;
    TBool printerBrd;
  } TParsedItem;

typedef TParsedItem TParsedItems[];
                  
extern __flash char parserMsgPrompt[];

// Parse - parser main
extern void PARSER_Comm(void);

#endif
