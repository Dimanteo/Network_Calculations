CC := gcc
CFLAGS := -g -Wall -Wextra -MD -std=c99

all: client.out server.out

client.out : client.o cmdargs.o

server.out : server.o cmdargs.o

client.o : client.c

server.o : server.c

cmdargs.o : cmdargs.c

%.out : %.o
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o *.out *.d

-include *.d