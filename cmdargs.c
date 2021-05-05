#include "cmdargs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

long int enter_N(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s number\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    long int number = arg_to_int(argv[1]);
    return number;
}

long int arg_to_int(char* argv)
{
    assert(argv);

    char* endptr = NULL;
    long int ret_val = strtol(argv, &endptr, 10);

    if (!isspace(*endptr) && *endptr != '\0') {
        fprintf(stderr, "Conversion error %s. Invalid symbol: %c\n", argv, *endptr);
        return -1;
    }
    if (ret_val == LONG_MAX && errno == ERANGE) {
        perror("strtol");
        return -1;
    }
    if (ret_val == LONG_MIN && errno == ERANGE) {
        perror("strtol");
        return -1;
    }
    return ret_val;
}
