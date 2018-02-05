#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define ERROR(fmt, ...)                                                                               \
    {                                                                                                 \
        fprintf(stderr, "[E] %s:%d in %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
        exit(1);                                                                                      \
    }

#define ERROR_MESSAGE(text) ERROR("%s", text)

#define WARNING(fmt, ...) \
    printf("[W] %s:%d in %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

#define WARNING_MESSAGE(text) WARNING("%s", text)

#define INFO(fmt, ...) \
    printf("[I] %s:%d in %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

#define INFO_MESSAGE(text) INFO("%s", text)

/* -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- -*- */

#define IF_NULL(callback, ptr_expr)                     \
    {                                                   \
        void* ptr = (ptr_expr);                         \
        if (ptr == NULL) {                              \
            (callback)(#ptr_expr " evaluated to NULL"); \
        }                                               \
    }

#define CHECK_NOT_NULL(ptr_expr)                         \
    {                                                    \
        void* ptr = (ptr_expr);                          \
        if (ptr == NULL) {                               \
            ERROR("%s", #ptr_expr " evaluated to NULL"); \
        }                                                \
    }

#endif /* utils.h */