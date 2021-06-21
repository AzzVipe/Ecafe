CC=gcc

OBJSPATH=objs
CFLAGS=-Iincludes
CFLAGS+=-g -Wall -Wmissing-declarations -Wshadow -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wpointer-arith -Wcast-qual -Wsign-compare

OBJECTS=$(OBJSPATH)/ecafe.o $(OBJSPATH)/ecafe_request.o $(OBJSPATH)/ecafe_response.o \
	$(OBJSPATH)/client.o $(OBJSPATH)/command.o $(OBJSPATH)/request.o $(OBJSPATH)/response.o \
	$(OBJSPATH)/easyio.o $(OBJSPATH)/sock_lib.o $(OBJSPATH)/str.o $(OBJSPATH)/validator.o \
	$(OBJSPATH)/list.o $(OBJSPATH)/iter.o

all: ecafe ecafe_request ecafe_response client command libs server test_server

server: server.c $(OBJECTS)
	$(CC) -o server server.c $(CFLAGS) $(OBJECTS) -lpthread -lm -ldl

test_server: test_server.c $(OBJECTS)
	$(CC) -o test_server test_server.c $(CFLAGS) $(OBJECTS) -lpthread -lm -ldl

ecafe: ecafe.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe.o ecafe.c $(CFLAGS)

ecafe_request: ecafe_request.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe_request.o ecafe_request.c $(CFLAGS)

ecafe_response: ecafe_response.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe_response.o ecafe_response.c $(CFLAGS)

client: client.c
	$(CC) -c -g -o $(OBJSPATH)/client.o client.c $(CFLAGS)

command: command.c
	$(CC) -c -g -o $(OBJSPATH)/command.o command.c $(CFLAGS)

libs: request.c response.c easyio.c sock_lib.c str.c validator.c list.c iter.c
	$(CC) -c -g -o $(OBJSPATH)/request.o request.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/response.o response.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/easyio.o easyio.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/sock_lib.o sock_lib.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/str.o str.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/validator.o validator.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/list.o list.c $(CFLAGS)
	$(CC) -c -g -o $(OBJSPATH)/iter.o iter.c $(CFLAGS)
