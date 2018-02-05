#ifndef TD_MATRIX_H
#define TD_MATRIX_H

#include "scalar.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct vector vector_t;

typedef struct matrix {
    size_t     m, n;
    scalar_t** rows;
} matrix_t;

#define matrix_delete(matrix)                    \
    if ((matrix) != NULL) {                      \
        for (size_t i = 0; i < matrix->m; i++) { \
            free(matrix->rows[i]);               \
        }                                        \
        free((matrix)->rows);                    \
        free(matrix);                            \
        (matrix) = NULL;                         \
    }

matrix_t* matrix_new(size_t m, size_t n);
matrix_t* matrix_square(size_t n);
matrix_t* matrix_eye(size_t n);
matrix_t* matrix_from_diag(vector_t* diag);
matrix_t* matrix_from_vector(vector_t* vector, bool line);
matrix_t* matrix_prod(matrix_t* a, matrix_t* b);
void      matrix_scale(matrix_t* matrix, scalar_t* scalar);
void      matrix_add(matrix_t* a, matrix_t* b);
void      matrix_sub(matrix_t* a, matrix_t* b);
void      matrix_mul(matrix_t* a, matrix_t* b);
vector_t* matrix_row(matrix_t* matrix, size_t i);
vector_t* matrix_col(matrix_t* matrix, size_t j);
vector_t* matrix_diag(matrix_t* matrix);
scalar_t* matrix_get(matrix_t* matrix, size_t i, size_t j);
void      matrix_set(matrix_t* matrix, size_t i, size_t j, scalar_t* x);
scalar_t* matrix_det(matrix_t* matrix);
bool      matrix_is_inversible(matrix_t* matrix);
matrix_t* matrix_inverse(matrix_t* matrix);
matrix_t* matrix_transpose(matrix_t* matrix);
void      matrix_lu(matrix_t* matrix, matrix_t** L, matrix_t** U);
matrix_t* matrix_chol(matrix_t* matrix);
char*     matrix_string(matrix_t* matrix);

#endif /* matrix.h */
