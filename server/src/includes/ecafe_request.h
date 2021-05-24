#ifndef __ECAFE_REQUEST_
#define __ECAFE_REQUEST_ 

#include <stdio.h>
#include <unistd.h>
#include <easyio.h>
#include <request.h>
#include <command.h>


#define PARAM_MESSAGE  "message"
#define PARAM_FOOD_ID  "foodid"
#define PARAM_FOOD_QTY "qty"
#define PARAM_PROG     "prog"
#define PARAM_STATE    "state"

/* Server Requests */

int ecafe_request_lock(struct request *req);
int ecafe_request_unlock(struct request *req);
int ecafe_request_action(struct request *req);
int ecafe_request_message(struct request *req);
int ecafe_request_ping(struct request *req);
int ecafe_request_poweroff(struct request *req);
int ecafe_request_screenshot(struct request *req);
int ecafe_request_getdetails(struct request *req);

// int ecafe_request_handle(char *buf, int client);
int ecafe_request_send(int client, struct request *req);



#endif