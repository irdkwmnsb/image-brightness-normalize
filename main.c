#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "ppm.h"
#include "normalize.h"
#include <time.h>

int main() {

    char *threads = getenv("HW5_THREADS");
    int threads_n = omp_get_max_threads();
    if (threads) {
        threads_n = (int) strtol(threads, (char **) (threads + strlen(threads)), 10);
    }
    fprintf(stderr, "Using %d threads\n", threads_n);
    omp_set_num_threads(threads_n);

    char *repeat = getenv("HW5_REPEAT");
    int repeat_n = 1;
    if (repeat) {
        repeat_n = (int) strtol(repeat, (char **) (repeat + strlen(repeat)), 10);
    }
    fprintf(stderr, "Calculating %d time(s)\n", repeat_n);

    image *img = ppm_read_image("in3.ppm");
    if (img == NULL) {
        fputs(ppm_error_message, stderr);
        return -1;
    }

    // read first pixel for debug purposes
    //    unsigned char (*img_data)[img->height][img->width][3] = (void *) img->img;
    //    int x = 2, y = 0;
    //    printf("%d %d %d\n", (*img_data)[x][y][0], (*img_data)[x][y][1], (*img_data)[x][y][2]);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < repeat_n; i++)
        normalize(img);

    clock_gettime(CLOCK_REALTIME, &end);

    long long ns_start = (long long) start.tv_nsec + start.tv_sec * 1000000000;
    long long ns_end = (long long) end.tv_nsec + end.tv_sec * 1000000000;
    printf("took %.6lf seconds (start=%lld, end=%lld)\n", ((double) (ns_end - ns_start)) / 1e9, ns_start, ns_end);
    ppm_write_image(img, "out3.ppm");
    image_destruct_image(img);
    return 0;
}
