#ifndef THREADS_HEADER
#define THREADS_HEADER
#include "integral.h"
#include "topology.h"

#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

struct worker {
    numb_t left, right;
    numb_t *result;
    size_t nsteps;
    pthread_t pthread;
    cpu_set_t *cpuset;
    size_t cpusetsize;
};

void *thread_start(void *arg);

struct worker *setup_workers(struct core_info *cores, int maxthreads, int ncores, 
                        int nthreads, numb_t left, numb_t right, size_t nsteps);

struct worker *create_idle_workers(struct worker *workers, int maxthreads,
                                        int nthreads);

void destr_workers(struct worker *workers, int nmemb);

#endif