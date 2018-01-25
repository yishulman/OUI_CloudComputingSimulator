#ifndef __CLOUD_SRV_H
#define __CLOUD_SRV_H

#define MAX_RESOURCES 5

void server_print_help();
void server_rx(int sockfd);
void server_tx(int sockfd);

#endif