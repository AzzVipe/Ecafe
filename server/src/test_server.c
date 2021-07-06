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
	int (*res_cb) (struct response *, int);
	int expected_status;
};

static int test_request_send(struct request *req, int connfd);

int test_response_screenshot(struct response *res, int connfd);
int test_request_run(struct test_request *test_req, int connfd);
int test_assert_response(struct test_request *test_req, int connfd);

struct test_request tests[] = {
	{"/lock",     {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/unlock",   {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/ping",     {"id", NULL}, {"1", NULL}, NULL, 200},
	// {"/poweroff", {"id", NULL}, {"1", NULL}, NULL, 201},
	{"/screenshot", {"id", NULL}, {"1", NULL}, test_response_screenshot, 200},
	{"/message",  {"id", "msg", NULL}, {"1", "test message", NULL}, NULL, 200},
	{NULL, NULL, NULL, NULL, 0}
};

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
	char buf[1024];
	struct response res = {0};

	if ((nbytes = read(connfd, buf, sizeof(buf))) == -1) {
		perror("test_assert_response/read error");
		return -1;
	}
	
	buf[nbytes] = 0;
	if (response_parse(buf, nbytes, &res) == -1) {
		perror("test_assert_response/response_parse error");
		// return -1;
	}
	puts(buf);
	// response_dump(&res);

	assert(res.status == test_req->expected_status);
	fprintf(stderr, "PASSED\n");

	if (!test_req->res_cb)
		return 0;

	if (test_req->res_cb(&res, connfd) == -1) 
		return -1;	
	
	return 0;
}

int test_response_screenshot(struct response *res, int connfd)
{
	int nbytes = 0, tbytes = 0;
	int imgfd, img_size;
	char *img_ssize;
	char buf[1024], img_buf[1024 * 1024];
	char *uri = "/screenshot", *stage;
	struct request req = {};

	stage = response_header_get(res, "stage");

	if (strcmp(stage, "request") != 0)
		return -1;

	if ((img_ssize = response_header_get(res, "content-len")) == NULL) 
		return -1;
	
	img_size = atoi(img_ssize);

	request_type_set(&req, REQ_TYPE_POST);
	request_uri_set(&req, uri);
	request_header_set(&req, "stage", "transfer");

	if (test_request_send(&req, connfd) == -1) /* Sending request to server for the same */
		return -1;

	while(tbytes != img_size) {
		if ((nbytes = read(connfd, buf, sizeof(buf))) == 0) {
			fprintf(stderr, "Server Terminated\n");
			return -1;
		}

		if (nbytes < 0) {
			perror("ecafe_response_screenshot/read error");
			return -1;
		}

		if ((tbytes + nbytes) > (int ) sizeof(img_buf)) {
			fprintf(stderr, "File too large\n");
			return -1;
		}

		memcpy(img_buf + tbytes, buf, nbytes);
		tbytes += nbytes;
	}

	printf("Image recieved !\n");

	if ((imgfd = open("/tmp/test_ecafe_image_out.jpeg", O_CREAT | O_RDWR | O_TRUNC, 0664)) == -1) {
		perror("ecafe_response_screenshot error");
		return -1;
	}

	if (write(imgfd, img_buf, tbytes) == -1) {
		perror("ecafe_response_screenshot/write error");
		return -1;
	}

	request_header_set(&req, "stage", "finished");

	if (test_request_send(&req, connfd) == -1) /* Sending request to client for the same */
		return -1;

	if ((nbytes = read(connfd, buf, sizeof(buf))) == -1) {
		perror("read error ");
		return -1;
	}
	else if(nbytes == 0) {
		fprintf(stderr, "Server Terminated\n");
		return -1;
	}

	buf[nbytes] = 0;
	if (response_parse(buf, nbytes, res) == -1) {
		perror("test_assert_response/response_parse error");
		return -1;
	}
	puts(buf);

	return tbytes;
}

static int test_request_send(struct request *req, int connfd)
{
	int nbytes;
	char buf[1024 * 10];

	if ((nbytes = request_prepare(req, buf, sizeof(buf))) == -1) {
		fprintf(stderr, "request_prepare error \n");
		return -1;
	}

	buf[nbytes] = 0;
	puts(buf);

	if ((nbytes = write(connfd, buf, nbytes)) == -1) {
		perror("write error");
		return -1;
	}

	return nbytes;
}