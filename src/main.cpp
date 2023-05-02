#include "bot.h"
#include <fstream>

using std::string, std::cout;

int main(int argc, char **argv)
{
    std::ifstream file("token.txt");
    string token;
    if (!file.is_open())
        return EXIT_FAILURE;
    getline(file, token);
    file.close();
    tgbot::start_bot(token);
    return 0;
}