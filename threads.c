#define _GNU_SOURCE
#include "threads.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_start(void *arg)
{
    struct worker *props = (struct worker *)arg;
    pthread_setaffinity_np(props->pthread, props->cpusetsize, props->cpuset);
    integrate(function, props->left, props->right, props->nsteps, props->result);
    return NULL;
}

struct worker *setup_workers(struct core_info *cores, int maxthreads,
    int ncores, int nthreads, numb_t left, numb_t right, size_t nsteps)
{
    size_t nworkers = nthreads > maxthreads ? nthreads : maxthreads;
    struct worker *workers = malloc(sizeof(workers[0]) * nworkers);
    if (workers == NULL)
        return NULL;
    size_t wrk_nsteps = nsteps / nthreads;
    numb_t segment = (right - left) / nthreads;
    size_t wrk_cnt = 0;

    for (int core_i = 0; core_i < ncores; core_i++) {
        for (int wrk_i = 0; wrk_i < cores[core_i].nworkers; wrk_i++) {
            int thr_id = cores[core_i].threads[wrk_i % 2];
            cpu_set_t *cpuset = CPU_ALLOC(maxthreads);
            if (cpuset == NULL) {
                destr_workers(workers, wrk_cnt);
                return NULL;
            }

            size_t cpusetsize = CPU_ALLOC_SIZE(maxthreads);
            CPU_ZERO_S(cpusetsize, cpuset);
            CPU_SET_S(thr_id, cpusetsize, cpuset);
            workers[wrk_cnt].cpuset = cpuset;
            workers[wrk_cnt].cpusetsize = cpusetsize;

            workers[wrk_cnt].result = cores[core_i].mempage + wrk_i;

            workers[wrk_cnt].left = left;
            workers[wrk_cnt].right = left + segment;
            workers[wrk_cnt].nsteps = wrk_nsteps;

            left += segment;
            wrk_cnt++;
        }
    }
    workers = create_idle_workers(workers, maxthreads, nthreads);
    return workers;
}

struct worker *create_idle_workers(struct worker *workers, int maxthreads, 
                                        int nthreads)
{
    struct worker *idle = workers;
    for (int i = nthreads; i < maxthreads; i++) {
        workers[i].left = idle->left;
        workers[i].right = idle->right;
        workers[i].nsteps = idle->nsteps;

        workers[i].result = (numb_t*)valloc(sysconf(_SC_PAGESIZE));
        if (workers[i].result == NULL) {
            perror("valloc");
            destr_workers(workers, i);
            return NULL;
        }
        workers[i].cpuset = CPU_ALLOC(maxthreads);
        if (workers[i].cpuset == NULL) {
            destr_workers(workers, i);
            return NULL;
        }
        workers[i].cpusetsize = CPU_ALLOC_SIZE(maxthreads);
        CPU_ZERO_S(workers[i].cpusetsize, workers[i].cpuset);
        CPU_SET_S(i, workers[i].cpusetsize, workers[i].cpuset);
    }
    return workers;
}

void destr_workers(struct worker *workers, int nmemb)
{
    for (int i = 0; i < nmemb; i++) {
        CPU_FREE(workers[i].cpuset);
    }
    free(workers);
}