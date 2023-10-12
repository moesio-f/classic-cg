#include <SDL.h>

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Event event;
    SDL_Renderer *renderer;
    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 640, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    for (int i = 0; i < 100; i++) {
        SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
    	SDL_RenderClear(renderer);	
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
	}

    return 0;
}
