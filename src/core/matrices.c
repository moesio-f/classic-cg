#include "matrices.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void assert_same_shape(Matrix *a, Matrix *b) {
  assert(a->rows == b->rows);
  assert(a->columns == b->columns);
}

void assert_compatible_shape(Matrix *a, Matrix *b) {
  assert(a->columns == b->rows);
}

Matrix *maybe_alloc_matrix(Matrix *ptr, int rows, int columns) {
  if (ptr == NULL) {
    ptr = const_matrix(rows, columns, 0.0);
  }

  return ptr;
}

Matrix *const_matrix(int rows, int columns, double value) {
  Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
  matrix->rows = rows;
  matrix->columns = columns;

  // Initializing array
  double **vectors = (double **)malloc(rows * sizeof(double *));
  for (int i = 0; i < rows; i++) {
    vectors[i] = malloc(columns * sizeof(double));
    for (int j = 0; j < columns; j++) {
      vectors[i][j] = value;
    }
  }
  matrix->arr = vectors;

  return matrix;
}

void destroy_matrix(Matrix *matrix) {
  for (int i = 0; i < matrix->rows; i++) {
    free(matrix->arr[i]);
  }
  free(matrix->arr);
  free(matrix);
}

Matrix *copy_matrix(Matrix *a, Matrix *dst) {
  int rows = a->rows;
  int columns = a->columns;
  dst = maybe_alloc_matrix(dst, rows, columns);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      dst->arr[i][j] = a->arr[i][j];
    }
  }

  return dst;
}

void print_matrix(Matrix *a, char *suffix) {
  int i, j;
  printf("\n");
  for (i = 0; i < a->rows; i++) {
    printf("[");
    for (j = 0; j < a->columns - 1; j++) {
      printf("%f ", a->arr[i][j]);
    }
    printf("%f]\n", a->arr[i][j]);
  }

  if (suffix != NULL) {
    printf("%s", suffix);
  }
}

Matrix *matrix_from_vector(Vector *a, bool copy) {
  Matrix *matrix = const_matrix(1, a->dims, 0.0);
  double *target = a->arr;

  // Create copy of original array if needed
  if (copy) {
    target = (double *)malloc(a->dims * sizeof(double));
    for (int i = 0; i < a->dims; i++) {
      target[i] = a->arr[i];
    }
  }

  matrix->arr[0] = target;
  return matrix;
}

Matrix *add_matrix(Matrix *a, Matrix *b, Matrix *dst) {
  assert_same_shape(a, b);
  int rows = a->rows;
  int columns = a->columns;
  dst = maybe_alloc_matrix(dst, rows, columns);

  // Apply operation
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      dst->arr[i][j] = a->arr[i][j] + b->arr[i][j];
    }
  }

  return dst;
}

Matrix *sub_matrix(Matrix *a, Matrix *b, Matrix *dst) {
  assert_same_shape(a, b);
  int rows = a->rows;
  int columns = a->columns;
  dst = maybe_alloc_matrix(dst, rows, columns);

  // Apply operation
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      dst->arr[i][j] = a->arr[i][j] - b->arr[i][j];
    }
  }

  return dst;
}

Matrix *mult_matrix(Matrix *a, Matrix *b) {
  assert_compatible_shape(a, b);
  Matrix *dst = maybe_alloc_matrix(NULL, a->rows, b->columns);

  // Apply operation
  for (int i = 0; i < dst->rows; i++) {
    for (int j = 0; j < dst->columns; j++) {
      double sum = 0.0;

      for (int k = 0; k < a->columns; k++) {
        sum += a->arr[i][k] * b->arr[k][j];
      }

      dst->arr[i][j] = sum;
    }
  }

  return dst;
}

Matrix *scalar_mult_matrix(float a, Matrix *b, Matrix *dst) {
  int rows = b->rows;
  int columns = b->columns;
  dst = maybe_alloc_matrix(dst, rows, columns);

  // Apply operation
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      dst->arr[i][j] = a * b->arr[i][j];
    }
  }

  return dst;
}
