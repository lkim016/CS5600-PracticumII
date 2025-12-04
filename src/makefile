CC = gcc
CFLAGS = -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) $^ -o server
	chmod +x ./server

client: client.c
	$(CC) $(CFLAGS) $^ -o rfs
	chmod +x ./rfs