#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#define exception(fmt, ...)                                                    \
    {                                                                          \
        fprintf(stderr, "%s:%d ~ " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                               \
    }

#endif /* error.h */