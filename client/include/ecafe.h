#ifndef __ECAFE_H_
#define __ECAFE_H_


#include <request.h>
#include <ecafe_request.h>
#include <response.h>

#define ECAFE_APPNAME "Ecafe"

int ecafe_request_handle(char *buf, int server);

#endif