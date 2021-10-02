#include <sock_lib.h>
#include <ecafe.h>
#include <command.h>

static void ecafe_client_prepare(struct response *res, struct client *temp);

int ecafe_request_handle(char *buf, int connfd) /* Handling requests from server */
{
	int id, rv, index;
	char *uri;
	struct request req = {};
	struct client *temp;
	
	if (!request_parse(buf, strlen(buf), &req)) {
		fprintf(stderr, "ecafe_request_handle/request_parse : error\n");
		return -1;
	}

	if ((uri = request_uri_get(&req)) == NULL)
		return -1;

	printf("%s\n--------------------\n\n", uri);
	request_dump(&req);

	if ((index = command_get_index_by_uri(uri)) == -1) {
		fprintf(stderr, "command_get_index_by_uri : error\n");
		return -1;
	}

	if (commands[index].req_handle_special) {
		return commands[index].req_handle_special(&req, connfd);
	}

	if(commands[index].req_handle(&req) == -1) {
		fprintf(stderr, "request_handle : error\n");
		return -1;
	}

	id = atoi(request_param_get(&req, "id"));
	temp = client_get(id);

	printf("Sending request to client.....\n");
	if ((rv = ecafe_request_send(&req, temp->fd)) == -1) { /* Sending request to client for the same */
		fprintf(stderr, "ecafe_request_send : error\n");
		return -1;
	}

	return 0;
}

int ecafe_response_handle(char *buf, int connfd) /* Hanlding client responses */
{
	char *uri;
	int index, nbytes, rv;
	struct response res = {};

	if ((nbytes = response_parse(buf, strlen(buf), &res)) == -1) {
		fprintf(stderr, "ecafe_response_handle/response_parse error \n");
		return -1;
	}

	if ((uri = response_header_get(&res, "uri")) == NULL ) {
		fprintf(stderr, "ecafe_response_handle/response_keyval_get error \n");
		return -1;
	}

	if ((index = command_get_index_by_uri(uri)) == -1) {
		fprintf(stderr, "command_get_index_by_uri : error\n");
		return -1;
	}
	printf("Client Response  : \n");

	if (commands[index].res_handle_special) {
		return commands[index].res_handle_special(&res, connfd);
	}

	if(commands[index].res_handle(&res) == -1) {
		fprintf(stderr, "request_handle : error\n");
		return -1;
	}

	printf("Sending response to server.....\n");
	if ((rv = ecafe_response_send(&res, connfd)) == -1) { /* Responsing back to server */
		fprintf(stderr, "ecafe_request_send : error\n");
		return -1;
	}
	puts("");

	return 0;
}


int ecafe_getdetails(struct client *cli_info)
{
	int rv;
	struct request req = {0};

	printf("/getdetails\n----------------------\n\n");

	if (ecafe_request_getdetails(&req) == -1)
		return -1;

	printf("Sending request to client......\n");
	if ((rv = ecafe_request_send(&req, cli_info->fd)) == -1) 
		return -1;

	return 0;
}

int ecafe_clientall(struct request *req, int connfd)
{
	int nclients;
	struct client **clients;
	struct response res = {};

	if (req == NULL)
		return -1;

	nclients = client_getall(&clients);

	for (int i = 0; i < nclients; ++i) {
		ecafe_client_prepare(&res, clients[i]);
	}
	
	response_header_set(&res, "uri", "/clientall");
	response_status_set(&res, RES_STATUS_OK);

	return ecafe_response_send(&res, connfd);
}

int ecafe_client(struct request *req, int connfd)
{
	struct client *client;
	struct response res = {};
	int id;

	if (req == NULL)
		return -1;
	
	id = atoi(request_param_get(req, "id"));
	client = client_get(id);

	ecafe_client_prepare(&res, client);
	
	response_header_set(&res, "uri", "/client");
	response_status_set(&res, RES_STATUS_OK);

	return ecafe_response_send(&res, connfd);
}

static void ecafe_client_prepare(struct response *res, struct client *temp)
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

	response_keyval_push(&rec, "ip", temp->ip);

	response_record_push(res, &rec);
}

void ecafe_log_error(int Errno, char *filename, int line)
{
	char str_msg[2048];
	char *error_msg;

	error_msg = strerror(Errno);

	sprintf(str_msg, "ERROR:[%s:%d]:%s", filename, line, error_msg);
	fprintf(stderr, "%s\n", str_msg);
}
