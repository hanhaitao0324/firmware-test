
#include "common.h"
#include "tbh125.h"


u8 file_name[FILE_NAME_LENGTH];
u32 Flash_Destination = ApplicationAddress; /* Flash user program offset */
u32 Ram_Source;
int YModemAktiv=0;
u8 packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH];

static  s32 Receive_Byte (u8 *c, u32 timeout)
{
	int ch;

    while (timeout-- > 0)
    {
		if((ch=com_getchar2())!=-1) {
			*c=(u8) ch;
            return 0;
		}
		os_tsk_pass();
    }
    return -1;
}

static u32 Send_Byte (u8 c)
{
	com_putchar2((int)c);
    return 0;
}

static s32 Receive_Packet (u8 *data, s32 *length, u32 timeout)
{
	u16 i, packet_size;
	u8 c;
	
	*length = 0;
	
	if (Receive_Byte(&c, timeout) != 0) 
	{
		return -1;
	}
	
	switch (c)
	{
		case SOH:
			packet_size = PACKET_SIZE;
		break;
		case STX:
			packet_size = PACKET_1K_SIZE;
		break;
		case EOT:
			return 0;
		case CA:
			if ((Receive_Byte(&c, timeout) == 0) && (c == CA)) 
			{
				*length = -1;
				return 0;
			}
			else 
			{
				return -1;
			}
		case ABORT1:
		case ABORT2:
			return 1;
		default:
			return -1;
	}
	
	*data = c;
	
	for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++) 
	{
		if (Receive_Byte(data + i, timeout) != 0)
		{
			return -1;
		}
	}
	
	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) 
	{
		return -1;
	}
	
	*length = packet_size;
	return 0;
}

s32 Ymodem_Receive (u8 *buf)
{
	u8 *file_ptr, *buf_ptr,verg;
	s32 i,  packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
	
	YModemAktiv=1;
	
	for (session_done = 0, errors = 0, session_begin = 0; ;) 
	{
		for (packets_received = 0, file_done = 0, buf_ptr = buf; ;) 
		{
			switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT)) 
			{
				case 0:
					errors = 0;
					switch (packet_length) 
					{
						case - 1:    /* Abort by sender */
							Send_Byte(ACK);
							return 0;
						case 0:     /* End of transmission */
							Send_Byte(ACK);
							file_done = 1;
						break;
						default:    /* Normal packet */
							verg=(u8)packets_received;
							if (packet_data[PACKET_SEQNO_INDEX] != verg) 
							{
								Send_Byte(NAK);
								com_bufclr2();
							}
							else 
							{
								if (packets_received == 0) 
								{   /* Filename packet */
									if (packet_data[PACKET_HEADER] != 0) 
									{    /* Filename packet has valid data */
										for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);) 
										{
											file_name[i++] = *file_ptr++;
										}
										
										file_name[i++] = '\0';
										for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);) 
										{
											file_size[i++] = *file_ptr++;
										}
										
										file_size[i++] = '\0';
										Str2Int(file_size, &size);
										if (size > 0x50000 /*|| strcmp(file_name,"VDAP.ROM")!=0*/)
										{  /* Image size is greater than bank0 size */
											Send_Byte(CA);
											Send_Byte(CA);
											return -1;
										}
										
										VincErase();
										Vinc_StartProg();
										Send_Byte(ACK);
										/* Erase the needed sectors where the user application will be loaded */
									}
									else 
									{  /* Filename packet is empty; end session */
										Send_Byte(ACK);
										file_done = 1;
										session_done = 1;
										break;
									}
								}
								else 
								{    /* Data packet */
									memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
									if(Vinc_Prog_Block(packets_received -1,buf_ptr,packet_length)!=0) 
									{
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
					if (session_begin > 0)
					{
						errors ++;
					}
					
					if (errors > MAX_ERRORS)
					{
						Send_Byte(CA);
						Send_Byte(CA);
						return 0;
					}
					
					Send_Byte(CRC16);
					break;
			}
			
			if (file_done != 0)
			{
				break;
			}
		}
		
		if (session_done != 0) 
		{
			break;
		}
	}
	
	YModemAktiv=0;
	return (s32)size;
}

