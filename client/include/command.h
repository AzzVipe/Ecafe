#ifndef __COMMAND_H_
#define __COMMAND_H_

#include <ecafe.h>

struct command {
	char *cmd;
	char *uri;
	int  (*req_handle)(struct request *req, struct response *res);
};

extern const struct command commands[];

int command_get_index(const char *cmd);
int command_get_index_by_uri(const char *uri);

#endif