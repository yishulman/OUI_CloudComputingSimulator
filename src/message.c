#include <string.h>
#include "message.h"

void message_fill(message *msg, u8 source, u8 req_type, char* ip_addr, u16 port_addr, u16 memory, u16 job_id, char *text)
{
	strncpy(msg->text, text, sizeof(msg->text));
	msg->header.source = source;
	memcpy(msg->header.ip_addr, ip_addr, MAX_IP_ADDR);
	msg->header.port_addr = port_addr;
	msg->header.req_type = req_type;
	msg->header.job_id = job_id;
	msg->header.length = strnlen(text, sizeof(msg->text));
}