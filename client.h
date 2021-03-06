#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#include "network.h"
#include "cmdargs.h"
#include "worker.h"

int wait_broadcast(struct sockaddr_in *addr);
int connect_server(long int workers);
int receive_task(int server_fd, struct Task *task);
int send_result(int server_fd, numb_t res);

#endif