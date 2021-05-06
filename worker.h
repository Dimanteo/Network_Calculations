#ifndef WORKER_H
#define WORKER_H
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <alloca.h>

#include "network.h"
#include "threads.h"
#include "topology.h"

numb_t run_workers();

#endif