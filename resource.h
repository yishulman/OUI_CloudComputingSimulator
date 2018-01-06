#ifndef __RESOURCE_H
#define __RESOURCE_H

#include "types.h"

typedef struct __resource {
	u32 ip_addr;
	u16 port_addr;
	u16 cpu_speed;
	u16 memory;
	u16 status;
} resource;

#endif