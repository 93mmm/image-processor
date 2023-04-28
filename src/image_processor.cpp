#include <filesystem>
#include <cassert>

#include "image_processor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

using std::string;

Image::Image(std::string fn) {
    filename = fn;
    extension = filename.substr(filename.find_last_of("."), filename.length());
    image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (image != NULL)
        throw "Error while loading image";
}

Image::~Image() {
    stbi_image_free(image);
}

string Image::save_image() {
    string new_filename = generate_new_filename();
    if (extension == ".png")
        stbi_write_png(filename.c_str(), width, height, channels, image, width * channels);
    else if (extension == ".jpg" or extension == ".jpeg") {
        stbi_write_jpg(filename.c_str(), width, height, channels, image, width * channels);
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

string Image::generate_new_filename() {
    string letters = "zxcvbnmasdfghjklqwertyuiop1234567890";
    string dir = filename.substr(0, filename.find_last_of("/") + 1);
    
    do
    {
        string random_string = "";
        std::srand(std::time(nullptr));
        int range = 7 + std::rand() % 11;
        for (int i = 0; i < range; i++)
            random_string += letters[std::rand() % 37];
        filename = dir + random_string + extension;
    } while (std::filesystem::exists(filename));

    return filename;
}

string Image::generate_new_filename(string dir) {
    string letters = "zxcvbnmasdfghjklqwertyuiop1234567890";
    
    do
    {
        string random_string = "";
        std::srand(std::time(nullptr));
        int range = 7 + std::rand() % 11;
        for (int i = 0; i < range; i++)
            random_string += letters[std::rand() % 37];
        filename = dir + random_string + extension;
    } while (std::filesystem::exists(filename));

    return filename;
}