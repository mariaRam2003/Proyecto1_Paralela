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

#define WIDTH 640
#define HEIGHT 480
#define MAX_FRAMES 100 // Límite de creación de imágenes (puede ser modificado)

// Estructura para almacenar la posición de inicio de las curvas
typedef struct {
    int x;
    int y;
} CurveStart;

// Función para eliminar archivos en la carpeta frames
void clear_frames_directory() {
    DIR *dir = opendir("frames");
    struct dirent *entry;
    char filepath[256];

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            // Ignorar . y ..
            if (entry->d_name[0] != '.') {
                // Copiar la ruta base
                strncpy(filepath, "frames/", sizeof(filepath) - 1);
                filepath[sizeof(filepath) - 1] = '\0'; // Asegurar la terminación nula
                
                // Concatenar el nombre del archivo al final de la ruta
                strncat(filepath, entry->d_name, sizeof(filepath) - strlen(filepath) - 1);
                
                // Eliminar el archivo
                remove(filepath);
            }
        }
        closedir(dir);
    }
}

// Función para dibujar una curva de Lissajous en un punto de inicio aleatorio
void draw_lissajous_curve(cairo_t *cr, int N, float time, int index, CurveStart start) {
    int amplitude = 150;
    float a = 2.0f + index, b = 3.0f + index, delta = M_PI / 2;

    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, (rand() % 256) / 255.0, (rand() % 256) / 255.0, (rand() % 256) / 255.0);
    cairo_move_to(cr, start.x, start.y);

    for (int i = 0; i < N; i++) {
        float t = (float)i / N;
        int x = start.x + amplitude * sin(a * t + time);
        int y = start.y + amplitude * sin(b * t + delta + time);

        cairo_line_to(cr, x, y);
    }

    cairo_stroke(cr);
}

void save_frame_as_image(int frame_num, int num_curves, CurveStart *starts) {
    char filename[150];
    snprintf(filename, sizeof(filename), "frames/frame_%04d.png", frame_num);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    for (int i = 0; i < num_curves; i++) {
        draw_lissajous_curve(cr, 1000, frame_num / 30.0, i, starts[i]);
    }

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void create_gif(int num_frames) {
    system("convert -delay 10 -loop 0 frames/frame_*.png lissajous.gif");
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

    // Crear directorio para los frames
    mkdir("frames", 0777);

    // Limpiar la carpeta frames al inicio
    clear_frames_directory();

    // Predefinir posiciones aleatorias para las curvas
    CurveStart *starts = malloc(num_curves * sizeof(CurveStart));
    for (int i = 0; i < num_curves; i++) {
        starts[i].x = rand() % WIDTH;
        starts[i].y = rand() % HEIGHT;
    }

    int frame_count = 0;
    double start_time = omp_get_wtime();
    
    while (frame_count < N) {
        double frame_start_time = omp_get_wtime();
        
        save_frame_as_image(frame_count, num_curves, starts);

        double frame_end_time = omp_get_wtime();
        double frame_time = frame_end_time - frame_start_time;
        double fps = 1.0 / frame_time;

        printf("Frame %d: FPS = %.2f\n", frame_count, fps);

        frame_count++;
    }

    double end_time = omp_get_wtime();
    double total_time = end_time - start_time;
    double average_fps = N / total_time;

    printf("Total time: %.2f seconds\n", total_time);
    printf("Average FPS: %.2f\n", average_fps);

    if (average_fps < 30) {
        printf("WARNING: FPS has dropped below 30!\n");
    }

    create_gif(frame_count);

    free(starts);

    return 0;
}
