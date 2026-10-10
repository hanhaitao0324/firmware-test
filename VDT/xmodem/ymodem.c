#include "osObjects.h"                      // RTOS object definitions
#include "system.h"
#include "ymodem.h"
#include "uart.h"
#include "vinculum.h"
#include "utils.h"
#include "defines.h"
#include <string.h>
#include "const.h"
#include "timer.h"

uint8_t file_name[FILE_NAME_LENGTH];
uint32_t Ram_Source;
int YModemAktiv=0;
uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH];

static  S32 Receive_Byte (uint8_t *c, uint32_t timeout)
{
	int ch;
	
	TIMER_Start(TIMER_YMODEM, TO_1SEC);
	do{	
		ch = com_getchar1();
		osThreadYield();
	}while(ch ==-1 && !TIMER_TimeOut(TIMER_YMODEM));
	if(ch!=-1){
		*c=(uint8_t) ch;
    return 0;
	}
    return -1;
}

static uint32_t Send_Byte(uint8_t c)
{
	RS232_SendChar((int)c);
	return 0;
}

static S32 Receive_Packet (uint8_t *data, S32 *length, uint32_t timeout)
{
	uint16_t i, packet_size;
  uint8_t c;

    *length = 0;
    if (Receive_Byte(&c, timeout) != 0) {
        return -1;
    }
    switch (c){
    	case SOH:
        	packet_size = PACKET_SIZE;
        	break;
    	case STX:
        	packet_size = PACKET_1K_SIZE;
        	break;
    	case EOT:
        	return 0;
    	case CA:
        	if ((Receive_Byte(&c, timeout) == 0) && (c == CA)) {
            	*length = -1;
            	return 0;
        	}
        	else {
            	return -1;
        	}
    	case ABORT1:
    	case ABORT2:
        	return 1;
    	default:
        	return -1;
    }
    *data = c;
    for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++) {
        if (Receive_Byte(data + i, timeout) != 0){
            return -1;
        }
    }
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return -1;
    }
    *length = packet_size;
    return 0;
}

S32 Ymodem_Receive (uint8_t *buf)
{
    uint8_t *file_ptr, *buf_ptr,verg;
    S32 i,  packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;

	YModemAktiv=1;

    for (session_done = 0, errors = 0, session_begin = 0; ;) {
        for (packets_received = 0, file_done = 0, buf_ptr = buf; ;) {
            switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT)) {
            	case 0:
                	errors = 0;
                	switch (packet_length) {
                		case - 1:    /* Abort by sender */
                    		Send_Byte(ACK);
                    		return 0;
                		case 0:     /* End of transmission */
                    		Send_Byte(ACK);
                    		file_done = 1;
                    		break;
                		default:    /* Normal packet */
												verg=(uint8_t)packets_received;
                    		if (packet_data[PACKET_SEQNO_INDEX] != verg) {
                        		Send_Byte(NAK);
														com_bufclr1();
                    		}
                    		else {
                        		if (packets_received == 0) {   /* Filename packet */
			                    	if (packet_data[PACKET_HEADER] != 0) {    /* Filename packet has valid data */
		                                for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);) {
		                                    file_name[i++] = *file_ptr++;
        		                        }
                		                file_name[i++] = '\0';
                        		        for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);) {
                                    		file_size[i++] = *file_ptr++;
                                		}
                                		file_size[i++] = '\0';
                                		Str2Int(file_size, &size);
                                		if (size > 0x40000 /*|| strcmp(file_name,"VDAP.ROM")!=0*/){  /* Image size is greater than bank0 size */
                            		        Send_Byte(CA);
                                    		Send_Byte(CA);
                                    		return -1;
                                		}
																		Vinc_StartProg();
																		Send_Byte(ACK);
                            		}
                            		else {  /* Filename packet is empty; end session */
	                            		Send_Byte(ACK);
                                		file_done = 1;
                                		session_done = 1;
                                		break;
                            		}
                        		}
                        		else {    /* Data packet */
		                            memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
																if(Vinc_Prog_Block(packets_received -1,buf_ptr,packet_length)!=0) {
	                                    Send_Byte(CA);
       		                            Send_Byte(CA);
               		                    return -2;
                       		       } 
            		                Send_Byte(ACK);
                    		     }
                        		 packets_received ++;
                        		 session_begin = 1;
                    		}
                		}
                		break;
            		case 1:
                		Send_Byte(CA);
                		Send_Byte(CA);
                		return -3;
            		default:
                		if (session_begin > 0){
                    		errors ++;
                		}
                		if (errors > MAX_ERRORS){
		                    Send_Byte(CA);
        		            Send_Byte(CA);
                		    return 0;
                		}
                		Send_Byte(CRC16);
                		break;
            	}
            	if (file_done != 0){
                	break;
            	}
        	}
        	if (session_done != 0) {
            	break;
	        }
    	}
		  YModemAktiv=0;
    	return (S32)size;
}

