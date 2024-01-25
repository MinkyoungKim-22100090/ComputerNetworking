# Makefile for occ_server and occ_client

# Compiler options
CC = gcc
CFLAGS = -pthread

# Server target
occ_server: occ_server.c
    $(CC) $(CFLAGS) -o occ_server occ_server.c

# Client target
occ_client: occ_client.c
    $(CC) -o occ_client occ_client.c

.PHONY: clean

clean:
    rm -f occ_server occ_client
