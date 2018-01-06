#ifndef __CLOUD_SRV_H
#define __CLOUD_SRV_H

#define SOCK_BACKLOG 5
#define MAX_RESOURCES 5

void print_help();
int init_server();
void server_rx(int sockfd);
void server_tx(int sockfd);

#endif