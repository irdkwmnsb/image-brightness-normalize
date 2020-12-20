//
// Created by me on 20.12.2020.
//

#ifndef HW5_IMAGE_H
#define HW5_IMAGE_H

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char *img;
} image;

#define image_r(img, x, y) img->img[((x) * (img)->height + (y)) * 3 + 0]
#define image_g(img, x, y) img->img[((x) * (img)->height + (y)) * 3 + 1]
#define image_b(img, x, y) img->img[((x) * (img)->height + (y)) * 3 + 2]

image *image_init_image(unsigned int width, unsigned int height) {
    image *img = malloc(sizeof(image));
    if (img == NULL) {
        return NULL;
    }
    img->width = width;
    img->height = height;
    img->img = malloc(sizeof(unsigned char[width][height][3]));
    if (img->img == NULL) {
        return NULL;
    }
    return img;
}


void image_destruct_image(image *img) {
    free(img->img);
    free(img);
}

#endif //HW5_IMAGE_H
