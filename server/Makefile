default: all

UNAME_P := $(shell uname -p)

COMPILER_EXECUTABLE=$(CROSS_COMPILE)gcc

SERVER_OBJ = -o aesdsocket server.c 

clean:
	rm -f aesdsocket

vars:
    UNAME_P := $(shell uname -p)

all: server

server: vars
	$(info $(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))
	@($(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))









