#ifndef __NETWORK_H
#define __NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define MTU (1500)
#define SOCK_BACKLOG (5)
#define SERVER_PORT	(4321)
#define SERVER_IP "127.0.0.1"
#define MAX_IP_ADDR (16)
#define MAX_ARGS	256

#endif