#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "cmdargs.h"

int send_broadcast();
int wait_clients();
int send_tasks();
int receive_results();
#endif