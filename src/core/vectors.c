#include "vectors.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Vector *maybe_alloc_vector(Vector *ptr, int dims, VectorType type) {
  if (ptr == NULL) {
    ptr = const_vector(dims, type, 0.0);
  }

  return ptr;
}

Vector *const_vector(int dims, VectorType type, double value) {
  Vector *vector = (Vector *)malloc(sizeof(Vector));
  assert(vector != NULL);

  vector->dims = dims;
  vector->type = type;
  vector->arr = (double *)malloc(dims * sizeof(double));

  for (int i = 0; i < dims; i++) {
    vector->arr[i] = value;
  }

  return vector;
}

Vector *create_vector(int dims, VectorType type, ...) {
  va_list args;
  va_start(args, type);

  Vector *vector = const_vector(dims, type, 0.0);
  for (int i = 0; i < dims; i++) {
    double value = va_arg(args, double);
    vector->arr[i] = value;
  }

  va_end(args);
  return vector;
}

void destroy_vector(Vector *vector) {
  free(vector->arr);
  free(vector);
}

Vector *copy_vector(Vector *a, Vector *dst) {
  if (dst == NULL) {
    dst = (Vector *)malloc(sizeof(Vector));
  }

  dst->dims = a->dims;
  dst->type = a->type;
  dst->arr = (double *)malloc(dst->dims * sizeof(double));
  for (int i = 0; i < dst->dims; i++) {
    dst->arr[i] = a->arr[i];
  }

  return dst;
}

void print_vector(Vector *a, char *suffix) {
  int i;
  printf("{");
  for (i = 0; i < a->dims - 1; i++) {
    printf("%f, ", a->arr[i]);
  }
  printf("%f}", a->arr[i]);

  if (suffix != NULL) {
    printf("%s", suffix);
  }
}

Vector *add_vector(Vector *a, Vector *b, Vector *dst) {
  assert(a->dims == b->dims);
  dst = maybe_alloc_vector(dst, a->dims, a->type);

  for (int i = 0; i < dst->dims; i++) {
    dst->arr[i] = a->arr[i] + b->arr[i];
  }

  return dst;
}

Vector *sub_vector(Vector *a, Vector *b, Vector *dst) {
  assert(a->dims == b->dims);
  dst = maybe_alloc_vector(dst, a->dims, a->type);

  for (int i = 0; i < dst->dims; i++) {
    dst->arr[i] = a->arr[i] - b->arr[i];
  }

  return dst;
}

Vector *scalar_mult_vector(double a, Vector *b, Vector *dst) {
  dst = maybe_alloc_vector(dst, b->dims, b->type);

  for (int i = 0; i < dst->dims; i++) {
    dst->arr[i] = a * b->arr[i];
  }

  return dst;
}

Vector *projection_vector(Vector *a, Vector *b, Vector *dst) {
  float scalar = dot_product(a, b) / dot_product(b, b);
  return scalar_mult_vector(scalar, b, dst);
}

double dot_product(Vector *a, Vector *b) {
  assert(a->dims == b->dims);
  double sum = 0.0;

  for (int i = 0; i < a->dims; i++) {
    sum += a->arr[i] * b->arr[i];
  }

  return sum;
}

double l2_norm(Vector *a) { return sqrt(dot_product(a, a)); }
