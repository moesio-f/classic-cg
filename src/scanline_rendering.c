#include "core/matrices.h"
#include "core/scene.h"
#include "core/vectors.h"
#include "rasterization/scanfill.h"
#include <SDL.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

Object *load_scene(char *camera_name, char *object_name, int width,
                   int height) {
  Camera *camera = load_camera(camera_name);
  Object *object = load_object(object_name);
  SpaceConverter *cvt = get_converter(camera);

  // Space conversions in the object vertices
  for (int i = 0; i < object->n_vertices; i++) {
    // Conversions
    Vector *camera_space = cvt_world_to_camera(object->vertices + i, cvt);
    Vector *projection = cvt_camera_to_projection(camera_space, camera, true);
    Vector *window_space = cvt_projection_to_window(projection, width, height);

    // Free previous allocated array
    free(object->vertices[i].arr);

    // Set values
    object->vertices[i] = *window_space;

    // Destroy vectors
    destroy_vector(camera_space);
    destroy_vector(projection);

    // Free allocated space for pointer
    //  but keep the underlying array
    free(window_space);
  }

  // Destroy converter and camera
  destroy_converter(cvt, false);

  return object;
}

void draw(Uint32 *buffer, RGBA **canvas, SDL_PixelFormat *format, int width,
          int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int offset = y * width + x;
      RGBA c = canvas[y][x];
      Uint32 color = SDL_MapRGBA(format, c.r, c.g, c.b, c.a);
      buffer[offset] = color;
    }
  }
}

void reload(char *camera_name, char *object_name, int width, int height,
            Object **object_2d, RGBA ***canvas, SDL_Surface *surface,
            Uint32 *buffer) {
  if (*object_2d != NULL) {
    // Destrou previously allocated
    //  object
    destroy_object(*object_2d);
    printf("Objeto anterior removido da memória.\n");
  }

  if (*canvas != NULL) {
    // Destroy previously allocated
    //  canvas
    destroy_canvas(*canvas, width, height);
    printf("Rasterização anterior removida da memória.\n");
  }

  // Initally load the object and canvas
  *object_2d = load_scene(camera_name, object_name, width, height);
  printf("Objeto carregado com sucesso.\n");

  *canvas = scanfill(*object_2d, width, height);
  printf("Rasterização finalizada com sucesso.\n");

  // Paint surface
  SDL_LockSurface(surface);
  draw(buffer, *canvas, surface->format, width, height);
  SDL_UnlockSurface(surface);
}

int main(int argc, char *argv[]) {
  assert(argc == 3);
  SDL_Window *window;
  SDL_Surface *surface, *win_surface;
  SDL_Event event;
  Object *object_2d = NULL;
  RGBA **canvas = NULL;
  int width, height;
  Uint32 *buffer, color;

  width = 1280;
  height = 720;

  // Init SDL video
  SDL_Init(SDL_INIT_VIDEO);

  // Create window and renderer
  printf("Configurando janela e superfície.\n");
  window = SDL_CreateWindow("Scanline Rendering", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height, 0);
  win_surface = SDL_GetWindowSurface(window);
  surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32,
                                           SDL_PIXELFORMAT_RGBA32);
  buffer = (Uint32 *)surface->pixels;
  printf("Janela e superfície configuradas.\n");

  // Reload surface
  reload(argv[1], argv[2], width, height, &object_2d, &canvas, surface, buffer);

  // Main loop
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
        reload(argv[1], argv[2], width, height, &object_2d, &canvas, surface,
               buffer);
      }
    }

    SDL_BlitSurface(surface, 0, win_surface, 0);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10);
  }
  SDL_Quit();

  return 0;
}
