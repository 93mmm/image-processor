#pragma once

#include <iostream>
#include <string>
#include <tgbot/tgbot.h>

namespace tgbot {
    TgBot::Bot init_bot(std::string token);
    void init_events(TgBot::Bot &bot);
    int start_polling(TgBot::Bot &bot);
    void get_convert_and_send_photo(TgBot::Bot &bot, TgBot::Message::Ptr message);
}