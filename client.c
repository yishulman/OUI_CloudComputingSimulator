#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network.h"

void print_help()
{
	printf("USAGE: cloudcli <hostname> <port>\n");
}

/**
 *	This function initialize the client's socket
 *	and connects to the server.
 **/
int init_client(char *hostname, short port_addr)
{
	int 				sockfd;
  	struct sockaddr_in 	srv_addr;
  	struct hostent 		*server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
  		perror("Opening socket");
  		goto out_ret;
	}

  	server = gethostbyname(hostname);
	if (server == NULL) {
		perror("Get host by hostname");
		goto out_err;
	}

	memset((char *)&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr, (char *)&srv_addr.sin_addr.s_addr, server->h_length);
	srv_addr.sin_port = htons(port_addr);

	if (connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0){
  		perror("Connect");
  		goto out_err;
	}

out_ret:
	return sockfd;

out_err:
	close(sockfd);
	return -1;
}

int do_client(int sockfd) 
{
	char buffer[MTU];
	ssize_t ret = 0;

	while(1) {
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);

		if (ret = send(sockfd, buffer, sizeof(buffer), 0), ret <= 0)
			break;

		memset(buffer, 0, sizeof(buffer));

		if(ret = recv(sockfd, buffer, sizeof(buffer), 0), ret <= 0)
			break;

		printf("RECV: %s \n", buffer);
	}

	return 0;
}

int main(int argc, char *argv[])
{
  int 					sockfd 		= 0;
  short 				port_addr 	= 0;
  char					*hostname	= 0;

	if (argc < 3) {
		print_help();
		goto out_ret;
	}

	hostname = argv[1];
	port_addr = (short)atoi(argv[2]);

	sockfd = init_client(hostname, port_addr);

	printf("CONNECTED\n");

	do_client(sockfd);

out:
	close(sockfd);
out_ret:
	return 0;
}

