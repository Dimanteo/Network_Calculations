#ifndef INTEGRAL_H
#define INTEGRAL_H
#include <stddef.h>

typedef double numb_t;

numb_t function(numb_t x);

void integrate(numb_t(*function)(numb_t), numb_t from, numb_t to, size_t n_steps, numb_t *sum);
#endif