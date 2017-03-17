.PHONY: clean

CC=gcc
CFLAGS=-g -O2 -std=c99
LDFLAGS=-lpthread -lncurses

all: client server

client: src/client.o src/vector.o
	$(CC) -o client src/client.o src/vector.o $(LDFLAGS)

server: src/server.o src/vector.o
	$(CC) -o server src/server.o src/vector.o $(LDFLAGS)

clean:
	-rm src/*.o server client