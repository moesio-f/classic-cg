#include "light.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

Color white() {
  Color p = {255, 255, 255, 255};
  return p;
}

Color black() {
  Color p = {0, 0, 0, 255};
  return p;
}

Color ambient_light(Light *light) {
  return mult_scalar_color(light->ka, light->ambient);
}

Color diffuse_light(Light *light, Vector *N, Vector *L) {
  double scalar = dot_product(N, L);
  Vector *aux = scalar_mult_vector(scalar, light->kd, NULL);
  element_wise_prod(aux, light->od, aux);

  // Create new color
  Color color = {(int)(*aux->x * light->local.r),
                 (int)(*aux->y * light->local.g),
                 (int)(*aux->z * light->local.b), 255};

  // Clip results to [0, 255]
  color.r = (color.r > 255) ? 255 : ((color.r) < 0 ? 0 : color.r);
  color.g = (color.g > 255) ? 255 : ((color.g) < 0 ? 0 : color.g);
  color.b = (color.b > 255) ? 255 : ((color.b) < 0 ? 0 : color.b);

  // Cleanup
  destroy_vector(aux);

  return color;
}

Color specular_light(Light *light, Vector *R, Vector *V) {
  double scalar = dot_product(R, V);
  scalar = pow(scalar, light->eta);
  scalar *= light->ks;
  return mult_scalar_color(scalar, light->local);
}

Color color_from_point(Vector *P, Vector *N, Light *light) {
  double aux = 0.0;
  Vector *R = NULL;
  Vector *L = NULL;
  Vector *V = NULL;
  Color ambient = {0, 0, 0, 255};
  Color diffuse = {0, 0, 0, 255};
  Color specular = {0, 0, 0, 255};
  bool discard_diffuse = false;
  bool discard_specular = false;

  // Make a copy of N
  N = copy_vector(N, NULL);

  // Calculate normalized V
  V = scalar_mult_vector(-1.0, P, NULL);
  aux = 1.0 / l2_norm(V);
  scalar_mult_vector(aux, V, V);

  // Calculate normalized L
  L = sub_vector(light->pl, P, NULL);
  aux = 1.0 / l2_norm(L);
  scalar_mult_vector(aux, L, L);

  // Calculate normalized R
  aux = 2.0 * dot_product(N, L);
  R = scalar_mult_vector(aux, N, NULL);
  sub_vector(R, L, R);
  aux = 1.0 / l2_norm(R);
  scalar_mult_vector(aux, R, R);

  // Checks if should discard any component
  if (dot_product(V, N) <= 0.001) {
    scalar_mult_vector(-1.0, N, N);
  }

  if (dot_product(N, L) <= 0.001) {
    discard_specular = true;
    discard_diffuse = true;
  }

  if (dot_product(V, R) < 0) {
    discard_specular = true;
  }

  // Get colors
  ambient = ambient_light(light);

  if (!discard_diffuse) {
    diffuse = diffuse_light(light, N, L);
  }

  if (!discard_specular) {
    specular = specular_light(light, R, V);
  }

  // Cleanup
  destroy_vector(R);
  destroy_vector(N);
  destroy_vector(L);
  destroy_vector(V);

  // I = (Ia + Id) + Is
  return add_color(add_color(ambient, diffuse), specular);
}
