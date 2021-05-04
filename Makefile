CC := gcc
CFLAGS := -g -Wall -MD -std=c99

all: client.out

client.out : client.o

server.out : server.o

client.o : client.c

server.o : server.c

%.out : %.o
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o *.out *.d

-include *.d