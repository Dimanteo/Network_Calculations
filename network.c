#include "network.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

const short UDP_PORT = 8000;
const short TCP_PORT = 8001;

#define CHECK(var, msg) \
    if (var < 0) { \
        perror(msg); \
        return -1; \
    }

int set_sock_options(int fd)
{
    int enable = 1;
    int keepidle = 5;
    int keepcnt = 3;
    int keepintvl = 3;
    struct timeval timeout = {.tv_sec = 10, .tv_usec = 0};
    int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
    CHECK(ret, "setsockopt keepalive");
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    CHECK(ret, "setsockopt keepidle");
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
    CHECK(ret, "setsockopt keepcnt");
    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    CHECK(ret, "setscokopt keepintvl");
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    CHECK(ret, "setsockopt rcvtimeo");
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    CHECK(ret, "setsockopt reuseaddr");
    return 0;
}

#undef CHECK