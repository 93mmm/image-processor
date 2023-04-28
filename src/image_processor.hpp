#pragma once

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#define HORIZONTAL_STRIPES 1
#define VERTICAL_STRIPES 2

using std::cout, std::string, std::vector;

struct Image
{
    unsigned char *image;
    int width;
    int height;
    int channels;
};

struct RGB
{
    int r, g, b;
};

namespace subprocesses
{
    void save_file(string filename, Image img)
    {
        string extension = filename.substr(filename.find_last_of("."), filename.length());
        if (extension == ".png")
            stbi_write_png(filename.c_str(), img.width, img.height, img.channels, img.image, img.width * img.channels);
        else if (extension == ".jpg" or extension == ".jpeg")
            stbi_write_jpg(filename.c_str(), img.width, img.height, img.channels, img.image, img.width * img.channels);
    }

    void set_pixel(int x_pos, int y_pos, int pixel_size, Image img)
    {
        RGB pallete;
        unsigned char *pixelOffset;
        int sum_r = 0, sum_g = 0, sum_b = 0;
        int pixel_square = pixel_size * pixel_size;
        int range_x = x_pos + pixel_size, range_y = y_pos + pixel_size;

        for (int x = x_pos; x < range_x; x++)
            for (int y = y_pos; y < range_y; y++)
            {
                pixelOffset = img.image + (y + img.width * x) * img.channels;
                sum_r += (int)pixelOffset[0];
                sum_g += (int)pixelOffset[1];
                sum_b += (int)pixelOffset[2];
            }
        
        pallete.r = sum_r / pixel_square;
        pallete.g = sum_g / pixel_square;
        pallete.b = sum_b / pixel_square;

        for (int x = x_pos; x < range_x; x++)
            for (int y = y_pos; y < range_y; y++)
            {
                pixelOffset = img.image + (y + img.width * x) * img.channels;
                pixelOffset[0] = pallete.r;
                pixelOffset[1] = pallete.g;
                pixelOffset[2] = pallete.b;
            }
    }
}

namespace image_processor
{
    bool stripe_image(string filename, string new_filename, unsigned int stripes)
    {
        Image img;
        unsigned char *pixelOffset;

        img.image = stbi_load(filename.c_str(), &img.width, &img.height, &img.channels, 0);
        if (img.image == NULL)
            return 0;

        if (stripes & VERTICAL_STRIPES)
            for (unsigned int i = 0; i < img.width; i++)
                if (i % 4 == 0 or i % 4 == 1)
                    for (unsigned int j = 0; j < img.height; j++)
                    {
                        pixelOffset = img.image + (i + img.width * j) * img.channels;
                        pixelOffset[0] = 0;
                        pixelOffset[1] = 0;
                        pixelOffset[2] = 0;
                    }

        if (stripes & HORIZONTAL_STRIPES)
            for (unsigned int j = 0; j < img.height; j++)
                if (j % 4 == 0 or j % 4 == 1)
                    for (unsigned int i = 0; i < img.width; i++)
                    {
                        pixelOffset = img.image + (i + img.width * j) * img.channels;
                        pixelOffset[0] = 0;
                        pixelOffset[1] = 0;
                        pixelOffset[2] = 0;
                    }
        subprocesses::save_file(new_filename, img);
        stbi_image_free(img.image);
        return 1;
    }

    bool vintage_image(string filename, string new_filename)
    {
        Image img;
        unsigned char *pixelOffset;
        img.image = stbi_load(filename.c_str(), &img.width, &img.height, &img.channels, 0);

        if (img.image == NULL)
            return 0;

        for (unsigned int x = 0; x < img.width; x++)
            for (unsigned int y = 0; y < img.height; y++)
            {
                pixelOffset = img.image + (x + img.width * y) * img.channels;
                pixelOffset[0] = (int)(pixelOffset[0] * 0.608);
                pixelOffset[1] = (int)(pixelOffset[1] * 0.335);
                pixelOffset[2] = (int)(pixelOffset[2] * 0.199);
            }

        subprocesses::save_file(new_filename, img);
        stbi_image_free(img.image);
        return 1;
    }

    bool pixelize_image(string filename, string new_filename, int pixel_size)
    {
        Image img;
        unsigned char *pixelOffset;

        img.image = stbi_load(filename.c_str(), &img.width, &img.height, &img.channels, 0);

        if (img.image == NULL)
            return 0;

        for (int y = 0; y < img.width; y += pixel_size)
            for (int x = 0; x < img.height; x += pixel_size)
                subprocesses::set_pixel(x, y, pixel_size, img);
        subprocesses::save_file(new_filename, img);
        stbi_image_free(img.image);
        return 1;
    }
}