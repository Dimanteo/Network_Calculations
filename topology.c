#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <stdlib.h>

#include "topology.h"
#include "threads.h"
#include "cmdargs.h"

static const size_t BUFSZ = 20;

static int get_value(int id, const char *file, char *buf, size_t bufsz);

static int parse_list(char *string, int retbuf[]);

int read_topology(struct core_info* cores, int nthreads)
{
    int ncores = 0;
    char buf[BUFSZ];
    
    for (int i = 0; i < nthreads; i++) {
        int retcd = get_value(i, "core_id", buf, BUFSZ);
        if (retcd < 0)
            return -1;

        int id = arg_to_int(buf);
        if (id < 0)
            return -1;
        cores[i].id = id;
        
        retcd = get_value(i, "thread_siblings_list", buf, BUFSZ);
        if (retcd < 0)
            return -1;

        cores[i].nthreads = parse_list(buf, cores[id].threads);
        if (cores[i].nthreads < 0)
            return -1;

        ncores++;
        for (int i_core = 0; i_core < i; i_core++) {
            if (cores[i_core].id == cores[i].id)
                ncores--;
        }
    }
    return ncores;
}

static int get_value(int id, const char *file, char *buf, size_t bufsz)
{
    const char format[] = "/sys/devices/system/cpu/cpu%d/topology/%s";
    char *path = alloca(sizeof(format) + strlen(file) + 4);
    sprintf(path, format, id, file);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    read(fd, buf, bufsz);
    return 0;
}

static int parse_list(char *string, int retbuf[])
{
    int num, read, cnt = 0;
    read = sscanf(string, "%d", &num);
    if (read == 0)
        return -1;
    retbuf[cnt++] = num;
    string = strchr(string, ',');
    while (string != NULL) {
        sscanf(++string, "%d", &num);
        retbuf[cnt++] = num;
        string = strchr(string, ',');
    }
    return cnt;
}

void dump_cpuinfo(int ncores, int nthreads, struct core_info *cores)
{
    printf("Physical cores: %d\n", ncores);
    printf("Threads : %d\n", nthreads);
    for (int i = 0; i < ncores; i++) {
        printf("\tcore %d: %d threads -", cores[i].id, cores[i].nthreads);
        for (int th = 0; th < cores[i].nthreads; th++) {
            printf(" %d", cores[i].threads[th]);
        }
        printf("\n");
    }
}

int map_threads(long int nthreads, int ncores, struct core_info *cores)
{
    for (int i = 0; i < ncores; i++) {
        cores[i].nworkers = nthreads / ncores;
    }
    for (int i = 0; i < ncores; i++) {
        if (i < nthreads % ncores)
            cores[i].nworkers++;
        size_t npages = cores[i].nworkers * sizeof(numb_t) / sysconf(_SC_PAGESIZE) + 1;
        cores[i].mempage = valloc(sysconf(_SC_PAGESIZE) * npages);
        if (cores[i].mempage == NULL) {
            perror("valloc");
            return -1;
        }
    }
    return 0;
}