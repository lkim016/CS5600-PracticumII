CC = gcc
CFLAGS = -Wall
SRC_FILES = src/socket_md.c src/utils.c src/socket_send.c src/socket_rcv.c
TEST_SRC_FILES = $(SRC_FILES)

.PHONY: all tests clean

all: server client

server: $(SRC_FILES) src/server_utils.c src/server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: $(SRC_FILES) src/client_utils.c src/client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs

test_client: $(TEST_SRC_FILES) tests/client_test.c
	$(CC) $(CFLAGS) $^ -o test_client
	./test_client

test_server: $(TEST_SRC_FILES) tests/client_test.c
	$(CC) $(CFLAGS) $^ -o test_server
	./test_server

clean:
	rm -f rfs
	rm -f server
	rm -f test*
