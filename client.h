#ifndef __CLOUD_CLI_H
#define __CLOUD_CLI_H

void print_help();
int do_client(int sockfd);
int init_client(char *hostname, short port_addr);

#endif