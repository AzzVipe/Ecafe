#ifndef __ECAFE_REQUEST_
#define __ECAFE_REQUEST_ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <request.h>
#include <response.h>
#include <easyio.h>
#include <validator.h>
#include <command.h>
#include <system_linux.h>

#define PARAM_MESSAGE  "msg"
#define PARAM_NOTIFICATION  "notification"
#define PARAM_FOOD_ID  "foodid"
#define PARAM_FOOD_QTY "qty"
#define PARAM_PROG     "prog"
#define PARAM_STATE    "state"

int ecafe_request_lock(struct request *req, struct response *res);
int ecafe_request_unlock(struct request *req, struct response *res);
int ecafe_request_action(struct request *req, struct response *res);
int ecafe_request_message(struct request *req, struct response *res);
int ecafe_request_ping(struct request *req, struct response *res);
int ecafe_request_poweroff(struct request *req, struct response *res);
int ecafe_request_getdetails(struct request *req, struct response *res);
int ecafe_request_screenshot(struct request *req, struct response *res);
int ecafe_request_notification(struct request *req, struct response *res);

int ecafe_response_send(struct response *res, int connfd);




#endif