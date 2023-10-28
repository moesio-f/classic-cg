#ifndef SCANFILL
#define SCANFILL

#include "../core/scene.h"

// Main rasterization function
Color **rasterize(Object *world_object, Light *light, SpaceConverter *cvt,
                  int width, int height);

// Cleanup
void destroy_canvas(Color **canvas, int width, int height);

#endif
