#ifndef NETWORK_H
#define NETWORK_H

const short UDP_PORT = 8000;
const short TCP_PORT = 8001;

struct Task {
    double from;
    double to;
    size_t nsteps;
};

#endif