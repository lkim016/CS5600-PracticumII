CC = gcc
CFLAGS = -Wall
SRC_FILES = src/socket.c src/utils.c

.PHONY: all clean

all: server client

server: $(SRC_FILES) src/server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: $(SRC_FILES) src/client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs

clean:
	rm -f rfs
	rm -f server
