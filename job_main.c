#include <stdio.h>
#include "message.h"
#include "client.h"
#include "socket_wrap.h"

int main(int argc, char *argv[])
{
  	int 					sock_listen 		= 0;
  	int 					sock_connect		= 0;
  	int 					sock_res;
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

	if (socket_wrap_listen(&sock_listen, client_ip, &port_addr)) {
		fprintf(stderr, "Client init failed.\n");
		goto out_ret;
	}

	if (socket_wrap_connect(&sock_connect, SERVER_IP, SERVER_PORT)) {
		fprintf(stderr, "Client init failed.\n");
		goto out_ret;
	}

	printf("CONNECTED\n");

	if (client_run_job(sock_connect, job, client_ip, port_addr)) {
		fprintf(stderr, "Sending job failed.\n");
		goto out1;
	}

	if (sock_res = socket_wrap_accept(sock_listen, NULL), sock_res < 0) {
		fprintf(stderr, "Accepting resource failed.\n");
		goto out1;
	}

	if (client_recv_res(sock_res)) {
		fprintf(stderr, "Sending job failed.\n");
		goto out2;
	}

out2:
	close(sock_res);
out1:
	close(sock_listen);
	close(sock_connect);
out_ret:
	return 0;
}