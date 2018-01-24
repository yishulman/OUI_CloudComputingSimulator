#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "message.h"
#include "resource.h"
#include "socket_wrap.h"

static void *run_resource_run(void *args)
{
	int **iarg = (int**)args;
	resource_run(iarg[0], *iarg[1]);
	return NULL;
}

int main(int argc, char *argv[])
{
  	int 					sockfd 		= 0;
  	short 					port_addr 	= 0;
  	char					*client_ip	= 0;
  	char 					text[MSG_TEXT_SIZE] = {0};
  	int 					*args[2];
  	pthread_t 				thread;
  	void 					*ret_thread;
  	char 					c;
  	int 					res_id;
  	int 					*running = malloc(sizeof(int));
	int					cpu;
	int					mem;

	if (argc < 3) {
		resource_print_help();
		printf("Running with default cpu and mem, 2GHz 8GB");
		cpu = 2000;
		mem = 8000;
	} else {
		sscanf(argv[1], "%d" ,&cpu );
		sscanf(argv[2], "%d" ,&mem );
	}

	if (socket_wrap_connect(&sockfd, SERVER_IP, SERVER_PORT)) {
		fprintf(stderr, "Resource init failed.\n");
		goto out_ret;
	}

	printf("CONNECTED\n");

	if (res_id = resource_register(sockfd, cpu, mem), res_id < 0) {
		fprintf(stderr, "Registration failed.\n");
		goto out;
	}

	args[0] = running;
	args[1] = &sockfd;

  	*running = 1;
	pthread_create(&thread, NULL, run_resource_run, (void*)args);

	printf("Resource is running. Press any key to exit.\n");

	scanf("%c", &c);
	printf("Exiting\n");
	*running = 0;
	close(sockfd);


	if (socket_wrap_connect(&sockfd, SERVER_IP, SERVER_PORT)) {
		fprintf(stderr, "Resource init failed.\n");
		goto out_ret;
	}

	resource_unregister(res_id, sockfd);

	free(running);

out:
	close(sockfd);
	printf("Resource shutdown\n");
out_ret:
	
	return 0;
}
