#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

void draw_lissajous_curve(SDL_Renderer *renderer, int N, float time) {
    int amplitude = 150;
    float a = 2.0f, b = 3.0f, delta = M_PI / 2;
    for (int i = 0; i < N; i++) {
        float t = (float)i / N;
        int x = WIDTH / 2 + amplitude * sin(a * t + time);
        int y = HEIGHT / 2 + amplitude * sin(b * t + delta + time);
        
        // Color pseudoaleatorio
        SDL_SetRenderDrawColor(renderer, rand() % 256, rand() % 256, rand() % 256, 255);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Lissajous Curve", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int quit = 0;
    SDL_Event event;
    Uint32 start_time = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        float time = (SDL_GetTicks() - start_time) / 1000.0f;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        draw_lissajous_curve(renderer, N, time);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // Control del frame rate (~60 FPS)
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
