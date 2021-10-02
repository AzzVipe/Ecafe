#ifndef __ECAFE_
#define __ECAFE_

#include <errno.h>
#include <request.h>
#include <response.h>
#include <client.h>
#include <ecafe_request.h>
#include <ecafe_response.h>

typedef struct {
	int id;
	int user_id;
	char *started_at;
	char *ended_at;
} Session;

typedef struct {
	int id;
	char *pincode;
	char *town;
	int district_id;
	int state_id;
} Town;

typedef struct {
	int id;
	char *foodname;
	int avaibility;
	int price;
} Food;

typedef struct {
	int id;
	int food_id;
	int quantity;
	int amount;
	int discount;
	int tax;
	int user_id;
	int state;
	char *created_at;
} Order;

typedef struct {
	int id;
	char *fullname;
	char *address_line;
	int town_id;
	int district_id;
	int state_id;
} Address ;

typedef struct {
	int id;
	char *district;
} District;

typedef struct {
	int id;
	char *state;
} State;

void ecafe_log_error(int Errno, char *filename, int line);

#define PRINT_ERROR(Errno) ecafe_log_error(Errno, __FILE__, __LINE__)
#define UNIXPATH    "/tmp/unixdomain"

int ecafe_request_handle(char *buf, int connfd);
int ecafe_response_handle(char *buf, int connfd);

int ecafe_getdetails(struct client *cli_info);
int ecafe_clientall(struct request *req, int connfd);
int ecafe_client(struct request *req, int connfd);


#endif