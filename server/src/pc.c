#include <kore/kore.h>
#include <kore/http.h>
#include <kore/seccomp.h>

#include <dynamic.h>
#include <ecafe.h>

static int	prepare_ecafe_request(struct http_request *, struct request *);
int		page_pc_show(struct http_request *);
int		page_pc_lock(struct http_request *);
int		page_pc_unlock(struct http_request *);
int		page_pc_message(struct http_request *);
int		page_pc_ping(struct http_request *);
int		page_pc_action(struct http_request *);
int		page_pc_poweroff(struct http_request *);

int
page_pc_show(struct http_request *req)
{
	struct request ecafe_req = {0};
	int id = 0;

	http_populate_get(req);
	http_argument_get_int32(req, "id", &id);
	kore_log(LOG_INFO, "You have requested for client #%d", id);

	return dynamic_html_server(req, "assets/pc/show.html");
}

int
page_pc_lock(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to lock pc\"}";
	char *res = "{\"message\": \"pc locked successfully\"}";

	request_uri_set(&ecafe_req, "/lock");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_lock(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process lock request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_unlock(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to unlock pc\"}";
	char *res = "{\"message\": \"pc unlocked successfully\"}";

	request_uri_set(&ecafe_req, "/unlock");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_unlock(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process unlock request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_message(struct http_request *req)
{
	char *msg = NULL;
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to send message\"}";
	char *res = "{\"message\": \"message sent successfully\"}";

	request_uri_set(&ecafe_req, "/message");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	http_argument_get_string(req, "msg", &msg);
	if (msg == NULL) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	request_param_set(&ecafe_req, "msg", msg);


	if (ecafe_message(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process 'send message' request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_ping(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to send ping request\"}";
	char *res = "{\"message\": \"ping reply came back from cleint\"}";

	request_uri_set(&ecafe_req, "/ping");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_ping(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process ping request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_action(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to perform action\"}";
	char *res = "{\"message\": \"Action performed successfully\"}";

	request_uri_set(&ecafe_req, "/action");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_action(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process action request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_poweroff(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to power off pc\"}";
	char *res = "{\"message\": \"PC powered down successfully\"}";

	request_uri_set(&ecafe_req, "/poweroff");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_action(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process power off request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
prepare_ecafe_request(struct http_request *req, struct request *ecafe_req)
{
	char *ids = NULL;
	http_populate_get(req);
	http_argument_get_string(req, "id", &ids);

	if (!ids) {
		return -1;
	}

	request_type_set(ecafe_req, REQ_TYPE_GET);
	request_param_set(ecafe_req, "id", ids);

	return 0;
}
