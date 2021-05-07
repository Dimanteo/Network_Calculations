#include "integral.h"
#include <math.h>

static const float eps = 1e-5;

static int feq(float a, float b) { return fabs(a - b) < eps; }

static numb_t simpson_int(numb_t(*function)(numb_t), numb_t from, numb_t to);

static numb_t simpson_int(numb_t(*f)(numb_t), numb_t a, numb_t b) 
{
    return (b - a) / 6 * (f(a) + 4 * f((a + b) / 2) + f(b));
}

void integrate(numb_t(*f)(numb_t), numb_t a, numb_t b, size_t n_steps, numb_t *sum)
{
    numb_t step = (b - a) / n_steps;
    while (!feq(a, b)) {
        *sum += simpson_int(f, a, a + step);
        a += step;
    }
}

numb_t function(numb_t x) 
{
    // return pow(cos(x), 3) - 3 * sin(x) * sin(x) * cos(x); // cos(3x)
    return 3 * sin(x) * pow(cos(x), 2) - pow(sin(x), 3); // sin(3x)
}