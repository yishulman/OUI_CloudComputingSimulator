#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network.h"
#include "server.h"

void print_help()
{
	printf("USAGE: cloudsrv <port>\n");
}

/**
 *	This function initialize the server's socket
 *	and binds it to the given port address.
 **/
int init_server(short port_addr)
{
	int 				sockfd = 0;
	struct sockaddr_in 	serv_addr;
	
	/* Open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Open socket");
		goto out_ret;
	}

	/* Initialize server address */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_addr);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind socket to address */
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
  		perror("Bind");
		goto out_err;
	}

	/* Listen to new connections */
  	if (listen(sockfd, SOCK_BACKLOG) < 0) {
  		perror("Listen");
  		goto out_err;
  	}

out_ret:
	return sockfd;

out_err:
	close(sockfd);
  	return -1;
}

/**
 * right now nothing. TODO: write a better comment.
 **/
int do_server(int sock_cli)
{
	char buffer[MTU];
	ssize_t ret = 0;

	while(1) {
		memset(buffer, 0, sizeof(buffer));

		if (ret = recv(sock_cli, buffer, sizeof(buffer), 0), ret <= 0)
			break;

		printf("RECV: %s", buffer);

		if(ret = send(sock_cli, "ACK", 4, 0), ret <= 0)
			break;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in 	cli_addr;
	int 				cli_len = sizeof(cli_addr);
	int 				newsockfd = 0;
	int 				sockfd = 0;
	short 				port_addr;

	if (argc < 2) {
		print_help();
		goto out_ret;
	}

	port_addr = (short)atoi(argv[1]);
	sockfd = init_server(port_addr);
	
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&cli_len);
	if (newsockfd < 0) {
  		perror("Accept");
  		goto out;
	}

	printf("CONNECTED\n");

	do_server(newsockfd);

out:
	close(sockfd);
out_ret:
	return 0;
}