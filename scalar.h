#ifndef TD_SCALAR_H
#define TD_SCALAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum scalar_cmp {
    EQ,
    NE,
    GE,
    GT,
    LE,
    LT,
} scalar_cmp_t;

typedef struct scalar {
    // Whether the number is negative
    bool negative;

    // The numerator
    uint64_t a;

    // The denominator
    uint64_t b;
} scalar_t;

#define scalar_delete(scalar) \
    if (scalar != NULL) {     \
        free(scalar);         \
        (scalar) = NULL;      \
    }

scalar_t* scalar_new(uint64_t a, uint64_t b, bool negative);
scalar_t* scalar_from(int64_t n);
void      scalar_copy(scalar_t* dst, scalar_t* src);

scalar_cmp_t scalar_compare(scalar_t* x, scalar_t* y);
bool         scalar_equals(scalar_t* x, scalar_t* y);
bool         scalar_greater_equal(scalar_t* x, scalar_t* y);
bool         scalar_greater_than(scalar_t* x, scalar_t* y);
bool         scalar_less_equal(scalar_t* x, scalar_t* y);
bool         scalar_less_than(scalar_t* x, scalar_t* y);

void scalar_opposite(scalar_t* r, scalar_t* s);
void scalar_abs(scalar_t* r, scalar_t* s);
void scalar_inverse(scalar_t* r, scalar_t* s);

void scalar_scale(scalar_t* r, scalar_t* s, uint64_t n, bool negative);
void scalar_add(scalar_t* r, scalar_t* x, scalar_t* y);
void scalar_sub(scalar_t* r, scalar_t* x, scalar_t* y);
void scalar_mul(scalar_t* r, scalar_t* x, scalar_t* y);
void scalar_div(scalar_t* r, scalar_t* x, scalar_t* y);

scalar_t* scalar_inverse_get(scalar_t* scalar);
scalar_t* scalar_abs_get(scalar_t* scalar);
scalar_t* scalar_opposite_get(scalar_t* scalar);

scalar_t* scalar_scale_get(scalar_t* scalar, uint64_t n, bool negative);
scalar_t* scalar_add_get(scalar_t* x, scalar_t* y);
scalar_t* scalar_sub_get(scalar_t* x, scalar_t* y);
scalar_t* scalar_mul_get(scalar_t* x, scalar_t* y);
scalar_t* scalar_div_get(scalar_t* x, scalar_t* y);

char*  scalar_string(scalar_t* s);
size_t scalar_string_length(scalar_t* s);

#endif /* scalar.h */