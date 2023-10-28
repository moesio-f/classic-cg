#ifndef RENDERING_MATH_UTILS
#define RENDERING_MATH_UTILS
#include "../core/scene.h"
#include "entities.h"

/*
 * Interpolate a point P (given in barycentric coordinates
 * of the parent Triangle's window coordinates) to camera
 * space.
 * */
Vector *interpolate_to_camera_space(BarycentricCoordinates *P,
                                    RenderTriangle *parent);

/*
 * Interpolate the normal in camera space of a point P (given
 * in barycentric coordinates of the parent Triangle's window
 * coordinates).
 * */
Vector *interpolate_normal(BarycentricCoordinates *P, RenderTriangle *parent);

/*
 * Obtain the slope of the line that intersects
 * both points A and B. Vectors must be 2D.
 * */
double get_slope(Vector *A, Vector *B);

/*
 * Obtain the barycentric coordinates of a point P defined
 * in the window space of the parent Triangle.
 * */
BarycentricCoordinates get_bcoordinates_from_window(Vector *P,
                                                    RenderTriangle *parent);

/*
 * Sort vertices of a triangle according to the
 * y component of the window space.
 * */
void sort_vertices_by_window_y(RenderTriangle *T);

/*
 * Sort vertices of a triangle according to the
 * y component of the window space.
 * */
void sort_vertices_by_window_horizontal_line(RenderTriangle *T, bool v1_v2);

/*
 * Check whether two points A and B define
 * a horizontal line.
 * */
bool is_horizontal(Vector *A, Vector *B);

/*
 * Check whether three 2D points define a
 * triangle.
 * */
bool is_valid_triangle(Vector *A, Vector *B, Vector *C);

#endif
