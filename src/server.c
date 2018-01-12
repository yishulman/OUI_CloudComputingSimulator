#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "network.h"
#include "server.h"
#include "message.h"
#include "message_queue.h"
#include "resource.h"

pthread_mutex_t 	queue_mutex;
msg_queue 			pending_queue;
resource 			*resources_table[MAX_RESOURCES] = {0};

void server_print_help()
{
	printf("USAGE: cloudsrv <port>\n");
}

int server_handle_client_msg(int sockfd, msg_queue *queue, message *msg)
{
	ssize_t				ret;

	/* TODO: validate message here */
	msg->header.req_type = TYPE_ACK;

	if (ret = send(sockfd, &msg, sizeof(message), 0), ret <= 0) {
		perror("Recv");
		return -1;
	}

	pthread_mutex_lock(&queue_mutex);
	msg_queue_push(queue, msg);
	pthread_mutex_unlock(&queue_mutex);

	return 0;
}

int server_handle_resource_msg(resource **table, message *msg)
{
	resource *r;
	int i;

	/* TODO: validate message here */

	for (i = 0; i < MAX_RESOURCES; i++) {
		if (table[i] != NULL) {
			table[i] = (resource*)malloc(sizeof(resource));
			r = table[i];
		}
	}

	/* TODO: fill resource here */

	return 0;
}

void server_rx(int sockfd)
{
	ssize_t 			ret = 0;
	int  				newsockfd = 0;
	message 			*msg;
	struct sockaddr_in 	cli_addr;
	int 				cli_len = sizeof(cli_addr);

	msg_queue_init(&pending_queue);

	while(1) {
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&cli_len);
		if (newsockfd < 0) {
	  		perror("Accept");
	  		break;
		}

		printf("CONNECTED\n");

		msg = malloc(sizeof(message));
		memset(msg, 0, sizeof(message));

		if (ret = recv(newsockfd, msg, sizeof(message), 0), ret <= 0)
			break;

		switch (msg->header.source) {
			case SOURCE_CLIENT:
			server_handle_client_msg(newsockfd, &pending_queue, msg);
			break;

			case SOURCE_RESOURCE:
			server_handle_resource_msg(resources_table, msg);
			break;

			default:
			break;
		}
	}
}

void server_tx(int sockfd)
{
	message *msg;
	while(1) {
		sleep(1);
		if (!msg_queue_is_empty(&pending_queue)) {
			pthread_mutex_lock(&queue_mutex);
			msg = msg_queue_pop(&pending_queue);
			pthread_mutex_unlock(&queue_mutex);
			
			printf("TX: %s\n", msg->text);

			/* TODO: choose resource here and send the message to it. */
		}
	}
}