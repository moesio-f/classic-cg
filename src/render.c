#include "core/matrices.h"
#include "core/scene.h"
#include "core/vectors.h"
#include "rendering/scanline.h"
#include <SDL.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  Camera *camera;
  Object *object;
  Light *light;
  SpaceConverter *cvt;
} Scene;

void destroy_scene(Scene *scene) {
  destroy_object(scene->object);
  destroy_light(scene->light);
  destroy_converter(scene->cvt, false);
  free(scene);
}

Scene *load_scene(char *camera_name, char *object_name, char *light_name) {
  Scene *scene = (Scene *)malloc(sizeof(Scene));
  scene->camera = load_camera(camera_name);
  scene->object = load_object(object_name);
  scene->light = load_light(light_name);
  scene->cvt = get_converter(scene->camera);
  return scene;
}

void draw(Uint32 *buffer, Color **canvas, SDL_PixelFormat *format, int width,
          int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int offset = y * width + x;
      Color c = canvas[y][x];
      Uint32 color = SDL_MapRGBA(format, c.r, c.g, c.b, c.a);
      buffer[offset] = color;
    }
  }
}

void reload(char *camera_name, char *object_name, char *light_name, int width,
            int height, Scene **scene, Color ***canvas, SDL_Surface *surface,
            Uint32 *buffer) {
  if (*scene != NULL) {
    // Destroy previously scene
    destroy_scene(*scene);
    printf("[main] Cena anterior removida da memória.\n");
  }

  if (*canvas != NULL) {
    // Destroy previously allocated
    //  canvas
    destroy_canvas(*canvas, width, height);
    printf("[main] Rasterização anterior removida da memória.\n");
  }

  // Initally load the object and canvas
  *scene = load_scene(camera_name, object_name, light_name);
  printf("[main] Cena carregada com sucesso.\n");

  *canvas = rasterize((*scene)->object, (*scene)->light, (*scene)->cvt, width,
                      height);
  printf("[main] Rasterização finalizada com sucesso.\n");

  // Paint surface
  SDL_LockSurface(surface);
  draw(buffer, *canvas, surface->format, width, height);
  SDL_UnlockSurface(surface);
}

int main(int argc, char *argv[]) {
  assert(argc == 4 || argc == 6);
  SDL_Window *window;
  SDL_Surface *surface, *win_surface;
  SDL_Event event;
  Scene *scene = NULL;
  Color **canvas = NULL;
  int width = 600;
  int height = 600;
  Uint32 *buffer, color;

  if (argc == 6) {
    width = atoi(argv[4]);
    height = atoi(argv[5]);
  }

  // Init SDL video
  SDL_Init(SDL_INIT_VIDEO);

  // Create window and renderer
  printf("[main] Configurando janela e superfície.\n");
  window = SDL_CreateWindow("Scanline Rendering", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height, 0);
  win_surface = SDL_GetWindowSurface(window);
  surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32,
                                           SDL_PIXELFORMAT_RGBA32);
  buffer = (Uint32 *)surface->pixels;
  printf("[main] Janela e superfície configuradas.\n");

  // Reload surface
  reload(argv[1], argv[2], argv[3], width, height, &scene, &canvas, surface,
         buffer);

  // Main loop
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
        printf("[main] === Recarregando cena ===\n");
        reload(argv[1], argv[2], argv[3], width, height, &scene, &canvas,
               surface, buffer);
        printf("[main] === Cena recarregada ===\n");
      }
    }

    SDL_BlitSurface(surface, 0, win_surface, 0);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10);
  }
  SDL_Quit();

  return 0;
}
