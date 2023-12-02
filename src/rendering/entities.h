#ifndef RENDERING_ENTITIES
#define RENDERING_ENTITIES
#include "../core/scene.h"
#include "../core/vectors.h"

typedef struct {
  double alpha, beta, gamma;
} BarycentricCoordinates;

typedef struct {
  Vector **world;
  Vector **camera;
  Vector **camera_normals;
  Vector **projection;
  Vector **window;
} RenderTriangle;

// Construction
RenderTriangle *triangles_from_world_object(Object *world_object,
                                            SpaceConverter *cvt, int width,
                                            int height);

// Destruction
void destroy_render_triangles(RenderTriangle *triangles, int n_triangles);

#endif
