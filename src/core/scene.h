#ifndef SCENE
#define SCENE

#include "matrices.h"
#include "vectors.h"
#include <stdbool.h>

typedef struct {
  int r, g, b, a;
} Color;

typedef struct {
  Vector *pl, *kd, *od;
  Color ambient, local;
  double ka, ks, eta;
} Light;

typedef struct {
  Vector *C, *N, *V;
  double d, hx, hy;
} Camera;

typedef struct {
  Vector *v1;
  Vector *v2;
  Vector *v3;
  int v1_idx, v2_idx, v3_idx;
} Triangle;

typedef struct {
  Vector *vertices;
  Triangle *triangles;
  int n_vertices, n_triangles;
} Object;

typedef struct {
  Matrix *world_to_camera;
  Matrix *camera_to_world;
  Camera *camera;
} SpaceConverter;

// Loading functions
Camera *load_camera(char *filename);
Object *load_object(char *filename);
Light *load_light(char *filename);

// Color manipulation
Color mult_scalar_color(double scalar, Color c);
Color add_color(Color a, Color b);

// Conversion matrices
SpaceConverter *get_converter(Camera *camera);

// Space mapping
Vector *cvt_world_to_camera(Vector *a, SpaceConverter *cvt);
Vector *cvt_camera_to_projection(Vector *a, Camera *camera, bool normalize);
Vector *cvt_projection_to_window(Vector *a, int width, int height);

// Utilities
void destroy_camera(Camera *camera);
void destroy_object(Object *object);
void destroy_light(Light *light);
void destroy_converter(SpaceConverter *cvt, bool keep_camera);

#endif
