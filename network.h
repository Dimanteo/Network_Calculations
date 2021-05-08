#ifndef NETWORK_H
#define NETWORK_H
#include <unistd.h>

extern const short UDP_PORT;
extern const short TCP_PORT;

struct Task {
    double from;
    double to;
    size_t nsteps;
};

int set_fdflags(int fd) ;

#define DBG_SLEEP(t) printf("SLEEP\n"); sleep(t);
#endif