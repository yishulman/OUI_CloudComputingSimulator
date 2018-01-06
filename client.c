#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network.h"
#include "message.h"

void print_help()
{
	printf("USAGE: cloudcli <hostname> <command>\n");
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

int do_client(int sockfd, char* text) 
{
	message msg;
	ssize_t ret = 0;

	memcpy(msg.text, text, sizeof(msg.text));
	msg.header.source = SOURCE_CLIENT;

	printf("%s\n", msg.text);

	if (ret = send(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Send");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*hostname	= 0;
  	char 					text[MSG_TEXT_SIZE] = {0};

	if (argc < 3) {
		print_help();
		goto out_ret;
	}

	hostname = argv[1];

	snprintf(text, sizeof(text), "%s", argv[2]);
	for(int i = 3; i < argc; i++) {
		snprintf(text, sizeof(text), "%s %s", text, argv[i]);
	}

	sockfd = init_client(hostname, SERVER_PORT);

	printf("CONNECTED\n");

	do_client(sockfd, text);

out:
	close(sockfd);
out_ret:
	return 0;
}

