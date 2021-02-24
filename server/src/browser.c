#include <kore/kore.h>
#include <kore/http.h>
#include <kore/seccomp.h>

#include "ecafe.h"
#include "browser.h"
#include "dynamic.h"

int		page(struct http_request *);
void 	app_init(void);

KORE_SECCOMP_FILTER("app",
	KORE_SYSCALL_ALLOW(socket),
	KORE_SYSCALL_ALLOW(bind),
	KORE_SYSCALL_ALLOW(listen),
	KORE_SYSCALL_ALLOW(write),
	KORE_SYSCALL_ALLOW(select),
	KORE_SYSCALL_ALLOW(close),
	KORE_SYSCALL_ALLOW(clone),
	KORE_SYSCALL_ALLOW(dup),
	KORE_SYSCALL_ALLOW(set_robust_list),
	KORE_SYSCALL_ALLOW(pselect6),
	KORE_SYSCALL_ALLOW(newfstatat),
);


void app_init(void)
{
	kore_log(LOG_NOTICE, "test");
	start_app();
}

int
page(struct http_request *req)
{
	return dynamic_html_server(req, "assets/index.html");
}

