CC = gcc
CFLAGS = -Wall

all: server client

server: src/server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: src/client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs
