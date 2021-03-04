#include <pthread.h>

#include "sock_lib.h"
#include "ecafe.h"
#include "browser.h"
#include <client.h>

#undef SERVER_PORT
#define SERVER_PORT "3030"

static pthread_t tid;
static int has_app_started;

static void ecafe_server_start(void);
static void *start_thread(void *arg);

void start_app(void)
{
	if (!has_app_started) {
		pthread_create(&tid, NULL, start_thread, NULL);
		pthread_detach(tid);
		has_app_started = 1;
	}
}

static void *start_thread(void *arg)
{
	ecafe_server_start();

	return NULL;
}

static void ecafe_server_start(void)
{
	int listenfd, connfd, maxfd;
	int nclients = 0, id = 0, nready;
	fd_set allset, rset;
	socklen_t socklen, cliaddr_len;
	struct client temp = {0}, **client_array;
	struct sockaddr_in cliaddr;

	listenfd = Tcp_listen("0.0.0.0", SERVER_PORT, &socklen);

	if (listenfd < 0) {
		fprintf(stderr, "Tcp_listen : %d Error \n", listenfd);
		exit(-1);
	}

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	maxfd = listenfd;

	clients_init();

	for (; ;)
	{
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {
			cliaddr_len = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len)) < 0) {
				perror("accept error");
				continue;
			}
			nclients++;
			temp.id = ++id;
			temp.fd = connfd;
			// @Danger: Add support for ipv6
			memcpy(&(temp.addr), &cliaddr, cliaddr_len);

			if (ecafe_getdetails(&temp) == -1) 
				fprintf(stderr, "ecafe_request_getdetails : error\n");

			client_add(&temp);
			FD_SET(connfd, &allset);

			if (connfd > maxfd)
				maxfd = connfd;

			nready--;
			puts("Client request accepted!");

			if (client_getall(&client_array) != nclients) {
				perror("client_getall error");
				continue;
			}

			for (int i = 0; i < nclients; ++i) {
				client_dump(client_array[i]);
			}
			if (nready == 0)
				continue;
		}

		if (client_is_dead(&rset, &allset) == 0) {  /* @Todo : Fix server infinite loop when client aborts*/
			nclients--;
		}

	}
}
