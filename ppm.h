#ifndef HW5_PPM_H
#define HW5_PPM_H

#include <string.h>
#include <limits.h>
#include <errno.h>
#include "image.h"

char *ppm_error_message = NULL;

image *ppm_read_image(char *path) {
    FILE *in = fopen(path, "r");
    if (in == NULL) {
        ppm_error_message = "Could not open image";
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, in) == -1) {
        ppm_error_message = "Could not read image (could not read header)";
        return NULL;
    }
    if (strcmp(line, "P6\n") != 0) {
        ppm_error_message = "Not a PPM (P6) file";
        return NULL;
    }
    if (getline(&line, &len, in) == -1) {
        ppm_error_message = "Format error (no width/height)";
        return NULL;
    }
    char *sep = strchr(line, ' ');
    if (sep == NULL) {
        ppm_error_message = "Format error (invalid width/height)";
        return NULL;
    }
    unsigned long width = strtoul(line, &sep, 10), height = strtoul(sep, (char **) (sep + strlen(sep)), 10);
    if (width == LLONG_MAX || height == LLONG_MAX) {
        ppm_error_message = "Format error (invalid width/height)";
        return NULL;
    }
    image *img = image_init_image(width, height);
    if (img == NULL) {
        ppm_error_message = "Could not create image";
        return NULL;
    }

    if (getline(&line, &len, in) == -1) {
        ppm_error_message = "Format error (no third line)";
        return NULL;
    }
    unsigned long l_maxval = strtoul(line, (char **) (line + strlen(line)), 10);
    if (l_maxval == LLONG_MAX) {
        ppm_error_message = "Format error (invalid maximum color value)";
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
            //            img->img[(i * height + j) * 3 + 0] = ((unsigned int) img->img[(i * height + j) * 3 + 0]) * 255 / l_maxval;
            //            img->img[(i * height + j) * 3 + 1] = ((unsigned int) img->img[(i * height + j) * 3 + 1]) * 255 / l_maxval;
            //            img->img[(i * height + j) * 3 + 2] = ((unsigned int) img->img[(i * height + j) * 3 + 2]) * 255 / l_maxval;
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