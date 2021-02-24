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
	bool body_started = false;
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

	while ((nbytes = getstring(&line)) >= 0) {
		++iline;
		fprintf(stderr, "%lu[%ld]: %s\n", iline, nbytes, line);
		if (nbytes == 0)
			continue;

		if (!body_started && request_isheader(line)) {
			// Extract header
			request_extract_key_value(req, line, REQ_HEADER_DELIM);
			continue;
		}

		body_started = true;

		request_extract_key_value(req, line, REQ_PARAM_DELIM);

		free(line);
	}

	return true;
}

void
request_param_set(struct request *req, const char *key, const char *value)
{
	if (req->iparam == REQ_MAX_PARAM_LEN - 1)
		return;

	req->param_keys[req->iparam] 		= strdup(key);
	req->param_values[req->iparam++] 	= strdup(value);
}

void
request_header_set(struct request *req, const char *key, const char *value)
{
	if (req->iheader == REQ_MAX_PARAM_LEN - 1)
		return;

	req->header_keys[req->iheader] 		= strdup(key);
	req->header_values[req->iheader++] 	= strdup(value);
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

	for (size_t i = 0; i < len; ++i) {
		if (strcmp(req->param_keys[i], key) == 0)
			return req->param_values[i];
	}

	return NULL; /* Not found */
}

char *
request_header_get(struct request *req, const char *key)
{
	size_t len = request_header_size(req);

	for (size_t i = 0; i < len; ++i) {
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
		request_header_set(req, line, p + 1);
		*p = REQ_HEADER_DELIM;
	} else if (delim == REQ_PARAM_DELIM) {
		str_trim(line);
		request_param_set(req, line, p + 1);
		*p = REQ_PARAM_DELIM;
	}

	return true;
}

static ssize_t
getstring(char **line)
{
	ssize_t len = 0, asize = 1024;

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

	/* Marking header end and starting of request body */
	if (bufi < buflen)
		buf[bufi++] = '\n';

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
