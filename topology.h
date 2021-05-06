#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "integral.h"

struct core_info {
    int id;
    int nthreads;
    int threads[2];
    long int nworkers;
    numb_t *mempage;
};

int read_topology(struct core_info* cores, int nthreads);

void dump_cpuinfo(int ncores, int nthreads, struct core_info *cores);

int map_threads(long int nthreads, int ncores, struct core_info *cores);

#endif