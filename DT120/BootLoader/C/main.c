/*****************************************************************************
* Description   : This Program allows an AVR with bootloader capabilities to
*                 write its own Flash/EEprom. 
****************************************************************************/
#include "defines.h"
//#include "serial.h"
#include "usb.h"
#include "flash.h"

#define CMD_ECHO	'E'
#define CMD_ERASE	'0'
#define CMD_WRITE	'1'
//#define CMD_READ        '2'
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

unsigned char BlockLoad(unsigned int size, unsigned char mem, unsigned int address);
//void BlockRead(unsigned int size, unsigned char mem, long *address);

#define BLOCKSIZE PAGESIZE

__C_task void main(void)
{
	long address;
	unsigned int temp_int;
	unsigned char val, status;
	
	__disable_interrupt();
	void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.
	initbootuart(); // Initialize UART.
	
	/* Main loop */
	for(;;)
	{
		if (recchar()!=COMM_CONF1) 					// 0x75
			continue;
		
		if (recchar()!=COMM_CONF2) 					// 0x55
			continue;
		
		val=recchar(); // Wait for command character.
		
		// Check echo
		/*if(val==CMD_ECHO)
		{
			sendchar(ANSW_ECHO);
		}*/
		
		// Chip erase(芯片擦除)---------------------------------
		if(val==CMD_ERASE) 									// '0'
		{
			for (address=0; address<APP_END+BLOCKSIZE; address+=BLOCKSIZE)
			{
				// we use address as a byte-address, not word-address
				#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
				_PAGE_ERASE(address);
				#pragma diag_default=Pe1053 // Back to default.
				_WAIT_FOR_SPM();
      }
      
      //address = FLASHSIZE-BLOCKSIZE;
      //#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
      //_PAGE_ERASE(address);
      //#pragma diag_default=Pe1053 // Back to default.
      temp_int = 0x940c;
      address = 0x0000;
      //_WAIT_FOR_SPM();
      #pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
      _FILL_TEMP_WORD(address, temp_int); 
      #pragma diag_default=Pe1053 // Back to default.
      temp_int = LOADER_ADDRESS>>1;
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
    
    // Start block load.(启动块加载)------------------------
    else if(val==CMD_WRITE) 						// '1'
    {
    	//address = ((unsigned long)recchar()<<16) | ((unsigned long)recchar()<<8) | (unsigned long)recchar(); // Get address
    	address = ((unsigned int)recchar()<<8) | (unsigned int)recchar(); // Get address
    	temp_int = ((unsigned int)recchar()<<8) | (unsigned int)recchar(); // Get block size.
    	val = recchar(); // Get memtype.
    	status = BlockLoad(temp_int,val,(unsigned int)address); // Block load
    	sendchar(status);
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
    
    // Reset controller(复位控制器)-------------------------
    else if(val==CMD_RESET) 						// 'R'
    {
    	address = 0;
    	#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
    	_PAGE_ERASE(address);
    	#pragma diag_default=Pe1053 // Back to default.
    	_WAIT_FOR_SPM();
    	
    	for (address=0; address<BLOCKSIZE; address+=2)
    	{
    		#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
    		//temp_int = _LOAD_PROGRAM_MEMORY(address+FLASHSIZE-BLOCKSIZE) | (_LOAD_PROGRAM_MEMORY(address+FLASHSIZE-BLOCKSIZE+1)<<8);
    		temp_int = _LOAD_PROGRAM_MEMORY(address+APP_END) | (_LOAD_PROGRAM_MEMORY(address+APP_END+1)<<8);
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
    /*else if(val==CMD_VER)
    {
    	sendchar('0');
    	sendchar('1');
    	sendchar(ANSW_OK);
    }*/
    
    // Return signature bytes.(返回签名字节)----------------
    else if(val==CMD_ID) 								// 'I'
    {
    	sendchar(SIGNATURE_BYTE_1);
    	sendchar(SIGNATURE_BYTE_2);
    	sendchar(SIGNATURE_BYTE_3);
    	//sendchar(ANSW_OK);
    }
    
    // Return device signature(返回设备签名)----------------
    else if(val==CMD_DEV) 							// 'D'
    {
    	sendchar(DEVICE_ID);
    	//sendchar(ANSW_OK);
    }
    
    // Unknown command(未知命令)----------------------------
    else
    {
    	sendchar(ANSW_UNKNOWN); 					// '?'
    }
  } // end: for(;;)
} // end: main

unsigned char BlockLoad(unsigned int size, unsigned char mem, unsigned int address)
{
	unsigned char buffer[BLOCKSIZE];
	unsigned int data;
	long tempaddress;
	unsigned int crc16;
	
	//crc16 = mem + (size>>8) + (size&0xff) + /*(address>>16) +*/ ((address>>8)&0xff) + (address&0xff);
	crc16 = mem + (size>>8) + (size&0xff) + (address>>8) + (address&0xff);
	
	for(data=0;data<size;data++)
	{
		buffer[data] = recchar();
		crc16 += buffer[data];
	}
	
	data = (unsigned int)recchar() | ((unsigned int)recchar() << 8);
	
	if (data!=crc16)
	{
		return ANSW_ERROR;
	}
	
	// EEPROM memory type.
	/*if(mem=='E')
	{
		// program the EEPROM
		//_WAIT_FOR_SPM();
		for (data=0; data<size; data++)
		{
			EEARL = address&0xff; // Setup EEPROM address
			EEARH = (address>>8);
			EEDR = buffer[data]; // Get byte.
			EECR |= (1<<EEMWE); // Write byte.
			EECR |= (1<<EEWE);
			while (EECR & (1<<EEWE)); // Wait for write operation to finish.
			address++; // Select next EEPROM byte
		}
		
		return ANSW_OK; // Report programming OK
	}*/
	
	// Flash memory type.
	//else if(mem=='F')
	//{ // NOTE: For flash programming, 'address' is given in words.
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
	
	tempaddress = (long)address<<1;  // Store address in page.
	
	if (tempaddress>(APP_END-BLOCKSIZE))
	{
		return ANSW_OK;
	}
	
	//if (tempaddress<BLOCKSIZE) tempaddress=tempaddress+FLASHSIZE-BLOCKSIZE;
	if (tempaddress<BLOCKSIZE)
		tempaddress=APP_END;
	
	#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
	_PAGE_WRITE(tempaddress);
	#pragma diag_default=Pe1053 // Back to default.
	_WAIT_FOR_SPM();
	_ENABLE_RWW_SECTION();
	
	//(address) >>= 1; // Convert address back to Flash words again.
	return ANSW_OK; // Report programming OK
	//}
	
	// Invalid memory type?
	//else
	//  return ANSW_UNKNOWN;
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
