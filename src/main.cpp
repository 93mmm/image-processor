#include "bot.h"
#include <filesystem>

using std::string, std::cout;

int main(int argc, char **argv)
{
    string token = "5946455553:AAFsKjYVA7VgSkC5cbNelHeABLZRZn5ff_A";
    TgBot::Bot bot = tgbot::init_bot(token);
    tgbot::start_polling(bot);
}