#ifndef __CLOUD_CLI_H
#define __CLOUD_CLI_H

void client_print_help();
int client_do(int sockfd, char* text);
int client_init(char *hostname, short port_addr);

#endif