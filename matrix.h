#ifndef TD_MATRIX_H
#define TD_MATRIX_H

#include "scalar.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct vector vector_t;

typedef struct matrix {
    size_t    m, n;
    vector_t* lines;
} matrix_t;

matrix_t* matrix_new(size_t m, size_t n);
matrix_t* matrix_square(size_t n);
matrix_t* matrix_eye(size_t n);
matrix_t* matrix_from_diag(vector_t* diag);
matrix_t* matrix_from_vec(vector_t* vector);
matrix_t* matrix_from_prod(matrix_t* a, matrix_t* b);

void matrix_scale(matrix_t* mat, scalar_t* scalar);
void matrix_add(matrix_t* a, matrix_t* b);
void matrix_sub(matrix_t* a, matrix_t* b);
void matrix_mul(matrix_t* a, matrix_t* b);

vector_t* matrix_row(matrix_t* a, size_t i);
vector_t* matrix_col(matrix_t* a, size_t j);
vector_t* matrix_diag(matrix_t* a);

scalar_t* matrix_get(matrix_t* a, size_t i, size_t j);
void      matrix_set(matrix_t* a, size_t i, size_t j, scalar_t* x);

scalar_t* matrix_det(matrix_t* mat);
bool      matrix_is_inversible(matrix_t* mat);
void      matrix_inverse(matrix_t* mat);

char* matrix_string(matrix_t* mat);

#endif /* matrix.h */
