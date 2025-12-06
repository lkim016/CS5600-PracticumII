CC = gcc
CFLAGS = -Wall
SRC_FILES = src/socket.c
CLIENT_SRC_FILES = src/utils.c

all: server client

server: $(SRC_FILES) src/server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: $(SRC_FILES) $(CLIENT_SRC_FILES)  src/client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs