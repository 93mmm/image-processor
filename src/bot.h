#pragma once

#include <iostream>
#include <string>
#include <tgbot/tgbot.h>

namespace tgbot {
    void start_bot(std::string token);
    void start_longpoll(TgBot::Bot &bot);
    TgBot::InlineKeyboardMarkup::Ptr get_main_keyboard();
    TgBot::InlineKeyboardMarkup::Ptr get_si_keyboard();
    std::string get_unique_filename_in_dir(std::string directory, std::string extension);

    std::string process_photo(std::string todo, std::string fn);
    void download_file(TgBot::Bot &bot, TgBot::Message::Ptr message, std::unordered_map<int, std::string> &mp);
}