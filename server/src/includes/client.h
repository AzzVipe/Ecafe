#ifndef __CLIENT_H_
#define __CLIENT_H_ 

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

struct client_timer {
	time_t created_at;
	time_t duration;
};

struct client {
	int id;
	int fd;
	pid_t pid;
	char *hostname;
	char *username;
	char *ip;
	char *state;
	char *uptime;
	char *timer_uri;
	long timer_created_at;
	long timer_duration;
};

void client_dump(struct client *temp);

#endif