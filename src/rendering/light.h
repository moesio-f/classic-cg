#ifndef RENDERING_LIGHT
#define RENDERING_LIGHT
#include "../core/scene.h"

Color white();
Color black();
Color ambient_light(Light *light);
Color diffuse_light(Light *light, Vector *N, Vector *L);
Color specular_light(Light *light, Vector *R, Vector *V);

/*
 * Obtain the color for a given point P in camera space,
 * given its normal N and the scene light parameters.
 * */
Color color_from_point(Vector *P, Vector *N, Light *light);

#endif
