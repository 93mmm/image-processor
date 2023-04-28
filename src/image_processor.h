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

private:
    std::string generate_new_filename();
    std::string generate_new_filename(string dir);
    
    std::string filename,
                extension;

    unsigned char *image;
    int width = 0, height, channels;
    
    struct RGB {
        int r, g, b;
    };
};
