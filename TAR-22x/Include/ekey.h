#ifndef _EKEY_H
  #define _EKEY_H

#include "defines.h"
#include "commands.h"
#include "timedate.h"

#define EKEY_KEY_LEN      (4)
#define EKEY_MAX_PERIOD   (15)

#define EKEY_MIN_LENGTH   (30)
#define EKEY_MAX_LENGTH   (1000)

#define EKEY_START_USAGE  (1111L)

enum EKEY_ANSWERS
{
  EKEY_ANSWER_PRINT,
  EKEY_ANSWER_OK,
  EKEY_ANSWER_ERROR
};

enum EKEY_STATUS
{
  EKEY_STATUS_OK,
  EKEY_STATUS_ALARM1,
  EKEY_STATUS_ALARM2,
  EKEY_STATUS_LOCKED,
  EKEY_STATUS_FIRST,
  EKEY_STATUS_BLOCKED,
  EKEY_STATUS_LAST
};

extern void EKEY_Menu(void);
extern void EKEY_Settings(void);
//extern TBool EKEY_CheckStatus(void);
extern TBool EKEY_Init(void);
extern TByte EKEY_Parser(char *cmd, char *answer, TByte *dataLen);
extern TByte EKEY_Check(TBool check);

#endif
