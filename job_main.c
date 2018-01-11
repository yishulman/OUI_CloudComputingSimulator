#include <stdio.h>
#include "message.h"
#include "client.h"

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*client_ip	= 0;
  	char 					job[MSG_TEXT_SIZE] = {0};

	if (argc < 3) {
		client_job_print_help();
		goto out_ret;
	}

	client_ip = argv[1];

	snprintf(job, sizeof(job), "%s", argv[2]);
	for(int i = 3; i < argc; i++) {
		snprintf(job, sizeof(job), "%s %s", job, argv[i]);
	}

	if (client_init(client_ip, &sockfd, &port_addr)) {
		fprintf(stderr, "Client init failed.\n");
		goto out_ret;
	}

	printf("CONNECTED\n");

	client_run_job(sockfd, job, client_ip, port_addr);

out:
	close(sockfd);
out_ret:
	return 0;
}