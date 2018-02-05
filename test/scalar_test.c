#include "../scalar.h"
#include "test.h"

static bool scalar_new_test(T* t)
{
    scalar_t* scalar = scalar_new(1, 2, false);
    ASSERT_NOT_NULL(scalar);
    ASSERT_EQUALS(scalar->a, 1);
    ASSERT_EQUALS(scalar->b, 2);
    ASSERT_FALSE(scalar->negative);
    free(scalar);
    return TEST_PASS;
}

static bool scalar_delete_test(T* t)
{
    scalar_t* scalar = scalar_new(1, 2, false);
    ASSERT_NOT_NULL(scalar);
    scalar_delete(scalar);
    ASSERT_NULL(scalar);
    return TEST_PASS;
}

static bool scalar_mul_test(T* t)
{
    scalar_t* x = scalar_new(1, 2, false);
    scalar_t* y = scalar_new(1, 2, true);
    scalar_t* z = scalar_new(1, 4, true);
    scalar_t* r = scalar_from(0);

    // 1/2 * (-1/2) = -1/4
    scalar_mul(r, x, y);

    ASSERT_EQUALS(r->a, z->a);
    ASSERT_EQUALS(r->b, z->b);
    ASSERT_EQUALS(r->negative, z->negative);

    // (-1/2) * 1/2 = -1/4
    scalar_mul(r, y, x);

    ASSERT_EQUALS(r->a, z->a);
    ASSERT_EQUALS(r->b, z->b);
    ASSERT_EQUALS(r->negative, z->negative);

    // (-1/4) * (-1/4) = 1/16
    scalar_mul(r, r, r);

    z->negative = false;

    z->a = 1;
    z->b = 16;

    ASSERT_EQUALS(r->a, z->a);
    ASSERT_EQUALS(r->b, z->b);
    ASSERT_EQUALS(r->negative, z->negative);

    scalar_delete(r);
    scalar_delete(z);
    scalar_delete(y);
    scalar_delete(x);
    return TEST_PASS;
}

int main(void)
{
    TEST_INIT();

    TEST(scalar_new);
    TEST(scalar_delete);
    TEST(scalar_mul);

    TEST_END();
}