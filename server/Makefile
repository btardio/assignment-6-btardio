# this is a system for squashfs - you cant load a binary on a squashfs b/c of striping
#
#C_COMPILER=gcc

UNITY_ROOT=test/Unity/

# CFLAGS=-std=c89
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
#CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
#CFLAGS += -Wno-misleading-indentation


default: all

UNAME_P := $(shell uname -p)

#COMPILER_EXECUTABLE=$(CROSS_COMPILE)gcc

TARGET_SERVER = aesdsocket
TARGET_TEST = aesdsocket_test

#SERVER_OBJ = -o aesdsocket src/server.c src/serverf.c
#TEST_OBJ = -o aesdsocket_test server_test.c

#PROGRAM=server
#BINDIR=/usr/local/bin
SRC_FILES=src/serverf.c src/server.c
TEST_SRC_FILES=$(UNITY_ROOT)/src/unity.c src/serverf.c test/server_test.c test/test_runners/server_test_runner.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
SYMBOLS=

clean:
	rm -f aesdsocket
	rm -f aesdsocket_test

all: clean $(TARGET_SERVER)

$(TARGET_SERVER):
	#$(info $(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))
	#@($(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(SERVER_OBJ))
	$(CC) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET_SERVER)

test: server_test_runner
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) -g $(TEST_SRC_FILES) -o $(TARGET_TEST)

#test/test_runners/server_test_runner.c: test/server_test.c
server_test_runner: test/server_test.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/server_test.c test/test_runners/server_test_runner.c
	#$(info $(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(TEST_OBJ))
	#@($(COMPILER_EXECUTABLE) -Wall $(CCFLAGS) $(TEST_OBJ))

#install:
#	install -d ${D}${bindir}
#	install -m 755 $(TARGET_SERVER) ${D}${bindir}/





