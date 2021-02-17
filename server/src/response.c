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
	int status, nrecords, nkeyvals;
	struct record *rec;

	if (res == NULL || buf == NULL)
		return -1;

	buf[0] = 0;
	status = res->status;

	/* Prepare response header which consists of two part
	 * <code> <message> */
	bufi = sprintf(buf, "%d %s\r\n", status, res->msg);

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

	return bufi >= size? -1 : bufi;
}

int response_parse(char *buf, size_t size, struct response *res)
{
	int ret = 0;
	char *key, *val, *ptr, *last, *bufcp;
	struct record rec = {0};

	buf[size] = 0;
	bufcp = strndup(buf, size);
	last = &bufcp[size - 1];
	if (!bufcp)
		return -1;
	/* Check response received is complete */
	if (!response_iscomplete(bufcp, size))
		return -1;
	/* Extract header */
	if ((ptr = strchr(bufcp, ' ')) == NULL) {
		fprintf(stderr, "response_parse: no response status code found\n");
		return -1;
	}
	res->status = atoi(bufcp);
	res->msg = response_status_msg_get(res);
	ptr = strchr(ptr, '\n');
	++ptr;

	/* If current and next char is '\r' and '\n' respectively then we have
	 * reached end of record */
	if (ptr < last && *ptr == '\r' && *(ptr + 1) == '\n')
		ptr = last;

	/* Extracting records */
	while (ptr < last) {
		/* extract key */
		key = ptr;
		while (ptr <= last && *ptr != '=')
			++ptr;
		/* If ptr does not point to '=' then key is missing */
		if (*ptr != '=') {
			fprintf(stderr, "response_parse: key expected here but something else given: %s\n", key);
			ret = -1;
			break;
		}
		*ptr = 0; /* terminate key with null-byte in place of '=' char */
		++ptr;

		/* extract value */
		val = ptr;
		while (ptr <= last && *ptr != '\n')
			++ptr;
		if (*ptr != '\n') {
			fprintf(stderr, "response_parse: value expected here but something else given: %s\n", val);
			ret = -1;
			break;
		}
		*ptr = 0; /* terminate key with null-byte in place of '\n' char */
		++ptr;


		/* If this is the EOL that is end of single record, single end of
		 * record marker is \r\n, then push the record in res */
		if (*(ptr - 2) == '\r') {
			*(ptr - 2) = 0; /* remove last char '\r' */
			response_keyval_push(&rec, key, val);
			response_record_push(res, &rec);
			rec.nkeyvals = 0; /* reset record so that rec can be reused */
		} else {
			response_keyval_push(&rec, key, val);
		}

		/* If reached EOR (ie. '\r\n\r\n') then work is done */
		if (ptr < last && *ptr == '\r' && *(ptr + 1) == '\n')
			break;
	}
	free(bufcp);

	return ret;
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



