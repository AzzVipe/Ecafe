#include "sock_lib.h"
#include <errno.h>
#include <syslog.h>

static int tcp_connect(const char *host, const char *service);
static int tcp_listen(const char *host, const char *service, socklen_t *addrlen);

static void server_iomux(
	int listenfd,
	void (*accepted)(int, struct sockaddr *, socklen_t),
	int (*requested)(int))
{
	int maxfd, nready, active_conn, connfd;
	fd_set rset, allset;
	struct sockaddr_in client;
	socklen_t clientlen;
	int clientfds[FD_SETSIZE];

	active_conn = 0;

	maxfd = listenfd;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	/* Initialize clientfds */
	for (int i = 0; i < FD_SETSIZE; ++i)
		clientfds[i] = -1;

	fprintf(stderr, "Server running...\n");
	for (; ;) {
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* If new connection arrived then handle it */
		if (FD_ISSET(listenfd, &rset)) {
			fprintf(stderr, "Server_iomux: New connection arrived..\n");
			connfd = accept(listenfd, (struct sockaddr *) &client, &clientlen);
			if (connfd == -1) {
				perror("accept error");
			}

			/* If connection limit exceeds then reject new connection */
			if (active_conn == FD_SETSIZE) {
				fprintf(stderr, "Warning: Connection limit hit, rejecting new connection.\n");
				/* TODO: Notify client about connection rejection */
				continue;
			}

			/* Call user's accepted hook */
			accepted(connfd, (struct sockaddr *) &client, clientlen);

			FD_SET(connfd, &allset);
			++active_conn;
			for (int i = 0; i < FD_SETSIZE; ++i) {
				if (clientfds[i] == -1) {
					clientfds[i] = connfd;
					break;
				}
			}

			/* Increment maxfd if required */
			if (connfd > maxfd)
				maxfd = connfd;

			if (--nready == 0)
				continue;
		}

		/* Handle existing connection */
		fprintf(stderr, "Server_iomux: Handling request..\n");
		for (int i = 0; i < FD_SETSIZE; ++i) {
			if (clientfds[i] == -1)
				continue;

			if (FD_ISSET(clientfds[i], &rset)) {
				/* @TODO If requested hook is NULL then handle the request here, only
				 * handle the FIN packet otherwise silently ignore request */

				int ret;
				/* If connection closed */
				if ((ret = requested(clientfds[i])) == 1) {
					FD_CLR(clientfds[i], &allset);
					clientfds[i] = -1;
					--active_conn;
				}
			}
		}
	}
}

static int tcp_listen(const char *host, const char *service, socklen_t *addrlen)
{
	int listenfd, n, option = 1;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, service, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_listen: getaddrinof error: %s\n", gai_strerror(n));
		exit(1);
	}

	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue; /* ignore */
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break; /* success */

		close(listenfd); /* bind error, close and try next one */
	} while ((res = res->ai_next));

	if (res == NULL) {
		perror("tcp_listen");
		fprintf(stderr, "tcp_listen error for %s, %s\n", host, service);
		exit(1);
	}

	Listen(listenfd, LISTENQ);
	
	if (addrlen)
		*addrlen = res->ai_addrlen;

	freeaddrinfo(ressave);

	return listenfd;
}

static int tcp_connect(const char *host, const char *service)
{
	struct addrinfo hints, *res;
	int ret, sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags 		= AI_CANONNAME;
	hints.ai_family 	= AF_INET;
	hints.ai_socktype 	= SOCK_STREAM;

	if ((ret = getaddrinfo(host, service, &hints, &res)) != 0) {
		fprintf(stderr, "Error: getaddrinfo: %s\n", gai_strerror(ret));
		return -ret;
	}

	sockfd = -1;
	struct addrinfo *tmp = res;
	while (tmp != NULL) {
		sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (sockfd < 0)
			continue;
		if (connect(sockfd, (struct sockaddr *) tmp->ai_addr,
					tmp->ai_addrlen) == 0) {
			break;
		}

		perror("Error: connect");
		close(sockfd);
		tmp = tmp->ai_next;
	}

	if (tmp == NULL)
		sockfd = -1;

	freeaddrinfo(res);
	
	return sockfd;
}

int Socket(int domain, int type, int protocol)
{
	int sockfd;
	if ((sockfd = socket(domain, type, protocol)) < 0) {
		int err_no = errno;
		perror("socket error");
		exit(-err_no);
	}

	return sockfd;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	if (connect(sockfd, addr, addrlen) < 0) {
		int err_no = errno;
		perror("connect error");
		exit(-err_no);
	}

	return 0;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	if (bind(sockfd, addr, addrlen) < 0) {
		int err_no = errno;
		perror("bind error");
		exit(-err_no);
	}

	return 0;
}

int Listen(int sockfd, int backlog)
{
	if (listen(sockfd, backlog) < 0) {
		int err_no = errno;
		perror("listen error");
		exit(-err_no);
	}

	return 0;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int connfd;
	if ((connfd = accept(sockfd, addr, addrlen)) < 0) {
		int err_no = errno;
		perror("accept error");
		exit(-err_no);
	}

	return connfd;
}

int Inet_pton(int af, const char *src, void *dst)
{
	if (inet_pton(af, src, dst) <= 0) {
		perror("inet_pton error");
		exit(-errno);
	}

	return 1;
}

int Tcp_listen(const char *host, const char *service, socklen_t *addrlen)
{
	return tcp_listen(host, service, addrlen);
}

int Tcp_connect(const char *host, const char *service)
{
	return tcp_connect(host, service);
}

int Daemon_init(const char *ident)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		return -1;
	else if(pid) 				/* Exit parent */
		exit(0);

	/* Child 1 continues... */

	if (setsid() < 0) /* Become session leader */
		return -1;

	/* Ignore SIGHUP signal to ignore process termination signal when
	 * child 1 terminates. When session leader terminates then SIGHUP is
	 * sent to all the children causing all children to terminates thats why
	 * ignoring SIGHUP signal is required. */
	signal(SIGHUP, SIG_IGN);
	if ((pid = fork()) < 0)
		return -1;
	else if (pid)
		exit(0); 				/* Exit child 1 */

	/* Child 2 continues... */

	chdir("/"); 				/* Change working directory */

	/* Close off file descriptors */
	for (int i = 0; i < MAXFD; ++i)
		close(i);

	/* Redirect stdin, stdout and stderr to /dev/null */
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(ident, LOG_PID, 0);

	return 0;
}

int Uxd_listen(const char *path, int flags)
{
	int sockfd;
	struct sockaddr_un addr;

	if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, flags)) == -1) {
		perror("Uxd_connect error");
		return -1;
	}

	/* Delete the path if exists, and ignore the return value */
	unlink(path);

	if (strlen(path) > sizeof(addr.sun_path)) {
		/* Set appropriate errno and return */
		/* TODO */
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, path);

	Bind(sockfd, (struct sockaddr *) &addr, SUN_LEN(&addr));
	Listen(sockfd, LISTENQ);

	return sockfd;
}

int Uxd_connect(const char *path)
{
	int sockfd;
	struct sockaddr_un addr;

	if (strlen(path) > sizeof(addr.sun_path)) {
		/* Set appropriate errno and return */
		/* TODO */
		fprintf(stderr, "path length is large\n");
		return -1;
	}

	sockfd = Socket(AF_LOCAL, SOCK_STREAM, 0);

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, path);

	Connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

	return sockfd;
}

void Server_iomux(
	int listenfd,
	void (*accepted)(int, struct sockaddr *, socklen_t),
	int (*requested)(int))
{
	server_iomux(listenfd, accepted, requested);
}

int Fcntl(int fd, int cmd, int arg)
{
	int n;

	if ((n = fcntl(fd, cmd, arg)) == -1) {
		perror("fcntl error");
		exit(n);
	}

	return n;
}

void *Malloc(size_t size)
{
	char *buf = malloc(size);
	if (buf == NULL) {
		perror("malloc error");
		exit(1);
	}

	return buf;
}

void *Calloc(size_t nmemb, size_t size)
{
	char *buf = calloc(nmemb, size);
	if (buf == NULL) {
		perror("calloc error");
		exit(1);
	}

	return buf;
}

int Ioctl(int fd, unsigned long request, void *ptr)
{
	int n;

	if ((n = ioctl(fd, request, ptr)) == -1) {
		perror("ioctl error");
		exit(1);
	}

	return n;
}
