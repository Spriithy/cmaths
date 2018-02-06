#include "matrix.h"
#include "utils.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

matrix_t* matrix_new(size_t m, size_t n)
{
    matrix_t* matrix = malloc(sizeof(*matrix));
    CHECK_NOT_NULL(matrix);

    matrix->m = m;
    matrix->n = n;

    matrix->rows = malloc(m * sizeof(scalar_t*));
    CHECK_NOT_NULL(matrix->rows);

    for (size_t i = 0; i < m; i++) {
        matrix->rows[i] = malloc(n * sizeof(scalar_t));
        CHECK_NOT_NULL(matrix->rows[i]);
        for (size_t j = 0; j < n; j++) {
            scalar_copy(&matrix->rows[i][j], &zero);
        }
    }

    return matrix;
}

matrix_t* matrix_square(size_t n)
{
    return matrix_new(n, n);
}

matrix_t* matrix_eye(size_t n)
{
    matrix_t* matrix = matrix_square(n);

    for (size_t i = 0; i < n; i++) {
        scalar_copy(&matrix->rows[i][i], &one);
    }

    return matrix;
}

matrix_t* matrix_from_diag(vector_t* diag)
{
    CHECK_NOT_NULL(diag);

    matrix_t* matrix = matrix_square(diag->n);

    for (size_t i = 0; i < diag->n; i++) {
        scalar_copy(&matrix->rows[i][i], &diag->items[i]);
    }

    return matrix;
}

matrix_t* matrix_from_vector(vector_t* vector, bool line)
{
    CHECK_NOT_NULL(vector);

    size_t m = line ? 1 : vector->n;
    size_t n = line ? vector->n : 1;

    matrix_t* matrix = matrix_new(m, n);

    if (line) {
        for (size_t i = 0; i < vector->n; i++) {
            scalar_copy(&matrix->rows[0][i], &vector->items[i]);
        }

        return matrix;
    }

    for (size_t i = 0; i < vector->n; i++) {
        scalar_copy(&matrix->rows[i][0], &vector->items[i]);
    }

    return matrix;
}

matrix_t* matrix_prod(matrix_t* a, matrix_t* b)
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
            scalar_copy(&mat->rows[i][j], val);
            scalar_delete(val);
            vector_delete(row);
            vector_delete(col);
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
            scalar_mul(&matrix->rows[i][j], &matrix->rows[i][j], scalar);
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
            scalar_add(&a->rows[i][j], &a->rows[i][j], &b->rows[i][j]);
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
            scalar_sub(&a->rows[i][j], &a->rows[i][j], &b->rows[i][j]);
        }
    }
}

vector_t* matrix_row(matrix_t* matrix, size_t i)
{
    CHECK_NOT_NULL(matrix);

    if (i >= matrix->m) {
        ERROR("row number out of bounds (i=%zu)", i);
    }

    return vector_from(matrix->rows[i], matrix->n);
}

vector_t* matrix_col(matrix_t* matrix, size_t j)
{
    CHECK_NOT_NULL(matrix);

    if (j >= matrix->n) {
        ERROR("column number out of bounds (j=%zu)", j);
    }

    vector_t* col = vector_new(matrix->m);
    for (size_t i = 0; i < matrix->m; i++) {
        scalar_copy(&col->items[i], &matrix->rows[i][j]);
    }

    return col;
}

vector_t* matrix_diag(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    vector_t* diag = vector_new(matrix->m);
    for (size_t i = 0; i < matrix->m; i++) {
        scalar_copy(&diag->items[i], &matrix->rows[i][i]);
    }

    return diag;
}

scalar_t* matrix_get(matrix_t* matrix, size_t i, size_t j)
{
    CHECK_NOT_NULL(matrix);

    if (i >= matrix->m) {
        ERROR("row number out of bounds (i=%zu)", i);
    }

    if (j >= matrix->n) {
        ERROR("column number out of bounds (j=%zu)", j);
    }

    return scalar_duplicate(&matrix->rows[i][j]);
}

void matrix_set(matrix_t* matrix, size_t i, size_t j, scalar_t* scalar)
{
    CHECK_NOT_NULL(matrix);
    CHECK_NOT_NULL(scalar);

    if (i >= matrix->m) {
        ERROR("row number out of bounds (i=%zu)", i);
    }

    if (j >= matrix->n) {
        ERROR("column number out of bounds (j=%zu)", j);
    }

    scalar_copy(&matrix->rows[i][j], scalar);
}

scalar_t* matrix_det(matrix_t* mat);
bool      matrix_is_inversible(matrix_t* mat);

matrix_t* matrix_pivotise(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    if (matrix->m != matrix->n) {
        ERROR_MESSAGE("not a square matrix");
    }

    matrix_t* P = matrix_eye(matrix->m);

    for (size_t i = 0; i < P->m; i++) {
        scalar_t* max = &matrix->rows[i][i];
        size_t    row = i;
        for (size_t j = i; j < P->n; j++)
            if (scalar_greater_than(&matrix->rows[j][i], max)) {
                max = &matrix->rows[j][i];
                row = j;
            }

        if (i != row) {
            scalar_t* tmp = P->rows[i];
            P->rows[i]    = P->rows[row];
            P->rows[row]  = tmp;
        }
    }

    return P;
}

matrix_t* matrix_inverse(matrix_t* mat);

matrix_t* matrix_transpose(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    matrix_t* transpose = matrix_new(matrix->n, matrix->m);

    for (size_t i = 0; i < matrix->m; i++) {
        for (size_t j = 0; j < matrix->n; j++) {
            scalar_copy(&transpose->rows[j][i], &matrix->rows[i][j]);
        }
    }

    return transpose;
}

void matrix_lu(matrix_t* matrix, matrix_t** L, matrix_t** U, matrix_t** P)
{
    CHECK_NOT_NULL(matrix);

    if (matrix->m != matrix->n) {
        ERROR_MESSAGE("not a square matrix");
    }

    size_t n = matrix->m;

    *L = matrix_eye(n);
    *U = matrix_square(n);
    *P = matrix_pivotise(matrix);

    matrix_t* A = matrix_prod(*P, matrix);

    scalar_t* tmp  = scalar_from(0);
    scalar_t* tmp2 = scalar_from(0);
    for (size_t j = 0; j < n; j++) {
        for (size_t i = 0; i < j + 1; i++) {
            scalar_t* sum = scalar_from(0);
            for (size_t k = 0; k < i; k++) {
                scalar_mul(tmp, &(*U)->rows[k][j], &(*L)->rows[i][k]);
                scalar_add(sum, sum, tmp);
            }
            scalar_sub(tmp, &A->rows[i][j], sum);
            scalar_copy(&(*U)->rows[i][j], tmp);
            scalar_delete(sum);
        }

        for (size_t i = j; i < n; i++) {
            scalar_t* sum = scalar_from(0);
            for (size_t k = 0; k < j; k++) {
                scalar_mul(tmp, &(*U)->rows[k][j], &(*L)->rows[i][k]);
                scalar_add(sum, sum, tmp);
            }
            scalar_sub(tmp, &A->rows[i][j], sum);
            scalar_copy(tmp2, tmp);
            scalar_div(tmp, tmp2, &(*U)->rows[j][j]);
            scalar_copy(&(*L)->rows[i][j], tmp);
        }
    }
    scalar_delete(tmp2);
    scalar_delete(tmp);
}

matrix_t* matrix_chol(matrix_t* matrix);

char* matrix_string(matrix_t* matrix)
{
    CHECK_NOT_NULL(matrix);

    if (matrix->m == 1) {
        vector_t* vector = vector_from(matrix->rows[0], matrix->n);
        return vector_string(vector);
    }

    size_t* col_width = calloc(matrix->n, sizeof(size_t));
    CHECK_NOT_NULL(col_width);

    // Compute each column's size
    for (size_t j = 0; j < matrix->n; j++) {
        col_width[j] = 1;
        for (size_t i = 0; i < matrix->m; i++) {
            size_t width = scalar_string_length(&matrix->rows[i][j]) - 1;
            if (width > col_width[j]) {
                col_width[j] = width;
            }
        }
    }

    // Compute the total string's length
    size_t str_length = 0;

    for (size_t j = 0; j < matrix->n; j++) {
        str_length += 3;             // left and right separator + line feed
        str_length += matrix->n - 1; // n-1 spaces to separate columns
        str_length += col_width[j];  // column's width
    }

    char* str = malloc(str_length);
    CHECK_NOT_NULL(str);

    char* loc     = str;
    char* val_fmt = "%%%zus ";
    char  tmp[64] = { 0 };

    for (size_t i = 0; i < matrix->m; i++) {
        // line prefix
        if (i == 0) {
            loc += sprintf(loc, "⎡");
        } else if (i == matrix->m - 1) {
            loc += sprintf(loc, "⎣");
        } else {
            loc += sprintf(loc, "⎢");
        }

        for (size_t j = 0; j < matrix->n; j++) {
            // build column-appropriate fmt string
            sprintf(tmp, val_fmt, col_width[j]);

            char* scalar = scalar_string(&matrix->rows[i][j]);
            loc += sprintf(loc, tmp, scalar);

            if (j == matrix->n - 1) {
                loc--;
            }
        }

        // line suffix
        if (i == 0) {
            loc += sprintf(loc, "⎤\n");
        } else if (i == matrix->m - 1) {
            loc += sprintf(loc, "⎦");
        } else {
            loc += sprintf(loc, "⎥\n");
        }
    }

    free(col_width);
    return str;
}
