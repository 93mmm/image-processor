#include "image_processor.h"
#include <filesystem>

using std::string, std::cout;

int main(int argc, char **argv)
{
    try {
        Image img = Image("folder/photo.jpeg");
        img.pixelize_image(100);
        string place = img.save_image();
        cout << place << "\n";
    } catch (const char *exit) {
        cout << exit << "\n";
    }
}