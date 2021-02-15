#ifndef _DYNAMIC_H
#define _DYNAMIC_H

#include <kore/kore.h>
#include <kore/http.h>

int dynamic_html_server(struct http_request *req, const char *);
struct kore_buf *file_buffer_get(const char *filepath);


#endif
