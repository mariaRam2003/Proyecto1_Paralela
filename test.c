#include <cairo/cairo.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

void draw_lissajous_curve(cairo_t *cr, int N, float time) {
    int amplitude = 150;
    float a = 2.0f, b = 3.0f, delta = M_PI / 2;

    for (int i = 0; i < N; i++) {
        float t = (float)i / N;
        int x = WIDTH / 2 + amplitude * sin(a * t + time);
        int y = HEIGHT / 2 + amplitude * sin(b * t + delta + time);

        // Set random color
        cairo_set_source_rgb(cr, 1 / (double)RAND_MAX, 1 / (double)RAND_MAX, 1 / (double)RAND_MAX);
        cairo_rectangle(cr, x, y, 1, 1);
        cairo_fill(cr);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    srand(time(NULL));

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(surface);

    int quit = 0;
    while (!quit) {
        float time = clock() / (float)CLOCKS_PER_SEC;

        // Clear the surface with black
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_paint(cr);

        draw_lissajous_curve(cr, N, time);

        // Write output to PNG file (for testing)
        cairo_surface_write_to_png(surface, "lissajous.png");

        // Add a delay (simulate frame rate)
        usleep(16000);  // ~60 FPS
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return 0;
}