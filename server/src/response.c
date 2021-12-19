#include "response.h"
#include <str.h>

static const struct {
	int code;
	char *msg;
} status[] = {
	{RES_STATUS_DATA, 				RES_STATUS_DATA_MSG				},
	{RES_STATUS_OK,  			 	RES_STATUS_OK_MSG  			 	},
	{RES_STATUS_CREATED, 	 		RES_STATUS_CREATED_MSG 	 		},
	{RES_STATUS_ERROR, 		 		RES_STATUS_ERROR_MSG 		 	},
	{RES_STATUS_ERROR_UNAUTH, 		RES_STATUS_ERROR_UNAUTH_MSG 	},
	{RES_STATUS_ERROR_NOTFOUND,		RES_STATUS_ERROR_NOTFOUND_MSG	},
	{RES_STATUS_ERROR_PAYLOAD,		RES_STATUS_ERROR_PAYLOAD_MSG	},
	{RES_STATUS_ERROR_TOOMANY,		RES_STATUS_ERROR_TOOMANY_MSG	},
	{RES_STATUS_INTERNAL,			RES_STATUS_INTERNAL_MSG			},
	{0,								NULL							},

};

static int response_iscomplete(const char *buf, size_t size);
static bool response_extract_key_value(struct response *res, char *line, char delim);
static bool response_continue_parsing_binary(struct response *res, char *bufptr);
static ssize_t response_body_append_binary(struct response *res, char *buf, size_t buflen, ssize_t offset);


void response_header_set(struct response *res, const char *key, const char *value)
{
	int i;
	size_t len = response_header_size(res);

	for (i = 0; i < (int) len; ++i) {
		if (strcmp(res->header_keys[i], key) == 0)
			break;
	}

	// Found the key
	if (i != (int) len) {
		free(res->header_keys[i]);
		free(res->header_values[i]);
		// Shift elements left
		for (; i < (int) len - 1; ++i) {
			res->header_keys[i] = res->header_keys[i + 1];
			res->header_values[i] = res->header_values[i + 1];
		}
		res->nheader--;
	}

	if (res->nheader == RES_MAX_PARAM_LEN - 1)
		return;

	res->header_keys[res->nheader] 		= strdup(key);
	res->header_values[res->nheader++] 	= strdup(value);
}

char *response_header_get(struct response *res, const char *key)
{
	size_t len = response_header_size(res);

	for (int i = 0; i < (int) len; ++i) {
		if (strcmp(res->header_keys[i], key) == 0)
			return res->header_values[i];
	}

	return NULL; /* Not found */
}

size_t response_header_size(struct response *res)
{
	return res->nheader;
}

int response_status_get(struct response *res)
{
	if (res == NULL)
		return -1;

	return res->status;
}

int response_status_set(struct response *res, int code)
{
	if (res == NULL || !response_status_isvalid(code))
		return -1;

	res->status = code;
	res->msg = response_status_msg_get(res);
	return 0;
}


bool response_status_isvalid(int code)
{
	if (code < RES_STATUS_DATA || code > RES_STATUS_INTERNAL)
		return false;

	return true;
}

int response_record_keyval_set(struct response *res, int irecord,
		const char *key, const char *val)
{
	if (res == NULL || key == NULL || val == NULL)
		return -1;
	if (irecord < 0 || irecord >= RES_MAX_RECORD_LEN)
		return -1;

	int *nkeyvals = &res->records[irecord].nkeyvals;
	if (*nkeyvals >= RES_MAX_KEYVAL_LEN)
		return -1;

	/* When a record is added in the records array in which no keyval was
	 * added previously that means this new entry will go in new record */
	if (*nkeyvals == 0)
		res->nrecords += 1;

	res->records[irecord].keyvals[*nkeyvals].key = strdup(key);
	res->records[irecord].keyvals[*nkeyvals].val = strdup(val);
	*nkeyvals = *nkeyvals + 1;

	return 0;
}

int response_keyval_push(struct record *rec, const char *key, const char *val)
{
	if (rec == NULL || key == NULL || val == NULL)
		return -1;

	int *nkeyvals = &rec->nkeyvals;

	if (*nkeyvals >= RES_MAX_KEYVAL_LEN)
		return -1;

	rec->keyvals[*nkeyvals].key = strdup(key);
	rec->keyvals[*nkeyvals].val = strdup(val);
	*nkeyvals = *nkeyvals + 1;

	return 0;
}

char *response_keyval_get(struct record *rec, const char *key)
{
	if (rec == NULL || key == NULL)
		return NULL;

	int *nkeyvals = &rec->nkeyvals;

	if (*nkeyvals >= RES_MAX_KEYVAL_LEN)
		return NULL;

	for (int i = 0; i < *nkeyvals; ++i) {
		if (strcmp(rec->keyvals[i].key, key) == 0)
			return rec->keyvals[i].val;
	}

	return NULL;
}

int response_record_push(struct response *res, struct record *rec)
{
	if (res == NULL || rec == NULL)
		return -1;

	int *nrecords = &res->nrecords;

	if (*nrecords >= RES_MAX_RECORD_LEN)
		return -1;

	res->records[*nrecords] = *rec;
	*nrecords = *nrecords + 1;

	return 0;
}


int response_prepare(struct response *res, char *buf, size_t size)
{
	char *ptr;
	size_t bufi;
	int st, nrecords, nkeyvals;
	struct record *rec;

	if (res == NULL || buf == NULL)
		return -1;

	buf[0] = 0;
	st = res->status;

	/* Prepare response header which consists of two part
	 * <code> <message> */
	bufi = sprintf(buf, "%d %s\r\n", st, res->msg);

	/* Constructing headers */
	for (int i = 0, hsize = response_header_size(res);
			i < hsize && bufi < size; ++i) {
		/* Copy header keys */
		ptr = res->header_keys[i];
		while (*ptr && bufi < size)
			buf[bufi++] = *ptr++;

		if (bufi < size) buf[bufi++] = ':';
		if (bufi < size) buf[bufi++] = ' ';

		/* Copy header values */
		ptr = res->header_values[i];
		while (*ptr && bufi < size)
			buf[bufi++] = *ptr++;

		if (bufi < size)
			buf[bufi++] = '\r';
		if (bufi < size)
			buf[bufi++] = '\n';
	}
	if (res->isbinary) {
		int nbytes = sprintf(buf + bufi, "content-len: %lu\r\n", res->bodylen);
		bufi += nbytes;
	}

	/* Marking header end and starting of response body */
	if (res->nrecords) {
		if (bufi < size) buf[bufi++] = '\r';
		if (bufi < size) buf[bufi++] = '\n';
	}

	if (res->isbinary) {
		return response_body_append_binary(res, buf, size, bufi);
	}

	/* Now status is 100 therefore we need to add the response data in buf */
	nrecords = res->nrecords;
	for (int i = 0; i < RES_MAX_RECORD_LEN && nrecords; ++i) {
		rec = &res->records[i];
		nkeyvals = rec->nkeyvals;

		if (rec->keyvals[0].key != NULL)
			--nrecords;

		for (int j = 0; j < nkeyvals; ++j) {
			/* Iterate on chars of key */
			ptr = rec->keyvals[j].key;
			while (*ptr && bufi < size) {
				buf[bufi++] = *ptr;
				++ptr;
			}
			if (bufi < size)
				buf[bufi++] = '=';
			/* Iterate on chars of val */
			ptr = rec->keyvals[j].val;
			while (*ptr && bufi < size) {
				buf[bufi++] = *ptr;
				++ptr;
			}

			if (bufi < size)
				buf[bufi++] = '\n';
		}
		/* Last char added in keyval is \n this will not be last char instead
		 * CRLF will be last two char of every record therefore remove the
		 * LF and add CRLF */
		bufi--;
		if (bufi < size - 2) {
			buf[bufi++] = '\r';
			buf[bufi++] = '\n';
		}
	}
	if (bufi < size - 2) {
		buf[bufi++] = '\r';
		buf[bufi++] = '\n';
	}

	return bufi >= size? -1 : (int) bufi;
}


int response_parse(char *buf, size_t size, struct response *res)
{
	char *last, *ptr, *prev, *key, *value;
	struct record rec = {0};

	ptr 	= buf;
	last 	= &buf[size - 1];

	/* 1. Extract response status (first line of header) */
	if ((ptr = strchr(ptr, '\r')) == NULL) {
		fprintf(stderr, "%s: status message not found\n", __func__);
		return -1;
	}
	res->status = strtol(buf, NULL, 10);
	res->msg = response_status_msg_get(res);

	/* 1.1. Check if header has ended 
	 *   "\r\n\r\n" is the seperator for header and body */
	if (strncmp(ptr, "\r\n\r\n", 4) == 0) {
		fprintf(stderr, "%s: empty header found\n", __func__);
		return 0; /* It is not an error (header and body are optional) */
	} else if (strncmp(ptr, "\r\n", 2) != 0) {
		fprintf(stderr, "%s: invalid header, status line ending is invalid\n", __func__);
		return -1;
	}

	/* 1.2. ptr points to first header key */
	ptr += 2;

	/* 2. Extract header */
	prev = ptr;
	while (ptr < last) {
		prev = ptr;
		if ((ptr = strchr(ptr, RES_HEADER_DELIM)) == NULL) {
			fprintf(stderr, "%s:%d: invalid header\n", __func__, __LINE__);
			return -1;
		}
		key = strndup(prev, ptr - prev);
		prev = ptr;

		if ((ptr = strchr(ptr, '\r')) == NULL) {
			fprintf(stderr, "%s:%d: invalid header\n", __func__, __LINE__);
			return -1;
		}
		value = strndup(prev + 1, ptr - prev - 1);
		str_trim(value);

		response_header_set(res, key, value);
		free(key);
		free(value);

		/* Check if header ended, if so then break otherwise update the prev and ptr pointers */
		if (strncmp(ptr, "\r\n\r\n", 4) == 0) {
			ptr += 4;
			break;
		} else if (strncmp(ptr, "\r\n", 2) != 0) {
			fprintf(stderr, "%s:%d: invalid header, status line ending is invalid\n", __func__, __LINE__);
			return -1;
		}

		ptr += 2;
	}
	/* @TODO The binary content handling */

	/* 3. Extract body */
	while (ptr < last) {
		prev = ptr;
		if ((ptr = strchr(ptr, '\r')) == NULL || ptr >= last) {
			fprintf(stderr, "%s:%d: invalid record termination\n", __func__, __LINE__);
			return -1;
		}
		/* tmp = points to end of the record */
		ptr = prev;
		while (1) {
			/* If there are no more key/value then the body of the response is invalid */
			if ((ptr = strchr(ptr, '=')) == NULL) {
				fprintf(stderr, "%s:%d: invalid keyval record\n", __func__, __LINE__);
				return -1;
			}
			key = strndup(prev, ptr - prev);
			prev = ptr; /* prev = points to '=' */
			if ((ptr = strchr(ptr, '\n')) == NULL) {
				free(key);
				fprintf(stderr, "%s:%d: invalid key value format\n", __func__, __LINE__);
				return -1;
			}

			/* If ptr - 1 char is '\r' then it is end of record so handle the
			 * '\r' char in value and break out of loop otherwise handle value normally */
			if (*(ptr - 1) == '\r') {
				ptr = ptr - 1;
				value = strndup(prev + 1, ptr - prev - 1);
				response_keyval_push(&rec, key, value);
				free(key);
				free(value);
				break;
			} else {
				value = strndup(prev + 1, ptr - prev - 1);
				response_keyval_push(&rec, key, value);
			}
			free(key);
			free(value);
			ptr += 1; /* ptr points to nex key of the record */
			prev = ptr;
		}
		response_record_push(res, &rec);
		rec.nkeyvals = 0; /* reset record so that rec can be reused */

		/* If this is the last record of the body then break */
		if (strncmp(ptr, "\r\n\r\n", 4) == 0)
			break;
		ptr += 2; /* moving ptr to point to next record */
	}

	return 0;
}


char *response_status_msg_get(struct response *res)
{
	if (res == NULL || res->status == 0)
		return NULL;
	if (res->msg)
		return res->msg;

	for (int i = 0; status[i].code; ++i) {
		if (status[i].code == res->status)
			return status[i].msg;
	}

	return NULL;
}

void response_dump(struct response *res)
{
	if (res == NULL)
		return;
	int nrecords, nkeyvals;
	char *key, *val, *end_obj_mark;
	bool is_empty;

	/* Dump status code and message */
	fprintf(stderr, "{\n");
	fprintf(stderr, "    %d %s\n", res->status, res->msg);

	/* Header structure */
	fprintf(stderr, "    {\n");
	for (int i = 0; i < res->nheader; ++i) {
		fprintf(stderr, "        \"%s\": \"%s\"%s\n",
				res->header_keys[i],
				res->header_values[i],
				(res->nheader == i + 1) ? "" : ",");
	}
	fprintf(stderr, "    }\n");

	fprintf(stderr, "    [\n");

	nrecords = res->nrecords;
	for (int i = 0; i < RES_MAX_RECORD_LEN && nrecords; ++i) {
		is_empty = res->records[i].keyvals[0].key == NULL;
		if (!is_empty) --nrecords;

		fprintf(stderr, "        %s", is_empty? "{" : "{\n");
		
		nkeyvals = res->records[i].nkeyvals;
		for (int j = 0; j < nkeyvals; ++j) {
			key = res->records[i].keyvals[j].key;
			val = res->records[i].keyvals[j].val;
			fprintf(stderr, "            \"%s\": \"%s\"", key, val);
			fprintf(stderr, "%s", j == nkeyvals - 1? "\n" : ",\n");
		}

		if (is_empty && nrecords == 0) end_obj_mark = "}";
		else if (is_empty && nrecords != 0) end_obj_mark = "},";
		else if (!is_empty && nrecords == 0) end_obj_mark = "        }";
		else if (!is_empty && nrecords != 0) end_obj_mark = "        },";
		fprintf(stderr, "%s\n", end_obj_mark);
	}

	fprintf(stderr, "    ]\n");
	fprintf(stderr, "}\n");
}

void response_body_binary_set(struct response *res, u_int8_t *buf, size_t buflen)
{
	res->isbinary = 1;

	response_header_set(res, "content-type", RES_HEADER_BINARY_VALUE);

	if ((res->body = malloc(buflen)) == NULL) {
		perror("response_body_binary_set error");
		return;
	}

	memcpy(res->body, buf, buflen);

	res->bodylen = buflen;

}



static bool response_extract_key_value(struct response *res, char *line, char delim)
{
	char *p;

	if ((p = strchr(line, delim)) == NULL)
		return false;

	*p = 0;
	if (delim == RES_HEADER_DELIM) {
		str_trim(line);
		str_trim(p + 1);
		response_header_set(res, line, p + 1);
		*p = RES_HEADER_DELIM;
	} 

	return true;
}

static bool response_continue_parsing_binary(struct response *res, char *bufptr)
{
	const char *content_lens;
	int content_len;

	content_lens = response_header_get(res, "content-len");
	if (!content_lens) {
		fprintf(stderr, "response_continue_parsing_binary: no content length header found\n");
		return false;
	}

	content_len = atoi(content_lens);
	res->isbinary = 1;
	res->bodylen  = content_len;

	if (!(res->body = malloc(content_len))) {
		perror("response_continue_parsing_binary");
		return false;
	}

	memcpy(res->body, bufptr, content_len);

	return true;
}

static int response_iscomplete(const char *buf, size_t size)
{
	/* If size is less than 4 than EOR marker is missing
	 * that is '\r\n\r\n' */
	if (size < 4)
		return 0;
	if (buf[size - 1] != '\n'
			|| buf[size - 2] != '\r'
			|| buf[size - 3] != '\n'
			|| buf[size - 4] != '\r')
		return 0;

	return 1;
}

static ssize_t response_body_append_binary(struct response *res, char *buf, size_t buflen, ssize_t offset)
{
	if (res->bodylen > (buflen - offset))
		return -1;

	memcpy(buf + offset, res->body, res->bodylen);

	return res->bodylen + offset;
}
