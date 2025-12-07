CC = gcc
CFLAGS = -Wall
SRC_FILES = src/socket_md.c src/utils.c
TEST_SRC_FILES = $(SRC_FILES)

.PHONY: all tests clean

all: server client

server: $(SRC_FILES) src/server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: $(SRC_FILES) src/client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs

test: $(TEST_SRC_FILES) tests/client_test.c
	$(CC) $(CFLAGS) $^ -o test
	./test

clean:
	rm -f rfs
	rm -f server
