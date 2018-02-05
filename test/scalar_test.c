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

int main(void)
{
    TEST_INIT();

    TEST(scalar_new);
    TEST(scalar_delete);

    TEST_END();
}