#ifndef __RESPONSE_H
#define __RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define RES_MAX_PARAM_LEN 		  		10
#define RES_MAX_PAYLOAD_LEN		  	  2048 /* Max len of response body */
#define RES_MAX_RECORD_LEN		  		20 /* Max len of records       */
#define RES_MAX_KEYVAL_LEN		  		20 /* Max len of key val       */

#define RES_STATUS_DATA			 		100
#define RES_STATUS_OK  			 		200
#define RES_STATUS_CREATED  	 		201 /* The request has succeeded and a new resource has been created as a result. */
#define RES_STATUS_ERROR  		 		400
#define RES_STATUS_ERROR_UNAUTH  		401 /* The client must authenticate itself to get the requested response. */
#define RES_STATUS_ERROR_NOTFOUND 		404 /* The server can not find the requested resource, means the URL is not recognized. */
#define RES_STATUS_ERROR_PAYLOAD 		413 /* Request entity is larger than limits defined by server. */
#define RES_STATUS_ERROR_TOOMANY 		429 /* The user has sent too many requests in a given amount of time ("rate limiting"). */
#define RES_STATUS_INTERNAL 			500 /* Internal server error, and request can not be processed */

#define RES_STATUS_DATA_MSG		 		"Data message"
#define RES_STATUS_OK_MSG  			 	"OK"
#define RES_STATUS_CREATED_MSG 	 		"Created"
#define RES_STATUS_ERROR_MSG 		 	"Bad Request"
#define RES_STATUS_ERROR_UNAUTH_MSG 	"Unauthorized"
#define RES_STATUS_ERROR_NOTFOUND_MSG	"Not Found"
#define RES_STATUS_ERROR_PAYLOAD_MSG	"Payload Too Large"
#define RES_STATUS_ERROR_TOOMANY_MSG	"Too Many Requests"
#define RES_STATUS_INTERNAL_MSG			"Internal Server Error"


struct keyval {
	char *key;
	char *val;
};

struct record {
	struct keyval 	keyvals[RES_MAX_KEYVAL_LEN];
	int 			nkeyvals;
};

struct response {
	int   			status;
	char 			*msg;
	char 			*header_keys[RES_MAX_PARAM_LEN];
	char 			*header_values[RES_MAX_PARAM_LEN];

	struct record 	records[RES_MAX_RECORD_LEN];
	char 			*body;
	size_t 			bodylen;
	int				nrecords;
	int				nheader;
};

int response_status_get(struct response *res);
int response_status_set(struct response *res, int code);
bool response_status_isvalid(int code);

/**
 * @Note This function call is dangerous 
 */
int response_record_keyval_set(struct response *res, int irecord,
		const char *key, const char *val);
int response_keyval_push(struct record *rec, const char *key, const char *val);
char *response_keyval_get(struct record *rec, const char *key);

int response_record_push(struct response *res, struct record *rec);
int response_prepare(struct response *res, char *buf, size_t size);
int response_parse(char *buf, size_t size, struct response *res);

/**
 * Get the response status message form res if non-null otherwise use the
 * status code to determine the status msg and return that message.
 * Note: Do not free the msg pointer.
 * 
 * @return char *  message string corresponding to response code set in res
 */
char *response_status_msg_get(struct response *res);

/**
 * Dumps struct response in json format. Helpful in debuging
 *
 */
void response_dump(struct response *res);

#endif
