#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_FRAMES 100 // Límite de creación de imágenes (puede ser modificado)

void draw_lissajous_curve(cairo_t *cr, int N, float time, int index) {
    int amplitude = 150;
    float a = 2.0f + index, b = 3.0f + index, delta = M_PI / 2;
    
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, (rand() % 256) / 255.0, (rand() % 256) / 255.0, (rand() % 256) / 255.0);
    cairo_move_to(cr, WIDTH / 2, HEIGHT / 2);

    for (int i = 0; i < N; i++) {
        float t = (float)i / N;
        int x = WIDTH / 2 + amplitude * sin(a * t + time);
        int y = HEIGHT / 2 + amplitude * sin(b * t + delta + time);
        cairo_line_to(cr, x, y);
    }
    
    cairo_stroke(cr);
}

void save_frame_as_image(int frame_num, int num_curves) {
    char filename[150];
    snprintf(filename, sizeof(filename), "frames/frame_%04d.png", frame_num);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    #pragma omp parallel for
    for (int i = 0; i < num_curves; i++) {
        draw_lissajous_curve(cr, 1000, frame_num / 30.0, i); // Ajuste de movimiento
    }

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void create_gif(int num_frames) {
    system("convert -delay 10 -loop 0 frames/frame_*.png lissajous.gif"); // Ajuste de delay
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <num_curves>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int num_curves = atoi(argv[2]);
    
    if (N > MAX_FRAMES) {
        N = MAX_FRAMES; // Límite de creación de imágenes
    }

    srand(time(NULL));

    // Crear directorio para los frames
    mkdir("frames", 0777);

    int frame_count = 0;
    while (frame_count < N) {
        save_frame_as_image(frame_count, num_curves);
        frame_count++;
    }
    
    create_gif(frame_count);

    return 0;
}