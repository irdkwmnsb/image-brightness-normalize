#include "ppm.h"
#include "normalize.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/timeb.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fputs("Usage: ", stderr);
        fputs(argv[0], stderr);
        fputs(" [filename]", stderr);
        return -1;
    }
    char *repeat = getenv("HW5_REPEAT");
    int repeat_n = 1;
    if (repeat) {
        repeat_n = (int) strtol(repeat, (char **) (repeat + strlen(repeat)), 10);
        fprintf(stderr, "Calculating %d time(s)\n", repeat_n);
    }
    image *img = ppm_read_image(argv[1]);
    //    image *img = ppm_read_image("im/in3.ppm");
    if (img == NULL) {
        fputs(ppm_error_message, stderr);
        return -1;
    }
    int is_benchmark = getenv("HW5_BENCHMARK") != NULL;
    for (int threads_n = 1; threads_n < omp_get_num_procs() + 3; threads_n++) {
        if (!is_benchmark) {
            threads_n = omp_get_num_procs();
        }
        omp_set_num_threads(threads_n);
        struct timeb start, end;
        ftime(&start);

        for (int i = 0; i < repeat_n; i++)
            normalize(img);

        ftime(&end);

        long long ns_start = (long long) start.millitm + start.time * 1000;
        long long ns_end = (long long) end.millitm + end.time * 1000;
        printf("Time (%i thread(s)) - %.4fms on average (%d times)\n",
               threads_n, ((float) (ns_end - ns_start)) / (float) repeat_n, repeat_n);
        fflush(stdout);
        if (!is_benchmark) {
            break;
        }
    }
    ppm_write_image(img, "out.ppm");
    image_destruct_image(img);
    return 0;
}
