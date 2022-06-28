#include <client.h>

void client_dump(struct client *temp)
{
	printf("Id        : %d\n", temp->id);
	printf("Hostname  : %s\n", temp->hostname);
	printf("Username  : %s\n", temp->username);
	printf("Uptime    : %s\n", temp->uptime);
	printf("State     : %s\n", temp->state);
	printf("IP        : %s\n", temp->ip);
	printf("Pid       : %d\n\n", temp->pid);
}