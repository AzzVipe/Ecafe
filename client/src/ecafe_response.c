#include <ecafe_request.h>

#define BLOCK   "block"
#define UNBLOCK "unblock"

int ecafe_request_lock(struct request *req, struct response *res)
{
	if (req == NULL || res == NULL)
		return -1;

	if(system("gnome-screensaver-command -l") == -1) {
		response_status_set(res, RES_STATUS_ERROR);
		return -1;
	}
	response_status_set(res, RES_STATUS_CREATED);

	return 0;
}

int ecafe_request_unlock(struct request *req, struct response *res)
{
	if (req == NULL || res == NULL)
		return -1;

	if(system("gnome-screensaver-command -u") == -1) {
		response_status_set(res, RES_STATUS_ERROR);
		return -1;
	}

	response_status_set(res, RES_STATUS_CREATED);

	return 0;
}

int ecafe_request_action(struct request *req, struct response *res)
{
	char *prog, *state;

	if (req == NULL || res == NULL)
		return -1;

	if((prog = request_param_get(req, PARAM_PROG)) == NULL) {
		response_status_set(res, RES_STATUS_ERROR);		
		return -1;
	}

	if((state = request_param_get(req, PARAM_STATE)) == NULL) {
		response_status_set(res, RES_STATUS_ERROR);		
		return -1;
	}

	if (strcmp(state, BLOCK)) {
		if (system("") == -1) {
			response_status_set(res, RES_STATUS_ERROR);		
			return -1;	
		}	
	}

	if (strcmp(state, UNBLOCK)) {
		if (system("") == -1) {
			response_status_set(res, RES_STATUS_ERROR);		
			return -1;	
		}	
	}

	response_status_set(res, RES_STATUS_CREATED);

	return 0;
}

int ecafe_request_message(struct request *req, struct response *res)
{
	char *msg;
	if (req == NULL || res == NULL)
		return -1;

	if((msg = request_param_get(req, PARAM_MESSAGE)) == NULL) {
		response_status_set(res, RES_STATUS_ERROR);		
		return -1;
	}
	
	fprintf(stderr, "Message from Server : %s\n", msg);

	response_status_set(res, RES_STATUS_OK);

	return 0;
}

int ecafe_request_ping(struct request *req, struct response *res)
{
	if (req == NULL || res == NULL)
		return -1;

	if (response_record_keyval_set(res, 0, "response", "pong") == -1) {
		fprintf(stderr, "response_record_keyval_set : error\n");
		response_status_set(res, RES_STATUS_ERROR);
		return -1;
	}

	response_status_set(res, RES_STATUS_OK);

	return 0;
}

int ecafe_request_poweroff(struct request *req, struct response *res)
{
	if (req == NULL || res == NULL)
		return -1;

	if(system("shutdown -f") == -1) {
		response_status_set(res, RES_STATUS_ERROR);
		return -1;
	}
	response_status_set(res, RES_STATUS_CREATED);

	return 0;
}

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

	uri = request_uri_get(&req);

	if ((index = command_get_index_by_uri(uri)) == -1) {
		fprintf(stderr, "command_get_index_by_uri : error\n");
		return -1;
	}

	if(commands[index].res_handle(&req, &res) == -1) {
		// fprintf(stderr, "response_handle : error\n");
		return -1;
	}

	// if (response)
	// {
	// 	/* code */
	// }

	return 0;
}