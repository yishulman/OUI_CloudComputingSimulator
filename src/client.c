#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "network.h"
#include "message.h"

void client_job_print_help()
{
	printf("USAGE: run_job <client_ip> <command>\n");
}

void client_status_print_help()
{
	printf("USAGE: run_status <client_ip>\n");
}

int client_run_job(int sockfd, char* job, char *ip, short port)
{
	message msg;
	int newsockfd;
	ssize_t ret = 0;

	memcpy(msg.text, job, sizeof(msg.text));
	msg.header.source = SOURCE_CLIENT;
	memcpy(msg.header.ip_addr, ip, MAX_IP_ADDR);
	msg.header.port_addr = port;
	msg.header.req_type = TYPE_COMMAND;
	msg.header.job_id = 1;
	msg.header.length = strnlen(job, sizeof(msg.text));

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
		printf("Server received the command successfully.\n");
		break;

		case TYPE_NACK:
		fprintf(stderr, "Server rejected the command. reason: %s\n", msg.text);
		return -1;

		default:
		fprintf(stderr, "Received invalid message type: %d.\n", msg.header.req_type);
		return -1;
	}

	return 0;
}

int client_recv_res(int sockfd)
{
	message msg;
	ssize_t ret = 0;

	if (ret = recv(sockfd, &msg, sizeof(msg), 0), ret <= 0) {
		perror("Recv");
		return -1;
	}

	printf("%s\n", msg.text);

	return 0;
}

int client_run_status(int sockfd)
{
	return 0;
}
