#include <stdio.h>
#include "message.h"
#include "client.h"

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*hostname	= 0;
  	char 					text[MSG_TEXT_SIZE] = {0};

	if (argc < 3) {
		client_print_help();
		goto out_ret;
	}

	hostname = argv[1];

	snprintf(text, sizeof(text), "%s", argv[2]);
	for(int i = 3; i < argc; i++) {
		snprintf(text, sizeof(text), "%s %s", text, argv[i]);
	}

	sockfd = client_init(hostname, SERVER_PORT);

	printf("CONNECTED\n");

	client_do(sockfd, text);

out:
	close(sockfd);
out_ret:
	return 0;
}