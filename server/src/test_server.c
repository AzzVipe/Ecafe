#include <sock_lib.h>
#include <request.h>
#include <response.h>
#include <assert.h>

#undef SERVER_PORT 
#define SERVER_PORT "4040"

struct test_request {
	char *uri;
	char *params[20];
	char *values[20];
	void (*res_cb) (int);
	int expected_status;
};

struct test_request tests[] = {
	{"/lock",     {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/unlock",   {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/ping",     {"id", NULL}, {"1", NULL}, NULL, 200},
	{"/poweroff", {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/message",  {"id", "msg", NULL}, {"1", "test message", NULL}, NULL, 200},
	{NULL, NULL, NULL, NULL, 0}
};

int test_request_run(struct test_request *test_req, int connfd);
int test_assert_response(struct test_request *test_req, int connfd);

int main(int argc, char *argv[])
{
	int sockfd;
	char *ip;

	if (argc != 2) {
		fprintf(stderr, "Usage : %s <IP>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	ip = argv[1];
	fprintf(stderr, "IP : %s\nPort : %s\n", ip, SERVER_PORT);

	if ((sockfd = Tcp_connect(ip, SERVER_PORT)) < 0) {
		fprintf(stderr, "Error: Tcp_connect\n");
		exit(1);
	}

	for (int i = 0; tests[i].uri != NULL; ++i) {
		fprintf(stderr, "%s: ", tests[i].uri);
		test_request_run(&tests[i], sockfd);
		test_assert_response(&tests[i], sockfd);
	}

	return 0;
}

int test_request_run(struct test_request *test_req, int connfd)
{
	int nbytes;
	char buf[1024];
	struct request req = {0};

	request_type_set(&req, REQ_TYPE_GET);
	request_uri_set(&req, test_req->uri);
	

	for (int i = 0; test_req->params[i] != NULL; ++i) {
		request_param_set(&req, test_req->params[i], test_req->values[i]);
	}

	if ((nbytes = request_prepare(&req, buf, sizeof(buf))) == -1) {
		perror("test_request_run/request_prepare error");
		return nbytes;
	}

	if (write(connfd, buf, nbytes) == -1) {
		perror("write error");
		return -1;
	}

	return nbytes;
}

int test_assert_response(struct test_request *test_req, int connfd)
{
	int nbytes;
	char buf[1024 * 1024];
	struct response res = {0};

	if ((nbytes = read(connfd, buf, sizeof(buf)) == -1)) {
		perror("test_assert_response/read error");
		return -1;
	}

	if (response_parse(buf, nbytes, &res) == -1) {
		perror("test_assert_response/response_parse error");
		return -1;
	}

	// response_dump(&res);

	assert(res.status == test_req->expected_status);
	fprintf(stderr, "PASSED\n");

	return 0;
}