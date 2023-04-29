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

string Image::save_image() {
    string new_filename = generate_new_filename();
    if (extension == ".png")
        stbi_write_png(new_filename.c_str(), width, height, channels, image, width * channels);
    else if (extension == ".jpg" or extension == ".jpeg") {
        stbi_write_jpg(new_filename.c_str(), width, height, channels, image, width * channels);
    }
    return new_filename;
}

void Image::stripe_image(unsigned int stripes) {
    unsigned char *pixelOffset;

    if (stripes & VERTICAL_STRIPES)
        for (unsigned int i = 0; i < width; i++)
            if (i % 4 == 0 or i % 4 == 1) {
                for (unsigned int j = 0; j < height; j++) {
                    pixelOffset = image + (i + width * j) * channels;

                    pixelOffset[0] = 0;
                    pixelOffset[1] = 0;
                    pixelOffset[2] = 0;
                }
            }
    if (stripes & HORIZONTAL_STRIPES)
        for (unsigned int j = 0; j < height; j++)
            if (j % 4 == 0 or j % 4 == 1)
                for (unsigned int i = 0; i < width; i++) {
                    pixelOffset = image + (i + width * j) * channels;
                    pixelOffset[0] = 0;
                    pixelOffset[1] = 0;
                    pixelOffset[2] = 0;
                }
}

void Image::vintage_image() {
    unsigned char *pixelOffset;
    for (unsigned int x = 0; x < width; x++)
        for (unsigned int y = 0; y < height; y++) {
            pixelOffset = image + (x + width * y) * channels;
            pixelOffset[0] = (int)(pixelOffset[0] * 0.608);
            pixelOffset[1] = (int)(pixelOffset[1] * 0.335);
            pixelOffset[2] = (int)(pixelOffset[2] * 0.199);
        }
}

void Image::pixelize_image(int pixel_size) {
    for (int y = 0; y < width; y += pixel_size)
        for (int x = 0; x < height; x += pixel_size)
            set_pixel(x, y, pixel_size);
}

string Image::generate_new_filename() {
    string new_filename;
    int incr = 0;
    do {
        incr++;
        new_filename = dir + filename + std::to_string(incr) + extension;
    } while (std::filesystem::exists(new_filename));
    return new_filename;
}

string Image::generate_new_filename(string dir) {
    string letters = "zxcvbnmasdfghjklqwertyuiop1234567890";
    
    do {
        string random_string = "";
        std::srand(std::time(nullptr));
        int range = 7 + std::rand() % 11;
        for (int i = 0; i < range; i++)
            random_string += letters[std::rand() % 37];
        filename = dir + random_string + extension;
    } while (std::filesystem::exists(filename));

    return filename;
}

void Image::set_pixel(int x_pos, int y_pos, int pixel_size) {
    int r = 0, g = 0, b = 0;
    unsigned char *pixelOffset;
    int pixel_square = pixel_size * pixel_size;
    int range_x = x_pos + pixel_size, range_y = y_pos + pixel_size;

    for (int x = x_pos; x < range_x; x++)
        for (int y = y_pos; y < range_y; y++) {
            pixelOffset = image + (y + width * x) * channels;
            r += (int)pixelOffset[0];
            g += (int)pixelOffset[1];
            b += (int)pixelOffset[2];
        }
    r = r / pixel_square;
    g = g / pixel_square;
    b = b / pixel_square;
    for (int x = x_pos; x < range_x; x++)
        for (int y = y_pos; y < range_y; y++) {
            pixelOffset = image + (y + width * x) * channels;
            pixelOffset[0] = r;
            pixelOffset[1] = g;
            pixelOffset[2] = b;
        }
}