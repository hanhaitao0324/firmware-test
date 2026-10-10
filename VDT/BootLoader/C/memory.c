/*----------------------------------------------------------------------------
 *      Name:    MEMORY.C
 *      Purpose: USB Mass Storage Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include "LPC17xx.h"

#include "type.h"

#include "memory.h"
#include "sbl_iap.h"
#include "sbl_config.h"

#define CMD_ECHO		'E'
#define CMD_ERASE		'0'
#define CMD_WRITE		'1'
//#define CMD_READ        '2'
#define CMD_RESET		'R'
#define CMD_VER 		'V'
#define CMD_DEV     	'D'
#define CMD_ID      	'I'

#define ANSW_ERROR      '-'
#define ANSW_OK			'!'
#define ANSW_UNKNOWN    '?'
#define ANSW_ECHO		'E'

#define COMM_CONF1      0x75
#define COMM_CONF2      0x55

#define SIGNATURE_BYTE_1  	0x20
#define SIGNATURE_BYTE_2  	0x01
#define SIGNATURE_BYTE_3  	0x05
#define DEVICE_ID      		14 //DT-Uni

#define RAM_START_ADDR      0x10001000
#define BLOCK_SIZE			256

#define pinLED			(8)

char buffer[BLOCK_SIZE];

uint32_t  user_flash_erased;

extern uint32_t Image$$RW_IRAM1$$ZI$$Base;
extern uint32_t Image$$RW_IRAM1$$ZI$$Length;

extern uint32_t Load$$RW_IRAM1$$Base;
extern uint32_t Image$$RW_IRAM1$$RW$$Base;
extern uint32_t Image$$RW_IRAM1$$RW$$Length;

void enter_usb_isp(void);

int i, crc16;
  
void  Prepare_Zero( uint32_t * dst, uint32_t len )
{
    while(len)
    {
        *dst++ = 0;

        len -= 4;
    }
}

void Prepare_Copy( uint32_t * src, uint32_t * dst, uint32_t len )
{
    if(dst != src)
    {
        while(len)
        {
            *dst++ = *src++;

            len -= 4;
        }
    }
}

/* Main Program */

int main(void) 
{
  unsigned char val, val2, status;
  unsigned int * ptr;
  unsigned int address, size;
  unsigned int crc32;

  get_mcu_type();
  
  ptr =(unsigned int *)RAM_START_ADDR;
  if(!(*ptr == 0x12345678 && *(ptr+1) == 0x43211234))
  {
    // If CRP3 is enabled and user flash start sector is not blank, execute the user code
    if (/*crp == CRP3 && user_code_present() &&*/ user_code_checksum())
    {
      execute_user_code();
    }
  }

  SystemInit();
  
  initbootuart();

  LPC_GPIO1->FIODIR = (1<<pinLED);           		// LEDs on PORT1 defined as Output
  LPC_GPIO1->FIOSET |= (1<<pinLED);

  for (;;)
  {
    
    if (recchar()!=COMM_CONF1) continue;
    if (recchar()!=COMM_CONF2) continue;
           
    val=recchar(); // Wait for command character.
            
    // Check echo
    /*if(val==CMD_ECHO)
    {
      sendchar(ANSW_ECHO);
    }*/

    // Chip erase
    if(val==CMD_ERASE)
    {
	  erase_user_flash();
      sendchar(ANSW_OK); // Send OK back.
	}

    // Start block load.
    else if(val==CMD_WRITE)
    {
	  crc16 = 0;
	  val = recchar();
	  val2 = recchar();
      address = ((unsigned int)val<<16) | ((unsigned int)val2<<8) | (unsigned int)recchar(); // Get address
	  val = recchar();
      size = ((unsigned int)val<<8) | (unsigned int)recchar(); // Get block size.
	  val = recchar(); // Get memtype
	  crc16 = val + (size>>8) + (size&0xff) + (address>>16) + ((address>>8)&0xff) + (address&0xff);
	  //address <<= 1;

	  for (i=0; i<size; i++)
	  {
	    buffer[i] = recchar();
		crc16 += buffer[i];
	  }

	  status = recchar();
	  i = (unsigned int)status | ((unsigned int)recchar() << 8);

      if (i!=crc16)
      {
        status = ANSW_ERROR;
      } else
	  {  
  	    if (val=='F')
	    {
		  if (address<USER_FLASH_START)
		  {
		    status = ANSW_OK;
		  } else
		  {
	        //write flash
	        status = write_flash(address, buffer, size);
	        if (status==CMD_SUCCESS)
		      status = ANSW_OK;
		    else
        	  status = ANSW_ERROR;
		  }
	    }
	  }

	  sendchar(status);
	}

	// Return software version.
    else if(val==CMD_VER)
    {
      sendchar('0');
      sendchar('1');
    }

    // Return signature bytes.
    else if(val==CMD_ID)
    {							
      sendchar(SIGNATURE_BYTE_1);
      sendchar(SIGNATURE_BYTE_2);
      sendchar(SIGNATURE_BYTE_3);
    }

    // Return device signature
    else if(val==CMD_DEV)
    {							
      sendchar(DEVICE_ID);
    }

    // Reset controller
    else if(val==CMD_RESET)
    {
	  //calculate checksum and write it to flash
	  address = CHECKSUM_START;
	  crc32 = user_calc_checksum();
      status = write_flash(address, (uint8_t*)&crc32, sizeof(unsigned int));

	  execute_user_code();
	}

    // Unknown command
    else
    {
      sendchar(ANSW_UNKNOWN);
    }
  }

  execute_user_code();

  /* User code not present or isp entry requested */
  enter_usb_isp();
}

void enter_usb_isp(void)
{
  uint32_t n,m,next_cluster;

  SystemInit();

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_SetPriorityGrouping(1);

  /* Set user_flash_erased flag to FALSE. This flag is used detect whether whole
     user flash needs to be erased or not when CRP2 or CRP3 is enabled */
  user_flash_erased = FALSE;

  Prepare_Copy((uint32_t *)&Load$$RW_IRAM1$$Base,(uint32_t *)&Image$$RW_IRAM1$$RW$$Base,(uint32_t)&Image$$RW_IRAM1$$RW$$Length);
  Prepare_Zero((uint32_t *)&Image$$RW_IRAM1$$ZI$$Base,(uint32_t)&Image$$RW_IRAM1$$ZI$$Length);

  while (1);                                /* Loop forever */
}
