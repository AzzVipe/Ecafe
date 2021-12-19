#include <sock_lib.h>
#include <ecafe.h>

static int ecafe_send_recieve(struct request *req, struct response *res);
static int _ecafe_request_screenshot(struct response *res);
static void ecafe_client_populate(struct client *client, struct record *rec);
static void ecafe_clientall_populate(struct client ***clients, struct response *res);

int ecafe_lock(struct request *req)
{
	struct response res = {};

	if (ecafe_request_lock(req) == -1) {
		fprintf(stderr, "ecafe_request_lock : error\n");
		return -1;
	}
	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_unlock(struct request *req)
{
	struct response res = {0};

	if (ecafe_request_unlock(req) == -1) {
		fprintf(stderr, "ecafe_request_unlock : error\n");
		return -1;
	}
	request_dump(req);
	
	return ecafe_send_recieve(req, &res);
}

int ecafe_ping(struct request *req)
{
	struct response res = {0};

	if (ecafe_request_ping(req) == -1) 
		return -1;

	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_message(struct request *req)
{
	struct response res = {0};

	if (ecafe_request_message(req) == -1)
		return -1;

	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_action(struct request *req)
{
	struct response res = {0};

	if (ecafe_request_action(req) == -1)
		return -1;

	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_poweroff(struct request *req)
{
	struct response res = {0};

	if (ecafe_request_poweroff(req) == -1)
		return -1;

	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_screenshot(struct request *req)
{
	struct response res = {0};

	ecafe_request_screenshot(req);

	request_dump(req);

	ecafe_send_recieve(req, &res);

	return _ecafe_request_screenshot(&res);
}

int ecafe_notification(struct request *req)
{	
	struct response res = {0};

	if (ecafe_request_notification(req) == -1)
		return -1;

	request_dump(req);

	return ecafe_send_recieve(req, &res);
}

int ecafe_clientall(struct client ***clients)
{
	struct request req = {};
	struct response res = {};

	if (ecafe_request_clientall(&req) == -1)
		return -1;

	request_dump(&req);

	if (ecafe_send_recieve(&req, &res) == -1) {
		return -1;
	}
	response_dump(&res);

	ecafe_clientall_populate(clients, &res);

	return res.nrecords;
}

int ecafe_client(struct client *client)
{
	struct request req = {};
	struct response res = {};
	char id[1024];

	if (ecafe_request_client(&req) == -1)
		return -1;

	sprintf(id, "%d", client->id);
	request_param_set(&req, "id", id);
	request_dump(&req);

	if (ecafe_send_recieve(&req, &res) == -1) {
		return -1;
	}
	response_dump(&res);

	ecafe_client_populate(client, &(res.records[0]));

	return res.nrecords;
}


static int _ecafe_request_screenshot(struct response *res)
{
	int connfd;
	int nbytes = 0, tbytes = 0;
	int imgfd, img_size;
	char *img_ssize;
	char buf[1024], img_buf[1024 * 1024];
	char *uri = "/screenshot", *stage;
	struct request req = {};

	connfd = get_server_fd();

	stage = response_header_get(res, "stage");

	if (strcmp(stage, "request") != 0)
		return -1;

	if ((img_ssize = response_header_get(res, "content-len")) == NULL) 
		return -1;
	
	img_size = atoi(img_ssize);

	request_type_set(&req, REQ_TYPE_POST);
	request_uri_set(&req, uri);
	request_header_set(&req, "stage", "transfer");

	if (ecafe_request_send(&req, connfd) == -1) /* Sending request to server for transfer stage */
		return -1;

	while(tbytes != img_size) {
		if ((nbytes = read(connfd, buf, sizeof(buf))) == 0) {
			fprintf(stderr, "Server Terminated\n");
			return -1;
		}

		if (nbytes < 0) {
			perror("ecafe_response_screenshot/read error");
			return -1;
		}

		if ((tbytes + nbytes) > (int ) sizeof(img_buf)) {
			fprintf(stderr, "File too large\n");
			return -1;
		}

		memcpy(img_buf + tbytes, buf, nbytes);
		tbytes += nbytes;
	}

	printf("Image recieved !\n");

	if ((imgfd = open("/tmp/ecafe_image_out.jpeg", O_CREAT | O_RDWR | O_TRUNC, 0664)) == -1) {
		perror("ecafe_response_screenshot error");
		return -1;
	}

	if (write(imgfd, img_buf, tbytes) == -1) {
		perror("ecafe_response_screenshot/write error");
		return -1;
	}

	request_header_set(&req, "stage", "finished");

	if (ecafe_request_send(&req, connfd) == -1) /* Sending request to client for the same */
		return -1;

	if(ecafe_response_recv(connfd, res) == -1)
		return -1;

	return tbytes;
}

static int ecafe_send_recieve(struct request *req, struct response *res)
{
	int rv = 0;
	int connfd = get_server_fd();

	if ((rv = ecafe_request_send(req, connfd)) == -1) {
		fprintf(stderr, "ecafe_request_send : error\n");
		return -1;
	}

	if ((rv = ecafe_response_recv(connfd, res)) == 0) {
		fprintf(stderr, "ecafe_response_recv : Server Terminated \n");
		return -2;
	} else if (rv == -1) {
		fprintf(stderr, "ecafe_response_recv : error\n");
		return -1;
	}

	return 0;
}

static void ecafe_clientall_populate(struct client ***clients, struct response *res)
{
	struct client *temp;
	struct client **temp_array;

	temp_array = malloc(sizeof(struct client *) * res->nrecords);

	if (temp_array == NULL) {
		fprintf(stderr, "malloc: empty\n");
		return;
	}

	for (int i = 0; i < res->nrecords; ++i) {
		temp_array[i] = malloc(sizeof(struct client));
		temp = temp_array[i];
		ecafe_client_populate(temp, &(res->records[i]));
	}

	*clients = temp_array;
}

static void ecafe_client_populate(struct client *client, struct record *rec)
{
	client->id = atoi(response_keyval_get(rec, "id"));
	client->hostname = strdup(response_keyval_get(rec, "hostname"));
	client->username = strdup(response_keyval_get(rec, "username"));
	client->state = strdup(response_keyval_get(rec, "state"));
	client->pid = atoi(response_keyval_get(rec, "pid"));
	client->uptime = strdup(response_keyval_get(rec, "uptime"));
	client->ip = strdup(response_keyval_get(rec, "ip"));
}