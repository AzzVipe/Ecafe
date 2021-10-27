#ifndef __ECAFE_REQUEST_
#define __ECAFE_REQUEST_ 

#include <stdio.h>
#include <unistd.h>
#include <easyio.h>
#include <request.h>

#include <macros.h>

/* Server Requests */

int ecafe_request_lock(struct request *req);
int ecafe_request_unlock(struct request *req);
int ecafe_request_action(struct request *req);
int ecafe_request_message(struct request *req);
int ecafe_request_ping(struct request *req);
int ecafe_request_poweroff(struct request *req);
int ecafe_request_screenshot(struct request *req);
int ecafe_request_notification(struct request *req);
int ecafe_request_getdetails(struct request *req);
int ecafe_request_client(struct request *req);

// int ecafe_request_handle(char *buf, int client);
int ecafe_request_send(struct request *req, int connfd);
int ecafe_request_recv(int connfd, struct request *req);



#endif