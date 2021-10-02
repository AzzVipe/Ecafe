#include <ecafe_response.h>

int ecafe_response_send(struct response *res, int connfd)
{
	int nbytes = 0;
	char buf[1024 * 10];

	if((nbytes = response_prepare(res, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "response_parse : error\n");
		return -1;
	}

	puts(buf);

	if (write(connfd, buf, nbytes) == -1) {
		perror("write error");
		return -1;
	}
	
	return nbytes;
}

int ecafe_response_recv(int connfd, struct response *res)
{
	int nbytes = 0;
	char buf[1024 * 10];

	if ((nbytes = read(connfd, buf, sizeof(buf))) == -1) {
		perror("read error");
		return -1;
	}
	
	if (nbytes == 0)
		return 0;
	
	puts(buf);

	if(response_parse(buf, nbytes, res) == -1) {
		fprintf(stderr, "response_parse : error\n");
		return -1;
	}

	return nbytes;
}