#include "worker.h"

numb_t run_workers(struct Task *task, long nthreads)
{
    int maxthreads = get_nprocs();
    struct core_info *cores = alloca(sizeof(struct core_info) * maxthreads);
    int ncores = read_topology(cores, maxthreads);
    if (ncores == -1) {
        return NAN;
    }
    if (map_threads(nthreads, ncores, cores) == -1) {
        return NAN;
    }
    // запустить вычисления
    struct worker *workers = setup_workers(cores, maxthreads, 
        ncores, nthreads, task->from, task->to, task->nsteps);
    if (workers == NULL) {
        fprintf(stderr, "Allocation error\n");
        return NAN;
    }

    long int running = nthreads > maxthreads ? nthreads : maxthreads;

    for (int i = 0; i < running; i++) {
        int crt_ret = pthread_create(&workers[i].pthread, NULL, 
                                        thread_start, &workers[i]);
        if (crt_ret != 0) {
            errno = crt_ret;
            perror("pthread_create");
            destr_workers(workers, running);
            return NAN;
        }
    }
    numb_t res = 0;
    for (int i = 0; i < nthreads; i++) {
        pthread_join(workers[i].pthread, NULL);
        res += *workers[i].result;
    }
    destr_workers(workers, running);
    return res;
}