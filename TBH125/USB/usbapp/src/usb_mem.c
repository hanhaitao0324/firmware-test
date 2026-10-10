/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_mem.c
* Author             : MCD Application Team
* Version            : V4.0.0
* Date               : 09/29/2008
* Description        : Utility functions for memory transfers to/from PMA.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tbh125.h"
//#include "usb_lib.h"
#include "usb_mem.h"
#include "usb_conf.h"
#include <rtl.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern void beep(void);
extern OS_TID CommTask; 
extern OS_TID BusyTask;

extern u8 buffer_in[];
extern u32 count_in;

extern BOOL BusyFlg;

static struct buf_st rbufusb = { 0, 0, };
#define SIO_RBUFLENUSB ((unsigned short)(rbufusb.in - rbufusb.out))



/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf: pointer to user memory area.
*                  - wPMABufAddr: address into PMA.
*                  - wNBytes: no. of bytes to be copied.
* Output         : None.
* Return         : None .
*******************************************************************************/
void UserToPMABufferCopy(u8 *pbUsrBuf, u16 wPMABufAddr, u16 wNBytes)
{

  u32 n = (wNBytes + 3) >> 2;  /*n=(wNBytes+1)/4*/
  u32 i, temp1, temp2, temp3, temp4;
  u32 *pdwVal;
  pdwVal = (u32 *)(PMAAddr + (u32)((wPMABufAddr)));
  for (i = n; i != 0; i--)
  {
    temp1 = (u32) * pbUsrBuf;
    temp2 = temp1 | (u32) * (pbUsrBuf + 1) << 8;
    temp3 = temp2 | (u32) * (pbUsrBuf + 2) << 16;
    temp4 = temp3 | (u32) * (pbUsrBuf + 3) << 24;
    pbUsrBuf += 4;
    *pdwVal = temp4;

    pdwVal++;
  }
}
/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void PMAToRxBufferCopy(u16 wPMABufAddr, u16 wNBytes)
{
  u8 *pbVal;
  struct buf_st *pbUsrBuf;
  unsigned char ch;

  	if (wNBytes == 0) return;

	pbUsrBuf=&rbufusb;

  	pbVal = (u8 *)(PMAAddr + wPMABufAddr);

	
  	while (wNBytes>0) {
	  	if (((pbUsrBuf->in - pbUsrBuf->out) & ~(RBUF_SIZE-1)) == 0) {
			ch=*pbVal++;
	        pbUsrBuf->buf [pbUsrBuf->in & (RBUF_SIZE-1)] = ch;
       		pbUsrBuf->in++;
			if(ch==0x0D) {
				if(!BusyFlg)
					isr_evt_set(0x0002,CommTask);
			else
					isr_evt_set(0x0002,BusyTask);
			}
    	}
		else beep();
		wNBytes--;
	}
}


void PMAToUserBufferCopy(u8 *pbUsrBuf, u16 wPMABufAddr, u16 wNBytes)
{
  u8 *pbVal;
  u16 wNTrasf = wNBytes;
  if ((wNBytes) == 0)
  {
    return;
  }
  pbVal = (u8 *)(PMAAddr + wPMABufAddr);
  while (1)
  {
    *pbUsrBuf++ = *pbVal++;
    if ((--wNTrasf) == 0)
    {
      return;
    }
    *pbUsrBuf++ = *pbVal++;
    if ((--wNTrasf) == 0)
    {
      return;
    }
  }
}


int usb_getchar (void) {
	struct buf_st *p = &rbufusb;

  	if (SIO_RBUFLENUSB == 0)
    	return (-1);
  	return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}


void putusbstr(char *s)
{
	unsigned int i;

	if(strlen((char *)s)>64) {
		count_in=64;
		memcpy(buffer_in,s,count_in);
	  	UserToPMABufferCopy(buffer_in, ENDP1_TXADDR, count_in);
  		SetEPTxCount(ENDP1, count_in);
  		SetEPTxValid(ENDP1);
		for(i=0;i<2500;i++);
		count_in=strlen((char *)s)-64;
		memcpy(buffer_in,&s[64],count_in);
	  	UserToPMABufferCopy(buffer_in, ENDP1_TXADDR, count_in);
  		SetEPTxCount(ENDP1, count_in);
  		SetEPTxValid(ENDP1);
	}
	else {
		count_in=strlen((char *)s);
		memcpy(buffer_in,s,count_in);
	  	UserToPMABufferCopy(buffer_in, ENDP1_TXADDR, count_in);
  		SetEPTxCount(ENDP1, count_in);
  		SetEPTxValid(ENDP1);
	}
	for(i=0;i<3500;i++);
}





