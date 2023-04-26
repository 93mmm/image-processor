#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

using std::cout, std::string, std::vector;

namespace image_processor
{
    string get_filename(string &filename, string folder, string info)
    {
        string extension = filename.substr(filename.find_last_of("."), filename.length());
        filename = filename.substr(0, filename.find_last_of("."));
        filename = filename.substr(filename.find_first_of("/"), filename.length());
        filename = folder + filename + info + extension;
        return extension;
    }

    bool save_file(string filename, string exstension, int width, int height, int channels, unsigned char *image, int quality)
    {
        if (exstension == ".jpg")
            stbi_write_jpg(filename.c_str(), width, height, channels, image, quality);
        else if (exstension == ".png")
            stbi_write_png(filename.c_str(), width, height, channels, image, quality);
        return 1;
    }

    bool stripe_image(string filename, bool vertical_lines, bool horizontal_lines)
    {
        int width, height, channels;
        string folder_where_save = "striped";
        string info_about_operations = "";
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &channels, 0);

        if (image == NULL)
            return 0;
        std::filesystem::create_directory("striped");

        unsigned char *pixelOffset;
        if (vertical_lines)
        {
            info_about_operations += "v_ln";
            for (unsigned int i = 0; i < width; i++)
            {
                if (i % 4 == 0 or i % 4 == 1)
                    for (unsigned int j = 0; j < height; j++)
                    {
                        pixelOffset = image + (i + width * j) * channels;
                        pixelOffset[0] = 0;
                        pixelOffset[1] = 0;
                        pixelOffset[2] = 0;
                    }
            }
        }
        if (horizontal_lines)
        {
            info_about_operations += "hz_ln";
            for (unsigned int j = 0; j < height; j++)
            {
                if (j % 4 == 0 or j % 4 == 1)
                    for (unsigned int i = 0; i < width; i++)
                    {
                        pixelOffset = image + (i + width * j) * channels;
                        pixelOffset[0] = 0;
                        pixelOffset[1] = 0;
                        pixelOffset[2] = 0;
                    }
            }
        }
        string extension = get_filename(filename, folder_where_save, info_about_operations);

        return save_file(filename, extension, width, height, channels, image, 10000);
    }

    void stripe_folder(string folder)
    {
        vector<string> paths;
        for (const auto &entry : std::filesystem::directory_iterator(folder))
            paths.push_back(entry.path().string());
        for (string element : paths)
        {
            stripe_image(element, 1, 1);
            stripe_image(element, 0, 1);
            stripe_image(element, 1, 0);
        }
    }
}