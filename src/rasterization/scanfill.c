#include "scanfill.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

RGBA white() {
  RGBA p = {255, 255, 255, 255};
  return p;
}

RGBA black() {
  RGBA p = {0, 0, 0, 255};
  return p;
}

bool horizontal_line(Vector *a, Vector *b) {
  return fabs(*a->y - *b->y) <= 0.0001;
}

void sort_vertices_by_y(Triangle *t) {
  // v1.y <= v2.y <= v3.y
  bool sorted = false;
  Vector *aux;

  while (!sorted) {
    if (*t->vertex_1->y > *t->vertex_2->y) {
      aux = t->vertex_1;
      t->vertex_1 = t->vertex_2;
      t->vertex_2 = aux;
    }

    if (*t->vertex_2->y > *t->vertex_3->y) {
      aux = t->vertex_2;
      t->vertex_2 = t->vertex_3;
      t->vertex_3 = aux;
    }

    sorted = (*t->vertex_1->y <= *t->vertex_2->y) &&
             (*t->vertex_2->y <= *t->vertex_3->y);
  }
}

void sort_vertices_in_horizontal_line(Triangle *t, bool v1_v2) {
  if (v1_v2) {
    // v1.x <= v2.x
    if (*t->vertex_1->x > *t->vertex_2->x) {
      Vector *aux = t->vertex_1;
      t->vertex_1 = t->vertex_2;
      t->vertex_2 = aux;
    }
    assert(*t->vertex_1->x <= *t->vertex_2->x);
  } else {
    // v2.x <= v3.x
    if (*t->vertex_2->x > *t->vertex_3->x) {
      Vector *aux = t->vertex_2;
      t->vertex_2 = t->vertex_3;
      t->vertex_3 = aux;
    }
    assert(*t->vertex_2->x <= *t->vertex_3->x);
  }
}

double get_slope(Vector *a, Vector *b) {
  Vector *sub = sub_vector(a, b, NULL);
  double slope = 0.0;
  if (fabs(*sub->x) > 0.1) {
    slope = *sub->y / *sub->x;
  }
  destroy_vector(sub);
  return slope;
}

void rasterize_from_bottom(Triangle *t, RGBA **pixels, int w, int h) {
  //   v1 ------ v2
  //     \       /
  //      \     /
  //       \   /
  //        \ /
  //         v3
  // Guarantee that v1.x <= v2.x
  sort_vertices_in_horizontal_line(t, true);

  // Obtaining inverse slopes
  double slope_v13 = get_slope(t->vertex_1, t->vertex_3);
  double inv_v13 = (fabs(slope_v13) <= 0.01) ? 0.0 : 1.0 / slope_v13;

  double slope_v23 = get_slope(t->vertex_2, t->vertex_3);
  double inv_v23 = (fabs(slope_v23) <= 0.01) ? 0.0 : 1.0 / slope_v23;

  // Starting lx and rx in the same spot
  double lx = *t->vertex_3->x;
  double rx = lx;

  for (int y = (int)*t->vertex_3->y; y >= (int)*t->vertex_1->y; y--) {
    // Scan line by line
    for (int x = (int)lx; x <= (int)rx; x++) {
      if (x < w && y < h) {
        // Paint interior pixel
        pixels[y][x] = white();
      }
    }

    lx -= inv_v13;
    rx -= inv_v23;
  }
}

void rasterize_from_top(Triangle *t, RGBA **pixels, int w, int h) {
  //         v1
  //        / \
  //       /   \
  //      /     \
  //     /       \
  //    v2 ----- v3
  // Guarantee that v2.x <= v3.x
  sort_vertices_in_horizontal_line(t, false);

  // Obtaining inverse slopes
  double slope_v12 = get_slope(t->vertex_1, t->vertex_2);
  double inv_v12 = (fabs(slope_v12) <= 0.01) ? 0.0 : 1.0 / slope_v12;

  double slope_v13 = get_slope(t->vertex_1, t->vertex_3);
  double inv_v13 = (fabs(slope_v13) <= 0.01) ? 0.0 : 1.0 / slope_v13;

  // Start lx and rx at the same spot
  double lx = *t->vertex_1->x;
  double rx = lx;

  for (int y = (int)*t->vertex_1->y; y <= (int)*t->vertex_2->y; y++) {
    // Scan line by line
    for (int x = (int)lx; x <= (int)rx; x++) {
      if (x < w && y < h) {
        // Paint interior pixel
        pixels[y][x] = white();
      }
    }

    lx += inv_v12;
    rx += inv_v13;
  }
}

void paint(Vector *vertex, RGBA **pixels, int w, int h) {
  int x = (int)*vertex->x;
  int y = (int)*vertex->y;

  if (x < w && y < h) {
    pixels[y][x] = white();
  }
}

RGBA **scanfill(Object *object, int width, int height) {
  // Initialize 2D array of pixels
  RGBA **pixels = (RGBA **)malloc(height * sizeof(RGBA *));
  for (int i = 0; i < height; i++) {
    pixels[i] = (RGBA *)malloc(width * sizeof(RGBA));

    // Initially, all pixels are black
    for (int j = 0; j < width; j++) {
      pixels[i][j] = black();
    }
  }

  // Rasterize object to 2D array of pixels
  for (int i = 0; i < object->n_triangles; i++) {
    // Obtain triangle in the i-th position of
    //  the object
    Triangle *t = object->triangles + i;

    /* Debug: paint vertices positions
    paint(t->vertex_1, pixels, width, height);
    paint(t->vertex_2, pixels, width, height);
    paint(t->vertex_3, pixels, width, height);
    */

    // Ensure that v1.y <= v2.y <= v3.y
    sort_vertices_by_y(t);

    // Check for special cases
    if (horizontal_line(t->vertex_1, t->vertex_2)) {
      //   v1 ------ v2
      //     \       /
      //      \     /
      //       \   /
      //        \ /
      //         v3
      rasterize_from_bottom(t, pixels, width, height);
    } else if (horizontal_line(t->vertex_2, t->vertex_3)) {
      //         v1
      //        / \
      //       /   \
      //      /     \
      //     /       \
      //    v2 ----- v3
      rasterize_from_top(t, pixels, width, height);
    } else {
      // We must divide the rectangle
      //    in two parts separated by
      //    a horizontal line
      // Since v1.y <= v2.y <= v3.y and
      //    there aren't degenerate triangles,
      //    the vertex v2 can be chosen to create
      //    a horizontal line.
      Vector *v4 = copy_vector(t->vertex_2, NULL);

      // The y-coordinate is the same as v2
      // The x-coordinate is found by
      double slope = get_slope(t->vertex_1, t->vertex_3);
      if (fabs(slope) <= 0.0001) {
        *v4->x = *t->vertex_1->x;
      } else {
        double b = *t->vertex_1->y - slope * (*t->vertex_1->x);
        *v4->x = (*v4->y - b) / slope;
      }
      assert(isfinite(*v4->x));

      // Guarantee that the new vertex
      //    is a horizontal line with v2
      assert(horizontal_line(t->vertex_2, v4));

      // Now, we can rasterize two sub-triangles
      Triangle t1 = {t->vertex_1, t->vertex_2, v4};
      rasterize_from_top(&t1, pixels, width, height);

      Triangle t2 = {v4, t->vertex_2, t->vertex_3};
      rasterize_from_bottom(&t2, pixels, width, height);

      // Cleanup
      destroy_vector(v4);
    }
  }

  return pixels;
}

void destroy_canvas(RGBA **canvas, int width, int height) {
  // Free sub-arrays
  for (int i = 0; i < height; i++) {
    free(canvas[i]);
  }

  // Free pointer
  free(canvas);
}
