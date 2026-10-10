/*****************************************************************************
* File              : serial.c
* Description       : UART communication routines
****************************************************************************/
#include "defines.h"

void initbootuart()
{
  // initialize UART
  BAUD_RATE_LOW_REG = BRREG_VALUE;
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) | (1 << ENABLE_TRANSMITTER_BIT);
}


void sendchar(char c)
{
  UART_DATA_REG = c;                                        // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}


char recchar(void)
{
  while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)));  // wait for data
  return UART_DATA_REG;
}
