#pragma once

#include <iostream>
#include <string>

#define HORIZONTAL_STRIPES 1
#define VERTICAL_STRIPES 2


class Image {
public:
    Image(std::string fn);
    ~Image();

    std::string save_image();

    void stripe_image(unsigned int stripes);
    void vintage_image();
    void pixelize_image(int pixel_size);
    
private:
    std::string generate_new_filename();
    std::string generate_new_filename(std::string dir);
    void set_pixel(int x_pos, int y_pos, int pixel_size);
    
    std::string dir,
                filename,
                extension;

    unsigned char *image;
    int width = 0, height, channels;
    
    struct RGB {
        int r, g, b;
    };
};
