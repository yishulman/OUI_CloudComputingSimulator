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
pthread_mutex_t 	resource_mutex;
msg_queue 			pending_queue;
resource 			*resources_table[MAX_RESOURCES] = {0};

void server_print_help()
{
	printf("USAGE: cloudsrv <port>\n");
}

static int server_add_resource(int newsockfd, int res_id, int cpu, int mem, resource **table)
{
	resource *r = NULL;
	int i;

	pthread_mutex_lock(&resource_mutex);

	for (i = 0; i < MAX_RESOURCES; i++) {
		if (table[i] == NULL) {
			table[i] = (resource*)malloc(sizeof(resource));
			r = table[i];
			break;
		}
	}

	if (!r) {
		fprintf(stderr, "Resource table full");
		return -1;
	}

	r->sock = newsockfd;
	r->res_id = res_id;
	r->memory = mem;
	r->cpu = cpu;
	r->score = mem / 8000 + cpu / 1000; 
	r->status = STATUS_AVAILABLE;

	pthread_mutex_unlock(&resource_mutex);

	printf("Resource %d registered. in table index %d\n", res_id, i);

	return 0;
}

static int server_remove_resource(int res_id, resource **table)
{
	for (int i = 0; i < MAX_RESOURCES; i++) {
		if (table[i] != NULL && table[i]->res_id == res_id) {
			free(table[i]);
			table[i] = NULL;
			break;
		}
	}

	printf("Resource %d unregistered.\n", res_id);

	return 0;
}

int server_handle_client_msg(int sockfd, msg_queue *queue, message *msg)
{
	ssize_t				ret;

	msg->header.req_type = TYPE_ACK;

	if (ret = send(sockfd, msg, sizeof(message), 0), ret <= 0) {
		perror("Recv");
		return -1;
	}
	
	
	printf("Registering new job : %s \n", msg->text);

	pthread_mutex_lock(&queue_mutex);
	msg_queue_push(queue, msg);
	pthread_mutex_unlock(&queue_mutex);

	return 0;
}

int server_handle_resource_msg(int newsockfd, resource **table, message *msg)
{
	static int res_id = 0;
	int ret = 0;

	switch(msg->header.req_type) {
		case TYPE_ADD_RESOURCE:
		ret = server_add_resource(newsockfd, res_id, msg->header.cpu, msg->header.memory, table);

		if (0 == ret) {
			msg->header.req_type = TYPE_ACK;
			msg->header.job_id = res_id;
			res_id++;
		}
		else
			msg->header.req_type = TYPE_NACK;

		if (ret = send(newsockfd, msg, sizeof(message), 0), ret <= 0) {
			perror("Recv");
			return -1;
		}
		break;

		case TYPE_REMOVE_RESOURCE:
		server_remove_resource(msg->header.job_id, table);
		break;

		case TYPE_JOB_DONE:
		
			printf("Resource ID %d to available \n",msg->header.job_id );
			pthread_mutex_lock(&resource_mutex);
			for (int i = 0; i < MAX_RESOURCES; i++) {
				if (resources_table[i] != NULL && resources_table[i]->status == STATUS_BUSY && resources_table[i]->res_id == msg->header.job_id) {
					resources_table[i]->status = STATUS_AVAILABLE;
				}
			}
			pthread_mutex_unlock(&resource_mutex);
		break;



		default:
		fprintf(stderr, "Resource sent invalid message type: %d\n", msg->header.req_type);
		ret = -1;
	}


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


		msg = malloc(sizeof(message));
		memset(msg, 0, sizeof(message));

		if (ret = recv(newsockfd, msg, sizeof(message), 0), ret <= 0)
			break;

		switch (msg->header.source) {
			case SOURCE_CLIENT:
			server_handle_client_msg(newsockfd, &pending_queue, msg);
			break;

			case SOURCE_RESOURCE:
			server_handle_resource_msg(newsockfd, resources_table, msg);
			break;

			default:
			break;
		}
	}
}

void server_tx(int sockfd)
{
	int i;
	message *msg;
	resource *r;
	ssize_t ret;
	
	while(1) {
		sleep(1);
		if (!msg_queue_is_empty(&pending_queue)) {
			pthread_mutex_lock(&queue_mutex);
			r = NULL;
			msg = msg_queue_pop(&pending_queue);
			pthread_mutex_unlock(&queue_mutex);

			pthread_mutex_lock(&resource_mutex);
			int max_score = 0;
			for (i = 0; i < MAX_RESOURCES; i++) {
				if (resources_table[i] != NULL && resources_table[i]->status == STATUS_AVAILABLE) {
					if (resources_table[i]->score > max_score) {
						r = resources_table[i];
						max_score = r->score;
					}
				}
			}
			// if we found a resources send the job
			if (r != NULL ) {
				r->status = STATUS_BUSY;
				printf("Sending Job to resource ID %d job %s \n",r->res_id, msg->text );
				printf("Resource ID %d to Busy \n",r->res_id );
				if (ret = send(r->sock, msg, sizeof(message), 0), ret <= 0) {
					perror("Recv");
					continue;
				}
			} else {
				pthread_mutex_lock(&queue_mutex);
				// push job back
				msg_queue_push_front(&pending_queue, msg);
				pthread_mutex_unlock(&queue_mutex);
			}
			pthread_mutex_unlock(&resource_mutex);
		}
	}
}
