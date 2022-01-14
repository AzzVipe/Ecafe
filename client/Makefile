CC=gcc
SRCSPATH=src
LIBSPATH=lib
OBJSPATH=obj
CFLAGS=-Iinclude -I$(LIBSPATH) -Wall

ifdef dev
	CFLAGS+=-DDEV
endif

OBJS= $(OBJSPATH)/ecafe_request.o $(OBJSPATH)/ecafe.o $(OBJSPATH)/command.o $(OBJSPATH)/system_linux.o
	
LIBS= $(LIBSPATH)/request.o $(LIBSPATH)/response.o $(LIBSPATH)/easyio.o $(LIBSPATH)/str.o $(LIBSPATH)/sock_lib.o \
     $(LIBSPATH)/validator.o $(OBJS) 

# CFLAGS+=-fPIE
CFLAGS+=-g

object: ecafe_request ecafe command system_linux request response easyio sock_lib str validator

ecafe: $(SRCSPATH)/ecafe.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe.o $(SRCSPATH)/ecafe.c $(CFLAGS)

command: $(SRCSPATH)/command.c
	$(CC) -c -g -o $(OBJSPATH)/command.o $(SRCSPATH)/command.c $(CFLAGS)

ecafe_request: $(SRCSPATH)/ecafe_request.c
	$(CC) -c -g -o $(OBJSPATH)/ecafe_request.o $(SRCSPATH)/ecafe_request.c $(CFLAGS)

system_linux: $(SRCSPATH)/system_linux.c
	$(CC) -c -g -o $(OBJSPATH)/system_linux.o $(SRCSPATH)/system_linux.c $(CFLAGS)

client: client.c object
	$(CC) -o client client.c $(CFLAGS) $(LIBS) -lpthread -lm -ldl


request: $(LIBSPATH)/request.c
	$(CC) -c -g -o $(LIBSPATH)/request.o $(LIBSPATH)/request.c $(CFLAGS)

response: $(LIBSPATH)/response.c
	$(CC) -c -g -o $(LIBSPATH)/response.o $(LIBSPATH)/response.c $(CFLAGS)

easyio: $(LIBSPATH)/easyio.c
	$(CC) -c -g -o $(LIBSPATH)/easyio.o $(LIBSPATH)/easyio.c $(CFLAGS)

sock_lib: $(LIBSPATH)/sock_lib.c
	$(CC) -c -g -o $(LIBSPATH)/sock_lib.o $(LIBSPATH)/sock_lib.c $(CFLAGS)

str: $(LIBSPATH)/str.c
	$(CC) -c -g -o $(LIBSPATH)/str.o $(LIBSPATH)/str.c $(CFLAGS)

validator: $(LIBSPATH)/validator.c
	$(CC) -c -g -o $(LIBSPATH)/validator.o $(LIBSPATH)/validator.c $(CFLAGS)

