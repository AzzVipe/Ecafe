#include "request.h"
#include <easyio.h>
#include <str.h>

#define REQ_TYPE_MAX_LEN 4

static const char *REQ_TYPES_STRING[] = {
	"GET",
	"POST",
	"PATCH",
	"DELETE",
	NULL,
};
static const int REQ_TYPES_CODE[] = {
	REQ_TYPE_GET,
	REQ_TYPE_POST,
	REQ_TYPE_PATCH,
	REQ_TYPE_DELETE,
};
static const char *reqbufp;
static size_t reqbuf_datalen;
static size_t reqbuf_datai;

/* Functions */
static bool request_isheader(char *line);
static bool request_uri_parse(struct request *req, char *line);
static bool request_extract_key_value(struct request *req, char *line, char delim);
static bool request_continue_parsing_binary(struct request *req);
static ssize_t request_body_append_binary(struct request *req, char *buf, size_t buflen, ssize_t offset);

/**
 * @return int 0 = empty line, +n = length of read line, -1 = EOF
 */
static ssize_t getstring(char **line);


const char *request_type_str_get(struct request *req)
{
	if (req == NULL)
		return NULL;

	if (req->type < 0 || req->type > REQ_TYPE_MAX_LEN)
		return NULL;

	return REQ_TYPES_STRING[req->type - 1];
}

int request_type_get(struct request *req)
{
	if (req == NULL)
		return -1;

	return req->type;
}

int request_type_set(struct request *req, int req_type)
{
	if (req == NULL)
		return  -1;

	if (req_type < 0 || req_type > REQ_TYPE_MAX_LEN)
		return -1;

	req->type = req_type;

	return 0;
}

bool
request_parse(const char *reqbuf, size_t rbdatalen, struct request *req)
{
	String line;
	size_t iline = 0;
	long int nbytes;

	reqbufp 		= reqbuf;
	reqbuf_datalen 	= rbdatalen;
	reqbuf_datai   	= 0;

	// Extract first line (request URI)
	++iline;
	if ((nbytes = getstring(&line)) < 1) {
		fprintf(stderr, "request_parse error at line %lu: %s\n", iline, line);
		return false;
	}
	fprintf(stderr, "%lu[%ld]: %s\n", iline, nbytes, line);

	if (!request_uri_parse(req, line)) {
		fprintf(stderr, "request_parse error at line %lu: %s\n", iline, line);
		return false;
	}

	free(line);

	/* Extract headers */
	while ((nbytes = getstring(&line)) > 0) {
		++iline;
		fprintf(stderr, "%lu[%ld]: %s\n", iline, nbytes, line);
		request_extract_key_value(req, line, REQ_HEADER_DELIM);
		free(line);
	}

	/* If request body has binary data then call binary body parser */
	if (request_header_get(req, "content-type") &&
		strcmp(request_header_get(req, "content-type"), REQ_HEADER_BINARY_VALUE) == 0) {
			return request_continue_parsing_binary(req);

	/* Otherwise call normal body parser */
	} else {
		while ((nbytes = getstring(&line)) >= 0) {
			if (nbytes == 0)
				continue;

			request_extract_key_value(req, line, REQ_PARAM_DELIM);

			free(line);
		}

	}


	return true;
}

void
request_param_set(struct request *req, const char *key, const char *value)
{
	int i;
	size_t len = request_param_size(req);

	for (i = 0; i < (int) len; ++i) {
		if (strcmp(req->param_keys[i], key) == 0)
			break;
	}

	// Found the key
	if (i != (int) len) {
		free(req->param_keys[i]);
		free(req->param_values[i]);
		// Shift elements left
		for (; i < (int) len - 1; ++i) {
			req->param_keys[i] = req->param_keys[i + 1];
			req->param_values[i] = req->param_values[i + 1];
		}
		req->iparam--;
	}

	if (req->iparam == REQ_MAX_PARAM_LEN - 1)
		return;

	req->param_keys[req->iparam] 		= strdup(key);
	req->param_values[req->iparam++] 	= strdup(value);

	str_trim(req->param_keys[req->iparam - 1]);
	str_trim(req->param_values[req->iparam - 1]);
}

void
request_header_set(struct request *req, const char *key, const char *value)
{
	int i;
	size_t len = request_header_size(req);

	for (i = 0; i < (int) len; ++i) {
		if (strcmp(req->header_keys[i], key) == 0)
			break;
	}

	// Found the key
	if (i != (int) len) {
		free(req->header_keys[i]);
		free(req->header_values[i]);
		// Shift elements left
		for (; i < (int) len - 1; ++i) {
			req->header_keys[i] = req->header_keys[i + 1];
			req->header_values[i] = req->header_values[i + 1];
		}
		req->iheader--;
	}

	if (req->iheader == REQ_MAX_PARAM_LEN - 1)
		return;

	req->header_keys[req->iheader] 		= strdup(key);
	req->header_values[req->iheader++] 	= strdup(value);

	str_trim(req->header_keys[req->iheader - 1]);
	str_trim(req->header_values[req->iheader - 1]);
}

size_t
request_header_size(struct request *req)
{
	return req->iheader;
}

size_t
request_param_size(struct request *req)
{
	return req->iparam;
}

void
request_uri_set(struct request *req, const char *uri)
{
	/* Prevent memory leak, if uri is set on already set uri */
	if (req->uri)
		free(req->uri);

	req->uri = strdup(uri);
}

char *
request_param_get(struct request *req, const char *key)
{
	size_t len = request_param_size(req);

	for (int i = 0; i < (int) len; ++i) {
		if (strcmp(req->param_keys[i], key) == 0)
			return req->param_values[i];
	}

	return NULL; /* Not found */
}

char *
request_header_get(struct request *req, const char *key)
{
	size_t len = request_header_size(req);

	for (int i = 0; i < (int) len; ++i) {
		if (strcmp(req->header_keys[i], key) == 0)
			return req->header_values[i];
	}

	return NULL; /* Not found */
}


static bool
request_isheader(char *line)
{
	return strchr(line, ':') != NULL;
}

static bool
request_uri_parse(struct request *req, char *line)
{
	char *p;
	char type_buf[100] = {0};
	bool is_valid_req = false;

	if ((p = strchr(line, ' ')) == NULL)
		return false;

	*p = 0;

	// @Incomplete not safe
	strncpy(type_buf, line, sizeof(type_buf) - 1);

	for (int i = 0; REQ_TYPES_STRING[i]; ++i) {
		if (strcmp(REQ_TYPES_STRING[i], type_buf) == 0) {
			req->type = REQ_TYPES_CODE[i];
			is_valid_req = true;
		}
	}

	*p = ' ';
	if (is_valid_req)
		req->uri = strdup(p + 1);

	return is_valid_req;
}

static bool
request_extract_key_value(struct request *req, char *line, char delim)
{
	char *p;

	if ((p = strchr(line, delim)) == NULL)
		return false;

	*p = 0;
	if (delim == REQ_HEADER_DELIM) {
		str_trim(line);
		str_trim(p + 1);
		request_header_set(req, line, p + 1);
		*p = REQ_HEADER_DELIM;
	} else if (delim == REQ_PARAM_DELIM) {
		str_trim(line);
		str_trim(p + 1);
		request_param_set(req, line, p + 1);
		*p = REQ_PARAM_DELIM;
	}

	return true;
}

static ssize_t
getstring(char **line)
{
	ssize_t len = 0, asize = 1024;

	if (reqbufp == NULL)
		return -1;

	char *s = malloc(sizeof(char) * asize);
	if (s == NULL) {
		perror("malloc error: failed to allocate memory for line");
		*line = NULL;
		return -1;
	}

	while (*reqbufp != '\n' && (reqbuf_datai != reqbuf_datalen)) {
		
		/* Grow the size of the 's' */
		if (len == asize) {
			asize *= 2;
			char *tmp = realloc(s, asize);
			if (tmp == NULL) {
				perror("realloc error: failed to grow memory");
				break;
			}

			if (tmp != s)
				free(s);
			s = tmp;
		}
		s[len++] = *reqbufp;

		++reqbufp;
		++reqbuf_datai;
	}

	/* When reqbufp hits the new line char then the while loop exits
	 * without entering in loop body therefor leaving reqbufp to point to 
	 * new line char and it never get passed to new line char for subsequent
	 * next call. Hence below check handles this case */
	if (*reqbufp == '\n') {
		++reqbufp;
		++reqbuf_datai;
	}

	s[len] = 0;
	if (!len && reqbuf_datai == reqbuf_datalen) {
		len = -1;
		reqbufp = NULL;
		reqbuf_datalen = reqbuf_datai = 0;
	}

	*line = s;

	return len;
}

char *request_uri_get(struct request *req)
{
	return (char *)req->uri;
}

ssize_t request_prepare(struct request *req, char *buf, ssize_t buflen)
{
	const char *req_type, *ptr;
	ssize_t bufi;

	bufi = 0;
	if ((req_type = request_type_str_get(req)) == NULL) {
		return -1; /* Error */
	}

	/* Constructing request type */
	ptr = req_type;
	while (*ptr && bufi < buflen)
		buf[bufi++] = *ptr++;

	if (bufi < buflen)
		buf[bufi++] = ' ';

	/* Constructing the uri */
	ptr = request_uri_get(req);
	while (*ptr && bufi < buflen)
		buf[bufi++] = *ptr++;

	if (bufi < buflen)
		buf[bufi++] = '\n';

	/* Constructing headers */
	for (int i = 0, hsize = request_header_size(req);
			i < hsize && bufi < buflen; ++i) {
		/* Copy header keys */
		ptr = req->header_keys[i];
		while (*ptr && bufi < buflen)
			buf[bufi++] = *ptr++;

		if (bufi < buflen) buf[bufi++] = ':';
		if (bufi < buflen) buf[bufi++] = ' ';

		/* Copy header values */
		ptr = req->header_values[i];
		while (*ptr && bufi < buflen)
			buf[bufi++] = *ptr++;

		if (bufi < buflen)
			buf[bufi++] = '\n';
	}
	if (req->isbinary) {
		int nbytes = sprintf(buf + bufi, "content-len: %lu\n", req->bodylen);
		bufi += nbytes;
	}

	/* Marking header end and starting of request body */
	if (bufi < buflen)
		buf[bufi++] = '\n';

	if (req->isbinary) {
		return request_body_append_binary(req, buf, buflen, bufi);
	}

	/* Constructing params */
	for (int i = 0, psize = request_param_size(req);
			i < psize && bufi < buflen; ++i) {
		/* Copy param keys */
		ptr = req->param_keys[i];
		while (*ptr && bufi < buflen)
			buf[bufi++] = *ptr++;

		if (bufi < buflen)
			buf[bufi++] = '=';

		/* Copy param values */
		ptr = req->param_values[i];
		while (*ptr && bufi < buflen)
			buf[bufi++] = *ptr++;

		if (bufi < buflen)
			buf[bufi++] = '\n';
	}

	/* NUL byte for string termination */
	buf[bufi] = 0;

	return bufi;
}

void request_dump(struct request *req)
{
	if (req == NULL)
		return;

	fprintf(stderr, "{\n");
	fprintf(stderr, "    type: \"%d\"\n", req->type);
	fprintf(stderr, "    uri: \"%s\"\n", req->uri);

	fprintf(stderr, "    header: [\n");
	for (size_t i = 0; i < request_header_size(req); ++i) {
		if (i == request_header_size(req) - 1) {
			fprintf(stderr, "        %s: \"%s\"\n",
					req->header_keys[i],
					req->header_values[i]);
		} else {
			fprintf(stderr, "        %s: \"%s\",\n",
					req->header_keys[i],
					req->header_values[i]);
		}
	}
	fprintf(stderr, "    ]\n");

	fprintf(stderr, "    params: [\n");
	for (size_t i = 0; i < request_param_size(req); ++i) {
		if (i == request_param_size(req) - 1) {
			fprintf(stderr, "        %s: \"%s\"\n",
					req->param_keys[i],
					req->param_values[i]);
		} else {
			fprintf(stderr, "        %s: \"%s\",\n",
					req->param_keys[i],
					req->param_values[i]);
		}
	}
	fprintf(stderr, "    ]\n");

	fprintf(stderr, "    body: \"%s\"\n", req->body);

	fprintf(stderr, "}\n");
}

static bool
request_continue_parsing_binary(struct request *req)
{
	const char *content_lens;
	int content_len;

	content_lens = request_header_get(req, "content-len");
	if (!content_lens) {
		fprintf(stderr, "request_continue_parsing_binary: no content length header found\n");
		return false;
	}

	content_len = atoi(content_lens);
	req->isbinary = 1;
	req->bodylen  = content_len;

	if (!(req->body = malloc(content_len))) {
		perror("request_continue_parsing_binary");
		return false;
	}

	memcpy(req->body, reqbufp, content_len);

	return true;
}

static ssize_t
request_body_append_binary(struct request *req, char *buf, size_t buflen, ssize_t offset)
{
	if (req->bodylen > (buflen - offset))
		return -1;

	memcpy(buf + offset, req->body, req->bodylen);

	return req->bodylen + offset;
}

void request_body_binary_set(struct request *req, u_int8_t *buf, size_t buflen)
{
	req->isbinary = 1;

	request_header_set(req, "content-type", REQ_HEADER_BINARY_VALUE);

	if ((req->body = malloc(buflen)) == NULL) {
		perror("request_body_binary_set error");
		return;
	}

	memcpy(req->body, buf, buflen);

	req->bodylen = buflen;

}