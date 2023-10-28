#include "scanline.h"
#include "entities.h"
#include "light.h"
#include "math_utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Z-buffer utilities
double **create_zbuffer(int width, int height);
void destroy_zbuffer(double **zbuffer, int width, int height);

// Rasterization utilities
void rasterize_from_bottom(RenderTriangle *T, Color **pixels, double **zbuffer,
                           int w, int h, Light *light);

void rasterize_from_top(RenderTriangle *T, Color **pixels, double **zbuffer,
                        int w, int h, Light *light);
void paint(double x, double y, RenderTriangle *T, Color **pixels,
           double **zbuffer, int w, int h, Light *light);

// Main function to rasterize a object defined in world space
Color **rasterize(Object *world_object, Light *light, SpaceConverter *cvt,
                  int width, int height) {
  printf("[scanline] Rasterização iniciada.\n");

  // Initialize z-buffer
  double **zbuffer = create_zbuffer(width, height);

  // Initialize 2D array of pixels
  Color **pixels = (Color **)malloc(height * sizeof(Color *));
  for (int i = 0; i < height; i++) {
    pixels[i] = (Color *)malloc(width * sizeof(Color));

    // Initially, all pixels are black
    for (int j = 0; j < width; j++) {
      pixels[i][j] = black();
    }
  }

  // Obtain the Triangles with all information required to render
  //    them (e.g., camera space, projection, window, normals)
  printf("[scanline] Calculando triângulo de renderização.\n");
  RenderTriangle *triangles =
      triangles_from_world_object(world_object, cvt, width, height);

  // Rasterize object to 2D array of pixels
  printf("[scanline] Iniciando rasterização dos triângulos.\n");
  for (int i = 0; i < world_object->n_triangles; i++) {
    // Obtain render triangle i
    RenderTriangle *t = triangles + i;

    // Ensure that v1.y <= v2.y <= v3.y
    sort_vertices_by_window_y(t);

    // Check for special cases
    if (!is_valid_triangle(t->window[0], t->window[1], t->window[2])) {
      // TODO: add line rasterizer
    } else if (is_horizontal(t->window[0], t->window[1])) {
      rasterize_from_bottom(t, pixels, zbuffer, width, height, light);
    } else if (is_horizontal(t->window[1], t->window[2])) {
      rasterize_from_top(t, pixels, zbuffer, width, height, light);
    } else {
      // We must divide the rectangle
      //    by a horizontal line
      // Since v1.y <= v2.y <= v3.y and
      //    there aren't degenerate triangles,
      //    the vertex v2 can be chosen to create
      //    a horizontal line.
      Vector *v4 = copy_vector(t->window[1], NULL);

      // The y-coordinate is the same as v2
      // The x-coordinate is found by the interception
      //    with the edge v1,3
      double slope = get_slope(t->window[0], t->window[2]);
      if (fabs(slope) <= 0.0001) {
        // There's a vertical line from v1.x and v3.x,
        //  which means that v4.x = v1.x = v3.x
        *v4->x = *t->window[0]->x;
      } else {
        // Otherwise, find the interception using
        //  the line equation
        double b = *t->window[0]->y - slope * (*t->window[0]->x);
        *v4->x = (*v4->y - b) / slope;
      }

      // Guarantee that v4 is valid
      assert(isfinite(*v4->y));
      assert(isfinite(*v4->x));

      // Guarantee that the new vertex
      //    is a horizontal line with v2
      assert(is_horizontal(t->window[1], v4));

      // Obtain the barycentric coordinates of v4
      BarycentricCoordinates coords = get_bcoordinates_from_window(v4, t);

      // Interpolate the point to camera space
      Vector *camera_v4 = interpolate_to_camera_space(&coords, t);

      // Interpolate the normal at this point
      Vector *normal_v4 = interpolate_normal(&coords, t);

      // Now, we can rasterize two sub-triangles
      // First the top
      Vector *c1[3] = {t->camera[0], t->camera[1], camera_v4};
      Vector *cn1[3] = {t->camera_normals[0], t->camera_normals[1], normal_v4};
      Vector *w1[3] = {t->window[0], t->window[1], v4};
      RenderTriangle t1 = {NULL, c1, cn1, NULL, w1};
      assert(is_valid_triangle(w1[0], w1[1], w1[2]));
      rasterize_from_top(&t1, pixels, zbuffer, width, height, light);

      // Then the bottom
      Vector *c2[3] = {camera_v4, t->camera[1], t->camera[2]};
      Vector *cn2[3] = {normal_v4, t->camera_normals[1], t->camera_normals[2]};
      Vector *w2[3] = {v4, t->window[1], t->window[2]};
      RenderTriangle t2 = {NULL, c2, cn2, NULL, w2};
      assert(is_valid_triangle(w2[0], w2[1], w2[2]));
      rasterize_from_bottom(&t2, pixels, zbuffer, width, height, light);

      // Cleanup
      destroy_vector(v4);
      destroy_vector(camera_v4);
      destroy_vector(normal_v4);
    }
  }

  // Cleanup
  destroy_zbuffer(zbuffer, width, height);
  destroy_render_triangles(triangles);

  return pixels;
}

void paint(double x, double y, RenderTriangle *T, Color **pixels,
           double **zbuffer, int w, int h, Light *light) {
  // Obtain barycentric coordinates of the
  //    current point (x, y)
  Vector *P = create_vector(2, POINT, x, y);
  BarycentricCoordinates coords = get_bcoordinates_from_window(P, T);

  // Obtain current point in camera space
  Vector *camera_space = interpolate_to_camera_space(&coords, T);

  // Obtain z-value
  double z = *camera_space->z;

  // Convert continuous points to discrete
  //    ones through floor
  int i = (int)floor(y);
  int j = (int)floor(x);

  // Check whether this point should be drawn
  bool inside_window = j < w && j >= 0;
  inside_window = inside_window && i < h && i >= 0;
  bool in_front = z < zbuffer[i][j];

  if (inside_window && in_front) {
    // Update z-buffer
    zbuffer[i][j] = z;

    // Interpolate normal for this vertex
    Vector *N = interpolate_normal(&coords, T);

    // Paint interior pixel usint the Phong's model
    //  of reflection and color
    pixels[i][j] = color_from_point(camera_space, N, light);
  }

  // Cleanup
  destroy_vector(P);
  destroy_vector(camera_space);
}

void rasterize_from_bottom(RenderTriangle *T, Color **pixels, double **zbuffer,
                           int w, int h, Light *light) {
  //   v1 ------ v2
  //     \       /
  //      \     /
  //       \   /
  //        \ /
  //         v3

  // Guarantee that v1.x <= v2.x
  sort_vertices_by_window_horizontal_line(T, true);

  // Obtaining inverse slopes
  double slope_v13 = get_slope(T->window[0], T->window[2]);
  double slope_v23 = get_slope(T->window[1], T->window[2]);
  double inv_v13 = (fabs(slope_v13) <= 0.01) ? 0.0 : 1.0 / slope_v13;
  double inv_v23 = (fabs(slope_v23) <= 0.01) ? 0.0 : 1.0 / slope_v23;

  // Starting lx and rx in the same spot
  double lx = *T->window[2]->x;
  double rx = lx;

  for (double y = *T->window[2]->y; y >= *T->window[0]->y; y--) {
    // Scan line by line
    for (double x = lx; x <= rx; x++) {
      paint(x, y, T, pixels, zbuffer, w, h, light);
    }

    lx -= inv_v13;
    rx -= inv_v23;
  }
}

void rasterize_from_top(RenderTriangle *T, Color **pixels, double **zbuffer,
                        int w, int h, Light *light) {
  //         v1
  //        / \
  //       /   \
  //      /     \
  //     /       \
  //    v2 ----- v3

  // Guarantee that v2.x <= v3.x
  sort_vertices_by_window_horizontal_line(T, false);

  // Obtaining inverse slopes
  double slope_v12 = get_slope(T->window[0], T->window[1]);
  double slope_v13 = get_slope(T->window[0], T->window[2]);
  double inv_v12 = (fabs(slope_v12) <= 0.01) ? 0.0 : 1.0 / slope_v12;
  double inv_v13 = (fabs(slope_v13) <= 0.01) ? 0.0 : 1.0 / slope_v13;

  // Start lx and rx at the same spot
  double lx = *T->window[0]->x;
  double rx = lx;

  for (double y = *T->window[0]->y; y <= *T->window[1]->y; y++) {
    // Scan line by line
    for (double x = lx; x <= rx; x++) {
      paint(x, y, T, pixels, zbuffer, w, h, light);
    }

    lx += inv_v12;
    rx += inv_v13;
  }
}

double **create_zbuffer(int width, int height) {
  double **zbuffer = (double **)malloc(height * sizeof(double *));
  for (int i = 0; i < height; i++) {
    zbuffer[i] = (double *)malloc(width * sizeof(double));
    for (int j = 0; j < width; j++) {
      // Initially, the z-buffer start with
      //    positive infinity
      zbuffer[i][j] = INFINITY;
    }
  }

  return zbuffer;
}

void destroy_zbuffer(double **zbuffer, int width, int height) {
  // Fre sub-arrays
  for (int i = 0; i < height; i++) {
    free(zbuffer[i]);
  }

  // Free pointer
  free(zbuffer);
}

void destroy_canvas(Color **canvas, int width, int height) {
  // Free sub-arrays
  for (int i = 0; i < height; i++) {
    free(canvas[i]);
  }

  // Free pointer
  free(canvas);
}
