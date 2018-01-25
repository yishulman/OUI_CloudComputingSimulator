#ifndef __SOCKET_WRAP_H
#define __SOCKET_WRAP_H

int socket_wrap_listen(int *socket, char *addr, short *port);
int socket_wrap_connect(int *socket, char *addr, short port);
int socket_wrap_accept(int socket, char *addr);

#endif