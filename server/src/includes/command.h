#ifndef __COMMAND_H_
#define __COMMAND_H_ 

#include <ecafe.h>

struct command {
	char *cmd;
	int  (*req_handle)(struct request *req);
};

extern const struct command commands[];

int command_get_index(const char *cmd);

#endif