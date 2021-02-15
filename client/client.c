#include <ecafe_request.h>
#include <sock_lib.h>

int main(int argc, char *argv[])
{
	int sockfd, nbytes, nfds, nready;
	char buf[MAXLINE], *ip, port_str[10];
	fd_set rset;

	if (argc != 2) {
		fprintf(stderr, "Usage : %s <IP>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	ip = argv[1];
	sprintf(port_str, "%d", SERVER_PORT);
	if ((sockfd = Tcp_connect(ip, port_str)) < 0) {
		fprintf(stderr, "Error: Tcp_connect\n");
		exit(1);
	}

	FD_ZERO(&rset);

	for (;;) {

		FD_SET(sockfd, &rset);

		nfds = sockfd + 1;
		nready = select(nfds, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {
			// Echoing back
			if ((nbytes = read(sockfd, buf, sizeof(buf) - 1)) == 0) {
				perror("read error: server terminated prematurely");
				exit(0);
			}
			// Check for any read error
			if (nbytes < 0) {
				perror("read error");
				exit(0);
			}

			buf[nbytes] = '\0';
			puts(buf);

			if (ecafe_request_handle(buf, sockfd) == -1) {
				fprintf(stderr, "ecafe_request_handle : error\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	return 0;
}
