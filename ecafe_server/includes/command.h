#ifndef __COMMAND_H_
#define __COMMAND_H_ 

#include <macros.h>
#include <ecafe.h>

struct command {
	char *cmd;
	char *uri;
	int  (*req_handle)(struct request *req);
	int  (*req_handle_special)(struct request *req, int connfd);
	int  (*res_handle)(struct response *res);
	int  (*res_handle_special)(struct response *res, int connfd);
};

extern const struct command commands[];

int command_get_index(const char *cmd);
int command_get_index_by_uri(const char *uri);

char *command_get_uri(const char *cmd);

#endif