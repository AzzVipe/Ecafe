#include <ecafe_request.h>

#define BLOCK   "block"
#define UNBLOCK "unblock"

/** TODO : 
 * 1. ecafe_request_linux.c 
 * 2. system_linux functions
 */

int ecafe_request_lock(struct request *req, struct response *res)
{
	if (req == NULL || res == NULL)
		return -1;

	if(system_linux_lock() == -1) {
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

	if(system_linux_unlock() == -1) {
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

	fprintf(stderr, "Ping form server\n");

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

	if(system_linux_poweroff() == -1) {
		response_status_set(res, RES_STATUS_ERROR);
		return -1;
	}
	response_status_set(res, RES_STATUS_CREATED);

	return 0;
}

int ecafe_request_getdetails(struct request *req, struct response *res)
{
	pid_t pid;
	char hostname[1024], buf[1024];

	if (req == NULL || res == NULL)
		return -1;

	if (gethostname(hostname, 1024) == -1) {
			strcpy(hostname, "*");
		}

	if (response_record_keyval_set(res, 0, "hostname", hostname) == -1) {
		fprintf(stderr, "response_record_keyval_set : error\n");
		response_status_set(res, RES_STATUS_ERROR);
		
		return -1;
	}

	pid = getpid();
	sprintf(buf, "%d", pid);
	
	if (response_record_keyval_set(res, 1, "pid", buf) == -1) {
		fprintf(stderr, "response_record_keyval_set : pid error\n");
		response_status_set(res, RES_STATUS_ERROR);
		
		return -1;
	}

	response_status_set(res, RES_STATUS_OK);

	return 0;
}

int ecafe_response_send(int client, struct response *res)
{
	int nbytes;
	char buf[1024];

	if ((nbytes = response_prepare(res, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "response_prepare error \n");
		return -1;
	}

	buf[nbytes] = 0;
	puts(buf); 

	if ((nbytes = write(client, buf, nbytes)) == -1) {
		perror("write error");
		return -1;
	}

	if (nbytes == 0) {
		fprintf(stderr, "Server Terminated \n");
		return -1;
	}

	return 0;
}
