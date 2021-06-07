#include <ecafe_response.h>

int ecafe_response_recv(int client, struct response *res)
{
	int nbytes = 0;
	char buf[1024 * 1024];

	// while((nbytes += read(client, (buf + nbytes), sizeof(buf))) > 0)
	// 	; // empty body

	if ((nbytes = read(client, buf, sizeof(buf))) == -1) {
		perror("read error");
		return -1;
	}

	printf("Size : %d \n", nbytes);

	if (nbytes == 0) {
		fprintf(stderr, "Client Terminated \n");
		return 0;
	}

	if(response_parse(buf, nbytes, res) == -1) {
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

int ecafe_response_getdetails(struct response *res, struct client *cli_info)
{
	char *hostname, *pid;

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	hostname = response_keyval_get(&(res->records[0]), "hostname");
	pid      = response_keyval_get(&res->records[1], "pid");

	fprintf(stderr, "Pid : %p\n", pid);

	cli_info->name = hostname;
	cli_info->pid = 0;
	
	if (pid)
		cli_info->pid = atoi(pid);


	return 0;
}

int ecafe_response_screenshot(struct response *res)
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