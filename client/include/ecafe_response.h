#ifndef __ECAFE_RESPONSE_
#define __ECAFE_RESPONSE_ 

#include <stdio.h>
#include <stdlib.h>
#include <response.h>
#include <request.h>
#include <command.h>

#define PARAM_MESSAGE  "message"
#define PARAM_FOOD_ID  "foodid"
#define PARAM_FOOD_QTY "qty"
#define PARAM_PROG     "prog"
#define PARAM_STATE    "state"

int ecafe_response_lock(struct request *req, struct response *res);
int ecafe_response_unlock(struct request *req, struct response *res);
int ecafe_response_action(struct request *req, struct response *res);
int ecafe_response_message(struct request *req, struct response *res);
int ecafe_response_ping(struct request *req, struct response *res);
int ecafe_response_poweroff(struct request *req, struct response *res);

#endif