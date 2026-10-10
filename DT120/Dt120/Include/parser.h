#ifndef _PARSER_H
  #define _PARSER_H
  
#include "defines.h"

//parsed item structure
typedef struct
  {
    //string 									// 字符串
    char __flash *itemStr;
    //format 									// 输入格式
    char __flash *inFmt;
    //output message format 	// 输出格式
    char __flash *outFmt;
    //help string 						// 帮助信息
    char __flash *help;
    //needs authorization 		// 是否需要授权
    TBool secure;
  } TParsedItem;

typedef TParsedItem TParsedItems[];
                  
extern __flash char parserMsgPrompt[];

// Parse - parser main
extern void PARSER_Comm(TByte channel);

#endif
