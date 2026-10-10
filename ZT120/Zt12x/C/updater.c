#include "commands.h"
#include "crc.h"
#include "pins.h"
#include "rs485.h"
#include "timer.h"
#include "updater.h"
#include "usb.h"

unsigned char device;
unsigned char device_code;
char upBuffer[BLOCK_SIZE];
TBool UPDATER_Busy=false;
  
void sendchar(char c)
{
  USB_SendChar(c);
}

//int havechar(void)
//{
//  if (LPC_UART3->LSR & (1 << 0))
//    return 1;
//  else
//    return 0;
//}

//char getchar(void)
//{
//  char c;

//  c = UDR1;
//  return c;
//}

char recchar(void)
{
  return USB_RecChar();
}

//------------------------------------------------------------------------------
// UPDATER_BusRun - start bus update procedure
//------------------------------------------------------------------------------
void UPDATER_BusRun(int device)
{
  TByte data;
  char b[3]={0,0,0};
  char res[3]={0x75, 0x55, 'R'};
  TBool reset=false;
  
  UPDATER_Busy=true;

  switch (device)
  {  
    case DEVICE_ID_DTAS:
      RS485_SendPackage(RS485_ADDR_SAMPLER, cmdUpdate, 0, NULL, true);
      break;
    case DEVICE_ID_PRINTER:
      RS485_SendPackage(RS485_ADDR_PRNRTC, cmdUpdate, 0, NULL, true);
      break;
    default:
      UPDATER_Busy=false;
      return;
  }
  
  while (RS485_GetStatus()!=RS485_STATUS_WAITING)
    continue;  
  
  __disable_interrupt();
  RS485_Stop();
  //read byte from USB and transmit it to RS485, leave this mode after 5 seconds timeout
  for (;;)
  {
    if (!(PIN_USB_STS & (1<<pinUSBRXF)))
    {
      data=recchar();
      b[0]=b[1];
      b[1]=b[2];
      b[2]=data;
      if (!strncmp(b, res, 3))
        reset=true;
      RS485_PutByte2(data);
    }
    if (RS485_GetByte(&data))
    {
      USB_SendChar(data);
      if (reset)
        break;
    }  
  }
  RS485_Init();
  UPDATER_Busy=false;
  __enable_interrupt();  
}

TBool UPDATER_GetBusy(void)
{
  return UPDATER_Busy;
}
