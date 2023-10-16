#ifndef SCENE
#define SCENE

#include "matrices.h"
#include "vectors.h"
#include <stdbool.h>

typedef struct {
  Vector *C, *N, *V;
  double d, hx, hy;
} Camera;

typedef struct {
  Vector *vertex_1;
  Vector *vertex_2;
  Vector *vertex_3;
} Triangle;

typedef struct {
  Vector **vertices;
  Triangle **triangles;
} Object;

typedef struct {
  Matrix *world_to_camera;
  Matrix *camera_to_world;
  Camera *camera;
} SpaceConverter;

// Loading functions
Camera *load_camera(char *filename);
Object *load_object(char *filename);

// Conversion matrices
SpaceConverter *get_converter(Camera *camera);

// Space mapping
Vector *cvt_world_to_camera(Vector *a, SpaceConverter *cvt);
Vector *cvt_camera_to_projection(Vector *a, Camera *camera, bool normalize);
Vector *cvt_projection_to_window(Vector *a, int width, int height);

// Utilities
void destroy_camera(Camera *camera);
void destroy_converter(SpaceConverter *cvt, bool keep_camera);

#endif
