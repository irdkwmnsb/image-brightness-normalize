#include "ppm.h"
#include "normalize.h"
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/timeb.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fputs("Usage: ", stderr);
        fputs(argv[0], stderr);
        fputs(" <threads> <input> <output>\n", stderr);
        return -1;
    }
    char *endptr;
    errno = 0;
    long threads_n = strtol(argv[1], &endptr, 10);
    if (endptr != argv[1] + strlen(argv[1]) || errno == ERANGE) {
        fputs("Number of threads must be an integer\n", stderr);
        return -1;
    }
    omp_set_num_threads(threads_n);

    image *img = ppm_read_image(argv[2]);
    if (img == NULL) {
        fputs(ppm_error_message, stderr);
        fputs("\n", stderr);
        return -1;
    }

    struct timeb start, end;
    ftime(&start);
    normalize(img);
    ftime(&end);

    long long ns_start = (long long) start.millitm + start.time * 1000;
    long long ns_end = (long long) end.millitm + end.time * 1000;
    printf("Time (%ld thread(s)) - %fms\n",
           threads_n, ((float) (ns_end - ns_start)));
//    printf("%f\n",
//           threads_n, ((float) (ns_end - ns_start)));

    if (!ppm_write_image(img, argv[3])) {
        fputs(ppm_error_message, stderr);
        fputs("\n", stderr);
        return -1;
    }
    image_destruct_image(img);
    return 0;
}
