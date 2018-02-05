#ifndef TD_VECTOR_H
#define TD_VECTOR_H

#include "scalar.h"
#include <stddef.h>

typedef struct matrix matrix_t;

typedef struct vector {
    size_t    n;
    scalar_t* items;
} vector_t;

#define vector_delete(vector)  \
    if ((vector) != NULL) {    \
        free((vector)->items); \
        free(vector);          \
        (vector) = NULL;       \
    }

vector_t* vector_new(size_t n);
vector_t* vector_from(scalar_t* vals, size_t n);
void      vector_copy(vector_t* dst, vector_t* src);
void      vector_scale(vector_t* vector, scalar_t* scalar);
void      vector_add(vector_t* u, vector_t* v);
void      vector_sub(vector_t* u, vector_t* v);
scalar_t* vector_dot_prod(vector_t* u, vector_t* v);
scalar_t* vector_get(vector_t* vector, size_t i);
void      vector_set(vector_t* vector, size_t i, scalar_t* x);
char*     vector_string(vector_t* vector);
size_t    vector_string_length(vector_t* vector);

#endif /* vector.h */
