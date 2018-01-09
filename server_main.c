#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "server.h"

extern pthread_mutex_t 	queue_mutex;

static void* run_server_rx(void* arg) 
{
	server_rx(*((int*)arg));
	return NULL;
}

static void* run_server_tx(void* arg) 
{
	server_tx(*((int*)arg));
	return NULL;
}

int main(int argc, char *argv[])
{
	int 				sockfd = 0;
	pthread_t 			rx_thread, tx_thread;
	void 				*ret_thread;

	if (pthread_mutex_init(&queue_mutex, NULL)) {
		perror("Creating mutex");
		return -1;
	}

	if (sockfd = server_init(), sockfd < 0) {
		perror("server_init");
		return -1;
	}

	pthread_create(&rx_thread, NULL, run_server_rx, (void*)&sockfd);
	pthread_create(&tx_thread, NULL, run_server_tx, (void*)&sockfd);

	pthread_join(rx_thread, &ret_thread);
	pthread_join(tx_thread, &ret_thread);

	close(sockfd);
	return 0;
}