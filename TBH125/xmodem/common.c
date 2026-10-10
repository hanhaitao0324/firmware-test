#include "common.h"
#include "tbh125.h"


void Int2Str(u8* str, s32 intnum)
{
    u32 i, Div = 1000000000, j = 0, Status = 0;

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


u32 Str2Int(u8 *inputstr, s32 *intnum)
{
    u32 i = 0, res = 0;
    u32 val = 0;

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




u32 SerialKeyPressed(u8 *key)

{
	int ch;

    if ((ch=com_getchar2())!=-1) {
        *key =(u8) ch;
        return(1);
    }
    return(0);
}












