#ifndef __CLOUD_CLI_H
#define __CLOUD_CLI_H

void client_job_print_help();
void client_status_print_help();
int client_run_status(int sockfd);
int client_run_job(int sockfd, char* job, char *ip, short port);
int client_recv_res(int sockfd);
int client_init(char *client_ip, int *o_socket, short *o_port);

#endif