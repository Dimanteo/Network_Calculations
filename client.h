#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "network.h"
#include "cmdargs.h"

int wait_broadcast(struct sockaddr_in *addr);
int connect_server(long int workers);
int receive_task(int server_fd, struct Task *task);
int run_workers();
int send_result();

#endif