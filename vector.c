#include "vector.h"
#include "exception.h"
#include "matrix.h"

vector_t* vector_new(size_t n, bool line)
{
    vector_t* vector = malloc(sizeof(*vector));

    if (vector == NULL) {
        exception("vector.new(%zu, %s): null pointer exception",
            n, line ? "true" : "false");
    }

    vector->n    = n;
    vector->line = line;
    vector->vec  = calloc(n, sizeof(scalar_t));

    for (size_t i = 0; i < n; i++) {
        vector->vec[i].a = 0;
        vector->vec[i].b = 1;
    }

    if (vector->vec == NULL) {
        free(vector);
        exception("vector.new(%zu, %s): null pointer exception",
            n, line ? "true" : "false");
    }

    return vector;
}

vector_t* vector_from(scalar_t* vals[], size_t n, bool line)
{
    vector_t* vector = vector_new(n, line);

    if (vector == NULL) {
        exception("vector.from(%p, %zu, %s): null pointer exception",
            vals, n, line ? "true" : "false");
    }

    for (size_t i = 0; i < n; i++) {
        scalar_copy(&vector->vec[i], vals[i]);
    }

    return vector;
}

void vector_copy(vector_t* u, vector_t* v)
{
    u->n = v->n;
    for (size_t i = 0; i < v->n; i++) {
        scalar_copy(&u->vec[i], &v->vec[i]);
    }
}

void vector_scale(vector_t* vector, scalar_t* scalar)
{
    if (vector == NULL || scalar == NULL) {
        exception("vector.new(%p, %p): null pointer exception", vector, scalar);
    }

    for (size_t i = 0; i < vector->n; i++) {
        scalar_mul(&vector->vec[i], &vector->vec[i], scalar);
    }
}

void vector_add(vector_t* u, vector_t* v)
{
    if (u == NULL || v == NULL) {
        exception("vector.add(%p, %p): null pointer exception", u, v);
    }

    if (u->line != v->line) {
        exception("vector.add(%p, %p): vector dimension mismatch. u is %s and v is %s\n",
            u, v, u->line ? "line" : "column", v->line ? "line" : "column");
    }

    if (u->n != v->n) {
        exception("vector.add(%p, %p): vector dimension mismatch (u=%zu, v=%zu)\n",
            u, v, u->n, v->n);
    }

    for (size_t i = 0; i < v->n; i++) {
        scalar_add(&v->vec[i], &u->vec[i], &v->vec[i]);
    }
}

void vector_sub(vector_t* u, vector_t* v)
{
    if (u == NULL || v == NULL) {
        exception("vector.sub(%p, %p): null pointer exception", u, v);
    }

    if (u->line != v->line) {
        exception("vector.sub(%p, %p): vector dimension mismatch. u is %s and v is %s\n",
            u, v, u->line ? "line" : "column", v->line ? "line" : "column");
    }

    if (u->n != v->n) {
        exception("vector.sub(%p, %p): vector dimension mismatch (u=%zu, v=%zu)\n",
            u, v, u->n, v->n);
    }

    for (size_t i = 0; i < v->n; i++) {
        scalar_sub(&v->vec[i], &u->vec[i], &v->vec[i]);
    }
}

scalar_t* vector_dot_prod(vector_t* u, vector_t* v)
{
    if (u == NULL || v == NULL) {
        exception("vector.dot_prod(%p, %p): null pointer exception", u, v);
    }

    if (u->n != v->n) {
        exception("vector.dot_prod(%p, %p): vector size mismatch (u=%zu, v=%zu)",
            u, v, u->n, v->n);
    }

    scalar_t* dot = scalar_from(0);

    scalar_t* tmp = scalar_from(0);
    for (size_t i = 0; i < u->n; i++) {
        scalar_mul(tmp, &u->vec[i], &v->vec[i]);
        scalar_add(dot, dot, tmp);
    }
    free(tmp);

    return dot;
}

scalar_t* vector_get(vector_t* vector, size_t i)
{
    if (i >= vector->n) {
        exception("vector.get(%p, %zu): access out of bounds (size=%zu)",
            vector, i, vector->n);
    }

    return scalar_new(vector->vec[i].a, vector->vec[i].b, vector->vec[i].negative);
}

void vector_set(vector_t* vector, size_t i, scalar_t* scalar)
{
    if (i >= vector->n) {
        exception("vector.get(%p, %zu): access out of bounds (size=%zu)",
            vector, i, vector->n);
    }

    vector->vec[i].negative = scalar->negative;

    vector->vec[i].a = scalar->a;
    vector->vec[i].b = scalar->b;
}

char* vector_string(vector_t* vector)
{
    if (vector == NULL) {
        exception("vector.string(%p): null pointer exception", vector);
    }

    char* str = malloc(vector_string_length(vector));
    char *ofs, *tmp;

    ofs = str + sprintf(str, "[");
    for (size_t i = 0; i < vector->n; i++) {
        tmp = scalar_string(&vector->vec[i]);
        ofs += sprintf(ofs, i == 0 ? "%s" : " %s", tmp);
        free(tmp);
    }
    sprintf(ofs, "]");

    return str;
}

size_t vector_string_length(vector_t* vector)
{
    if (vector == NULL) {
        exception("vector.string(%p): null pointer exception", vector);
    }

    size_t len = 3; // '[' + ']' + '\0'

    for (size_t i = 0; i < vector->n; i++) {
        len += scalar_string_length(&vector->vec[i]) + 2; // ' ' + scalar
    }

    // remove first ' ' if vector isn't empty
    if (len > 3) {
        len--;
    }

    return len;
}