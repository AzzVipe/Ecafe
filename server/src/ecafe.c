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

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
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
	if (ecafe_request_unlock(req) == -1) {
		fprintf(stderr, "ecafe_request_unlock : error\n");
		return -1;
	}
	
	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
		fprintf(stderr, "ecafe_request_send : error\n");
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

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
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

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	if (ecafe_request_message(req) == -1)
		return -1;

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
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

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
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

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
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

int ecafe_screenshot(struct request *req)
{
	int id, rv;
	struct client *temp;
	struct response res = {0};

	id = atoi(request_param_get(req, "id"));
	temp = client_get(id);

	if (ecafe_request_screenshot(req) == -1)
		return -1;

	if ((rv = ecafe_request_send(temp->fd, req)) == -1) {
		return -1;
	}

	if ((rv = ecafe_response_recv(temp->fd, &res)) == 0) {
		return -2;
	} else if (rv == -1) {
		return -1;
	}

	ecafe_response_screenshot(&res);

	return 0;

}
int ecafe_getdetails(struct client *cli_info)
{
	int rv;
	struct request req = {0};
	struct response res = {0};

	if (ecafe_request_getdetails(&req) == -1)
		return -1;

	if ((rv = ecafe_request_send(cli_info->fd, &req)) == -1) 
		return -1;

	if ((rv = ecafe_response_recv(cli_info->fd, &res)) == 0)
		return -2;
	else if (rv == -1)
		return -1;

	ecafe_response_getdetails(&res, cli_info);

	return 0;
}

int ecafe_clientall(struct client ***clients)
{
	return client_getall(clients);
}