//
// Created by me on 20.12.2020.
//

#ifndef HW5_NORMALIZE_H
#define HW5_NORMALIZE_H

#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define max(a, b) ((a) < (b)?(b):(a))
#define min(a, b) ((a) > (b)?(b):(a))

void *normalize(image *img) {
    // convert to ycbcr
    // find max and min values of y
    float (*ys)[img->width][img->height] = malloc(sizeof(float[img->width][img->height]));
    float max_y = 0, min_y = 255;
#pragma omp parallel for collapse(2) default(none) shared(ys, img) reduction(min:min_y) reduction(max:max_y)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            float yy = 16 + r * 65.481f / 255 + 128.553f * g / 255 + 24.966f * b / 255;
            (*ys)[x][y] = yy;
            max_y = fmaxf(max_y, yy);
            min_y = fminf(min_y, yy);
            if (min_y == yy) {
                printf("%d %d - %f\n", x, y, yy);
            }
        }
    }
    const float new_min = 16;
    const float new_max = 235;
    // normalize
#pragma omp parallel for collapse(2) default(none) shared(ys, max_y, min_y, img, new_max, new_min)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            float cb = 128 - r * 37.797f / 255 - 74.203f * g / 255 + 112.f * b / 255;
            float cr = 128 + 112 * r / 255 - 93.786f * g / 255 - 18.214f * b / 255;
            float yy2 =
                    298.082f * ((((*ys)[x][y] - min_y) * (new_max - new_min) / (max_y - min_y)) + new_min) / 256;
            image_r(img, x, y) = min(255, max(0,
                                              (yy2 + 408.583 * cr / 256 - 222.921)));
            image_g(img, x, y) = min(255, max(0,
                                              (yy2 - 100.291 * cb / 256 - 208.120 * cr / 256 +
                                               135.576)));
            image_b(img, x, y) = min(255, max(0,
                                              (yy2 + 516.412 * cb / 256 - 276.836)));
        }
    }
    // convert to rgb
    free(ys);
}


#endif //HW5_NORMALIZE_H
