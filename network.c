#include "network.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

const short UDP_PORT = 8000;
const short TCP_PORT = 8001;

int set_fdflags(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        perror("fcntl");
        return -1;
    }
    flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (flags < 0) {
        perror("fcntl");
        return -1;
    }
    return 0;
}