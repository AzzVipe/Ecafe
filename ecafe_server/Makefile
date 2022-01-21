CC=gcc

OBJSPATH=objs
SRCSPATH=src
LIBSPATH=libs
CFLAGS=-Iincludes -Ilibs
CFLAGS+=-g -Wall -Wmissing-declarations -Wshadow -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wpointer-arith -Wcast-qual -Wsign-compare

LIBS=$(LIBSPATH)/request.o $(LIBSPATH)/response.o $(LIBSPATH)/easyio.o $(LIBSPATH)/sock_lib.o \
		$(LIBSPATH)/str.o $(LIBSPATH)/validator.o $(LIBSPATH)/list.o $(LIBSPATH)/iter.o

OBJECTS=$(OBJSPATH)/ecafe.o $(OBJSPATH)/ecafe_request.o $(OBJSPATH)/ecafe_response.o \
	$(OBJSPATH)/client.o $(OBJSPATH)/command.o $(LIBS)

all: libs command ecafe_request ecafe_response ecafe client server test_server

server: server.c $(OBJECTS)
	$(CC) -o server server.c $(CFLAGS) $(OBJECTS) -lpthread -lm -ldl

test_server: test_server.c $(OBJECTS)
	$(CC) -o test_server test_server.c $(CFLAGS) $(OBJECTS) -lpthread -lm -ldl

ecafe: $(SRCSPATH)/ecafe.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe.o $(SRCSPATH)/ecafe.c $(CFLAGS)

ecafe_request: $(SRCSPATH)/ecafe_request.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe_request.o $(SRCSPATH)/ecafe_request.c $(CFLAGS)

ecafe_response: $(SRCSPATH)/ecafe_response.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe_response.o $(SRCSPATH)/ecafe_response.c $(CFLAGS)

client: $(SRCSPATH)/client.c
	$(CC) -c -g -o $(OBJSPATH)/client.o $(SRCSPATH)/client.c $(CFLAGS)

command: $(SRCSPATH)/command.c
	$(CC) -c -g -o $(OBJSPATH)/command.o $(SRCSPATH)/command.c $(CFLAGS)

libs: $(LIBSPATH)/request.c $(LIBSPATH)/response.c $(LIBSPATH)/easyio.c $(LIBSPATH)/sock_lib.c \
		$(LIBSPATH)/str.c $(LIBSPATH)/validator.c $(LIBSPATH)/list.c $(LIBSPATH)/iter.c
	$(CC) -c -g -o $(LIBSPATH)/request.o $(LIBSPATH)/request.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/response.o $(LIBSPATH)/response.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/easyio.o $(LIBSPATH)/easyio.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/sock_lib.o $(LIBSPATH)/sock_lib.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/str.o $(LIBSPATH)/str.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/validator.o $(LIBSPATH)/validator.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/list.o $(LIBSPATH)/list.c $(CFLAGS)
	$(CC) -c -g -o $(LIBSPATH)/iter.o $(LIBSPATH)/iter.c $(CFLAGS)
