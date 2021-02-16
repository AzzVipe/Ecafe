#include <stdio.h>
#include <regex.h>
#include <netdb.h>
#include <validator.h>
#include <string.h>

/*
static regex_t *regex_init(char *regexstr)
{
	static regex_t regex;

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return NULL;
	}

	return regex;
}
*/
bool validate_name(const char *name)
{
	regex_t regex;
	char regexstr[64];

	sprintf(regexstr, "^[a-zA-Z ]{2,32}$");

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return false;
	}

	if (regexec(&regex, name, 0, NULL, 0) == REG_NOMATCH)
		return false;

	return true;
}

bool validate_email(const char *email)
{
	regex_t regex;
	char regexstr[64];

	sprintf(regexstr, "^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-z0-9.-]+.[a-z]{2,}$");

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return false;
	}

	if (regexec(&regex, email, 0, NULL, 0) == REG_NOMATCH)
		return false;

	return true;
}

bool validate_site(const char *site)
{
	int rv;
	struct addrinfo *res, hints;
	const char service[] = "https";

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_CANONNAME;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(site, service, &hints, &res)) != 0) {
		printf("%s\n", gai_strerror(rv));

		return false;
	}

	freeaddrinfo(res);

	return true;
}

bool validate_number(const char *number)
{
	regex_t regex;
	char regexstr[64] = "^[0-9]+$";

	puts(number);

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return false;
	}

	if (regexec(&regex, number, 0, NULL, 0) == REG_NOMATCH)
		return false;

	return true;
}

bool validate_dob(const char *dob)
{
	regex_t regex;
	char regexstr[1024] = "^([0]?[1-9]|[1|2][0-9]|[3][0|1])[./-]([0]?[1-9]|[1][0-2])[./-]([0-9]{4}|[0-9]{2})$";

	puts(dob);

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return false;
	}

	if (regexec(&regex, dob, 0, NULL, 0) == REG_NOMATCH)
		return false;

	return true;
}

bool validate_pincode(const char *pincode)
{
	regex_t regex;
	char regexstr[256] = "^[1-9]{1}[0-9]{5}$";

	puts(pincode);

	if (regcomp(&regex, regexstr, REG_EXTENDED) != 0) {
		puts("regcomp error");
		return false;
	}

	if (regexec(&regex, pincode, 0, NULL, 0) == REG_NOMATCH)
		return false;
	
	return true;
}



