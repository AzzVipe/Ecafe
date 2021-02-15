#include <sock_lib.h>
#include <ecafe.h>

int ecafe_lock(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	printf("Id %d\n", id);

	request_dump(req);

	if (ecafe_request_lock(req) == -1) {
		fprintf(stderr, "ecafe_request_lock : error\n");
		return -1;
	}

	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		fprintf(stderr, "ecafe_request_send : error\n");
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		fprintf(stderr, "ecafe_response_recv : Client Terminated \n");
		return -2;
	} else if (rv == -1) {
		fprintf(stderr, "ecafe_response_recv : error\n");
		return -1;
	}

	ecafe_response_lock(&res);

	return 0;
}

int ecafe_unlock(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	puts("OK");
	if (ecafe_request_unlock(req) == 0)
		return -1;
	
	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_unlock(&res);

	return 0;
}

int ecafe_ping(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	if (ecafe_request_ping(req) == -1) 
		return -1;

	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_ping(&res);

	return 0;
}

int ecafe_message(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};
	char *message;

	id = atoi(request_param_get(req, "id"));
	message = request_param_get(req, "message");
	temp = client_get(id);

	if (ecafe_request_message(req) == -1)
		return -1;

	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_message(&res);

	return 0;

}

int ecafe_action(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	if (ecafe_request_action(req) == -1)
		return -1;

	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_action(&res);

	return 0;
}

int ecafe_poweroff(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	if (ecafe_request_poweroff(req) == -1)
		return -1;

	if ((rv = ecafe_request_send(req, temp->fd)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_action(&res);

	return 0;
}