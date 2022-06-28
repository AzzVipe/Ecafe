#include <ecafe_response.h>

#define PARAM_STAGE "stage"

static int ecafe_response_screenshot_client(char *img_buf, int size, struct response *res);
static int ecafe_response_screenshot_server(char *img_buf, int img_size, struct response *res, int connfd);

int ecafe_response_lock(struct response *res)
{
	struct client *client;

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	client = client_active_get();
	client->is_online = false;

	return 0;
}

int ecafe_response_unlock(struct response *res)
{
	struct client *client;
	fprintf(stderr, "%s\n", res->body);

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	client = client_active_get();
	client->is_online = true;

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
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_poweroff(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

int ecafe_response_timer(struct response *res)
{
	// @TODO
	
	return 0;
}

int ecafe_response_getdetails(struct response *res, int connfd)
{
	struct client *client;
	char *hostname, *pid, *username, *uptime;
	struct record *rec = &(res->records[0]);

	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	hostname = response_keyval_get(rec, "hostname");
	username = response_keyval_get(rec, "username");
	pid      = response_keyval_get(rec, "pid");
	uptime   = response_keyval_get(rec, "uptime");

	client = client_active_get();
	client->hostname = strdup(hostname);
	client->username = strdup(username);
	client->uptime = strdup(uptime);
	client->pid = atoi(pid);

	return 0;
}

int ecafe_response_screenshot(struct response *res, int connfd)
{
	int img_size = 0;
	char img_buf[1024 * 1024];

	/* Receiving image from client */
	if ((img_size = ecafe_response_screenshot_client(img_buf, sizeof(img_buf), res)) == -1 ) 
		return -1;

	printf("Sending response to server......\n");

	return ecafe_response_screenshot_server(img_buf, img_size, res, connfd);
	
}

int ecafe_response_client(struct response *res)
{
	struct client *client;

	if (ecafe_response_getdetails(res, 0) == -1) {
		response_status_set(res, RES_STATUS_OK);
		return -1;
	}
	
	client = client_active_get();

	res->nrecords = 0;
	ecafe_response_prepare_client(res, client);
	response_status_set(res, RES_STATUS_OK);

	return 0;
}

int ecafe_response_notification(struct response *res)
{
	if (ecafe_response_print(res, stderr) == -1)
		return -1;

	return 0;
}

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
		// fprintf(fp, "Success : %s\n", message);  /*Call Your Own Success Printer*/
		response_dump(res);
		puts("");
		return 0;
	}

	return -1;
}

void ecafe_response_prepare_client(struct response *res, struct client *temp)
{
	struct record rec = {};
	char buf[256];
	
	sprintf(buf, "%d", temp->id);
	response_keyval_push(&rec, "id", buf);
	response_keyval_push(&rec, "hostname", temp->hostname);
	response_keyval_push(&rec, "username", temp->username);
	response_keyval_push(&rec, "uptime", temp->uptime);
	
	sprintf(buf, "%s", temp->is_online ? "online" : "offline");
	response_keyval_push(&rec, "state", buf);
	
	sprintf(buf, "%d", temp->pid);
	response_keyval_push(&rec, "pid", buf);

	if (temp->timer_uri) {
		sprintf(buf, "%ld", temp->timer->created_at);
		response_keyval_push(&rec, "timer_created_at", buf);

		sprintf(buf, "%ld", temp->timer->duration);
		response_keyval_push(&rec, "timer_duration", buf);

		response_keyval_push(&rec, "timer_uri", temp->timer_uri);
	}
	
	response_keyval_push(&rec, "ip", temp->ip);
	response_record_push(res, &rec);
}

static int ecafe_response_screenshot_client(char *img_buf, int size, struct response *res)
{
	int nbytes = 0, tbytes = 0;
	int imgfd, img_size;
	char *img_ssize;
	char buf[1024];
	struct request req = {};
	struct client *client;

	if (strcmp(response_header_get(res, PARAM_STAGE), "request") != 0)
		return -1;

	if ((img_ssize = response_header_get(res, "content-len")) == NULL) 
		return -1;
	
	img_size = atoi(img_ssize);

	ecafe_request_screenshot(&req);
	request_header_set(&req, PARAM_STAGE, "transfer");

	client = client_active_get();

	if (ecafe_request_send(&req, client->fd) == -1) /* Sending request to client for the same */
		return -1;

	while(tbytes != img_size) {
		if ((nbytes = read(client->fd, buf, sizeof(buf))) == 0) {
			fprintf(stderr, "Client Terminated\n"); /* @TODO : Remove client from the client array */
			return -1;
		}

		if (nbytes < 0) {
			perror("ecafe_response_screenshot/read error");
			return -1;
		}

		if ((tbytes + nbytes) > size ) {
			fprintf(stderr, "File too large\n");
			return -1;
		}

		memcpy(img_buf + tbytes, buf, nbytes);
		tbytes += nbytes;
	}

	printf("Image recieved !\n\n");

	if ((imgfd = open("/tmp/ecafe_image_out.jpeg", O_CREAT | O_RDWR | O_TRUNC, 0664)) == -1) {
		perror("ecafe_response_screenshot error");
		return -1;
	}

	if (write(imgfd, img_buf, tbytes) == -1) {
		perror("ecafe_response_screenshot/write error");
		return -1;
	}

	request_header_set(&req, PARAM_STAGE, "finished");

	if (ecafe_request_send(&req, client->fd) == -1) /* Sending request to client for the same */
		return -1;

	return tbytes;
}

static int ecafe_response_screenshot_server(char *img_buf, int img_size, struct response *res, int connfd)
{
	int nready, rv;
	fd_set rset;
	char *stage;
	struct timeval tv = {};
	struct request req = {};
	
	tv.tv_sec = 2;

	FD_ZERO(&rset);
	FD_SET(connfd, &rset);

	response_status_set(res, RES_STATUS_OK);
	if (ecafe_response_send(res, connfd) == -1) /* Now sending to server */
		return -1;

	while(1) {
		
		nready = select(connfd + 1, &rset, NULL, NULL, &tv);

		if (nready == 0) {
			fprintf(stderr, "Timeout\n");
			return -1;
		} else if (nready == -1) {
			perror("ecafe_response_screenshot_server select error ");
			return -1;
		}

		if (FD_ISSET(connfd, &rset)) {
			if ((rv = ecafe_request_recv(connfd, &req)) == -1) {
				perror("ecafe_response_screenshot_server/ecafe_request_recv error ");
				return -1;
			} else if (rv == 0) {
				fprintf(stderr, "Test Server terminated !\n");
				return -1;
			}

			if ((stage = request_header_get(&req, PARAM_STAGE)) == NULL) {
				fprintf(stderr, "Header not found!\n");
				return -1;
			}

			if (strcmp(stage, "transfer") == 0) {
				if (write(connfd, img_buf, img_size) == -1) {
					perror("ecafe_request_screenshot/write error");
					return -1;
				}
			} else if (strcmp(stage, "finished") == 0) {
				response_header_set(res, "stage", "finished");
				puts("finished");
				response_dump(res);
				if (ecafe_response_send(res, connfd) == -1)
					return -1;
				break;
			}
		}
	}
	
	return 0;	
}