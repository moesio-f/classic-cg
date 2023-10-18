#include "core/matrices.h"
#include "core/scene.h"
#include "core/vectors.h"
#include <SDL.h>

void sdl_basic_window() {
  SDL_Window *window;
  SDL_Event event;
  SDL_Renderer *renderer;

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 640, 640, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  for (int i = 0; i < 100; i++) {
    SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_Quit();
}

void vector_nd_operations(int n) {
  Vector *vec = const_vector(n, POINT, 1.0);
  char newline[] = "\n";

  printf("======= %dD Vectors =======\n", n);
  printf("Initial: ");
  print_vector(vec, newline);

  add_vector(vec, vec, vec);
  printf("Addition: ");
  print_vector(vec, newline);

  scalar_mult_vector(0.5, vec, vec);
  printf("Scalar mult: ");
  print_vector(vec, newline);

  printf("Dot: %f\n", dot_product(vec, vec));
  printf("L2: %f\n", l2_norm(vec));
  destroy_vector(vec);
  printf("===================\n");
}

void matrix_operations() {
  Matrix *matrix = const_matrix(3, 3, 1.0);
  char newline[] = "\n";

  printf("======= Matrix =======\n");
  printf("Initial: ");
  print_matrix(matrix, newline);

  add_matrix(matrix, matrix, matrix);
  printf("Addition: ");
  print_matrix(matrix, newline);

  scalar_mult_matrix(0.5, matrix, matrix);
  printf("Scalar mult: ");
  print_matrix(matrix, newline);

  printf("Determinant: %f\n", determinant(matrix));

  Matrix *mult = mult_matrix(matrix, matrix);
  printf("Matrix product: ");
  print_matrix(mult, newline);

  destroy_matrix(matrix);
  destroy_matrix(mult);
  printf("===================\n");
}

void space_conversion() {
  double c[] = {1.0, 1.0, 2.0};
  double n[] = {-1.0, -1.0, -1.0};
  double v[] = {0.0, 0.0, 1.0};
  double p[] = {1.0, -3.0, -5.0};
  char newline[] = "\n";
  Vector C = {c, c, c + 1, c + 2, 3, POINT};
  Vector N = {n, n, n + 1, n + 2, 3, POINT};
  Vector V = {v, v, v + 1, v + 2, 3, POINT};
  Vector P = {p, p, p + 1, p + 2, 3, POINT};
  Camera camera = {&C, &N, &V, 1.0, 1.0, 1.0};
  SpaceConverter *cvt = get_converter(&camera);

  printf("======= Space Conversion =======\n");
  printf("World Coordinates: ");
  print_vector(&P, newline);

  Vector *P_ = cvt_world_to_camera(&P, cvt);
  printf("Camera Coordinates: ");
  print_vector(P_, newline);

  Vector *proj = cvt_camera_to_projection(P_, &camera, true);
  printf("Projection Coordinates: ");
  print_vector(proj, newline);

  Vector *win = cvt_projection_to_window(proj, 400, 400);
  printf("Window Coordinates (400x400): ");
  print_vector(win, newline);

  // Cleanup
  destroy_vector(P_);
  destroy_vector(proj);
  destroy_converter(cvt, true);
  printf("===================\n");
}

void camera_file_load() {
  Camera *camera = load_camera("data/camera/camera_1.txt");
  char *newline = "\n";
  printf("======= Camera Parameter Loading =======\n");
  printf("C = ");
  print_vector(camera->C, newline);

  printf("N = ");
  print_vector(camera->N, newline);

  printf("V = ");
  print_vector(camera->V, newline);

  printf("d = %f\nhx = %f\nhy = %f\n", camera->d, camera->hx, camera->hy);

  printf("===================\n");
  destroy_camera(camera);
}

void object_file_load() {
  Object *object = load_object("data/objects/sample_1.byu");
  char *newline = "\n";
  printf("======= Object Parameter Loading =======\n");
  printf("n_vertices = %d | n_triangles = %d\n", object->n_vertices,
         object->n_triangles);

  printf("First vertex: ");
  print_vector(object->vertices, newline);

  printf("Last vertex: ");
  print_vector(object->vertices + object->n_vertices - 1, newline);

  printf("===================\n");
  destroy_object(object);
}

int main(int argc, char *argv[]) {
  vector_nd_operations(2);
  vector_nd_operations(3);
  vector_nd_operations(4);
  matrix_operations();
  space_conversion();
  camera_file_load();
  object_file_load();
  sdl_basic_window();
  return 0;
}
