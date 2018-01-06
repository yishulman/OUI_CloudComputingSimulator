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

void print_help()
{
	printf("USAGE: cloudsrv <port>\n");
}

/**
 *	This function initialize the server's socket
 *	and binds it to the given port address.
 **/
int init_server(short port_addr)
{
	int 				sockfd = 0;
	struct sockaddr_in 	serv_addr;
	
	/* Open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Open socket");
		goto out_ret;
	}

	/* Initialize server address */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_addr);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind socket to address */
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
  		perror("Bind");
		goto out_err;
	}

	/* Listen to new connections */
  	if (listen(sockfd, SOCK_BACKLOG) < 0) {
  		perror("Listen");
  		goto out_err;
  	}

out_ret:
	return sockfd;

out_err:
	close(sockfd);
  	return -1;
}

int handle_client_msg(msg_queue *queue, message *msg)
{
	/* TODO: validate message here */


	pthread_mutex_lock(&queue_mutex);
	msg_queue_push(queue, msg);
	pthread_mutex_unlock(&queue_mutex);

	return 0;
}

int handle_resource_msg(resource **table, message *msg)
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

static void* run_server_rx(void* arg) 
{
	server_rx(*((int*)arg));
	return NULL;
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
			handle_client_msg(&pending_queue, msg);
			break;

			case SOURCE_RESOURCE:
			handle_resource_msg(resources_table, msg);
			break;

			default:
			break;
		}
	}
}

static void* run_server_tx(void* arg) 
{
	server_tx(*((int*)arg));
	return NULL;
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
			
			printf("TX: %s", msg->text);

			/* TODO: choose resource here and send the message to it. */
		}
	}
}

int main(int argc, char *argv[])
{
	int 				sockfd = 0;
	short 				port_addr;
	pthread_t 			rx_thread, tx_thread;
	void 				*ret_thread;

	if (argc < 2) {
		print_help();
		return -1;
	}

	if (pthread_mutex_init(&queue_mutex, NULL)) {
		perror("Creating mutex");
		return -1;
	}

	port_addr = (short)atoi(argv[1]);
	if (sockfd = init_server(port_addr), sockfd < 0) {
		perror("init_server");
		return -1;
	}

	pthread_create(&rx_thread, NULL, run_server_rx, (void*)&sockfd);
	pthread_create(&tx_thread, NULL, run_server_tx, (void*)&sockfd);

	pthread_join(rx_thread, &ret_thread);
	pthread_join(tx_thread, &ret_thread);

	close(sockfd);
	return 0;
}