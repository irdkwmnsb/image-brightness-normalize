//
// Created by me on 20.12.2020.
//

#ifndef HW5_NORMALIZE_H
#define HW5_NORMALIZE_H

#include <math.h>
#include "image.h"

#define max(a, b) ((a) < (b)?(b):(a))
#define min(a, b) ((a) > (b)?(b):(a))

void *normalize(image *img) {
    // convert to ycbcr
    float (*ycbcr)[img->width][img->height][3] = malloc(sizeof(float[img->width][img->height][3]));
#pragma omp parallel for collapse(2) shared(img, ycbcr) default(none)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            (*ycbcr)[x][y][0] = 16 + r * 65.481f / 255 + 128.553f * g / 255 + 24.966f * b / 255;
            (*ycbcr)[x][y][1] = 128 - r * 37.797f / 255 - 74.203f * g / 255 + 112.f * b / 255;
            (*ycbcr)[x][y][2] = 128 + 112 * r / 255 - 93.786f * g / 255 - 18.214f * b / 255;
        }
    }

    // find max and min values of y
    float max_y = 0, min_y = 255;
#pragma omp parallel default(none) shared(ycbcr, img, max_y, min_y)
    {
        float local_max = 0, local_min = 255;
        //#pragma omp for collapse(2)
        for (int x = 0; x < img->width; x++) {
            for (int y = 0; y < img->height; y++) {
                local_max = max(local_max, (*ycbcr)[x][y][0]);
                local_min = min(local_min, (*ycbcr)[x][y][0]);
            }
        }
#pragma omp critical
        {
            max_y = max(max_y, local_max);
            min_y = min(min_y, local_min);
        };
    };
    const float new_min = 0;
    const float new_max = 255;
    // normalize
#pragma omp parallel for collapse(2) shared(ycbcr, max_y, min_y, img, new_max, new_min) default(none)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            (*ycbcr)[x][y][0] = (((*ycbcr)[x][y][0] - min_y) * (new_max - new_min) / (max_y - min_y)) + new_min;
        }
    }
    // convert to rgb
#pragma omp parallel for collapse(2) shared(ycbcr, img) default(none)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float cb = (*ycbcr)[x][y][1], cr = (*ycbcr)[x][y][2];
            float yy2 = 298.082 * (*ycbcr)[x][y][0] / 256;
            image_r(img, x, y) = min(255, max(0,
                                              lround(yy2 + 408.583 * cr / 256 - 222.921)));
            image_g(img, x, y) = min(255, max(0,
                                              lround(yy2 - 100.291 * cb / 256 - 208.120 * cr / 256 +
                                                     135.576)));
            image_b(img, x, y) = min(255, max(0,
                                              lround(yy2 + 516.412 * cb / 256 - 276.836)));
        }
    }
    free(ycbcr);
}


#endif //HW5_NORMALIZE_H
