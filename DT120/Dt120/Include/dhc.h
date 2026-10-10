#ifndef _DHC_H
  #define _DHC_H

#include "defines.h"

#define DHC_MAX_TRY           (3)

#define DHC_ID_STRING_LEN     (3)
#define DHC_PID_STRING_LEN    (14)

#define DHC_NUM_CAL_POINTS    (2)

#define DHC_MAX_TEMP_DIFF     (10.0)

enum DHC_STATUS
{
  DHC_STATUS_FREE,
  DHC_STATUS_SENDING,
  DHC_STATUS_WAITING,
  DHC_STATUS_READY
};

enum DHC_COMMAND_SET
{
  DHC_CMD_CAL_FIRST_POINT,
  DHC_CMD_CAL_SECOND_POINT,
  DHC_CMD_STANDBY,
  DHC_CMD_GET_PID,
  DHC_CMD_GET_ID,
  DHC_CMD_GET_TEMP,
  DHC_CMD_SET_TEMP,
  DHC_NUM_COMMANDS
};

enum DHC_ERROR_CODES
{
  DHC_NO_ERR,
  DHC_ERR_OK,
  DHC_ERR_WRONG_COMMAND,
  DHC_ERR_FIRST_POINT,
  DHC_ERR_SAVE_CFG,
  DHC_ERR_CHKSUM,
  DHC_ERR_SENSOR,
  DHC_NUM_ERRORS
};

extern void DHC_Init(void);
extern TByte DHC_GetStatus(void);
extern void DHC_SetStatus(TByte status);
extern void DHC_SetRequest(void);
extern void DHC_IntHandler(void);
extern TBool DHC_FirstPCal(TReal32 *temp);
extern TBool DHC_SecondPCal(TReal32 *temp);
extern TBool DHC_Standby(void);
extern TBool DHC_GetPID(void);
extern TBool DHC_GetID(void);
extern TBool DHC_GetTemp(TReal32 *temp);
extern TBool DHC_SetTemp(TReal32 *temp);

#endif
