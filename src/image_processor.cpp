#include <filesystem>

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
            pixelOffset[0] = (int)(pixelOffset[0] * 0.3882);
            pixelOffset[1] = (int)(pixelOffset[1] * 0.2427);
            pixelOffset[2] = (int)(pixelOffset[2] * 0.20);
        }
    std::srand(std::time(nullptr));
    int n = std::rand() % 30 + 5;
    for (int i = 0; i < n; i++) {
        int x = std::rand() % width;
        int length = std::rand() % height;
        bool up = std::rand() % 2;
        for (int y = 0; y < length; y++) {
            if (not up) {
                pixelOffset = image + (x + width * y) * channels;
            } else {
                pixelOffset = image + (x + width * (height - y)) * channels;
            }
            if (std::rand() % 15 < 1)
                y += std::rand() % 15 + 3;
            pixelOffset[0] = 137;
            pixelOffset[1] = 81;
            pixelOffset[2] = 58;
        }
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

void Image::glitch_image() {
    unsigned char *pixel_offset,
                  *shifted_pixel_offset;
    int shift = width * 0.02;
    for (int y = 0; y < height; y++)
        for (int x = width - 1; x >= 0; x--) {
            if (x + shift < width) {
                pixel_offset = image + (x + width * y) * channels;
                shifted_pixel_offset = image + (x + shift + width * y) * channels;
                shifted_pixel_offset[0] = pixel_offset[0];
                shifted_pixel_offset[2] = pixel_offset[2];
            }
        }
    
    std::srand(std::time(nullptr));
    int length, start_line, end_line;
    bool sift_to_right;
    for (int i = 0; i < height; i+=length) {
        length = std::rand() % (int)(height * 0.05) + 10;
        sift_to_right = std::rand() % 2;
        shift = std::rand() % (int)(height * 0.04) + height * 0.01;
        start_line = i;
        end_line = start_line + length;

        shift_random_pieces(start_line, end_line, shift, sift_to_right);
    }
}

void Image::shift_random_pieces(int start_line, int end_line, int shift, bool sift_to_right) {
    unsigned char *pixel_offset,
                  *shifted_pixel_offset;
    if (sift_to_right) {
        for (int y = start_line; y < end_line; y++)
            for (int x = width - 1; x >= 0; x--) {
                if (x + shift < width) {
                    pixel_offset = image + (x + width * y) * channels;
                    shifted_pixel_offset = image + (x + shift + width * y) * channels;
                    shifted_pixel_offset[0] = pixel_offset[0];
                    shifted_pixel_offset[1] = pixel_offset[1];
                    shifted_pixel_offset[2] = pixel_offset[2];
                }
            }
        for (int y = start_line; y < end_line; y++) {
            for (int x = 0; x < shift; x++) {
                pixel_offset = image + (x + width * y) * channels;
                pixel_offset[0] = 0;
                pixel_offset[1] = 0;
                pixel_offset[2] = 0;
            }
        }
        return;
    }
    for (int y = start_line; y < end_line; y++)
        for (int x = 0; x < width; x++) {
            if (x - shift >= 0) {
                pixel_offset = image + (x + width * y) * channels;
                shifted_pixel_offset = image + (x - shift + width * y) * channels;
                shifted_pixel_offset[0] = pixel_offset[0];
                shifted_pixel_offset[1] = pixel_offset[1];
                shifted_pixel_offset[2] = pixel_offset[2];
            }
        }
    for (int y = start_line; y < end_line; y++) {
        for (int x = width; x > width - shift; x--) {
            pixel_offset = image + (x + width * y) * channels;
            pixel_offset[0] = 0;
            pixel_offset[1] = 0;
            pixel_offset[2] = 0;
        }
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