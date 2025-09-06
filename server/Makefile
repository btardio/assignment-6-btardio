default: all

UNAME_P := $(shell uname -p)

COMPILER_EXECUTABLE=$(CROSS_COMPILE)gcc

SERVER_OBJ = -o aesdsocket server.c 
CLIENT_OBJ = -o client client.c

clean:
	rm -f aesdsocket
	rm -f client

vars:

    UNAME_P := $(shell uname -p)

all: server client

server: vars
	$(info $(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))
	@($(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))

client: vars
	$(info $(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(CLIENT_OBJ))
	@($(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(CLIENT_OBJ))








