#include <stdio.h>
#include <string.h>
#include "message.h"
#include "client.h"
#include "socket_wrap.h"

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*client_ip	= 0;
  	char 					text[MSG_TEXT_SIZE] = {0};

	if ((argc < 2) || (strncmp(argv[1], "-h", 2))){
		client_status_print_help();
		goto out_ret;
	}

	client_ip = argv[1];

	if (socket_wrap_listen(&sockfd, client_ip, &port_addr)) {
		fprintf(stderr, "Client init failed.\n");
		goto out_ret;
	}

	printf("CONNECTED\n");

	client_run_status(sockfd);

out:
	close(sockfd);
out_ret:
	return 0;
}