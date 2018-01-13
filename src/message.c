#include <string.h>
#include "message.h"

void message_fill(message *msg,
					u8 source,
					u8 req_type, 
					char* ip_addr, 
					u16 port_addr, 
					u16 memory, 
					u16 job_id, 
					char *text)
{
	memset(msg, 0, sizeof(message));
	msg->header.source = source;
	msg->header.port_addr = port_addr;
	msg->header.req_type = req_type;
	msg->header.job_id = job_id;
	
	if (ip_addr)
		memcpy(msg->header.ip_addr, ip_addr, MAX_IP_ADDR);

	if (text) {
		msg->header.length = strnlen(text, sizeof(msg->text));
		strncpy(msg->text, text, sizeof(msg->text));
	}
}