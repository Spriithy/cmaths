#include "matrix.h"
#include "utils.h"
#include "vector.h"
#include <stdlib.h>

matrix_t* matrix_new(size_t m, size_t n)
{
    matrix_t* mat = malloc(sizeof(*mat));
    CHECK_NOT_NULL(mat);

    mat->m = m;
    mat->n = n;

    mat->lines = calloc(m, sizeof(vector_t));
    CHECK_NOT_NULL(mat->lines);

    for (size_t i = 0; i < m; i++) {
        mat->lines[i] = *vector_new(n);
    }

    return mat;
}

matrix_t* matrix_square(size_t n)
{
    return matrix_new(n, n);
}

matrix_t* matrix_eye(size_t n)
{
    matrix_t* mat = matrix_square(n);

    for (size_t i = 0; i < n; i++) {
        mat->lines[i].items[i].negative = false;
        mat->lines[i].items[i].a        = 1;
        mat->lines[i].items[i].b        = 1;
    }

    return mat;
}

matrix_t* matrix_from_diag(vector_t* diag)
{
    CHECK_NOT_NULL(diag);

    matrix_t* mat = matrix_square(diag->n);

    for (size_t i = 0; i < diag->n; i++) {
        scalar_copy(&mat->lines[i].items[i], &diag->items[i]);
    }

    return mat;
}

matrix_t* matrix_from_vector(vector_t* vector, bool line)
{
    CHECK_NOT_NULL(vector);

    size_t m = line ? 1 : vector->n;
    size_t n = line ? vector->n : 1;

    matrix_t* mat = matrix_new(m, n);

    if (line) {
        for (size_t i = 0; i < vector->n; i++) {
            scalar_copy(&mat->lines[0].items[i], &vector->items[i]);
        }

        return mat;
    }

    for (size_t i = 0; i < vector->n; i++) {
        scalar_copy(&mat->lines[i].items[0], &vector->items[i]);
    }

    return mat;
}

matrix_t* matrix_from_prod(matrix_t* a, matrix_t* b)
{
    CHECK_NOT_NULL(a);
    CHECK_NOT_NULL(b);

    size_t m = a->m;
    size_t n = b->n;

    if (a->n != b->m) {
        ERROR("matrix dimension mismatch A is (%zu, %zu), B is (%zu, %zu)", a->m, a->n, b->m, b->n);
    }

    matrix_t* mat = matrix_new(m, n);

    scalar_t* val;
    vector_t *col, *row;
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            row = matrix_row(a, i);
            col = matrix_col(b, j);
            val = vector_dot_prod(row, col);
            scalar_copy(&mat->lines[i].items[j], val);
            free(val);
            free(row);
            free(col);
        }
    }

    return mat;
}

void matrix_scale(matrix_t* matrix, scalar_t* scalar)
{
    CHECK_NOT_NULL(matrix);
    CHECK_NOT_NULL(scalar);

    for (size_t i = 0; i < matrix->m; i++) {
        for (size_t j = 0; j < matrix->n; j++) {
            scalar_mul(&matrix->lines[i].items[j], &matrix->lines[i].items[j], scalar);
        }
    }
}

void matrix_add(matrix_t* a, matrix_t* b)
{
    CHECK_NOT_NULL(a);
    CHECK_NOT_NULL(b);

    if (a->m != b->m || a->n != b->n) {
        ERROR("matrix dimensions mismatch a=(%zu, %zu), b=(%zu, %zu)", a->m, a->n, b->m, b->n);
    }

    for (size_t i = 0; i < a->n; i++) {
        for (size_t j = 0; j < b->m; j++) {
            scalar_add(&a->lines[i].items[j], &a->lines[i].items[j], &b->lines[i].items[j]);
        }
    }
}

void matrix_sub(matrix_t* a, matrix_t* b)
{
    CHECK_NOT_NULL(a);
    CHECK_NOT_NULL(b);
    if (a->m != b->m || a->n != b->n) {
        ERROR("matrix dimensions mismatch a=(%zu, %zu), b=(%zu, %zu)", a->m, a->n, b->m, b->n);
    }

    for (size_t i = 0; i < a->n; i++) {
        for (size_t j = 0; j < b->m; j++) {
            scalar_sub(&a->lines[i].items[j], &a->lines[i].items[j], &b->lines[i].items[j]);
        }
    }
}

vector_t* matrix_row(matrix_t* matrix, size_t i)
{
    CHECK_NOT_NULL(matrix);

    if (i >= matrix->m) {
        ERROR("row number out of bounds (i=%zu)", i);
    }

    vector_t* line = vector_new(matrix->n);
    vector_copy(line, &matrix->lines[i]);
    return line;
}

vector_t* matrix_col(matrix_t* matrix, size_t j)
{
    CHECK_NOT_NULL(matrix);

    if (j >= matrix->n) {
        ERROR("column number out of bounds (j=%zu)", j);
    }

    vector_t* col = vector_new(matrix->m);
    for (size_t i = 0; i < matrix->m; i++) {
        scalar_copy(&col->items[i], &matrix->lines[i].items[j]);
    }

    return col;
}

vector_t* matrix_diag(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    vector_t* col = vector_new(matrix->m);

    for (size_t i = 0; i < matrix->m; i++) {
        scalar_copy(&col->items[i], &matrix->lines[i].items[i]);
    }

    return col;
}

scalar_t* matrix_get(matrix_t* matrix, size_t i, size_t j)
{
    CHECK_NOT_NULL(matrix);

    if (i >= matrix->m || j >= matrix->n) {
        ERROR("matrix index out of bounds a=(%zu, %zu), (i=%zu, j=%zu)", matrix->m, matrix->n, i, j);
    }

    return scalar_new(matrix->lines[i].items[j].a, matrix->lines[i].items[j].b, matrix->lines[i].items[j].negative);
}

void matrix_set(matrix_t* matrix, size_t i, size_t j, scalar_t* scalar)
{
    CHECK_NOT_NULL(matrix);
    CHECK_NOT_NULL(scalar);

    if (i >= matrix->m || j >= matrix->n) {
        ERROR("matrix index out of bounds a=(%zu, %zu), (i=%zu, j=%zu)", matrix->m, matrix->n, i, j);
    }

    scalar_copy(&matrix->lines[i].items[j], scalar);
}

scalar_t* matrix_det(matrix_t* mat);
bool      matrix_is_inversible(matrix_t* mat);
matrix_t* matrix_inverse(matrix_t* mat);

matrix_t* matrix_transpose(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    matrix_t* inverse = matrix_new(matrix->n, matrix->m);

    for (size_t i = 0; i < matrix->m; i++) {
        for (size_t j = 0; j < matrix->n; j++) {
            scalar_copy(&inverse->lines[j].items[i], &matrix->lines[i].items[j]);
        }
    }

    return inverse;
}

void matrix_lu(matrix_t* matrix, matrix_t* L, matrix_t* U)
{
    CHECK_NOT_NULL(matrix);

    if (matrix->m != matrix->n) {
        ERROR_MESSAGE("not a square matrix");
    }

    size_t n = matrix->m;

    L = matrix_eye(n);
    U = matrix_square(n);

    for (size_t i = 0; i < n; i++) {

        // Upper Triangular
        for (size_t k = i; k < n; k++) {

            // Summation of L(i, j) * U(j, k)
            scalar_t* sum = scalar_from(0);
            scalar_t* tmp = scalar_from(0);
            for (size_t j = 0; j < i; j++) {
                scalar_mul(tmp, &L->lines[i].items[j], &U->lines[j].items[k]);
                scalar_add(sum, sum, tmp);
            }

            // Evaluating U(i, k)
            scalar_sub(tmp, &matrix->lines[i].items[k], sum);
            scalar_copy(&U->lines[i].items[k], tmp);

            scalar_delete(tmp);
            scalar_delete(sum);
        }

        // Lower Triangular
        for (size_t k = i; k < n; k++) {
            if (i != k) {

                // Summation of L(k, j) * U(j, i)
                scalar_t* sum = scalar_from(0);
                scalar_t* tmp = scalar_from(0);
                for (size_t j = 0; j < i; j++) {
                    scalar_mul(tmp, &L->lines[k].items[j], &U->lines[j].items[i]);
                    scalar_add(sum, sum, tmp);
                }

                // Evaluating L(k, i)
                scalar_sub(tmp, &matrix->lines[k].items[i], sum);
                scalar_div(tmp, tmp, &U->lines[i].items[i]);
                scalar_copy(&L->lines[k].items[i], tmp);

                scalar_delete(tmp);
                scalar_delete(sum);
            }
        }
    }

    puts(matrix_string(L));
    puts(matrix_string(U));
}

matrix_t* matrix_chol(matrix_t* matrix);

char* matrix_string(matrix_t* mat)
{
    if (mat == NULL || mat->lines == NULL) {
        ERROR("matrix.string(%p): null pointer ERROR (lines=%p)", mat, mat->lines);
    }

    size_t len = 3; // '[' + mat + ']\0'

    for (size_t i = 0; i < mat->m; i++) {
        len += 2 + vector_string_length(&mat->lines[i]); // ' ' + vec + '\n'
    }

    // discard the first space if matrix isn't empty
    if (mat->m > 0) {
        len--;
    }

    char* str = malloc(len);

    CHECK_NOT_NULL(str);

    if (str == NULL) {
        ERROR("matrix.string(%p): null pointer ERROR", mat);
    }

    char *ofs, *tmp;

    ofs = str + sprintf(str, "[");
    for (size_t i = 0; i < mat->m; i++) {
        tmp = vector_string(&mat->lines[i]);
        ofs += sprintf(ofs, i == 0 ? "%s\n" : " %s\n", tmp);
        free(tmp);
    }

    if (mat->m > 0) {
        sprintf(ofs - 1, "]");
    }

    return str;
}
