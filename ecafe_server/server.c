#include <sock_lib.h>
#include <ecafe.h>
#include <client.h>

#undef SERVER_PORT
#define SERVER_PORT "3030"
#define SERVER_PORT2 "4040"

static struct client temp = {0}, **client_array;
static struct sockaddr_in cliaddr, servaddr;

char *inet_ip(struct sockaddr *addr);

int main(void)
{
	char buf[1024];
	int c_listenfd, s_listenfd, c_connfd, s_connfd, maxfd; /*s prefix is for server and c prefix is for client*/
	int nclients = 0, id = 0, nready, nbytes;
	fd_set allset, rset;
	socklen_t c_socklen,s_socklen, cliaddr_len, servaddr_len;

	s_listenfd = Uxd_listen(UNIXPATH, 0);
	c_listenfd = Tcp_listen("0.0.0.0", SERVER_PORT, &c_socklen);

	if (s_listenfd < 0 || c_listenfd < 0) {
		fprintf(stderr, "Tcp_listen : %d %d Error \n", c_listenfd, s_listenfd);
		exit(-1);
	}

	FD_ZERO(&allset);
	FD_SET(s_listenfd, &allset);
	FD_SET(c_listenfd, &allset);

	maxfd = max(s_listenfd, c_listenfd);

	clients_init();

	fprintf(stderr, "Server Started....\n");


	for (; ;)
	{
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* Connection request of webserver/server */

		if (FD_ISSET(s_listenfd, &rset)) {
			// 	continue;

			servaddr_len = sizeof(servaddr);
			if ((s_connfd = accept(s_listenfd, (struct sockaddr *) &servaddr, &servaddr_len)) < 0) {
				perror("accept error");
				continue;
			}
			FD_SET(s_connfd, &allset);

			if (s_connfd > maxfd)
				maxfd = s_connfd;

			fprintf(stderr, "Test Server Connected !\n\n");
			
			if (--nready == 0)
				continue;
		}

		/* Connection requests of clients */

		if (FD_ISSET(c_listenfd, &rset)) {
			cliaddr_len = sizeof(cliaddr);
			if ((c_connfd = accept(c_listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len)) < 0) {
				perror("accept error");
				continue;
			}
			puts("Client request accepted!\n");
			nclients++;
			temp.id = ++id;
			temp.fd = c_connfd;
			temp.is_online = true;
			// @Danger: Add support for ipv6
			memcpy(&(temp.addr), &cliaddr, cliaddr_len);
			temp.ip = inet_ip((struct sockaddr *) &cliaddr);
			client_add(&temp);
			ecafe_getdetails(&temp);
			
			FD_SET(c_connfd, &allset);

			if (c_connfd > maxfd)
				maxfd = c_connfd;


			if (client_getall(&client_array) != nclients) {
				perror("client_getall error");
				continue;
			}

			for (int i = 0; i < nclients; ++i) 
				client_dump(client_array[i]);
			
			if (--nready == 0)
				continue;
		}
		/* @TODO: Implement non-blocking and timeout in read system calls */

		/* Requests from server */

		if (FD_ISSET(s_connfd, &rset)) {

			// Test server operations
			if ((nbytes = read(s_connfd, buf, sizeof(buf))) == 0) {
				fprintf(stderr, "Test Sever %d Terminated\n", s_connfd);
				close(s_connfd);
				FD_CLR(s_connfd, &allset);

			} else if (nbytes > 0) {
				buf[nbytes] = '\0';
				ecafe_request_handle(buf, s_connfd);
			}
			nready--;
		}

		/* Responses from clients */

		for (int i = 0; i < FD_SETSIZE && nready != 0; ++i) {
			if (i < nclients && FD_ISSET(client_array[i]->fd, &rset)) {
				client_active_set(client_array[i]);
				// Client read operations
				if ((nbytes = read(client_array[i]->fd, buf, sizeof(buf))) == 0) {
					fprintf(stderr, "Client %d Terminated\n", client_array[i]->id);
					nclients--;
					FD_CLR(client_array[i]->fd, &allset);
					close(client_array[i]->fd);
					client_remove(client_array[i]->id);
				} else if (nbytes > 0) {
					buf[nbytes] = '\0';
					ecafe_response_handle(buf, s_connfd);
				}
				
				nready--;
			}
		}
	}

	return 0;
}

char *inet_ip(struct sockaddr *addr)
{
	static char ip[256];
	struct sockaddr_in *taddr;

	switch(addr->sa_family) {
		case AF_INET : 
			taddr = (struct sockaddr_in *)addr;
			inet_ntop(AF_INET, &taddr->sin_addr, ip, sizeof(ip));
			return ip;

		case AF_INET6 : 
			taddr = (struct sockaddr_in *)addr;
			inet_ntop(AF_INET6, &taddr->sin_addr, ip, sizeof(ip));
			return ip;

		default : return "UNSUPPORTED";
	}
}