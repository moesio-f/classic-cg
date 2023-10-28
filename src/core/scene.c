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
  float x, y, z;
  assert(fp != NULL);

  // Reading C
  fscanf(fp, "C = %f %f %f ", &x, &y, &z);
  camera->C = create_vector(3, POINT, x, y, z);

  // Reading N
  fscanf(fp, "N = %f %f %f ", &x, &y, &z);
  camera->N = create_vector(3, POINT, x, y, z);

  // Reading V
  fscanf(fp, "V = %f %f %f ", &x, &y, &z);
  camera->V = create_vector(3, POINT, x, y, z);

  // Reading d, hx and hy
  fscanf(fp, "d = %f\nhx = %f\n hy = %f", &x, &y, &z);
  camera->d = x;
  camera->hx = y;
  camera->hy = z;

  // Close the opened file
  fclose(fp);

  return camera;
}

Object *load_object(char *filename) {
  Object *object = (Object *)malloc(sizeof(Object));
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);

  int n_vertices, n_triangles;

  // Readning number of vertices and triangles
  fscanf(fp, "%d %d ", &n_vertices, &n_triangles);
  object->n_vertices = n_vertices;
  object->n_triangles = n_triangles;

  // Allocate vertices and triangles
  Vector *vertices = (Vector *)malloc(n_vertices * sizeof(Vector));
  Triangle *triangles = (Triangle *)malloc(n_triangles * sizeof(Triangle));

  // Store vertices
  for (int i = 0; i < n_vertices; i++) {
    float x, y, z;

    // Load vertex information
    fscanf(fp, "%f %f %f ", &x, &y, &z);

    // Assign values
    vertices[i].dims = 3;
    vertices[i].type = POINT;
    vertices[i].arr = (double *)malloc(3 * sizeof(double));
    vertices[i].arr[0] = x;
    vertices[i].arr[1] = y;
    vertices[i].arr[2] = z;
  }

  // Store triangles
  for (int i = 0; i < n_triangles; i++) {
    int tr1, tr2, tr3;

    // Load triangle information
    fscanf(fp, "%d %d %d ", &tr1, &tr2, &tr3);

    // Assign values
    triangles[i].v1_idx = tr1 - 1;
    triangles[i].v2_idx = tr2 - 1;
    triangles[i].v3_idx = tr3 - 1;
    triangles[i].v1 = vertices + triangles[i].v1_idx;
    triangles[i].v2 = vertices + triangles[i].v2_idx;
    triangles[i].v3 = vertices + triangles[i].v3_idx;
  }

  // Store in object
  object->vertices = vertices;
  object->triangles = triangles;

  return object;
}

Light *load_light(char *filename) {
  Light *light = (Light *)malloc(sizeof(Light));
  Vector *aux = NULL;
  float scalar = 0.0;
  float x, y, z;
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);

  // Load ambient color
  fscanf(fp, "Iamb = %d %d %d ", &light->ambient.r, &light->ambient.g,
         &light->ambient.b);
  light->ambient.a = 255;

  // Load Ka
  fscanf(fp, "Ka = %f ", &scalar);
  light->ka = scalar;

  // Load local color
  fscanf(fp, "Il = %d %d %d ", &light->local.r, &light->local.g,
         &light->local.b);
  light->local.a = 255;

  // Load Pl
  aux = const_vector(3, POINT, 0.0);
  fscanf(fp, "Pl = %f %f %f ", &x, &y, &z);
  aux->arr[0] = x;
  aux->arr[1] = y;
  aux->arr[2] = z;
  light->pl = aux;

  // Load Kd
  aux = const_vector(3, POINT, 0.0);
  fscanf(fp, "Kd = %f %f %f ", &x, &y, &z);
  aux->arr[0] = x;
  aux->arr[1] = y;
  aux->arr[2] = z;
  light->kd = aux;

  // Load Od
  aux = const_vector(3, POINT, 0.0);
  fscanf(fp, "Od = %f %f %f ", &x, &y, &z);
  aux->arr[0] = x;
  aux->arr[1] = y;
  aux->arr[2] = z;
  light->od = aux;

  // Load ks
  fscanf(fp, "Ks = %f ", &scalar);
  light->ks = scalar;

  // Load eta
  fscanf(fp, "eta = %f ", &scalar);
  light->eta = scalar;

  return light;
}

Color mult_scalar_color(double scalar, Color c) {
  int r, g, b;

  // Multiply components
  r = (int)floor(scalar * c.r);
  g = (int)floor(scalar * c.g);
  b = (int)floor(scalar * c.b);

  // Clipt to [0, 255]
  r = (r > 255) ? 255 : (r < 0 ? 0 : r);
  g = (g > 255) ? 255 : (g < 0 ? 0 : g);
  b = (b > 255) ? 255 : (b < 0 ? 0 : b);

  Color new = {r, g, b, c.a};
  return new;
}

Color add_color(Color a, Color b) {
  int r, g, c;

  // Add components
  r = a.r + b.r;
  g = a.g + b.g;
  c = a.b + b.b;

  // Clipt to [0, 255]
  r = (r > 255) ? 255 : r;
  g = (g > 255) ? 255 : g;
  c = (c > 255) ? 255 : c;

  Color new = {r, g, c, a.a};
  return new;
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
  orthogonal_v = scalar_mult_vector(1 / norm_v, orthogonal_v, orthogonal_v);
  u = scalar_mult_vector(1 / norm_u, u, u);
  n = scalar_mult_vector(1 / norm_n, n, n);

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

  // Update
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

  assert(isfinite(new->arr[0]));
  assert(isfinite(new->arr[1]));
  assert(isfinite(new->arr[2]));
  return new;
}

Vector *cvt_camera_to_projection(Vector *a, Camera *camera, bool normalize) {
  Vector *new = const_vector(2, POINT, 0.0);
  new->arr[0] = camera->d *(*a->x / *a->z);
  new->arr[1] = camera->d *(*a->y / *a->z);

  if (normalize) {
    new->arr[0] /= camera->hx;
    new->arr[1] /= camera->hy;
  }

  assert(isfinite(new->arr[0]));
  assert(isfinite(new->arr[1]));
  return new;
}

Vector *cvt_projection_to_window(Vector *a, int width, int height) {
  Vector *new = const_vector(2, POINT, 0.0);
  new->arr[0] = floor(width * (a->arr[0] + 1) / 2 + 0.5);
  new->arr[1] = floor(height - (height * (a->arr[1] + 1) / 2) + 0.5);

  assert(isfinite(new->arr[0]));
  assert(isfinite(new->arr[1]));
  return new;
}

void destroy_camera(Camera *camera) {
  destroy_vector(camera->C);
  destroy_vector(camera->N);
  destroy_vector(camera->V);
  free(camera);
}

void destroy_object(Object *object) {
  for (int i = 0; i < object->n_vertices; i++) {
    free(object->vertices[i].arr);
  }
  for (int i = 0; i < object->n_triangles; i++) {
    Triangle *t = object->triangles + i;
  }
  free(object->triangles);
  free(object->vertices);
  free(object);
}

void destroy_light(Light *light) {
  destroy_vector(light->pl);
  destroy_vector(light->kd);
  destroy_vector(light->od);
  free(light);
}

void destroy_converter(SpaceConverter *cvt, bool keep_camera) {
  destroy_matrix(cvt->camera_to_world);
  destroy_matrix(cvt->world_to_camera);
  if (!keep_camera) {
    destroy_camera(cvt->camera);
  }
  free(cvt);
}
