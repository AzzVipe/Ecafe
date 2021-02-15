#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "dynamic.h"

int dynamic_html_server(struct http_request *req, const char *filepath)
{
	struct kore_buf *korebuf;

	if (!(korebuf = file_buffer_get(filepath))) {
		http_response(req, 500, NULL, 0);
		return (KORE_RESULT_OK);
	}

	http_response_header(req, "content-type", "text/html");
	http_response(req, 200, korebuf->data, korebuf->offset);
	kore_buf_free(korebuf);

	return (KORE_RESULT_OK);
}

struct kore_buf *file_buffer_get(const char *filepath)
{
	int fd;
	struct kore_buf *korebuf;
	char buf[1024];
	ssize_t nbytes;

	if ((fd = open(filepath, O_RDONLY)) == -1) {
		perror("file_buffer_get");
		return NULL;
	}

	korebuf = kore_buf_alloc(1024);
	while ((nbytes = read(fd, buf, sizeof(buf))) > 0) {
		kore_buf_append(korebuf, buf, nbytes);
	}
	close(fd);

	return korebuf;
}

