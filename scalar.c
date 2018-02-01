#include "scalar.h"
#include "exception.h"
#include <stdio.h>
#include <stdlib.h>

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

static void scalar_norm(scalar_t* f)
{
    if (f == NULL) {
        return;
    }

    if (f->a == 0 || f->b == 1) {
        return;
    }

    if (f->a == f->b) {
        f->a = 1;
        f->b = 1;
        return;
    }

    uint64_t gcd = uint64_gcd(f->a, f->b);

    if (gcd == 1) {
        return;
    }

    f->a /= gcd;
    f->b /= gcd;
}

static void scalar_cpy(scalar_t* dst, scalar_t* src)
{
    dst->negative = src->negative;
    dst->a        = src->a;
    dst->b        = src->b;
}

scalar_t* scalar_new(uint64_t a, uint64_t b, bool negative)
{
    scalar_t* scalar = malloc(sizeof(*scalar));

    if (scalar == NULL) {
        return NULL;
    }

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
    if (dst == NULL || src == NULL) {
        exception("scalar.copy(%p, %p): null pointer exception", dst, src);
    }

    scalar_cpy(dst, src);
}

scalar_cmp_t scalar_compare(scalar_t* x, scalar_t* y)
{
    if (x == NULL && y == NULL) {
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

void scalar_opposite(scalar_t* r, scalar_t* scalar)
{
    if (r == NULL || scalar == NULL) {
        exception("scalar.opposite(%p, %p): null pointer exception", r, scalar);
    }

    scalar_cpy(r, scalar);
    r->negative = !r->negative;
}

void scalar_abs(scalar_t* r, scalar_t* scalar)
{
    if (r == NULL || scalar == NULL) {
        exception("scalar.abs(%p, %p): null pointer exception", r, scalar);
    }

    scalar_cpy(r, scalar);
    r->negative = false;
}

void scalar_inverse(scalar_t* r, scalar_t* scalar)
{
    if (r == NULL || scalar == NULL) {
        exception("scalar.inverse(%p, %p): null pointer exception", r, scalar);
    }

    if (scalar->a == 0) {
        exception("scalar.inverse(%p, %p): division by 0", r, scalar);
    }

    r->negative = scalar->negative;
    r->a        = scalar->b;
    r->b        = scalar->a;
    scalar_norm(r);
}

void scalar_scale(scalar_t* r, scalar_t* scalar, uint64_t n, bool negative)
{
    if (r == NULL || scalar == NULL) {
        exception("scalar.scale(%p, %p, %zu, %s): null pointer exception",
            r, scalar, n, negative ? "true" : "false");
    }

    scalar_cpy(r, scalar);

    if ((r->negative && negative) || (!r->negative && !negative)) {
        r->negative = false;
    } else {
        r->negative = true;
    }

    r->a *= n;
    scalar_norm(r);
}

void scalar_add(scalar_t* r, scalar_t* x, scalar_t* y)
{
    if (r == NULL || x == NULL || y == NULL) {
        exception("scalar.add(%p, %p, %p): null pointer exception", r, x, y);
    }

    uint64_t xx = x->a;
    uint64_t yy = y->a;

    if (xx == 0 && yy == 0) {
        r->a = 0;
        return;
    }

    // Copy LHS initial sign
    r->negative = x->negative;

    if (x->b == y->b) {
    same_denominator:
        if (xx > yy) {
            if (y->negative) {
                r->a = xx - yy;
                goto norm;
            }

            r->a = xx + yy;
            goto norm;
        }

        if (xx < yy) {
            if (y->negative) {
                r->negative = true;
                r->a        = yy - xx;
                goto norm;
            }

            if (x->negative) {
                r->negative = false;
                r->a        = yy - xx;
                goto norm;
            }

            r->a = xx + yy;
            goto norm;
        }

        // xx == yy
        r->a = xx * 2;
        goto norm;
    }

    uint64_t lcm = uint64_lcm(x->b, y->b);

    uint64_t mx = lcm / x->b;
    uint64_t my = lcm / y->b;

    xx = mx * x->a;
    yy = my * y->a;

    r->b = lcm;
    goto same_denominator;

norm:
    scalar_norm(r);
}

void scalar_sub(scalar_t* r, scalar_t* x, scalar_t* y)
{
    if (r == NULL || x == NULL || y == NULL) {
        exception("scalar.sub(%p, %p, %p): null pointer exception", r, x, y);
    }

    scalar_inverse(y, y);
    scalar_add(r, x, y);
    scalar_inverse(y, y);
}

void scalar_mul(scalar_t* r, scalar_t* x, scalar_t* y)
{
    if (r == NULL || x == NULL || y == NULL) {
        exception("scalar.mul(%p, %p, %p): null pointer exception", r, x, y);
    }

    scalar_cpy(r, x);

    r->a *= y->a;
    r->b *= y->b;

    if (x->negative == y->negative) {
        r->negative = false;
    } else {
        r->negative = true;
    }

    scalar_norm(r);
}

void scalar_div(scalar_t* r, scalar_t* x, scalar_t* y)
{
    if (r == NULL || x == NULL || y == NULL) {
        exception("scalar.div(%p, %p, %p): null pointer exception", r, x, y);
    }

    if (y->a == 0) {
        exception("scalar.div(%p, %p, %p): division by 0", r, x, y);
    }

    scalar_inverse(y, y);
    scalar_mul(r, x, y);
    scalar_inverse(y, y);
}

scalar_t* scalar_opposite_get(scalar_t* scalar)
{
    if (scalar == NULL) {
        exception("scalar.opposite(%p): null pointer exception", scalar);
    }

    return scalar_new(scalar->a, scalar->b, !scalar->negative);
}

scalar_t* scalar_abs_get(scalar_t* scalar)
{
    if (scalar == NULL) {
        exception("scalar.abs(%p): null pointer exception", scalar);
    }

    return scalar_new(scalar->a, scalar->b, false);
}

scalar_t* scalar_inverse_get(scalar_t* scalar)
{
    if (scalar == NULL) {
        exception("scalar.inverse(%p): null pointer exception", scalar);
    }

    scalar_t* s = scalar_from(0);
    scalar_inverse(s, s);
    return s;
}

scalar_t* scalar_scale_get(scalar_t* scalar, uint64_t n, bool negative)
{
    if (scalar == NULL) {
        exception("scalar.scale(%p, %zu, %s): null pointer exception",
            scalar, n, negative ? "true" : "false");
    }

    scalar_t* s = scalar_from(0);
    scalar_scale(s, s, n, negative);
    return s;
}

scalar_t* scalar_add_get(scalar_t* x, scalar_t* y)
{
    if (x == NULL || y == NULL) {
        exception("scalar.add(%p, %p): null pointer exception", x, y);
    }

    scalar_t* scalar = scalar_from(0);
    scalar_add(scalar, x, y);
    return scalar;
}

scalar_t* scalar_sub_get(scalar_t* x, scalar_t* y)
{
    if (x == NULL || y == NULL) {
        exception("scalar.sub(%p, %p): null pointer exception", x, y);
    }

    scalar_t* scalar = scalar_from(0);
    scalar_sub(scalar, x, y);
    return scalar;
}

scalar_t* scalar_mul_get(scalar_t* x, scalar_t* y)
{
    if (x == NULL || y == NULL) {
        exception("scalar.mul(%p, %p): null pointer exception", x, y);
    }

    scalar_t* scalar = scalar_from(0);
    scalar_mul(scalar, x, y);
    return scalar;
}

scalar_t* scalar_div_get(scalar_t* x, scalar_t* y)
{
    if (x == NULL || y == NULL) {
        exception("scalar.div(%p, %p): null pointer exception", x, y);
    }

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
    if (scalar == NULL) {
        exception("scalar.string(%p): null pointer exception", scalar);
    }

    char* str = malloc(scalar_string_length(scalar));

    if (str == NULL) {
        exception("scalar.string(%p): null pointer exception", scalar);
    }

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
    if (scalar == NULL) {
        exception("scalar.string_length(%p): null pointer exception", scalar);
    }

    size_t len = num_len(scalar->a) + 1; // '\0'

    if (scalar->b != 1) {
        len += num_len(scalar->b) + 1; // '/' + divisor
    }

    if (scalar->negative) {
        len++;
    }

    return len;
}
