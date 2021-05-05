#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "cmdargs.h"
#include "network.h"

struct Client {
    long int workers;
    int fd;
};

int send_broadcast();
int wait_clients();
int send_tasks();
int receive_results();
int open_TCPsocket();
#endif