#pragma once

#include <iostream>
#include <string>

#define HORIZONTAL_STRIPES 1
#define VERTICAL_STRIPES 2


class Image {
public:
    Image(std::string fn);
    ~Image();

    void save_image(std::string save_as);

    void stripe_image(unsigned int stripes);
    void vintage_image();
    void pixelize_image(int pixel_size);
    
private:
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
