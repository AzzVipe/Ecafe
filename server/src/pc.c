#include <kore/kore.h>
#include <kore/http.h>
#include <kore/seccomp.h>

#include <dynamic.h>
#include <ecafe.h>
#include <client.h>

static int	prepare_ecafe_request(struct http_request *, struct request *);
static void prepare_client_json(struct kore_buf *buf, const char *client_json_format, struct client *client);
static void get_file_content(const char *filepath, struct kore_buf *result);

int		page_pc_index(struct http_request *);
int		page_pc_show(struct http_request *);
int		page_pc_lock(struct http_request *);
int		page_pc_unlock(struct http_request *);
int		page_pc_message(struct http_request *);
int		page_pc_ping(struct http_request *);
int		page_pc_action(struct http_request *);
int		page_pc_screenshot(struct http_request *);
int		page_pc_notification(struct http_request *);
int		page_pc_poweroff(struct http_request *);

int
page_pc_index(struct http_request *req)
{
	int ret;
	struct kore_buf *buf;
	const char *client_json_format = "{\"id\": %d, \"hostname\": \"%s\", \"username\": \"%s\", \"status\": \"%s\", \"uptime\": \"%s\", \"ip\": \"%s\", \"pid\": %d}";
	struct client **clients;
	// char *res_error = "{\"message\": \"Failed to fetch clients\"}";

	if ((ret = ecafe_clientall(&clients)) == -1) 
		kore_log(LOG_ERR, "Failed to process fetch clients");
	
	buf = kore_buf_alloc(1024);
	kore_buf_appendf(buf, "{ \"clients\": [");

	for (int i = 0; i < ret; ++i) {
		client_dump(clients[i]);
		prepare_client_json(buf, client_json_format, clients[i]);

		if (i != ret - 1) {
			kore_buf_appendf(buf, ", ");
		}
	}

	kore_buf_appendf(buf, "]}");

	http_response_header(req, "content-type", "application/json");
	http_response(req, HTTP_STATUS_OK, buf->data, buf->offset);
	kore_buf_free(buf);

	return (KORE_RESULT_OK);
}

int
page_pc_show(struct http_request *req)
{
	int ret, id = 0;
	struct kore_buf *buf;
	const char *client_json_format = "{\"id\": %d, \"hostname\": \"%s\", \"username\": \"%s\", \"status\": \"%s\", \"uptime\": \"%s\", \"ip\": \"%s\", \"pid\": %d}";
	struct client client = {};
	// char *res_error = "{\"message\": \"Failed to fetch client\"}";

	http_populate_get(req);
	http_argument_get_int32(req, "id", &id);
	kore_log(LOG_INFO, "You have requested for client #%d", id);
	client.id = id;

	if ((ret = ecafe_client(&client)) == -1) 
		kore_log(LOG_ERR, "Failed to process fetch client");

	buf = kore_buf_alloc(1024);

	kore_buf_appendf(buf, "{ \"client\":");
	client_dump(&client);
	prepare_client_json(buf, client_json_format, &client);
	kore_buf_appendf(buf, "}");

	http_response_header(req, "content-type", "application/json");
	http_response(req, HTTP_STATUS_OK, buf->data, buf->offset);
	kore_buf_free(buf);

	return dynamic_html_server(req, "assets/pc/show.html");
}

int
page_pc_lock(struct http_request *req)
{
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to lock pc\"}";
	char *res = "{\"message\": \"pc locked successfully\"}";

	http_response_header(req, "content-type", "application/json");
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

	http_response_header(req, "content-type", "application/json");
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

	http_response_header(req, "content-type", "application/json");
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

	http_response_header(req, "content-type", "application/json");
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

	http_response_header(req, "content-type", "application/json");
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

	http_response_header(req, "content-type", "application/json");
	request_uri_set(&ecafe_req, "/poweroff");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_poweroff(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process power off request");
		res = res_error;
	}

	http_response(req, HTTP_STATUS_OK, res, strlen(res));

	return (KORE_RESULT_OK);
}

int
page_pc_screenshot(struct http_request *req)
{
	struct kore_buf *result = NULL;
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to send screenshot request\"}";

	request_uri_set(&ecafe_req, "/screenshot");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response_header(req, "content-type", "application/json");
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	if (ecafe_screenshot(&ecafe_req) == -1) {
		http_response_header(req, "content-type", "application/json");
		http_response(req, HTTP_STATUS_OK, res_error, strlen(res_error));
		kore_log(LOG_ERR, "Failed to process screenshot request");
		goto cleanup;
	}

	result = kore_buf_alloc(1024 * 1024);
	get_file_content("/tmp/ecafe_image_out.jpeg", result);

	http_response_header(req, "content-type", "image/jpeg");
	http_response(req, HTTP_STATUS_OK, result->data, result->offset);

cleanup:
	if (result) kore_buf_free(result);

	return (KORE_RESULT_OK);
}

int
page_pc_notification(struct http_request *req)
{
	char *notification = NULL;
	struct request ecafe_req = {0};
	char *res_error = "{\"message\": \"Failed to send message\"}";
	char *res = "{\"message\": \"message sent successfully\"}";

	request_uri_set(&ecafe_req, "/notification");
	if (prepare_ecafe_request(req, &ecafe_req) == -1) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	http_argument_get_string(req, "notification", &notification);
	if (notification == NULL) {
		http_response(req, 400, NULL, 0);
		return (KORE_RESULT_OK);
	}
	request_param_set(&ecafe_req, "notification", notification);


	if (ecafe_notification(&ecafe_req) == -1) {
		kore_log(LOG_ERR, "Failed to process 'send message' request");
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

static void
get_file_content(const char *filepath, struct kore_buf *result)
{
	int fd;
	char buf[BUFSIZ];
	ssize_t nbytes;

	if ((fd = open(filepath, O_RDONLY)) == -1)
		return;

	while ((nbytes = read(fd, buf, sizeof(buf))) > 0)
		kore_buf_append(result, buf, nbytes);

	close(fd);
}

static void 
prepare_client_json(struct kore_buf *buf, const char *client_json_format, struct client *client)
{
	kore_buf_appendf(buf, client_json_format,
		client->id,
		client->hostname,
		client->username,
		client->state,
		client->uptime,
		client->ip,
		client->pid);
}