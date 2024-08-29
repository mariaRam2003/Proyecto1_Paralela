#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_FRAMES 100 // Límite de creación de imágenes (puede ser modificado)
#define NUM_CURVE_TYPES 5

// Estructura para almacenar la posición de inicio de las curvas
typedef struct {
    int x;
    int y;
} CurveStart;

// Estructura para almacenar los parámetros de las curvas de Lissajous
typedef struct {
    float a;
    float b;
    float delta;
} CurveParams;

// Parámetros predefinidos para diferentes tipos de curvas
CurveParams curve_types[NUM_CURVE_TYPES] = {
    {1.0f, 1.0f, M_PI / 2},    // Circle
    {2.0f, 1.0f, M_PI / 2},    // Figure-8
    {3.0f, 2.0f, M_PI / 2},    // Trefoil
    {3.0f, 4.0f, M_PI / 2},    // Complex curve
    {5.0f, 4.0f, M_PI / 2}     // Star-like curve
};

// Función para eliminar archivos en la carpeta frames
void clear_frames_directory() {
    DIR *dir = opendir("frames");
    struct dirent *entry;
    char filepath[PATH_MAX];  // Use PATH_MAX for maximum path length

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {
                if (snprintf(filepath, sizeof(filepath), "frames/%s", entry->d_name) < (int)sizeof(filepath)) {
                    remove(filepath);
                } else {
                    fprintf(stderr, "Error: Filename too long: %s\n", entry->d_name);
                }
            }
        }
        closedir(dir);
    }
}

// Función para dibujar una curva de Lissajous en un punto de inicio específico y con parámetros predefinidos
void draw_lissajous_curve(cairo_t *cr, int N, float time, CurveParams params, CurveStart start) {
    int amplitude = 150;
    float a = params.a, b = params.b, delta = params.delta;

    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, (rand() % 256) / 255.0, (rand() % 256) / 255.0, (rand() % 256) / 255.0);

    float t = 0;
    int x = start.x + amplitude * sin(a * t + time);
    int y = start.y + amplitude * sin(b * t + delta + time);
    cairo_move_to(cr, x, y);

    for (int i = 1; i <= N; i++) {
        t = (float)i / N * 2 * M_PI;
        x = start.x + amplitude * sin(a * t + time);
        y = start.y + amplitude * sin(b * t + delta + time);
        cairo_line_to(cr, x, y);
    }

    cairo_close_path(cr);
    cairo_stroke(cr);
}

// Función para guardar un fotograma como imagen PNG
void save_frame_as_image(int frame_num, int num_curves, CurveStart *starts, CurveParams *params) {
    char filename[150];
    snprintf(filename, sizeof(filename), "frames/frame_%04d.png", frame_num);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    for (int i = 0; i < num_curves; i++) {
        draw_lissajous_curve(cr, 1000, frame_num / 30.0, params[i], starts[i]);
    }

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

// Función para crear un GIF a partir de los fotogramas generados
void create_gif(int num_frames) {
    char command[256];
    snprintf(command, sizeof(command), "convert -delay 10 -loop 0 frames/frame_*.png lissajous.gif");
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <num_curves>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int num_curves = atoi(argv[2]);

    if (N > MAX_FRAMES) {
        N = MAX_FRAMES;
    }

    srand(time(NULL));

    mkdir("frames", 0777);
    clear_frames_directory();

    CurveStart *starts = malloc(num_curves * sizeof(CurveStart));
    CurveParams *params = malloc(num_curves * sizeof(CurveParams));

    for (int i = 0; i < num_curves; i++) {
        starts[i].x = rand() % WIDTH;
        starts[i].y = rand() % HEIGHT;
        params[i] = curve_types[rand() % NUM_CURVE_TYPES];  // Elegir un tipo de curva al inicio y mantenerla fija
    }

    double start_time = omp_get_wtime();
    
    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic)
        for (int frame_count = 0; frame_count < N; frame_count++) {
            double frame_start_time = omp_get_wtime();
            
            save_frame_as_image(frame_count, num_curves, starts, params);

            double frame_end_time = omp_get_wtime();
            double frame_time = frame_end_time - frame_start_time;
            double fps = 1.0 / frame_time;

            #pragma omp critical
            {
                printf("Frame %d: FPS = %.2f\n", frame_count, fps);
            }
        }
    }

    double end_time = omp_get_wtime();
    double total_time = end_time - start_time;
    double average_fps = N / total_time;

    printf("Total time: %.2f seconds\n", total_time);
    printf("Average FPS: %.2f\n", average_fps);

    if (average_fps < 30) {
        printf("WARNING: FPS has dropped below 30!\n");
    }

    create_gif(N);

    free(starts);
    free(params);

    return 0;
}
