CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

all: client_tcp_bonus server_tcp_bonus

client_tcp_bonus: src/TCP/client_tcp_bonus.c 
	$(CC) $(CFLAGS) -o client_tcp_bonus src/TCP/client_tcp_bonus.c

server_tcp_bonus: src/TCP/server_tcp_bonus.c src/TCP/header_tcp_bonus.h src/TCP/utils.c
	$(CC) $(CFLAGS) -o server_tcp_bonus src/TCP/server_tcp_bonus.c


clean:
	rm -f client_tcp_bonus server_tcp_bonus
