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

int ecafe_request_getdetails(struct request *req)
{
	char uri[] = "/getdetails";

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

int ecafe_request_screenshot(struct request *req)
{
	char uri[] = "/screenshot";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);
	request_header_set(req, PARAM_STAGE, "request");

	return 0;
}

int ecafe_request_notification(struct request *req)
{
	char uri[] = "/notification";

	request_type_set(req, REQ_TYPE_POST);
	request_uri_set(req, uri);

	return 0;
}

int ecafe_request_send(struct request *req, int connfd)
{
	int nbytes;
	char buf[1024 * 10];

	if ((nbytes = request_prepare(req, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "request_prepare error \n");
		return -1;
	}

	puts(buf);

	if ((nbytes = write(connfd, buf, nbytes)) == -1) {
		perror("write error");
		return -1;
	}

	return nbytes;
}

int ecafe_request_recv(int connfd, struct request *req)
{
	int nbytes = 0;
	char buf[1024 * 10];

	if ((nbytes = read(connfd, buf, sizeof(buf))) == -1) {
		perror("read error");
		return -1;
	}
	
	if (nbytes == 0)
		return 0;
	
	buf[nbytes] = 0;
	
	puts(buf);

	if(!request_parse(buf, nbytes, req)) {
		fprintf(stderr, "response_parse : error\n");
		return -1;
	}

	return nbytes;
}