#include "utils.h"
#include <math.h>
#include "system.h"
#include "stdio.h"
#include "timedate.h"

extern void AKC_RTCGetDate(DTIM *tim);
extern unsigned int DatFormat;

void getdatstr(char *dbu)
{
 	DTIM dat;

	AKC_RTCGetDate(&dat);
	if(DatFormat==0)
		sprintf(dbu,"%02d.%02d.%d",dat.Day,dat.Month,dat.Year);
	else
		sprintf(dbu,"%02d/%02d/%d",dat.Month,dat.Day,dat.Year);
}


void gettimstr(char *tbu) 
{
	DTIM tim;

	AKC_RTCGetDate(&tim);
	sprintf(tbu,"%02d:%02d",tim.Hour,tim.Minute);
}

//------------------------------------------------------------------------------
// UTILS_ToHexDec - convert number to DEC
//------------------------------------------------------------------------------
uint16_t UTILS_ToHexDec(uint8_t num)
{
  uint16_t result;
  
  if (num>99) return 0;
  result = ((uint16_t)(num/10)<<8) + num%10;
  
  return result;
}
// Reverse the order of bits within a byte.
// Returns: The reversed byte value.
uint8_t ReverseBits(uint8_t c)
{
  uint8_t result = 0;
  uint8_t i;
  
  for (i=0; i<8; ++i)
  {
    result = result<<1;
    result |= (c&1);
    c = c>>1;
  }
  
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

signed short CalculateRMS(signed short *inbuf, int anz)
{
	unsigned long long sumbuf=0;
	signed short ret;
	int i;
	
	for(i=0;i<anz;i++) 
		sumbuf+= inbuf[i] * inbuf[i];
	sumbuf/=anz;
	ret=sqrt(sumbuf);	
	return(ret);
}

//------------------------------------------------------------------------------
// UTILS_Median3Filter - return median value for 3 values
//------------------------------------------------------------------------------
uint16_t UTILS_Median3Filter(uint16_t a, uint16_t b, uint16_t c)
{
  int Temp = 0;
  
  if (a>b)
    Temp += 4;
  if (a>c)
    Temp += 2;
  if (b>c)
    Temp += 1;
  
  switch (Temp)
  {
    case 0:
    case 7:
      return b;
    case 3:
    case 4:
      return a;
    default:
      return c;
  }
}

//------------------------------------------------------------------------------
// UTILS_Median5Filter - return median value for 5 values
//------------------------------------------------------------------------------
uint16_t UTILS_Median5Filter(uint16_t *Values)
{
  int i, j;
  uint16_t TempVal;
  
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4 - i; j++)
    {
      if (Values[j] > Values[j + 1])
      {
        TempVal = Values[j];
        Values[j] = Values[j + 1];
        Values[j + 1] = TempVal;
      }  
    }
  }
  
  return Values[2];
}

//------------------------------------------------------------------------------
// UTILS_MinValue - get buffer minimal value
//------------------------------------------------------------------------------
uint8_t UTILS_MinValue(uint8_t *Buff, uint16_t Len)
{
  uint16_t i;
  uint8_t Min = 0xFF;
  
  if (!Len)
    return 0;
  
  for (i=0; i<Len; i++)
  {
    if (*Buff++<Min)
      Min = *Buff;
    Buff++;
  }  
  
  return Min;
}

void Int2Str(uint8_t* str, S32 intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}


uint32_t Str2Int(uint8_t *inputstr, S32 *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                res = 1; /* return 1; */
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }

        if (i >= 11) res = 0; /* over 8 digit hex --invalid */
    }
    else /* max 10-digit decimal input */
    {
        for (i = 0;i < 11;i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {

                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
                val = val * 10 + CONVERTDEC(inputstr[i]);
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11) res = 0; /* Over 10 digit decimal --invalid */
    }

    return res;
}

uint32_t SerialKeyPressed(uint8_t *key)

{
//	int ch;

//    if ((ch=com_getchar1())!=-1) {
//        *key =(U8) ch;
//        return(1);
//    }
    return(0);
}

//// Change the data to little? endian for VNC2.
//void to_little_endian(){
//	for (j=0; j < PAGESIZE; j += 2)
//	{
//		endianBuf[j] = databuf[j+1];
//		endianBuf[j+1] = databuf[j];
//	}
//}
