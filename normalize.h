//
// Created by me on 20.12.2020.
//

#ifndef HW5_NORMALIZE_H
#define HW5_NORMALIZE_H

#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max(a, b) ((a) < (b)?(b):(a))
#endif
#ifndef min
#define min(a, b) ((a) > (b)?(b):(a))
#endif

void normalize(image *img) {
    // convert to ycbcr
    // find max and min values of y
    float (*ys)[img->width][img->height] = malloc(sizeof(float[img->width][img->height]));
    float max_y = 0, min_y = 255;
#pragma omp parallel for schedule(static) collapse(2) default(none) shared(ys, img) reduction(min:min_y) reduction(max:max_y)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            if(r == g && g == b && b == 0) {
                printf("%d %d\n", x, y);
            }
            float yy = 16 + r * 65.481f / 255 + g * 128.553f / 255 + b * 24.966f / 255;
            (*ys)[x][y] = yy;
            max_y = fmaxf(max_y, yy);
            min_y = fminf(min_y, yy);
        }
    }
    const float new_min = 16;
    const float new_max = 240;
#pragma omp parallel for schedule(static) collapse(2) default(none) shared(ys, max_y, min_y, img, new_max, new_min)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            float cb = 128 - r * 37.797f / 255 - 74.203f * g / 255 + 112.f * b / 255;
            float cr = 128 + 112 * r / 255 - 93.786f * g / 255 - 18.214f * b / 255;
            // normalize
            float yy2 =
                    298.082f * ((((*ys)[x][y] - min_y) * (new_max - new_min) / (max_y - min_y)) + new_min) / 256;
            // convert to rgb
            image_r(img, x, y) = min(255, max(0,
                                              lroundf(yy2 + 408.583f * cr / 256 - 222.921f)));
            image_g(img, x, y) = min(255, max(0,
                                              lroundf(yy2 - 100.291f * cb / 256 - 208.120f * cr / 256 +
                                                      135.576f)));
            image_b(img, x, y) = min(255, max(0,
                                              lroundf(yy2 + 516.412f * cb / 256 - 276.836f)));
        }
    }
    free(ys);
}


#endif //HW5_NORMALIZE_H
