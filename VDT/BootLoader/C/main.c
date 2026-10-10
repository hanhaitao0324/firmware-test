/**
  ******************************************************************************
  * @file    main.c
  * @author  Erweka GmbH development Team
  * @version V1.0
  * @date    22.06.2015
  * @brief   Bootloader Source File.
  * 
  *         
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 Erweka GmbH Germany</center></h2>
  ******************************************************************************
  */

#include <stdio.h>
#include "programm.h"
#include "serial.h"
#include "defines.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

int i, crc16;
char buffer[BLOCK_SIZE];

extern void execute_user_code(void);
extern void enter_usb_isp(void);

/**
 * @brief inits the GPIO Ports
 * @param none
 * @return none
 */
void init_GPIO(void){
	uint32_t reg;
	
	// turn on power for USART1 and GPIOA and GPIOB
  RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	GPIOA->CRH |= (1 << 29)|(1 << 28); 	  											 // set HB_LED Output mode, max speed 50 MHz
	GPIOA->CRH &= ~(1 << 31)|(1 << 30); 												 // set HB_LED Output mode, max speed 50 MHz
	GPIOB->CRH |= ((1 << 2) | (1 << 22)); 							 				 // PB0&PB5 floating input for SB1 & SB6 Boot Option
	
	reg = AFIO->MAPR;
  reg &= ~(1<<24 | 1<<26);																		 // JTAG-DP Disabled and SW-DP Enabled
  reg |= (1<<25);
  AFIO->MAPR = reg;
	GPIOA->ODR &=  ~( 1 << 15 );																 // HB_LED off
	
}

/**
 * @brief main routine
 * @param none
 * @return none
 */
int main(void) 
{
  unsigned char val, val2, status;
  unsigned int * ptr;
  unsigned int address, size;
  unsigned int crc32;

  //get_mcu_type();
	
#ifdef BL_DEBUG
  PROGRAMM_ExecuteCode();
#endif

	init_GPIO();
	
	if( !(GPIOB->IDR & (GPIO_IDR_IDR0 | GPIO_IDR_IDR5))  ){	// Buttons SB1&SB6 for interrupting bootloader
		//go into Bootloader and wait for FW
	}else{
		ptr =(unsigned int *)RAM_START_ADDR;
		if(!(*ptr == BOOTLOADER_KEY1 && *(ptr+1) == BOOTLOADER_KEY2))
		{
			if (PROGRAMM_CodeChecksum() )
			{
				PROGRAMM_ExecuteCode();
			}
		}
	}
  SERIAL_Init();

  for (;;)
  {
    
   if (SERIAL_GetChar()!=COMM_CONF1) continue;
   if (SERIAL_GetChar()!=COMM_CONF2) continue;
           
    val=SERIAL_GetChar(); // Wait for command character.
            
    // Check echo
    /*if(val==CMD_ECHO)
    {
      SERIAL_SendChar(ANSW_ECHO);
    }*/

    // Chip erase
    if(val==CMD_ERASE)
    {
      PROGRAMM_EraseFlash();
      SERIAL_SendChar(ANSW_OK); // Send OK back.
    }

    // Start block load.
    else if(val==CMD_WRITE)
    {
      crc16 = 0;
      val = SERIAL_GetChar();
      val2 = SERIAL_GetChar();
      address = ((unsigned int)val<<16) | ((unsigned int)val2<<8) | (unsigned int)SERIAL_GetChar(); // Get address
      val = SERIAL_GetChar();
      size = ((unsigned int)val<<8) | (unsigned int)SERIAL_GetChar(); // Get block size.
      val = SERIAL_GetChar(); // Get memtype
      crc16 = val + (size>>8) + (size&0xff) + (address>>16) + ((address>>8)&0xff) + (address&0xff);
      //address <<= 1;

      for (i=0; i<size; i++)
      {
        buffer[i] = SERIAL_GetChar();
        crc16 += buffer[i];
      }

      status = SERIAL_GetChar();
      i = (unsigned int)status | ((unsigned int)SERIAL_GetChar() << 8);

      if (i!=crc16)
      {
        status = ANSW_ERROR;
      } else
      {  
  	    if (val=='F')
        {
          address += USER_FLASH_ADD;
          if (address<USER_FLASH_START)
          {
            status = ANSW_OK;
          } else
          {
            //write flash
            status = PROGRAMM_WriteFlash(buffer, (void*)address, size);
            if (status==CMD_SUCCESS)
              status = ANSW_OK;
            else
              status = ANSW_ERROR;
          }
        }
      }

      SERIAL_SendChar(status);
    }

    // Return software version.
    else if(val==CMD_VER)
    {
      SERIAL_SendChar('0');
      SERIAL_SendChar('1');
    }

    // Return signature bytes.
    else if(val==CMD_ID)
    {							
      SERIAL_SendChar(SIGNATURE_BYTE_1);
      SERIAL_SendChar(SIGNATURE_BYTE_2);
      SERIAL_SendChar(SIGNATURE_BYTE_3);
    }

    // Return device signature
    else if(val==CMD_DEV)
    {							
      SERIAL_SendChar(DEVICE_ID);
    }

    // Reset controller
    else if(val==CMD_RESET)
    {
      //calculate checksum and write it to flash
      address = CHECKSUM_START;
      crc32 = PROGRAMM_CalcChecksum();
      status = PROGRAMM_WriteFlash((void*)&crc32, (void*)address, sizeof(unsigned int));

      PROGRAMM_ExecuteCode();
    }
    
    // stop bootloader, execute user code
    else if(val==CMD_STOP)
    {
      PROGRAMM_ExecuteCode();
    }

    // Unknown command
    else
    {
      SERIAL_SendChar(ANSW_UNKNOWN);
    }
  }

  execute_user_code();
  enter_usb_isp();
}
