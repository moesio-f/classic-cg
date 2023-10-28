#include "math_utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

Vector *interpolate_to_camera_space(BarycentricCoordinates *P,
                                    RenderTriangle *parent) {
  // Use mapper to obtain v1, v2 and v3
  //    in the camera space
  Vector *v1 = parent->camera[0];
  Vector *v2 = parent->camera[1];
  Vector *v3 = parent->camera[2];

  // Update v1, v2, and v3
  v1 = scalar_mult_vector(P->alpha, v1, NULL);
  v2 = scalar_mult_vector(P->beta, v2, NULL);
  v3 = scalar_mult_vector(P->gamma, v3, NULL);

  // Obtain current point in camera space
  Vector *eye_space = const_vector(3, POINT, 0.0);
  add_vector(eye_space, v1, eye_space);
  add_vector(eye_space, v2, eye_space);
  add_vector(eye_space, v3, eye_space);

  // Cleanup
  destroy_vector(v1);
  destroy_vector(v2);
  destroy_vector(v3);

  // Assertions
  assert(isfinite(*eye_space->x));
  assert(isfinite(*eye_space->y));
  assert(isfinite(*eye_space->z));

  return eye_space;
}

Vector *interpolate_normal(BarycentricCoordinates *P, RenderTriangle *parent) {
  Vector *N = const_vector(3, POINT, 0.0);
  Vector *a = scalar_mult_vector(P->alpha, parent->camera_normals[0], NULL);
  Vector *b = scalar_mult_vector(P->beta, parent->camera_normals[1], NULL);
  Vector *c = scalar_mult_vector(P->gamma, parent->camera_normals[2], NULL);

  // Sum normals
  add_vector(a, b, N);
  add_vector(N, c, N);

  // Normalize normal
  double aux = 1.0 / l2_norm(N);
  scalar_mult_vector(aux, N, N);

  // Cleanup
  destroy_vector(a);
  destroy_vector(b);
  destroy_vector(c);

  // Assertions
  assert(isfinite(*N->x));
  assert(isfinite(*N->y));
  assert(isfinite(*N->z));

  return N;
}

double get_slope(Vector *A, Vector *B) {
  Vector *sub = sub_vector(A, B, NULL);
  double slope = 0.0;
  if (fabs(*sub->x) > 0.001) {
    slope = *sub->y / *sub->x;
  }
  destroy_vector(sub);
  return slope;
}

BarycentricCoordinates get_bcoordinates_from_window(Vector *P,
                                                    RenderTriangle *parent) {
  // Pre-conditions
  assert(P->dims == 2);
  assert(isfinite(*P->x));
  assert(isfinite(*P->y));

  // Obtain barycentric parameters
  Vector *v0 = sub_vector(parent->window[1], parent->window[0], NULL);
  Vector *v1 = sub_vector(parent->window[2], parent->window[0], NULL);
  Vector *v2 = sub_vector(P, parent->window[0], NULL);
  double d00 = dot_product(v0, v0);
  double d01 = dot_product(v0, v1);
  double d11 = dot_product(v1, v1);
  double d20 = dot_product(v2, v0);
  double d21 = dot_product(v2, v1);
  double mult = 1.0 / (d00 * d11 - d01 * d01);
  double alpha = mult * (d00 * d21 - d01 * d20);
  double beta = mult * (d11 * d20 - d01 * d21);
  double gamma = 1.0 - alpha - beta;

  // Post-conditions
  assert(isfinite(alpha));
  assert(isfinite(beta));
  assert(isfinite(gamma));
  assert(alpha + beta + gamma <= 1.0001);

  // Cleanup
  destroy_vector(v0);
  destroy_vector(v1);
  destroy_vector(v2);

  BarycentricCoordinates coords = {alpha, beta, gamma};
  return coords;
}

void swap_indices_array(Vector **arr, int prev_idx, int new_idx) {
  if (arr == NULL) {
    // If the pointer is null, there's
    //  nothinh to sort
    return;
  }

  Vector *aux = arr[prev_idx];
  arr[prev_idx] = arr[new_idx];
  arr[new_idx] = aux;
}

void swap_indices_rt(RenderTriangle *T, int prev_idx, int new_idx) {
  swap_indices_array(T->world, prev_idx, new_idx);
  swap_indices_array(T->camera, prev_idx, new_idx);
  swap_indices_array(T->camera_normals, prev_idx, new_idx);
  swap_indices_array(T->projection, prev_idx, new_idx);
  swap_indices_array(T->window, prev_idx, new_idx);
}

void sort_vertices_by_window_y(RenderTriangle *T) {
  // v1.y <= v2.y <= v3.y
  bool sorted = false;
  Vector *aux;

  while (!sorted) {
    if (*T->window[0]->y > *T->window[1]->y) {
      swap_indices_rt(T, 0, 1);
    }

    if (*T->window[1]->y > *T->window[2]->y) {
      swap_indices_rt(T, 1, 2);
    }

    sorted = (*T->window[0]->y <= *T->window[1]->y) &&
             (*T->window[1]->y <= *T->window[2]->y);
  }
}

void sort_vertices_by_window_horizontal_line(RenderTriangle *T, bool v1_v2) {
  if (v1_v2) {
    // v1.x <= v2.x
    if (*T->window[0]->x > *T->window[1]->x) {
      swap_indices_rt(T, 0, 1);
    }
    assert(*T->window[0]->x <= *T->window[1]->x);
  } else {
    // v2.x <= v3.x
    if (*T->window[1]->x > *T->window[2]->x) {
      swap_indices_rt(T, 1, 2);
    }
    assert(*T->window[1]->x <= *T->window[2]->x);
  }
}

bool is_horizontal(Vector *A, Vector *B) {
  return fabs(*A->y - *B->y) <= 0.0001;
}

bool is_valid_triangle(Vector *A, Vector *B, Vector *C) {
  double area = *A->x * (*B->y - *C->y);
  area += *B->x * (*C->y - *A->y);
  area += *C->x * (*A->y - *B->y);
  area /= 2.0;

  return isfinite(area) && fabs(area) > 0.01;
}
