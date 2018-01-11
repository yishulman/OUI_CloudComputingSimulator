#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network.h"
#include "message.h"

void client_job_print_help()
{
	printf("USAGE: run_job <client_ip> <command>\n");
}

void client_status_print_help()
{
	printf("USAGE: run_status <client_ip>\n");
}

/**
 *	This function initialize the client's socket
 *	and connects to the server.
 **/
int client_init(char *client_ip, int *o_socket, short *o_port)
{
	int 				sockfd;
	struct sockaddr_in 	cli_addr;
  	struct sockaddr_in 	srv_addr;
  	struct hostent 		*server;
  	socklen_t			addr_len = sizeof(cli_addr);

  	if (!o_socket || ! o_port) {
  		fprintf(stderr, "NULL param\n");
  		return -1;
  	}

	*o_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (*o_socket < 0) {
  		perror("Opening socket");
  		return -1;
	}

	/* Initialize client address */
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = 0;
	memcpy(&cli_addr.sin_addr.s_addr, client_ip, sizeof(cli_addr.sin_addr.s_addr));

	/* Bind socket to address */
	if (bind(*o_socket, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0) {
  		perror("Bind");
		close(*o_socket);
		return -1;
	}

	if (getsockname(*o_socket, (struct sockaddr*)&cli_addr, &addr_len))
	{
		perror("getsockname");
		close(*o_socket);
		return -1;
	}

	*o_port = ntohs(cli_addr.sin_port);

	/* Listen to new connections */
  	if (listen(*o_socket, SOCK_BACKLOG) < 0) {
  		perror("Listen");
  		close(*o_socket);
		return -1;
  	}

  	server = gethostbyname(SERVER_IP);
	if (server == NULL) {
		perror("Get host by server_ip");
		close(*o_socket);
		return -1;
	}

	memset((char *)&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	memcpy((char *)&srv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	srv_addr.sin_port = htons(SERVER_PORT);

	if (connect(*o_socket, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0){
  		perror("Connect");
  		close(*o_socket);
		return -1;
	}

	return 0;
}

int client_run_job(int sockfd, char* job, char *ip, short port)
{
	message msg;
	int newsockfd;
	ssize_t ret = 0;
	struct sockaddr_in 	res_addr;
	int 				res_len = sizeof(res_addr);

	memcpy(msg.text, job, sizeof(msg.text));
	msg.header.source = SOURCE_CLIENT;
	memcpy(msg.header.ip_addr, ip, MAX_IP_ADDR);
	msg.header.port_addr = port;
	msg.header.req_type = TYPE_COMMAND;
	msg.header.job_id = 1;
	msg.header.length = strnlen(job, sizeof(msg.text));

	if (ret = send(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Send");
		return -1;
	}

	newsockfd = accept(sockfd, (struct sockaddr *)&res_addr, (socklen_t*)&res_len);
	if (newsockfd < 0) {
  		perror("Client accept");
  		return -1;
	}

	if (ret = recv(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Recv");
		return -1;
	}

	printf("%s\n", msg.text);

	return 0;
}

int client_run_status(int sockfd)
{
	return 0;
}
