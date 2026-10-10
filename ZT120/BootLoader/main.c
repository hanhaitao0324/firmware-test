/*****************************************************************************
* Description   : This Program allows an AVR with bootloader capabilities to
*                 Read/write its own Flash/EEprom. 
****************************************************************************/
#include <inavr.h>
#include "defines.h"
//#include "serial.h"
#include "usb.h"
#include "flash.h"
//#include "crc.h"

#define CMD_ECHO	'E'
#define CMD_ERASE	'0'
#define CMD_WRITE	'1'
#define CMD_READ        '2'
#define CMD_RESET	'R'
#define CMD_VER 	'V'
#define CMD_DEV         'D'
#define CMD_ID          'I'

#define ANSW_ERROR      '-'
#define ANSW_OK		'!'
#define ANSW_UNKNOWN    '?'
#define ANSW_ECHO	'E'

#define COMM_CONF1      0x75
#define COMM_CONF2      0x55

/*** WARNING: The AVRprog application in AVR Studio does not support block mode,
     but it will try to use it anyway. Therefore, block mode must be disabled below
     if AVRprog is going to be used. ***/

#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, long address);
//void BlockRead(unsigned int size, unsigned char mem, long *address);

// BLOCKSIZE should be chosen so that the following holds: BLOCKSIZE*n = PAGESIZE,  where n=1,2,3... 
#define BLOCKSIZE PAGESIZE

#endif

__C_task void main(void)
{
    long address;
    unsigned int temp_int;
    unsigned char val;

    __disable_interrupt();
    void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.
    initbootuart(); // Initialize UART.


        /* Main loop */
        for(;;)
        {

	    if (recchar()!=COMM_CONF1) continue;
	    if (recchar()!=COMM_CONF2) continue;
            
            val=recchar(); // Wait for command character.
            
            // Check echo
            if(val==CMD_ECHO)
            {
                sendchar(ANSW_ECHO);
            }

            // Chip erase
            else if(val==CMD_ERASE)
            {
                for (address=0; address<APP_END; address+=BLOCKSIZE)
                { // we use address as a byte-address, not word-address
                    #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                    _PAGE_ERASE(address);
                    #pragma diag_default=Pe1053 // Back to default.
                    _WAIT_FOR_SPM();
                }
                address = FLASHSIZE-BLOCKSIZE;
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                _PAGE_ERASE(address);
                #pragma diag_default=Pe1053 // Back to default.
                temp_int = 0x940c;
                address = 0x0000;
                _WAIT_FOR_SPM();
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                _FILL_TEMP_WORD(address, temp_int); 
                #pragma diag_default=Pe1053 // Back to default.
                temp_int = APP_END>>1;
                address += 2;
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                _FILL_TEMP_WORD(address, temp_int); 
	        #pragma diag_default=Pe1053 // Back to default.
                address = 0x0000;
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
        	_PAGE_WRITE(address);
	        #pragma diag_default=Pe1053 // Back to default.
	        
                _WAIT_FOR_SPM();
	        _ENABLE_RWW_SECTION();                
                sendchar(ANSW_OK); // Send OK back.
            }

            // Start block load.
    	    else if(val==CMD_WRITE)
    	    {
	        //address = ((unsigned long)recchar()<<16) | ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get address
                address = ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get address
	        temp_int = ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get block size.
		val = recchar(); // Get memtype.
		sendchar(BlockLoad(temp_int,val,address)); // Block load.
            }
		
            /*
            // Start block read.
    	    else if(val==CMD_READ)
    	    {
	        address = ((unsigned long)recchar()<<16) | ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get address
	        temp_int = ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get block size.
    		val = recchar(); // Get memtype
	    	BlockRead(temp_int,val,&address); // Block read
    	    }*/		

            // Reset controller
            else if(val==CMD_RESET)
            {
                address = 0;
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                _PAGE_ERASE(address);
	        #pragma diag_default=Pe1053 // Back to default.
                _WAIT_FOR_SPM();

                for (address=0; address<BLOCKSIZE; address+=2)
                {  
                  #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
                  temp_int = _LOAD_PROGRAM_MEMORY(address+FLASHSIZE-BLOCKSIZE) | (_LOAD_PROGRAM_MEMORY(address+FLASHSIZE-BLOCKSIZE+1)<<8);
                  _FILL_TEMP_WORD(address,temp_int);
                  #pragma diag_default=Pe1053 // Back to default.
                }

                address = 0;
                #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
	        _PAGE_WRITE(address);
	        #pragma diag_default=Pe1053 // Back to default.
                _WAIT_FOR_SPM();
                _ENABLE_RWW_SECTION();
                sendchar(ANSW_OK);
                __enable_interrupt();
                funcptr(); // Jump to Reset vector 0x0000 in Application Section.
            }

            // Return software version.
            else if(val==CMD_VER)
            {
                sendchar('0');
                sendchar('1');
                sendchar(ANSW_OK);
            }


            // Return signature bytes.
            else if(val==CMD_DEV)
            {							
                sendchar(SIGNATURE_BYTE_3);
                sendchar(SIGNATURE_BYTE_2);
                sendchar(SIGNATURE_BYTE_1);
                sendchar(ANSW_OK);
            }

            // Return device signature
            else if(val==CMD_ID)
            {							
                sendchar(DEVICE_ID);
                sendchar(ANSW_OK);
            }

            // Unknown command
            else
            {
                sendchar(ANSW_UNKNOWN);
            }
        } // end: for(;;)

} // end: main

unsigned char BlockLoad(unsigned int size, unsigned char mem, long address)
{
    unsigned char buffer[BLOCKSIZE];
    unsigned int data;
    long tempaddress;
    unsigned int crc16;

    crc16 = mem + (size>>8) + (size&0xff) + /*(address>>16) +*/ ((address>>8)&0xff) + (address&0xff);
    //crc16 = mem + (size>>8) + (size&0xff) + (address>>8) + (address&0xff);
    
    for(tempaddress=0;tempaddress<size;tempaddress++)
    {  
      buffer[tempaddress] = recchar();
      crc16 += buffer[tempaddress];
    }
        
    data = (unsigned int)recchar() | ((unsigned int)recchar() << 8);
        
    if (data!=crc16)
    {
      sendchar(ANSW_ERROR);
      return 0;
    }  

    // EEPROM memory type.
    if(mem=='E')
    {
        // program the EEPROM
        //_WAIT_FOR_SPM();
    	for( tempaddress=0; tempaddress < size; tempaddress++)
    	{
	    EEARL = (address)&0xff; // Setup EEPROM address
            EEARH = ((address)>>8);
            EEDR = buffer[tempaddress]; // Get byte.
            EECR |= (1<<EEMWE); // Write byte.
            EECR |= (1<<EEWE);
            while (EECR & (1<<EEWE)); // Wait for write operation to finish.
            (address)++; // Select next EEPROM byte
        }

        sendchar(ANSW_OK); // Report programming OK
    }

    // Flash memory type.
    else if(mem=='F')
    { // NOTE: For flash programming, 'address' is given in words.
        //(address) <<= 1; // Convert address to bytes temporarily.
        //tempaddress = (*address);  // Store address in page.
        
        //if (tempaddress<BLOCKSIZE) tempaddress=tempaddress+FLASHSIZE-BLOCKSIZE;
	tempaddress = 0;
      
        crc16 = 0;
        
        do
	{
            //data = recchar();
            //data |= (recchar() << 8);
            data = (unsigned int)buffer[crc16] | ((unsigned int)buffer[crc16+1]<<8);
            #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
            //_FILL_TEMP_WORD(*address,data);
            _FILL_TEMP_WORD(tempaddress,data);
            #pragma diag_default=Pe1053 // Back to default.
            //(*address)+=2; // Select next word in memory.
            tempaddress += 2; // Select next word in memory.
            //size -= 2; // Reduce number of bytes to write by two.
            crc16 += 2;
        } while(crc16<size); // Loop until all bytes written.

        tempaddress = (address);  // Store address in page.
        if (tempaddress<BLOCKSIZE) tempaddress=tempaddress+FLASHSIZE-BLOCKSIZE;
        
        #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
	_PAGE_WRITE(tempaddress);
	#pragma diag_default=Pe1053 // Back to default.
	_WAIT_FOR_SPM();
	_ENABLE_RWW_SECTION();

        //(address) >>= 1; // Convert address back to Flash words again.
        sendchar(ANSW_OK); // Report programming OK
    }

    // Invalid memory type?
    else
    {
        sendchar(ANSW_UNKNOWN);
    }
    
    return 0;
}

/*
void BlockRead(unsigned int size, unsigned char mem, long *address)
{
    unsigned int crc16=0;
    
    // EEPROM memory type.
    if (mem=='E') // Read EEPROM
    {
        do
        {
            EEARL = *address; // Setup EEPROM address
            EEARH = ((*address) >> 8);
            (*address)++; // Select next EEPROM byte
            EECR |= (1<<EERE); // Read EEPROM
            sendchar(EEDR); // Transmit EEPROM dat ato PC
            size--; // Decrease number of bytes to read
        } while (size); // Repeat until all block has been read
    }

    // Flash memory type.
    else if(mem=='F')
    {
        (*address) <<= 1; // Convert address to bytes temporarily.
	
        do
        {
            #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
            sendchar( _LOAD_PROGRAM_MEMORY(*address) );
            sendchar( _LOAD_PROGRAM_MEMORY((*address)+1) );
            #pragma diag_default=Pe1053 // Back to default.
            (*address) += 2; // Select next word in memory.
            size -= 2; // Subtract two bytes from number of bytes to read
        } while (size); // Repeat until all block has been read

        (*address) >>= 1; // Convert address back to Flash words again.
    }
}
*/
