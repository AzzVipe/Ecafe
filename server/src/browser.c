#include <kore/kore.h>
#include <kore/http.h>
#include <kore/seccomp.h>
#include <assert.h>

#include "ecafe.h"
#include "browser.h"
#include "dynamic.h"
#include "request.h"
#include "response.h"
#include "sock_lib.h"

int		page(struct http_request *);
void 	app_init(void);

KORE_SECCOMP_FILTER("app",
	KORE_SYSCALL_ALLOW(socket),
	KORE_SYSCALL_ALLOW(connect),
	KORE_SYSCALL_ALLOW(write),
	KORE_SYSCALL_ALLOW(close),
	KORE_SYSCALL_ALLOW(newfstatat),
);

static int sockfd;

void
app_init(void)
{
	sockfd = Uxd_connect(UNIXPATH);
	if (sockfd == -1) {
		perror("sockfd");
		exit(-1);
	}

	return;
}

int
page(struct http_request *req)
{
	return dynamic_html_server(req, "assets/index.html");
}

int     
get_server_fd(void)
{
	return sockfd;
}