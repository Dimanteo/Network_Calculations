#ifndef NETWORK_H
#define NETWORK_H

static const short UDP_PORT = 8000;
static const short TCP_PORT = 8001;

struct Task {
    double from;
    double to;
    size_t nsteps;
};

#endif