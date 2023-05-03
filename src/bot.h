#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <tgbot/tgbot.h>

namespace tgbot {
    struct Args {
        TgBot::Bot &bot;
        TgBot::InlineKeyboardMarkup::Ptr main_keyboard;
        TgBot::InlineKeyboardMarkup::Ptr si_keyboard;
        std::unordered_map<int, std::string> image_to_edit;
        std::unordered_map<int, int> message_to_delete;
    };

    void start_bot(std::string token);
    void start_longpoll(Args &args);

    std::vector<TgBot::BotCommand::Ptr> setup_commands(Args &args);
    void setup_callback_query(Args &args);
    void setup_reactions_for_messages(Args &args);

    TgBot::InlineKeyboardMarkup::Ptr get_main_keyboard();
    TgBot::InlineKeyboardMarkup::Ptr get_si_keyboard();
    
    std::string get_unique_filename_in_dir(std::string directory, std::string extension);
    std::string process_photo(std::string todo, std::string fn);

    void delete_message(Args &args, TgBot::CallbackQuery::Ptr query);
    void download_file(Args &args, TgBot::Message::Ptr message);
}