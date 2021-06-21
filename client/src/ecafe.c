#include <ecafe.h>

int ecafe_request_handle(char *buf, int server)
{
	int index;
	char *uri;
	struct request req = {0};
	struct response res = {0};

	if(!request_parse(buf, strlen(buf), &req)) {
		fprintf(stderr, "resquest_parse : error\n");
		return -1;
	}

	if ((uri = request_uri_get(&req)) == NULL) {
		return -1;
	}

	if ((index = command_get_index_by_uri(uri)) == -1) {
		fprintf(stderr, "command_get_index_by_uri : error\n");
		return -1;
	}

	response_header_set(&res, "uri", uri);

	if(commands[index].req_handle(&req, &res) == -1) {
		fprintf(stderr, "request_handle : error\n");
		return -1;
	}

	if (ecafe_response_send(&res, server)) {
		fprintf(stderr, "ecafe_response_send : error \n");
		return -1;
	}

	return 0;
}