#ifndef __ECAFE_
#define __ECAFE_

#include <errno.h>
#include <request.h>
#include <response.h>
#include <client.h>
#include <ecafe_request.h>
#include <ecafe_response.h>

#define UNIXPATH    "/tmp/unixdomain"

void ecafe_log_error(int Errno, char *filename, int line);

#define PRINT_ERROR(Errno) ecafe_log_error(Errno, __FILE__, __LINE__)

int get_server_fd(void);

int ecafe_clientall(struct client ***clients);
int ecafe_client(struct client *client);
int ecafe_lock(struct request *req);
int ecafe_unlock(struct request *req);
int ecafe_message(struct request *req);
int ecafe_ping(struct request *req);
int ecafe_poweroff(struct request *req);
int ecafe_action(struct request *req);
int ecafe_timer(struct request *req);
int ecafe_getdetails(struct client *cli_info);
int ecafe_clientall(struct client ***clients);
int ecafe_screenshot(struct request *req);
int ecafe_notification(struct request *req);

#endif