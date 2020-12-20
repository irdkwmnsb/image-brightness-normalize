#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main() {
    int n = 5, m = 6;
    char (*img)[n] = malloc(sizeof(char[n][m]));
    if (img == NULL) {
        fprintf(stderr, "Could not create image");
        exit(-1);
    }
    printf("%d\n", img[3][4]);
    return 0;
}
