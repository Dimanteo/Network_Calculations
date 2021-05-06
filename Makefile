CC := gcc
CFLAGS := -g -Wall -Wextra -MD -std=c99
LDFLAGS := -pthread -lm
LIBWORKER := worker.o integral.o topology.o threads.o

all: client.out server.out

client.out : client.o cmdargs.o $(LIBWORKER)

server.out : server.o cmdargs.o

client.o : client.c

server.o : server.c

cmdargs.o : cmdargs.c

worker.o : worker.c

integral.o : integral.c

topology.o : topology.c

threads.o : threads.c

%.out : %.o
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o *.out *.d

-include *.d