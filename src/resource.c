#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "network.h"
#include "message.h"
#include "resource.h"
#include "socket_wrap.h"

extern int running;

int resource_id = 0;

void resource_print_help()
{
	printf("USAGE: run_resource <cpu_speed> <mem_size>\n");
}


int resource_register(int sockfd, u16 cpu, u16 mem)
{
	message msg;
	ssize_t ret;

	message_fill(&msg, SOURCE_RESOURCE, TYPE_ADD_RESOURCE, NULL, 0,cpu, mem, 0, NULL);

	if (ret = send(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Send");
		return -1;
	}

	if (ret = recv(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Recv");
		return -1;
	}

	switch (msg.header.req_type) {
		case TYPE_ACK:
		resource_id = msg.header.job_id;
		printf("Registered successfully resource ID %d .\n", resource_id);
		break;

		case TYPE_NACK:
		fprintf(stderr, "Server rejected. reason: %s\n", msg.text);
		return -1;

		default:
		fprintf(stderr, "Received invalid message type: %d.\n", msg.header.req_type);
		return -1;
	}

	return resource_id;
}

void resource_unregister(int res_id, int sockfd)
{
	message msg;
	ssize_t ret;

	message_fill(&msg, SOURCE_RESOURCE, TYPE_REMOVE_RESOURCE, NULL, 0, 0, 0, res_id, NULL);

	if (ret = send(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Send");
	}
}

int resource_run(int *running, int sockfd)
{
	message msg;
	ssize_t ret;
	int client_sock;
	int server_sock;
	pid_t child_pid;
	int child_status;
	FILE *fp;
	char cmd[sizeof(msg.text) + 4];

	while (*running) {
		if (ret = recv(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
			continue;
		}

		printf("Recv command: %s\n", msg.text);

		/* run command on shell and get the output */
  		fp = popen(msg.text, "r");
		

		if (socket_wrap_connect(&client_sock, (char*)msg.header.ip_addr, msg.header.port_addr)) {
			continue;
		}
  		memset(msg.text, 0, sizeof(msg.text));
		msg.header.req_type = TYPE_OUTPUT_STREAM;
 		while(fread(msg.text, 1, sizeof(msg.text), fp)) {
			
			if (ret = send(client_sock, &msg, sizeof(msg), 0), ret <= 0) {
				perror("Send");
				continue;
			}
  			memset(msg.text, 0, sizeof(msg.text));
		};
  	
		pclose(fp);


		msg.header.req_type = TYPE_JOB_DONE;

		if (ret = send(client_sock, &msg, sizeof(msg), 0), ret <= 0) {
			perror("Send");
			continue;
		}
		
		close(client_sock);

		if (socket_wrap_connect(&server_sock, SERVER_IP, SERVER_PORT)) {
			fprintf(stderr, "Resource server job done failed.\n");
		}
		// Send job done to server as well
		msg.header.source = SOURCE_RESOURCE;
		msg.header.job_id = resource_id;
		if (ret = send(server_sock, &msg, sizeof(msg), 0), ret <= 0) {
			perror("Send");
			continue;
		}
		close(server_sock);

	}

	return 0;
}
