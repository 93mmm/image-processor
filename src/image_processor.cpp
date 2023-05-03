#include <filesystem>
#include <cassert>

#include "image_processor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

using std::string;

Image::Image(std::string fn) {
    dir = fn.substr(0, fn.find_last_of("/") + 1);
    extension = fn.substr(fn.find_last_of("."), fn.length());
    filename = fn.substr(fn.find_last_of("/") + 1, fn.length());
    filename = filename.substr(0, filename.find_last_of("."));
    image = stbi_load(fn.c_str(), &width, &height, &channels, 0);
    if (image == NULL)
        throw "Error while loading image";
}

Image::~Image() {
    stbi_image_free(image);
}

void Image::save_image(string save_as) {
    if (extension == ".png")
        stbi_write_png(save_as.c_str(), width, height, channels, image, width * channels);
    else if (extension == ".jpg" or extension == ".jpeg") {
        stbi_write_jpg(save_as.c_str(), width, height, channels, image, width * channels);
    }
}

void Image::stripe_image(unsigned int stripes) {
    unsigned char *pixelOffset;

    if (stripes & VERTICAL_STRIPES)
        for (int x = 0; x < width; x++) {
            for (int i = 0; i < 2; i++)
                for (int y = 0; y < height; y++) {
                    pixelOffset = image + (x + width * y) * channels;
                    pixelOffset[0] = 0;
                    pixelOffset[1] = 0;
                    pixelOffset[2] = 0;
                }
            x += 2;
            }
    if (stripes & HORIZONTAL_STRIPES)
        for (int y = 0; y < height; y++) {
            for (int i = 0; i < 2; i++)
                for (int x = 0; x < width; x++) {
                    pixelOffset = image + (x + width * y) * channels;
                    pixelOffset[0] = 0;
                    pixelOffset[1] = 0;
                    pixelOffset[2] = 0;
                }
            y += 2;
        }
}

void Image::vintage_image() {
    unsigned char *pixelOffset;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            pixelOffset = image + (x + width * y) * channels;
            pixelOffset[0] = (int)(pixelOffset[0] * 0.608);
            pixelOffset[1] = (int)(pixelOffset[1] * 0.335);
            pixelOffset[2] = (int)(pixelOffset[2] * 0.199);
        }
}

void Image::pixelize_image(int pixel_size) {
    int pxsz_x = pixel_size, pxsz_y = pixel_size;
    for (int y = 0; y < height; y += pixel_size)
        for (int x = 0; x < width; x += pixel_size) {
            if (width - x < pixel_size)
                pxsz_x = width - x;
            if (height - y < pixel_size)
                pxsz_y = height - y;
            set_pixel(x, y, pxsz_x, pxsz_y);
            pxsz_x = pixel_size;
            pxsz_y = pixel_size;
        }
}

void Image::set_pixel(int x_pos, int y_pos, int pixel_size_x, int pixel_size_y) {
    int r = 0, g = 0, b = 0;
    unsigned char *pixelOffset;
    int pixel_square = pixel_size_x * pixel_size_y;
    int range_x = x_pos + pixel_size_x, range_y = y_pos + pixel_size_y;
    for (int y = y_pos; y < range_y; y++)
        for (int x = x_pos; x < range_x; x++) {
            pixelOffset = image + (x + width * y) * channels;
            r += (int)pixelOffset[0];
            g += (int)pixelOffset[1];
            b += (int)pixelOffset[2];
        }
    r = r / pixel_square;
    g = g / pixel_square;
    b = b / pixel_square;
    for (int y = y_pos; y < range_y; y++)
        for (int x = x_pos; x < range_x; x++) {
            pixelOffset = image + (x + width * y) * channels;
            pixelOffset[0] = r;
            pixelOffset[1] = g;
            pixelOffset[2] = b;
        }
}