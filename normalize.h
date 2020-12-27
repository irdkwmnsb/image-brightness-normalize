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

const float BT601a = 0.299f;
const float BT601b = 0.587f;
const float BT601c = 0.114f;
const float BT601d = 1.772f;
const float BT601e = 1.402f;
const float BT601gcrcoeff = (BT601a * BT601e / BT601b);
const float BT601gcbcoeff = (BT601c * BT601d / BT601b);

void normalize(image *img) {
    // convert to ycbcr
    // find max and min values of y
    float (*ys)[img->width][img->height] = malloc(sizeof(float[img->width][img->height]));
    float max_y = 0, min_y = 255;
#pragma omp parallel for schedule(auto) collapse(2) default(none) shared(ys, img, BT601a, BT601b, BT601c) reduction(min:min_y) reduction(max:max_y)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), g = image_g(img, x, y), b = image_b(img, x, y);
            float yy = BT601a * r + BT601b * g + BT601c * b;
            (*ys)[x][y] = yy;
            max_y = fmaxf(max_y, yy);
            min_y = fminf(min_y, yy);
        }
    }
    const float new_min = 0;
    const float new_max = 255;
    fprintf(stderr, "%f %f\n", min_y, max_y);
#pragma omp parallel for schedule(auto) collapse(2) default(none) shared(ys, max_y, min_y, img, new_max, new_min, BT601gcrcoeff, BT601gcbcoeff, BT601d, BT601e)
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            float r = image_r(img, x, y), b = image_b(img, x, y);
            // finishing converting
            float old_y = (*ys)[x][y];
            float cb = (b - old_y) / BT601d;
            float cr = (r - old_y) / BT601e;
            // normalize
            float new_y =
                    ((old_y - min_y) * (new_max - new_min) / (max_y - min_y)) + new_min;
            // convert to rgb
            image_r(img, x, y) = min(255, max(0,
                                              lroundf(new_y + BT601e * cr)));
            image_g(img, x, y) = min(255, max(0,
                                              lroundf(new_y - BT601gcrcoeff * cr - BT601gcbcoeff * cb)));
            image_b(img, x, y) = min(255, max(0,
                                              lroundf(new_y + BT601d * cb)));
        }
    }
    free(ys);
}


#endif //HW5_NORMALIZE_H
