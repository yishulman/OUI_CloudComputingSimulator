#include <stdio.h>
#include <string.h>
#include "network.h"

int socket_wrap_listen(int *sock, char *addr, short *port)
{
	struct sockaddr_in 	sock_addr;
  	struct hostent		*host;
  	socklen_t			addr_len = sizeof(sock_addr);
  	int 				option = 1;

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock < 0) {
  		perror("Opening socket");
  		return -1;
	}
	
	/*if (setsockopt(*sock, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option))) {
		perror("setsockopt");
		close(*sock);
  		return -1;
	} */

	host = gethostbyname(addr);
	if (host == NULL) {
		perror("Get host by address");
		close(*sock);
		return -1;
	}

	/* Initialize host address */
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(*port);
	if (addr == INADDR_ANY)
		sock_addr.sin_addr.s_addr = INADDR_ANY;
	else
		memcpy((char *)&sock_addr.sin_addr.s_addr, (char *)host->h_addr, host->h_length);

	/* Bind socket to address */
	if (bind(*sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
  		perror("Bind");
		close(*sock);
		return -1;
	}

	if (getsockname(*sock, (struct sockaddr*)&sock_addr, &addr_len))
	{
		perror("getsockname");
		close(*sock);
		return -1;
	}

	*port = ntohs(sock_addr.sin_port);

	/* Listen to new connections */
  	if (listen(*sock, SOCK_BACKLOG) < 0) {
  		perror("Listen");
  		close(*sock);
		return -1;
  	}

	return 0;
}

int socket_wrap_connect(int *sock, char *addr, short port)
{
	struct sockaddr_in 	sock_addr;
  	struct hostent		*host;
  	socklen_t			addr_len = sizeof(sock_addr);

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock < 0) {
  		perror("Opening socket");
  		return -1;
	}

	host = gethostbyname(addr);
	if (host == NULL) {
		perror("Get host by address");
		close(*sock);
		return -1;
	}

	memset((char *)&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	memcpy((char *)&sock_addr.sin_addr.s_addr, (char *)host->h_addr, host->h_length);
	sock_addr.sin_port = htons(port);

	if (connect(*sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0){
  		perror("Connect");
  		close(*sock);
		return -1;
	}

	return 0;
}

int socket_wrap_accept(int sock, char *addr)
{
	int 				newsockfd;
	struct sockaddr_in 	cli_addr;
	int 				cli_len = sizeof(cli_addr);

	newsockfd = accept(sock, (struct sockaddr *)&cli_addr, (socklen_t*)&cli_len);
	if (newsockfd < 0) {
		perror("Accept");
		return -1;
	}

	if (addr)
		memcpy(addr, &cli_addr.sin_addr.s_addr, sizeof(cli_addr.sin_addr.s_addr));

	return newsockfd;
}