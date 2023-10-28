#include "entities.h"
#include "math_utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Construction
RenderTriangle *triangles_from_world_object(Object *world_object,
                                            SpaceConverter *cvt, int width,
                                            int height) {
  printf("[scanline/entities] Iniciando carregamento dos triângulos de "
         "renderização.\n");
  int n_triangles = world_object->n_triangles;
  int n_vertices = world_object->n_vertices;
  RenderTriangle *T = malloc(n_triangles * sizeof(RenderTriangle));

  // Holds the normal for the i-th triangle
  Vector **triangle_normals = malloc(n_triangles * sizeof(Vector *));

  // We convert each triangle in the original
  //    object. Note that the vertices in camera,
  //    projection and window space won't share the
  //    same address among the RenderTriangles.
  for (int i = 0; i < n_triangles; i++) {
    // Obtain current triangles
    Triangle *object_t = world_object->triangles + i;
    RenderTriangle *ti = T + i;

    // Dynamic allocate array of 3 pointers
    ti->world = malloc(3 * sizeof(Vector *));
    ti->camera = malloc(3 * sizeof(Vector *));
    ti->camera_normals = malloc(3 * sizeof(Vector *));
    ti->projection = malloc(3 * sizeof(Vector *));
    ti->window = malloc(3 * sizeof(Vector *));

    // Set values in RenderTriangle
    for (int j = 0; j < 3; j++) {
      // Obtain object vertex
      Vector *vertex = NULL;
      switch (j) {
      case 0:
        vertex = object_t->v1;
        break;
      case 1:
        vertex = object_t->v2;
        break;
      default:
        vertex = object_t->v3;
        break;
      }

      // Vertex conversions
      Vector *c = cvt_world_to_camera(vertex, cvt);
      Vector *p = cvt_camera_to_projection(c, cvt->camera, true);
      Vector *w = cvt_projection_to_window(p, width, height);

      // Store the pointer in the correct spot
      ti->world[j] = vertex;
      ti->camera[j] = c;
      ti->window[j] = w;

      // Vertex normal will be set afterwards
      ti->camera_normals[j] = NULL;
    }

    // Obtain triangle normal
    triangle_normals[i] = sub_vector(ti->camera[2], ti->camera[0], NULL);
    Vector *aux = sub_vector(ti->camera[1], ti->camera[0], NULL);
    cross_product(triangle_normals[i], aux, triangle_normals[i]);
    double norm = 1.0 / l2_norm(triangle_normals[i]);
    scalar_mult_vector(norm, triangle_normals[i], triangle_normals[i]);
    destroy_vector(aux);
  }

  printf("[scanline/entities] Calculando normais dos vértices.\n");
  // Obtain vertex normals
  for (int i = 0; i < n_triangles; i++) {
    RenderTriangle *t1 = T + i;
    if (!is_valid_triangle(t1->window[0], t1->window[1], t1->window[2])) {
      continue;
    }

    // For each vertex in this triangle
    for (int v = 0; v < 3; v++) {
      Vector *camera = t1->camera[v];

      // Initialize normal with the current
      //    triangle normal
      Vector *normal = copy_vector(triangle_normals[i], NULL);

      // For every RenderTriangle
      for (int j = 0; j < n_triangles; j++) {
        RenderTriangle *t2 = T + j;
        bool valid_triangle =
            is_valid_triangle(t2->window[0], t2->window[1], t2->window[2]);
        bool same_vertex = vector_equals(camera, t2->camera[0]);
        same_vertex = same_vertex || vector_equals(camera, t2->camera[1]);
        same_vertex = same_vertex || vector_equals(camera, t2->camera[2]);
        if (i != j && valid_triangle && same_vertex) {
          // Add this triangle normal to the normal vector
          add_vector(triangle_normals[j], normal, normal);
        }
      }

      // Normalize the new normal
      double norm = 1.0 / l2_norm(normal);
      scalar_mult_vector(norm, normal, normal);

      // Set triangle normal for this vertex
      t1->camera_normals[v] = normal;
    }
  }

  // Sort triangles by baricenter
  for (int i = 0; i < n_triangles - 1; i++) {
    Vector *min = NULL;
    Vector *i_center = add_vector(T[i].camera[0], T[i].camera[1], NULL);
    int t = -1;
    add_vector(i_center, T[i].camera[2], i_center);
    scalar_mult_vector(1.0 / 3.0, i_center, i_center);

    for (int j = i + 1; j < n_triangles; j++) {
      Vector *j_center = add_vector(T[j].camera[0], T[j].camera[1], NULL);
      add_vector(j_center, T[j].camera[2], j_center);
      scalar_mult_vector(1.0 / 3.0, j_center, j_center);
      if (min == NULL || *j_center->z < *min->z) {
        min = j_center;
        t = j;
      } else {
        destroy_vector(j_center);
      }
    }

    if (*min->z < *i_center->z) {
      RenderTriangle aux = T[i];
      T[i] = T[t];
      T[t] = aux;
    }

    destroy_vector(min);
    destroy_vector(i_center);
  }

  printf("[scanline/entities] Triângulos de renderização carregados.\n");
  return T;
}

// Destruction
void destroy_render_triangles(RenderTriangle *triangles) {}
