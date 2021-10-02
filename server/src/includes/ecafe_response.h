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
#include <ecafe_request.h>

int ecafe_response_send(struct response *res, int connfd);
int ecafe_response_recv(int connfd, struct response *res);

#endif