#include <stdio.h>
#include <command.h>

#define COMMAND_LOCK "lock"
#define COMMAND_UNLOCK "unlock"
#define COMMAND_PING "ping"
#define COMMAND_MESSAGE "message"
#define COMMAND_POWEROFF "poweroff"
#define COMMAND_ACTION "action"
#define COMMAND_GETDETAILS "getdetails"
#define COMMAND_SCREENSHOT "screenshot"

#define URI_LOCK "/lock"
#define URI_UNLOCK "/unlock"
#define URI_PING "/ping"
#define URI_MESSAGE "/message"
#define URI_POWEROFF "/poweroff"
#define URI_ACTION "/action"
#define URI_GETDETAILS "/getdetails"
#define URI_SCREENSHOT "/screenshot"

const struct command commands[] = {
	{COMMAND_LOCK,       URI_LOCK,       ecafe_request_lock,       ecafe_response_lock, NULL},
	{COMMAND_UNLOCK,     URI_UNLOCK,     ecafe_request_unlock,     ecafe_response_unlock, NULL},
	{COMMAND_PING,       URI_PING,       ecafe_request_ping,       ecafe_response_ping, NULL},
	{COMMAND_MESSAGE,    URI_MESSAGE,    ecafe_request_message,    ecafe_response_message, NULL},
	{COMMAND_POWEROFF,   URI_POWEROFF,   ecafe_request_poweroff,   ecafe_response_poweroff, NULL},
	{COMMAND_ACTION,     URI_ACTION,     ecafe_request_action,     ecafe_response_action, NULL},
	{COMMAND_GETDETAILS, URI_GETDETAILS, ecafe_request_getdetails, NULL, ecafe_response_getdetails},
	{COMMAND_SCREENSHOT, URI_SCREENSHOT, ecafe_request_screenshot, NULL, ecafe_response_screenshot},
	{NULL, NULL, NULL, NULL}
};

int command_get_index_by_uri(const char *uri)
{
	for (int i = 0; commands[i].uri; ++i) {
		if (strcmp(uri, commands[i].uri) == 0)
			return i;
	}

	return -1;
}

int command_is_getdetails(const char *uri)
{
	return strcmp(uri, URI_GETDETAILS) == 0;
}
