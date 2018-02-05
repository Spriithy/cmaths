#include "scalar.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

scalar_t zero = (scalar_t){ .negative = false, .a = 0, .b = 1 };

scalar_t one = (scalar_t){ .negative = false, .a = 1, .b = 1 };

#define scalar_cpy(dst, src)           \
    {                                  \
        dst->negative = src->negative; \
        dst->a        = src->a;        \
        dst->b        = src->b;        \
    }

static uint64_t uint64_gcd(uint64_t a, uint64_t b)
{
    uint64_t r;

    while (b != 0) {
        r = a % b;
        a = b;
        b = r;
    }

    return a;
}

static uint64_t uint64_lcm(uint64_t a, uint64_t b)
{
    return (a * b) / uint64_gcd(a, b);
}

static void scalar_norm(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);

    if (scalar->a == 0 || scalar->b == 1) {
        return;
    }

    if (scalar->a == scalar->b) {
        scalar->a = 1;
        scalar->b = 1;
        return;
    }

    uint64_t gcd = uint64_gcd(scalar->a, scalar->b);

    if (gcd == 1) {
        return;
    }

    scalar->a /= gcd;
    scalar->b /= gcd;
}

scalar_t* scalar_new(uint64_t a, uint64_t b, bool negative)
{
    scalar_t* scalar = malloc(sizeof(*scalar));
    CHECK_NOT_NULL(scalar);

    scalar->negative = negative;

    scalar->a = a;
    scalar->b = b;

    return scalar;
}

scalar_t* scalar_from(int64_t n)
{
    return scalar_new(n >= 0 ? n : -n, 1, n < 0);
}

void scalar_copy(scalar_t* dst, scalar_t* src)
{
    CHECK_NOT_NULL(dst);
    CHECK_NOT_NULL(src);
    scalar_cpy(dst, src);
}

scalar_t* scalar_duplicate(scalar_t* scalar)
{
    scalar_t* duplicate = malloc(sizeof(*duplicate));
    scalar_copy(duplicate, scalar);
    return duplicate;
}

scalar_cmp_t scalar_compare(scalar_t* x, scalar_t* y)
{
    if (x == y) {
        return EQ;
    }

    if (x == NULL || y == NULL) {
        return NE;
    }

    if (x->a == 0 && y->a == 0) {
        return EQ;
    }

    if (x->negative && !y->negative) {
        return LT;
    }

    if (!x->negative && y->negative) {
        return GT;
    }

    if (x->b == y->b) {
        return x->a > y->a
            ? GT
            : x->a == y->a
                ? EQ
                : LT;
    }

    uint64_t lcm = uint64_lcm(x->b, y->b);

    uint64_t mx = lcm / x->b;
    uint64_t xx = mx * x->a;

    uint64_t my = lcm / y->b;
    uint64_t yy = my * y->a;

    return xx > yy
        ? GT
        : xx == yy
            ? EQ
            : LT;
}

bool scalar_equals(scalar_t* x, scalar_t* y)
{
    return scalar_compare(x, y) == EQ;
}

bool scalar_greater_equal(scalar_t* x, scalar_t* y)
{
    scalar_cmp_t cmp = scalar_compare(x, y);
    return cmp == GT || cmp == EQ;
}

bool scalar_greater_than(scalar_t* x, scalar_t* y)
{
    return scalar_compare(x, y) == GT;
}

bool scalar_less_equal(scalar_t* x, scalar_t* y)
{
    scalar_cmp_t cmp = scalar_compare(x, y);
    return cmp == LT || cmp == EQ;
}

bool scalar_less_than(scalar_t* x, scalar_t* y)
{
    return scalar_compare(x, y) == LT;
}

void scalar_opposite(scalar_t* result, scalar_t* scalar)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(scalar);
    scalar_cpy(result, scalar);
    result->negative = !result->negative;
}

void scalar_abs(scalar_t* result, scalar_t* scalar)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(scalar);
    scalar_cpy(result, scalar);
    result->negative = false;
}

void scalar_inverse(scalar_t* result, scalar_t* scalar)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(scalar);

    if (scalar->a == 0) {
        ERROR_MESSAGE(" division by 0");
    }

    result->negative = scalar->negative;

    result->a = scalar->b;
    result->b = scalar->a;

    scalar_norm(result);
}

void scalar_scale(scalar_t* result, scalar_t* scalar, uint64_t n, bool negative)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(scalar);

    scalar_cpy(result, scalar);

    if ((result->negative && negative) || (!result->negative && !negative)) {
        result->negative = false;
    } else {
        result->negative = true;
    }

    result->a *= n;
    scalar_norm(result);
}

void scalar_add(scalar_t* result, scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);

    uint64_t xx = x->a;
    uint64_t yy = y->a;

    if (xx == 0 && yy == 0) {
        result->a = 0;
        return;
    }

    // Copy LHS initial sign
    result->negative = x->negative;

    if (x->b == y->b) {
    same_denominator:
        if (xx > yy) {
            if (y->negative) {
                result->a = xx - yy;
                goto norm;
            }

            result->a = xx + yy;
            goto norm;
        }

        if (xx < yy) {
            if (y->negative) {
                result->negative = true;
                result->a        = yy - xx;
                goto norm;
            }

            if (x->negative) {
                result->negative = false;
                result->a        = yy - xx;
                goto norm;
            }

            result->a = xx + yy;
            goto norm;
        }

        // xx == yy
        result->a = xx * 2;
        goto norm;
    }

    uint64_t lcm = uint64_lcm(x->b, y->b);

    uint64_t mx = lcm / x->b;
    uint64_t my = lcm / y->b;

    xx = mx * x->a;
    yy = my * y->a;

    result->b = lcm;
    goto same_denominator;

norm:
    scalar_norm(result);
}

void scalar_sub(scalar_t* result, scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);
    scalar_opposite(y, y);
    scalar_add(result, x, y);
    scalar_opposite(y, y);
}

void scalar_mul(scalar_t* result, scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);

    scalar_cpy(result, x);

    result->a *= y->a;
    result->b *= y->b;

    if (x->negative == y->negative) {
        result->negative = false;
    } else {
        result->negative = true;
    }

    scalar_norm(result);
}

void scalar_div(scalar_t* result, scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(result);
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);

    if (y->a == 0) {
        ERROR_MESSAGE("division by 0");
    }

    scalar_inverse(y, y);
    scalar_mul(result, x, y);
    scalar_inverse(y, y);
}

scalar_t* scalar_opposite_get(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);
    return scalar_new(scalar->a, scalar->b, !scalar->negative);
}

scalar_t* scalar_abs_get(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);
    return scalar_new(scalar->a, scalar->b, false);
}

scalar_t* scalar_inverse_get(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);
    scalar_t* s = scalar_from(0);
    scalar_inverse(s, scalar);
    return s;
}

scalar_t* scalar_scale_get(scalar_t* scalar, uint64_t n, bool negative)
{
    CHECK_NOT_NULL(scalar);
    scalar_t* s = scalar_from(0);
    scalar_scale(s, scalar, n, negative);
    return s;
}

scalar_t* scalar_add_get(scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);
    scalar_t* scalar = scalar_from(0);
    scalar_add(scalar, x, y);
    return scalar;
}

scalar_t* scalar_sub_get(scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);
    scalar_t* scalar = scalar_from(0);
    scalar_sub(scalar, x, y);
    return scalar;
}

scalar_t* scalar_mul_get(scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);
    scalar_t* scalar = scalar_from(0);
    scalar_mul(scalar, x, y);
    return scalar;
}

scalar_t* scalar_div_get(scalar_t* x, scalar_t* y)
{
    CHECK_NOT_NULL(x);
    CHECK_NOT_NULL(y);
    scalar_t* s = scalar_from(0);
    scalar_div(s, x, y);
    return s;
}

static size_t num_len(uint64_t x)
{
    size_t   len = 0;
    uint64_t tmp = 1;

    while (tmp < x) {
        tmp = (tmp << 3) + (tmp << 1);
        len++;
    }

    return len;
}

char* scalar_string(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);

    char* str = malloc(scalar_string_length(scalar));
    CHECK_NOT_NULL(str);

    if (scalar->a == 0) {
        sprintf(str, "0");
        return str;
    }

    if (scalar->b == 1) {
        sprintf(str, scalar->negative ? "-%zu" : "%zu", scalar->a);
        return str;
    }

    sprintf(str, scalar->negative ? "-%zu/%zu" : "%zu/%zu", scalar->a, scalar->b);
    return str;
}

size_t scalar_string_length(scalar_t* scalar)
{
    CHECK_NOT_NULL(scalar);

    size_t len = num_len(scalar->a) + 1; // '\0'

    if (scalar->b != 1) {
        len += num_len(scalar->b) + 1; // '/' + divisor
    }

    if (scalar->negative) {
        len++;
    }

    return len;
}
