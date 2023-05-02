#include "bot.h"
#include <filesystem>

using std::string, std::cout, std::vector;

int main(int argc, char **argv)
{
    string token = "5946455553:AAFsKjYVA7VgSkC5cbNelHeABLZRZn5ff_A";
    tgbot::start_bot(token);
    return 0;
}