#ifndef HW5_PPM_H
#define HW5_PPM_H

#include "image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

char *ppm_error_message = NULL;

image *ppm_read_image(char *path) {
    FILE *in = fopen(path, "r");
    if (in == NULL) {
        ppm_error_message = "Could not open image";
        return NULL;
    }
    int width, height, l_maxval;
    if (fscanf(in, "P6%d%d%d\n", &width, &height, &l_maxval) != 3) {
        ppm_error_message = "Format error";
        return NULL;
    }
    if (l_maxval > 255) {
        ppm_error_message = "Cannot work with non 8 bit images";
        return NULL;
    }
    image *img = image_init_image(width, height);
    if (img == NULL) {
        ppm_error_message = "Could not create image";
        return NULL;
    }
    fread(img->img, height * width * 3, 1, in);
    fclose(in);
#pragma omp parallel for collapse(2) shared(img, height, width, l_maxval) default(none)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_r(img, i, j) = ((unsigned int) image_r(img, i, j)) * 255 / l_maxval;
            image_g(img, i, j) = ((unsigned int) image_g(img, i, j)) * 255 / l_maxval;
            image_b(img, i, j) = ((unsigned int) image_b(img, i, j)) * 255 / l_maxval;
        }
    }
    return img;
}

int ppm_write_image(image *img, char *p) {
    FILE *out = fopen(p, "w");
    fprintf(out, "P6\n%d %d\n255\n", img->width, img->height);
    fwrite(img->img, img->width * img->height * 3, 1, out);
    fclose(out);
    return 0;
}

#endif