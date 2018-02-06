#include "vector.h"
#include "matrix.h"
#include "utils.h"

vector_t* vector_new(size_t n)
{
    vector_t* vector = malloc(sizeof(*vector));
    CHECK_NOT_NULL(vector);

    vector->n = n;

    vector->items = malloc(n * sizeof(scalar_t));
    CHECK_NOT_NULL(vector->items);

    for (size_t i = 0; i < n; i++) {
        scalar_copy(&vector->items[i], &zero);
    }

    return vector;
}

vector_t* vector_from(scalar_t* vals, size_t n)
{
    CHECK_NOT_NULL(vals);

    vector_t* vector = vector_new(n);

    for (size_t i = 0; i < n; i++) {
        scalar_copy(&vector->items[i], &vals[i]);
    }

    return vector;
}

void vector_copy(vector_t* u, vector_t* v)
{
    CHECK_NOT_NULL(u);
    CHECK_NOT_NULL(v);

    u->n = v->n;
    for (size_t i = 0; i < v->n; i++) {
        scalar_copy(&u->items[i], &v->items[i]);
    }
}

void vector_scale(vector_t* vector, scalar_t* scalar)
{
    CHECK_NOT_NULL(vector);
    CHECK_NOT_NULL(scalar);

    for (size_t i = 0; i < vector->n; i++) {
        scalar_mul(&vector->items[i], &vector->items[i], scalar);
    }
}

void vector_add(vector_t* u, vector_t* v)
{
    CHECK_NOT_NULL(u);
    CHECK_NOT_NULL(v);

    if (u->n != v->n) {
        ERROR("vector dimension mismatch (u=%zu, v=%zu)\n", u->n, v->n);
    }

    for (size_t i = 0; i < v->n; i++) {
        scalar_add(&u->items[i], &u->items[i], &v->items[i]);
    }
}

void vector_sub(vector_t* u, vector_t* v)
{
    CHECK_NOT_NULL(u);
    CHECK_NOT_NULL(v);

    if (u->n != v->n) {
        ERROR("vector dimension mismatch (u=%zu, v=%zu)\n", u->n, v->n);
    }

    for (size_t i = 0; i < v->n; i++) {
        scalar_sub(&u->items[i], &u->items[i], &v->items[i]);
    }
}

scalar_t* vector_dot_prod(vector_t* u, vector_t* v)
{
    CHECK_NOT_NULL(u);
    CHECK_NOT_NULL(v);

    if (u->n != v->n) {
        ERROR("vector dimension mismatch (u=%zu, v=%zu)", u->n, v->n);
    }

    scalar_t* prod = scalar_from(0);

    scalar_t* tmp = scalar_from(0);
    for (size_t i = 0; i < u->n; i++) {
        scalar_mul(tmp, &u->items[i], &v->items[i]);
        scalar_add(prod, prod, tmp);
    }
    free(tmp);

    return prod;
}

scalar_t* vector_get(vector_t* vector, size_t i)
{
    if (i >= vector->n) {
        ERROR("index out of bounds (i=%zu, size=%zu)", i, vector->n);
    }

    return scalar_duplicate(&vector->items[i]);
}

void vector_set(vector_t* vector, size_t i, scalar_t* scalar)
{
    if (i >= vector->n) {
        ERROR("index out of bounds (i=%zu,size=%zu)", i, vector->n);
    }

    scalar_copy(&vector->items[i], scalar);
}

char* vector_string(vector_t* vector)
{
    CHECK_NOT_NULL(vector);

    char* str = malloc(vector_string_length(vector));
    CHECK_NOT_NULL(str);

    char *ofs, *tmp;

    ofs = str + sprintf(str, "[");
    for (size_t i = 0; i < vector->n; i++) {
        tmp = scalar_string(&vector->items[i]);
        ofs += sprintf(ofs, i == 0 ? "%s" : " %s", tmp);
        free(tmp);
    }
    sprintf(ofs, "]");

    return str;
}

size_t vector_string_length(vector_t* vector)
{
    CHECK_NOT_NULL(vector);

    size_t len = 3; // '[' + ']' + '\0'

    for (size_t i = 0; i < vector->n; i++) {
        len += scalar_string_length(&vector->items[i]) + 2; // ' ' + scalar
    }

    // remove first ' ' if vector isn't empty
    if (len > 3) {
        len--;
    }

    return len;
}
