#include "image_processor.hpp"
#include <filesystem>

void stripe_folder(string folder)
{
    vector<string> paths;
    for (const auto &entry : std::filesystem::directory_iterator(folder))
        paths.push_back(entry.path().string());
    for (string element : paths)
    {
        image_processor::stripe_image(element, "striped/img_strpd.png", HORIZONTAL_STRIPES | VERTICAL_STRIPES);
    }
}

int main(int argc, char **argv)
{
    cout << image_processor::pixelize_image("folder/photo.jpeg", "folder/photo1.png", 7);
    cout << image_processor::stripe_image("folder/photo.jpeg", "folder/photo2.jpeg", HORIZONTAL_STRIPES);
    cout << image_processor::vintage_image("folder/photo.jpeg", "folder/photo3.png");
}