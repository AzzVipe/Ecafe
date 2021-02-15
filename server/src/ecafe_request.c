#include <ecafe_request.h>

int ecafe_request_lock(struct request *req)
{
	char uri[] = "/lock";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);

	return 0;
}

int ecafe_request_unlock(struct request *req)
{
	char uri[] = "/unlock";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);

	return 0;
}

int ecafe_request_message(struct request *req)
{
	char uri[] = "/message";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);	

	return 0;
}

int ecafe_request_ping(struct request *req)
{
	char uri[] = "/ping";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);

	return 0;

}

int ecafe_request_poweroff(struct request *req)
{
	char uri[] = "/poweroff";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);

	return 0;

}

int ecafe_request_action(struct request *req)
{
	char uri[] = "/action";
	char prog[1024] = "Chrome\n";
	char state[1024] = "block\n";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);	
	request_param_set(req, PARAM_PROG, prog);
	request_param_set(req, PARAM_STATE, state);

	return 0;
}

int ecafe_request_send(struct request *req, int client)
{
	int nbytes;
	char buf[1024];

	if ((nbytes = request_prepare(req, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "request_prepare error \n");
		return -1;
	}

	puts(buf);

	if ((nbytes = write(client, buf, nbytes)) == -1) {
		perror("write error");
		return -1;
	}

	return nbytes;
}

int ecafe_request_handle(char *buf, int client)
{
	int index;
	
	if ((index = command_get_index(buf)) == -1) {
		fprintf(stderr, "Invalid command\n");
		return -1;
	}

	if (commands[index].req_handle(client) == -1) {
		fprintf(stderr, "ecafe_request_handle error\n");
		return -1;
	}

	return 0;
}

