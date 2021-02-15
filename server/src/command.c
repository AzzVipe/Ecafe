#include <stdio.h>
#include <command.h>

#define COMMAND_LOCK "lock"
#define COMMAND_UNLOCK "unlock"
#define COMMAND_PING "ping"
#define COMMAND_MESSAGE "message"
#define COMMAND_POWEROFF "poweroff"
#define COMMAND_ACTION "action"

const struct command commands[] = {
	{COMMAND_LOCK, ecafe_lock},
	{COMMAND_UNLOCK, ecafe_unlock},
	{COMMAND_PING, ecafe_ping},
	{COMMAND_MESSAGE, ecafe_message},
	{COMMAND_POWEROFF, ecafe_poweroff},
	{COMMAND_ACTION, ecafe_action},
	{NULL, NULL}
};

int command_get_index(const char *cmd)
{
	for (int i = 0; commands[i].cmd; ++i) {
		if (strcmp(cmd, commands[i].cmd) == 0)
			return i;
	}

	return -1;
}
