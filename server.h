#ifndef __CLOUD_SRV_H
#define __CLOUD_SRV_H

#define SOCK_BACKLOG 5

void print_help();
int init_server(short port_addr);
int do_server(int sock_cli);

#endif