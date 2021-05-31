#ifndef __ECAFE_
#define __ECAFE_

#include <request.h>
#include <response.h>
#include <command.h>
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

int ecafe_lock(struct request *req);
int ecafe_unlock(struct request *req);
int ecafe_message(struct request *req);
int ecafe_ping(struct request *req);
int ecafe_poweroff(struct request *req);
int ecafe_action(struct request *req);
int ecafe_getdetails(struct client *cli_info);
int ecafe_clientall(struct client ***clients);
int ecafe_screenshot(struct request *req);

#endif