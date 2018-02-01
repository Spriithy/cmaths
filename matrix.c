#include "matrix.h"
#include "exception.h"
#include "vector.h"
#include <stdlib.h>

matrix_t* matrix_new(size_t m, size_t n)
{
    matrix_t* mat = malloc(sizeof(*mat));

    if (mat == NULL) {
        exception("matrix.new(%zu, %zu): null pointer exception", m, n);
    }

    mat->m = m;
    mat->n = n;

    mat->lines = calloc(m, sizeof(vector_t));

    if (mat->lines == NULL) {
        exception("matrix.new(%zu, %zu): null pointer exception", m, n);
    }

    for (size_t i = 0; i < m; i++) {
        mat->lines[i] = *vector_new(n, true);
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
        mat->lines[i].vec[i].negative = false;
        mat->lines[i].vec[i].a        = 1;
        mat->lines[i].vec[i].b        = 1;
    }

    return mat;
}

matrix_t* matrix_from_diag(vector_t* diag)
{
    if (diag == NULL) {
        exception("matrix.diag(%p): null pointer exception", diag);
    }

    matrix_t* mat = matrix_square(diag->n);

    for (size_t i = 0; i < diag->n; i++) {
        scalar_copy(&mat->lines[i].vec[i], &diag->vec[i]);
    }

    return mat;
}

matrix_t* matrix_from_vector(vector_t* vector)
{
    if (vector == NULL) {
        exception("matrix.from_vec(%p): null pointer exception", vector);
    }

    size_t m = vector->line ? 1 : vector->n;
    size_t n = vector->line ? vector->n : 1;

    matrix_t* mat = matrix_new(m, n);

    if (vector->line) {
        for (size_t i = 0; i < vector->n; i++) {
            scalar_copy(&mat->lines[0].vec[i], &vector->vec[i]);
        }

        return mat;
    }

    for (size_t i = 0; i < vector->n; i++) {
        scalar_copy(&mat->lines[i].vec[0], &vector->vec[i]);
    }

    return mat;
}

matrix_t* matrix_from_prod(matrix_t* a, matrix_t* b)
{
    if (a == NULL || b == NULL) {
        exception("matrix.from_prod(%p, %p): null pointer exception", a, b);
    }

    size_t m = a->m;
    size_t n = b->n;

    if (a->n != b->m) {
        exception("matrix.from_prod(%p, %p): matrix dimension mismatch A is (%zu, %zu), B is (%zu, %zu)",
            a, b, a->m, a->n, b->m, b->n);
    }

    matrix_t* mat = matrix_new(m, n);

    scalar_t* val;
    vector_t *col, *row;
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            row = matrix_row(a, i);
            col = matrix_col(b, j);
            val = vector_dot_prod(row, col);
            scalar_copy(&mat->lines[i].vec[j], val);
            free(val);
            free(row);
            free(col);
        }
    }

    return mat;
}

void matrix_scale(matrix_t* mat, scalar_t* scalar)
{
    if (mat == NULL || mat->lines == NULL || scalar == NULL) {
        exception("matrix.scale(%p, %p): null pointer exception", mat, scalar);
    }

    for (size_t i = 0; i < mat->m; i++) {
        for (size_t j = 0; j < mat->n; j++) {
            scalar_mul(&mat->lines[i].vec[j], &mat->lines[i].vec[j], scalar);
        }
    }
}

void matrix_add(matrix_t* a, matrix_t* b)
{
    if (a == NULL || a->lines == NULL || b == NULL || b->lines == NULL) {
        exception("matrix.add(%p, %p): null pointer exception", a, b);
    }

    if (a->m != b->m || a->n != b->n) {
        exception("matrix.add(%p, %p): matrix dimensions mismatch a=(%zu, %zu), b=(%zu, %zu)",
            a, b, a->m, a->n, b->m, b->n);
    }

    for (size_t i = 0; i < a->n; i++) {
        for (size_t j = 0; j < b->m; j++) {
            scalar_add(&a->lines[i].vec[j], &a->lines[i].vec[j], &b->lines[i].vec[j]);
        }
    }
}

void matrix_sub(matrix_t* a, matrix_t* b)
{
    if (a == NULL || a->lines == NULL || b == NULL || b->lines == NULL) {
        exception("matrix.sub(%p, %p): null pointer exception", a, b);
    }

    if (a->m != b->m || a->n != b->n) {
        exception("matrix.sub(%p, %p): matrix dimensions mismatch a=(%zu, %zu), b=(%zu, %zu)",
            a, b, a->m, a->n, b->m, b->n);
    }

    for (size_t i = 0; i < a->n; i++) {
        for (size_t j = 0; j < b->m; j++) {
            scalar_sub(&a->lines[i].vec[j], &a->lines[i].vec[j], &b->lines[i].vec[j]);
        }
    }
}

vector_t* matrix_row(matrix_t* a, size_t i)
{
    if (a == NULL || a->lines == NULL) {
        exception("matrix.row(%p, %zu): null pointer exception", a, i);
    }

    if (i >= a->m) {
        exception("matrix.row(%p, %zu): row number out of bounds", a, i);
    }

    vector_t* line = vector_new(a->n, true);
    vector_copy(line, &a->lines[i]);
    return line;
}

vector_t* matrix_col(matrix_t* a, size_t j)
{
    if (a == NULL || a->lines == NULL) {
        exception("matrix.col(%p, %zu): null pointer exception", a, j);
    }

    if (j >= a->n) {
        exception("matrix.col(%p, %zu): column number out of bounds", a, j);
    }

    vector_t* col = vector_new(a->m, false);
    for (size_t i = 0; i < a->m; i++) {
        scalar_copy(&col->vec[i], &a->lines[i].vec[j]);
    }

    return col;
}

vector_t* matrix_diag(matrix_t* a)
{
    if (a == NULL || a->lines == NULL) {
        exception("matrix.line(%p): null pointer exception", a);
    }

    vector_t* col = vector_new(a->m, false);

    for (size_t i = 0; i < a->m; i++) {
        scalar_copy(&col->vec[i], &a->lines[i].vec[i]);
    }

    return col;
}

scalar_t* matrix_get(matrix_t* a, size_t i, size_t j)
{
    if (a == NULL || a->lines == NULL) {
        exception("matrix.get(%p, %zu, %zu): null pointer exception", a, i, j);
    }

    if (i >= a->m || j >= a->n) {
        exception("matrix.get(%p, %zu, %zu): matrix index out of bounds a=(%zu, %zu), (i=%zu, j=%zu)",
            a, i, j, a->m, a->n, i, j);
    }

    return scalar_new(a->lines[i].vec[j].a, a->lines[i].vec[j].b, a->lines[i].vec[j].negative);
}

void matrix_set(matrix_t* a, size_t i, size_t j, scalar_t* x)
{
    if (a == NULL || a->lines == NULL || x == NULL) {
        exception("matrix.set(%p, %zu, %zu, %p): null pointer exception", a, i, j, x);
    }

    if (i >= a->m || j >= a->n) {
        exception("matrix.set(%p, %zu, %zu, %p): matrix index out of bounds a=(%zu, %zu), (i=%zu, j=%zu)",
            a, i, j, x, a->m, a->n, i, j);
    }

    scalar_copy(&a->lines[i].vec[j], x);
}

scalar_t* matrix_det(matrix_t* mat);
bool      matrix_is_inversible(matrix_t* mat);
matrix_t* matrix_inverse(matrix_t* mat);

matrix_t* matrix_transpose(matrix_t* matrix)
{
    if (matrix == NULL) {
        exception("matrix.transpose(%p): null pointer exception", matrix);
    }

    matrix_t* mat = matrix_new(matrix->n, matrix->m);

    for (size_t i = 0; i < matrix->m; i++) {
        for (size_t j = 0; j < matrix->n; j++) {
            scalar_copy(&mat->lines[j].vec[i], &matrix->lines[i].vec[j]);
        }
    }

    return mat;
}

char* matrix_string(matrix_t* mat)
{
    if (mat == NULL || mat->lines == NULL) {
        exception("matrix.string(%p): null pointer exception (lines=%p)", mat, mat->lines);
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

    if (str == NULL) {
        exception("matrix.string(%p): null pointer exception", mat);
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