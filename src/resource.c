#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "network.h"
#include "message.h"
#include "resource.h"
#include "socket_wrap.h"

extern int running;

int resource_register(int sockfd)
{
	message msg;
	ssize_t ret;

	message_fill(&msg, SOURCE_RESOURCE, TYPE_ADD_RESOURCE, NULL, 0, 0, 0, NULL);

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
		printf("Registered successfully.\n");
		break;

		case TYPE_NACK:
		fprintf(stderr, "Server rejected. reason: %s\n", msg.text);
		return -1;

		default:
		fprintf(stderr, "Received invalid message type: %d.\n", msg.header.req_type);
		return -1;
	}

	return msg.header.job_id;
}

void resource_unregister(int res_id, int sockfd)
{
	message msg;
	ssize_t ret;

	message_fill(&msg, SOURCE_RESOURCE, TYPE_REMOVE_RESOURCE, NULL, 0, 0, res_id, NULL);

	if (ret = send(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Send");
	}
}

int resource_run(int *running, int sockfd)
{
	message msg;
	ssize_t ret;
	int client_sock;
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
  		memset(msg.text, 0, sizeof(msg.text));
 		if (fread(msg.text, 1, sizeof(msg.text), fp) <= 0)
 			sprintf(msg.text, "Invalid command.");
  		pclose(fp);

		if (socket_wrap_connect(&client_sock, (char*)msg.header.ip_addr, msg.header.port_addr)) {
			continue;
		}

		msg.header.req_type = TYPE_JOB_DONE;

		if (ret = send(client_sock, &msg, sizeof(msg), 0), ret <= 0) {
			perror("Send");
			continue;
		}
	}

	return 0;
}