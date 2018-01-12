#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "types.h"
#include "network.h"

typedef struct __msg_header{
	u8 source;
	u8 req_type;
	u8	ip_addr[MAX_IP_ADDR];
	u16 port_addr;
	u16 memory;
	u16 job_id;
	u16 length;
} message_header;

#define MSG_TEXT_SIZE (MTU - sizeof(message_header))

typedef struct __message{
	message_header header;
	char text[MSG_TEXT_SIZE];
} message;

enum msg_source_e {
	SOURCE_CLIENT,
	SOURCE_SERVER,
	SOURCE_RESOURCE
};

enum msg_type_e {
	TYPE_ACK,
	TYPE_NACK,
	TYPE_COMMAND,
	TYPE_STATUS,
	TYPE_ADD_RESOURCE,
	TYPE_REMOVE_RESOURCE,
	TYPE_OUTPUT_STREAM,
	TYPE_JOB_DONE
};

void message_fill(message *msg, u8 source, u8 req_type, char* ip_addr, u16 port_addr, u16 memory, u16 job_id, char *text);

#endif