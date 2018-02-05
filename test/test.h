#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct test {
    size_t err_count;
} T;

#define STR_OF(x) #x

#define TEST_PASS true

#define TEST_FAIL false

#define TEST_INIT() \
    T t = (T) { 0 }

#define TEST_END()                                                                     \
    if (t.err_count > 0) {                                                             \
        printf("--------------------\nTEST FAIL (%s) (%zu)\n", __FILE__, t.err_count); \
        return EXIT_FAILURE;                                                           \
    } else {                                                                           \
        printf("--------------------\nTEST SUCCESS (%s)\n", __FILE__);                 \
        return EXIT_SUCCESS;                                                           \
    }

#define TEST(func)                                  \
    if ((func##_test)(&t) == TEST_PASS) {           \
        printf("PASS %s\n", STR_OF(func##_test));   \
    } else {                                        \
        printf("FAIL %s\n\n", STR_OF(func##_test)); \
    }

#define ASSERT_TRUE(expr)                                         \
    {                                                             \
        bool b = (expr);                                          \
        if (!b) {                                                 \
            t->err_count++;                                       \
            printf("FAIL %s\n", __FUNCTION__);                    \
            printf("\t%s\tExpected TRUE, got FALSE.\n\n", #expr); \
            return TEST_FAIL;                                     \
        }                                                         \
    }

#define ASSERT_FALSE(expr)                                        \
    {                                                             \
        bool b = (expr);                                          \
        if (b) {                                                  \
            t->err_count++;                                       \
            printf("FAIL %s\n", __FUNCTION__);                    \
            printf("\t%s\tExpected FALSE, got TRUE.\n\n", #expr); \
            return TEST_FAIL;                                     \
        }                                                         \
    }

#define ASSERT_NULL(expr)                              \
    {                                                  \
        bool b = (expr) == NULL;                       \
        if (!b) {                                      \
            t->err_count++;                            \
            printf("FAIL %s\n", __FUNCTION__);         \
            printf("\t%s\tExpected NULL.\n\n", #expr); \
            return TEST_FAIL;                          \
        }                                              \
    }

#define ASSERT_NOT_NULL(expr)                                        \
    {                                                                \
        bool b = (expr) == NULL;                                     \
        if (b) {                                                     \
            t->err_count++;                                          \
            printf("FAIL %s\n", __FUNCTION__);                       \
            printf("\t%s\tExpected not-NULL, got NULL.\n\n", #expr); \
            return TEST_FAIL;                                        \
        }                                                            \
    }

#define ASSERT_EQUALS(expr1, expr2)                                         \
    {                                                                       \
        bool eq = (expr1) == (expr2);                                       \
        if (!eq) {                                                          \
            t->err_count++;                                                 \
            printf("FAIL %s\n", __FUNCTION__);                              \
            printf("\t%s\tExpected equality with %s.\n\n", #expr1, #expr2); \
            return TEST_FAIL;                                               \
        }                                                                   \
    }

#define ASSERT_NOT_EQUAL(expr1, expr2)                                           \
    {                                                                            \
        bool eq = (expr1) == (expr2);                                            \
        if (eq) {                                                                \
            t->err_count++;                                                      \
            printf("FAIL %s\n", __FUNCTION__);                                   \
            printf("\t%s\tDidn't expect equality with %s.\n\n", #expr1, #expr2); \
            return TEST_FAIL;                                                    \
        }                                                                        \
    }

#endif /* test.h */