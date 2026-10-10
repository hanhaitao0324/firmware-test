#include "utils.h"
#include "strings.h"
#include "const.h"
#include <pgmspace.h>

TWord UTILS_ToHexDec(TByte num)
{
  TWord result;
  
  if (num>99) return 0;
  result = ((TWord)(num/10)<<8) + num%10;
  
  return result;
}

/*void UTILS_TimeToStr(TDWord time, char *str, TBool point)
{
  TByte hour;
  TByte min;
    
  if (time>(DT_MAX_TIME))    
    time=DT_MAX_TIME;
    
  hour=(TByte)(time/3600);
  min=(TByte)((time%3600)/60);
  if (point)
    sprintf_P(str, timeFmt, hour, min);
  else
    sprintf_P(str, timeFmtWP, hour, min);
}*/
