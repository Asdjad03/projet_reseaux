CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

all: client_tcp server_tcp

client_tcp: src/TCP/client_tcp.c src/TCP/header_tcp.h
	$(CC) $(CFLAGS) -o client_tcp src/TCP/client_tcp.c

server_tcp: src/TCP/server_tcp.c src/TCP/header_tcp.h src/TCP/utils.c
	$(CC) $(CFLAGS) -o server_tcp src/TCP/server_tcp.c

clean:
	rm -f client_tcp server_tcp
