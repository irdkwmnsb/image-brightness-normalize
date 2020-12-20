#include "ppm.h"
#include "normalize.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/timeb.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fputs("Usage: ", stderr);
        fputs(argv[0], stderr);
        fputs(" [filename]", stderr);
        return -1;
    }
    char *repeat = getenv("HW5_REPEAT");
    int repeat_n = 1;
    if (repeat) {
        repeat_n = (int) strtol(repeat, (char **) (repeat + strlen(repeat)), 10);
    }
    fprintf(stderr, "Calculating %d time(s)\n", repeat_n);
    image *img = ppm_read_image(argv[1]);
    //    image *img = ppm_read_image("im/in3.ppm");
    if (img == NULL) {
        fputs(ppm_error_message, stderr);
        return -1;
    }
    int threads_n = omp_get_max_threads();
    omp_set_num_threads(threads_n);

    // read first pixel for debug purposes
//    unsigned char (*img_data)[img->height][img->width][3] = (void *) img->img;
//    int x = 94, y = 94;
//    printf("%d %d %d\n", (*img_data)[x][y][0], (*img_data)[x][y][1], (*img_data)[x][y][2]);
    struct timeb start, end;
    ftime(&start);

    for (int i = 0; i < repeat_n; i++)
        normalize(img);

    ftime(&end);

    long long ns_start = (long long) start.millitm + start.time * 1000;
    long long ns_end = (long long) end.millitm + end.time * 1000;
    printf("%d threads - took %.4f seconds on average (%d times) (start=%lld, end=%lld)\n", threads_n,
           ((float) (ns_end - ns_start)) / (float) repeat_n / 1000, repeat_n, ns_start, ns_end);
    fflush(stdout);
    ppm_write_image(img, "out.ppm");
    image_destruct_image(img);
    return 0;
}
