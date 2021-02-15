#ifndef __LIBSOCKET_H
#define __LIBSOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/ioctl.h>

#define MAXFD 	        64
#define MAXLINE         1024
#define BUFFSIZE        1024
#define LISTENQ         8
#define SERVER_IP       "127.0.0.1"
#define SERVER_PORT     3333
#define SA     			struct sockaddr

#define max(a, b)		(((a) > (b)) ? (a) : (b))
#define min(a, b)		(((a) < (b)) ? (a) : (b))

int Socket(int domain, int type, int protocol);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Inet_pton(int af, const char *src, void *dst);
int Tcp_listen(const char *host, const char *service, socklen_t *addrlen);
int Tcp_connect(const char *host, const char *service);
int Daemon_init(const char *ident);
int Uxd_listen(const char *path, int flags);
int Uxd_connect(const char *path);
int Fcntl(int, int, int);
void *Malloc(size_t size);
void *Calloc(size_t nmemb, size_t size);
int Ioctl(int fd, unsigned long request, void *ptr);

/**
 *
 * accepted(int connfd, (struct sockaddr *) &clientaddr, socklen_t len)
 * requested(int sockfd)  returns int  0=success, 1=FIN recieved, -1=error
 */
void Server_iomux(
	int listenfd,
	void (*accepted)(int, struct sockaddr *, socklen_t),
	int (*requested)(int));

#endif	/* LIBSOCKET end */
