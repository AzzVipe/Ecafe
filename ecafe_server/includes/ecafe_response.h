/* Response Handler */

#ifndef __ECAFE_RESPONSE_
#define __ECAFE_RESPONSE_

#define BUFSIZE 1024

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <request.h>
#include <easyio.h>
#include <response.h>
#include <client.h>
#include <ecafe_request.h>

int  ecafe_response_send(struct response *res, int connfd);
int  ecafe_response_recv(int connfd, struct response *res);
int  ecafe_response_print(struct response *res, FILE *fp);
void ecafe_response_prepare_client(struct response *res, struct client *temp);

int ecafe_response_lock(struct response *res);
int ecafe_response_unlock(struct response *res);
int ecafe_response_action(struct response *res);
int ecafe_response_message(struct response *res);
int ecafe_response_ping(struct response *res);
int ecafe_response_timer(struct response *res);
int ecafe_response_poweroff(struct response *res);
int ecafe_response_notification(struct response *res);
int ecafe_response_client(struct response *res);

int ecafe_response_screenshot(struct response *res, int connfd);
int ecafe_response_getdetails(struct response *res, int connfd);

#endif