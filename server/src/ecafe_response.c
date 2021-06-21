#include <ecafe_response.h>

int ecafe_response_send(struct response *res, int connfd)
{
	int nbytes = 0, rv = 0;
	char buf[1024 * 10];

	if((nbytes = response_prepare(res, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "response_parse : error\n");
		return -1;
	}

	puts(buf);

	if ((rv = write(connfd, buf, nbytes)) == -1) {
		perror("write error");
		return -1;
	}
	
	if (rv != nbytes)
		return -1;

	return nbytes;
}

int ecafe_response_recv(int connfd, struct response *res)
{
	int nbytes = 0, rv = 0;
	char buf[1024 * 10];

	if ((nbytes = read(connfd, buf, sizeof(buf))) == -1) {
		perror("read error");
		return -1;
	}
	
	if (nbytes == 0)
		return 0;
	
	puts(buf);

	if((rv = response_parse(buf, nbytes, res)) == -1) {
		fprintf(stderr, "response_parse : error\n");
		return -1;
	}

	return nbytes;
}

int ecafe_response_print(struct response *res, FILE *fp)
{
	int status;
	char *message;

	if ((status = response_status_get(res)) >= 400) {
		message = response_status_msg_get(res);
		fprintf(fp, "Error : %s\n", message);	/*Call Your Own Error Printer*/

		return -1;
	}

	if (status >= 200 && status < 400) {
		message = response_status_msg_get(res);
		fprintf(fp, "Success : %s\n", message);  /*Call Your Own Success Printer*/

		return 0;
	}

	return -1;
}

int ecafe_response_lock(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_unlock(struct response *res)
{
	fprintf(stderr, "%s\n", res->body);

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_message(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_action(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_ping(struct response *res)
{
	char *msg;

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	msg = response_keyval_get(&(res->records[0]), "response");
	fprintf(stderr, "Response from client : %s\n", msg);

	return 0;
}

int ecafe_response_poweroff(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_getdetails(struct response *res, int connfd)
{
	struct client *client;
	char *hostname, *pid;

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	hostname = response_keyval_get(&(res->records[0]), "hostname");
	pid      = response_keyval_get(&res->records[1], "pid");

	client = client_active_get();

	client->name = strdup(hostname);
	client->pid = atoi(pid);


	return 0;
}

int ecafe_response_screenshot(struct response *res, int connfd)
{
	int imgfd;

	if ((imgfd = open("/tmp/ecafe_image_out.jpeg", O_CREAT | O_RDWR | O_TRUNC, 0664)) == -1) {
		perror("ecafe_response_screenshot error");

		return -1;
	}

	write(imgfd, res->body, res->bodylen);
	printf("ECAFE: bodylen %lu\n", res->bodylen);
	printf("ECAFE: %16s\n", res->body);

	close(imgfd);
	return 0;
}

int ecafe_response_notification(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}