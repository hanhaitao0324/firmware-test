#ifndef	_DEF_H	

#include <inavr.h>

#define	_ATMEGA128	// device select: _ATMEGAxxx

#define DEVICE_ID      2 //DT

#define LOADERSIZE     768

/* baud rate register value calculation */		
#define	CPU_FREQ	14745600L
#define	BAUDRATE	115200
#define	BRREG_VALUE	((CPU_FREQ/(16*BAUDRATE)) - 1)

#ifdef _ATMEGA128
  #include "iom128.h"
  #define FLASHSIZE     131072L
  #define PAGESIZE	256
  #define EEPROMSIZE    4096
  #define EEPROMPAGE	4
  /* definitions for SPM control */		
  #define SPMCR_REG	SPMCSR
  #define LARGE_MEMORY	
  /* definitions for device recognition */	
  #define PARTCODE	    0x44
  #define SIGNATURE_BYTE_1  0x1E
  #define SIGNATURE_BYTE_2  0x97
  #define SIGNATURE_BYTE_3  0x02
  /* definitions for UART control */		
  #define BAUD_RATE_LOW_REG	  UBRR0L
  #define UART_CONTROL_REG	  UCSR0B
  #define ENABLE_TRANSMITTER_BIT  TXEN0
  #define ENABLE_RECEIVER_BIT	  RXEN0
  #define UART_STATUS_REG         UCSR0A
  #define TRANSMIT_COMPLETE_BIT	  TXC0
  #define RECEIVE_COMPLETE_BIT	  RXC0
  #define UART_DATA_REG	          UDR0
#endif

#ifdef _ATMEGA64
  #include "iom64.h"
  #define FLASHSIZE     65536L
  #define PAGESIZE	256
  #define EEPROMSIZE    2048
  #define EEPROMPAGE	4
  /* definitions for SPM control */		
  #define SPMCR_REG	SPMCR
  /* definitions for device recognition */	
  #define PARTCODE	    0x46
  #define SIGNATURE_BYTE_1  0x1E
  #define SIGNATURE_BYTE_2  0x96
  #define SIGNATURE_BYTE_3  0x02
  /* definitions for UART control */		
  #define BAUD_RATE_LOW_REG	  UBRR0L
  #define UART_CONTROL_REG	  UCSR0B
  #define ENABLE_TRANSMITTER_BIT  TXEN0
  #define ENABLE_RECEIVER_BIT	  RXEN0
  #define UART_STATUS_REG         UCSR0A
  #define TRANSMIT_COMPLETE_BIT	  TXC0
  #define RECEIVE_COMPLETE_BIT	  RXC0
  #define UART_DATA_REG	          UDR0
#endif

#ifdef _ATMEGA32
  #include "iom32.h"
  #define FLASHSIZE     32768L
  #define PAGESIZE	128
  #define EEPROMSIZE    1024
  #define EEPROMPAGE	4
  /* definitions for SPM control */		
  #define SPMCR_REG	SPMCR
  /* definitions for device recognition */	
  #define PARTCODE	    0x7F
  #define SIGNATURE_BYTE_1  0x1E
  #define SIGNATURE_BYTE_2  0x95
  #define SIGNATURE_BYTE_3  0x02
  /* definitions for UART control */		
  #define BAUD_RATE_LOW_REG	  UBRRL
  #define UART_CONTROL_REG	  UCSRB
  #define ENABLE_TRANSMITTER_BIT  TXEN
  #define ENABLE_RECEIVER_BIT	  RXEN
  #define UART_STATUS_REG         UCSRA
  #define TRANSMIT_COMPLETE_BIT	  TXC
  #define RECEIVE_COMPLETE_BIT	  RXC
  #define UART_DATA_REG	          UDR
#endif

#ifdef _ATMEGA16
  #include "iom16.h"
  #define FLASHSIZE     16384L
  #define PAGESIZE	128
  #define EEPROMSIZE    512
  #define EEPROMPAGE	4
  /* definitions for SPM control */		
  #define SPMCR_REG	SPMCR
  /* definitions for device recognition */	
  #define PARTCODE	    0x75
  #define SIGNATURE_BYTE_1  0x1E
  #define SIGNATURE_BYTE_2  0x94
  #define SIGNATURE_BYTE_3  0x03
  /* definitions for UART control */		
  #define BAUD_RATE_LOW_REG	  UBRRL
  #define UART_CONTROL_REG	  UCSRB
  #define ENABLE_TRANSMITTER_BIT  TXEN
  #define ENABLE_RECEIVER_BIT	  RXEN
  #define UART_STATUS_REG         UCSRA
  #define TRANSMIT_COMPLETE_BIT	  TXC
  #define RECEIVE_COMPLETE_BIT	  RXC
  #define UART_DATA_REG	          UDR
#endif

#define FULLLOADERSIZE  (LOADERSIZE+PAGESIZE)

#define APP_END         (FLASHSIZE-FULLLOADERSIZE)

#define LOADER_ADDRESS  (FLASHSIZE-LOADERSIZE)

/* indicate that preprocessor result is included */		
#define	_DEF_H	
#endif		
