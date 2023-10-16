#include "scene.h"
#include "matrices.h"
#include "vectors.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Camera *load_camera(char *filename) {
  Camera *camera = (Camera *)malloc(sizeof(Camera));
  FILE *fp = fopen(filename, "r");

  assert(fp != NULL);

  return camera;
}

Object *load_object(char *filename) {
  Object *object = (Object *)malloc(sizeof(Object));
  FILE *fp = fopen(filename, "r");

  assert(fp != NULL);

  return object;
}

SpaceConverter *get_converter(Camera *camera) {
  // World to Camera
  Vector *projection = projection_vector(camera->V, camera->N, NULL);
  Vector *orthogonal_v = sub_vector(camera->V, projection, NULL);
  Vector *u = cross_product(camera->N, orthogonal_v, NULL);
  Vector *n = copy_vector(camera->N, NULL);

  // Normalizing bases
  double norm_v = l2_norm(orthogonal_v);
  double norm_u = l2_norm(u);
  double norm_n = l2_norm(n);
  scalar_mult_vector(1 / norm_v, orthogonal_v, orthogonal_v);
  scalar_mult_vector(1 / norm_u, u, u);
  scalar_mult_vector(1 / norm_n, n, n);

  // Creating conversion matrix
  Matrix *matrix = const_matrix(3, 3, 0.0);

  // Free previously allocated memory
  for (int i = 0; i < matrix->rows; i++) {
    free(matrix->arr[i]);
  }

  // Update pointers to bases
  matrix->arr[0] = u->arr;
  matrix->arr[1] = orthogonal_v->arr;
  matrix->arr[2] = n->arr;

  // Cleanup
  destroy_vector(projection);
  free(orthogonal_v);
  free(u);
  free(n);

  // Upda
  SpaceConverter *cvt = (SpaceConverter *)malloc(sizeof(SpaceConverter));
  cvt->camera_to_world = matrix;
  cvt->world_to_camera = inverse(matrix, NULL);
  cvt->camera = camera;

  return cvt;
}

Vector *cvt_world_to_camera(Vector *a, SpaceConverter *cvt) {
  // (a - C)
  Vector *tmp = sub_vector(a, cvt->camera->C, NULL);

  // Matrix by vector multiplication
  Vector *new = mult_matrix_by_vector(cvt->world_to_camera, tmp);

  // Free temporary vector
  destroy_vector(tmp);

  return new;
}

Vector *cvt_camera_to_projection(Vector *a, Camera *camera, bool normalize) {
  Vector *new = const_vector(2, POINT, 0.0);
  new->arr[0] = camera->d *(a->arr[0] / a->arr[2]);
  new->arr[1] = camera->d *(a->arr[1] / a->arr[2]);

  if (normalize) {
    new->arr[0] /= camera->hx;
    new->arr[1] /= camera->hy;
  }

  return new;
}

Vector *cvt_projection_to_window(Vector *a, int width, int height) {
  Vector *new = const_vector(2, POINT, 0.0);
  new->arr[0] = floor(width * (a->arr[0] + 1) / 2 + 0.5);
  new->arr[1] = floor(height - (height * (a->arr[1] + 1) / 2) + 0.5);
  return new;
}

void destroy_camera(Camera *camera) {
  destroy_vector(camera->C);
  destroy_vector(camera->N);
  destroy_vector(camera->V);
  free(camera);
}

void destroy_converter(SpaceConverter *cvt, bool keep_camera) {
  destroy_matrix(cvt->camera_to_world);
  destroy_matrix(cvt->world_to_camera);
  if (!keep_camera) {
    destroy_camera(cvt->camera);
  }
  free(cvt);
}
