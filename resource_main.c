#include <stdio.h>
#include <string.h>
#include "message.h"
#include "resource.h"
#include "socket_wrap.h"

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*client_ip	= 0;
  	char 					text[MSG_TEXT_SIZE] = {0};

	if (socket_wrap_connect(&sockfd, SERVER_IP, SERVER_PORT)) {
		fprintf(stderr, "Resource init failed.\n");
		goto out_ret;
	}

	printf("CONNECTED\n");

	if (resource_register(sockfd)) {
		fprintf(stderr, "Registration failed.\n");
		goto out;
	}

	if (resource_run(sockfd)) {
		fprintf(stderr, "Running failed.\n");
		goto out;
	}

out:
	close(sockfd);
out_ret:
	return 0;
}