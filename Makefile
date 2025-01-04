CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

all: client_udp server_udp

client_udp: src/UDP/client_ud.c
	$(CC) $(CFLAGS) -o client_udp src/UDP/client_ud.c

server_udp: src/UDP/server_udp.c src/UDP/header_udp.h src/UDP/utils_udp.c
	$(CC) $(CFLAGS) -o server_udp src/UDP/server_udp.c 

clean:
	rm -f client_udp server_udp
