#ifndef SCANFILL
#define SCANFILL

#include "../core/scene.h"

typedef struct {
  int r, g, b, a;
} RGBA;


RGBA **scanfill(Object *object, int width, int height);

void destroy_canvas(RGBA **canvas, int width, int height);

#endif
