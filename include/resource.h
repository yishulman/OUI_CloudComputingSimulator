#ifndef __RESOURCE_H
#define __RESOURCE_H

#include "types.h"

typedef struct __resource {
	u32 sock;
	u16 cpu_speed;
	u16 memory;
	u16 status;
} resource;

enum resource_status_e {
	STATUS_AVAILABLE,
	STATUS_BUSY,
};

int resource_register(int sockfd);
int resource_run(int sockfd);

#endif