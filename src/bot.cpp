#include "bot.h"
#include "image_processor.h"

#include <exception>
#include <fstream>
#include <unordered_map>

#define STRIPE_IMAGE_SIGNAL "si"
#define PIXELIZE_IMAGE_SIGNAL "pi"
#define VINTAGE_IMAGE_SIGNAL "vi"
#define VERTICAL_LINE_SIGNAL "ver_line"
#define HORIZONTAL_LINE_SIGNAL "hor_line"

#define DIR_FOR_RECIEVED_FILES "docs"

using std::string, std::ofstream, std::vector, std::unordered_map, std::cout;
using namespace TgBot;

void tgbot::start_bot(string token) {
    Bot bot(token);

    unordered_map<int, string> waiting_for_reply;

    InlineKeyboardMarkup::Ptr main_keyboard = get_main_keyboard();
    InlineKeyboardMarkup::Ptr si_keyboard = get_si_keyboard();
    
    bot.getEvents().onCallbackQuery([&bot, &si_keyboard, &waiting_for_reply](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, STRIPE_IMAGE_SIGNAL)) {
            string response = "stripe image";
            bot.getApi().sendMessage(query->message->chat->id, response, false, 0, si_keyboard);
            return;
        }
        bool convert_image = (StringTools::startsWith(query->data, PIXELIZE_IMAGE_SIGNAL) || StringTools::startsWith(query->data, VINTAGE_IMAGE_SIGNAL) || StringTools::startsWith(query->data, VERTICAL_LINE_SIGNAL) || StringTools::startsWith(query->data, HORIZONTAL_LINE_SIGNAL));
        if (convert_image) {
            string fn = waiting_for_reply[query->message->chat->id];
            if (fn == "") {
                bot.getApi().sendMessage(query->message->chat->id, "an error occurred");
                return;
            }
            string tmp_fn = process_photo(query->data, fn);
            bot.getApi().sendMessage(query->message->chat->id, "here is your image:");
            bot.getApi().sendDocument(query->message->chat->id, InputFile::fromFile(tmp_fn, ""));
            waiting_for_reply.erase(query->message->chat->id);
            return;
        }
    });

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hello! This bot can process your image!");
        bot.getApi().sendMessage(message->chat->id, "Send image as file to me and follow instructions to convert your image");
    });

    bot.getEvents().onCommand("what", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Send image as file to me and follow instructions to convert your image");
    });

    bot.getEvents().onAnyMessage([&bot, &main_keyboard, &waiting_for_reply](Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/start"))
            return;
        else if (StringTools::startsWith(message->text, "/what"))
            return;
        else if (message->document != nullptr) {
            bot.getApi().sendMessage(message->chat->id, "What I will do?", false, 0, main_keyboard);
            download_file(bot, message, waiting_for_reply);
        }
        else
            bot.getApi().sendMessage(message->chat->id, "Type \"/what\" to see what can I do");
    });

    start_longpoll(bot);
}

void tgbot::start_longpoll(Bot &bot) {
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();
        TgLongPoll longPoll(bot);
        while (true)
            longPoll.start();
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}

InlineKeyboardMarkup::Ptr tgbot::get_main_keyboard() {
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);

    vector<InlineKeyboardButton::Ptr> stripe_image;
    InlineKeyboardButton::Ptr stripe_image_btn(new InlineKeyboardButton);
    stripe_image_btn->text = "stripe image";
    stripe_image_btn->callbackData = STRIPE_IMAGE_SIGNAL;
    stripe_image.push_back(stripe_image_btn);
    keyboard->inlineKeyboard.push_back(stripe_image);

    vector<InlineKeyboardButton::Ptr> pixelize_image;
    InlineKeyboardButton::Ptr pixelize_image_btn(new InlineKeyboardButton);
    pixelize_image_btn->text = "pixelize image";
    pixelize_image_btn->callbackData = PIXELIZE_IMAGE_SIGNAL;
    pixelize_image.push_back(pixelize_image_btn);
    keyboard->inlineKeyboard.push_back(pixelize_image);

    vector<InlineKeyboardButton::Ptr> vintage_image;
    InlineKeyboardButton::Ptr vintage_image_btn(new InlineKeyboardButton);
    vintage_image_btn->text = "vintage image";
    vintage_image_btn->callbackData = VINTAGE_IMAGE_SIGNAL;
    vintage_image.push_back(vintage_image_btn);
    keyboard->inlineKeyboard.push_back(vintage_image);
    return keyboard;
}

InlineKeyboardMarkup::Ptr tgbot::get_si_keyboard() {
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);

    vector<InlineKeyboardButton::Ptr> line1;
    InlineKeyboardButton::Ptr hor_line(new InlineKeyboardButton);

    hor_line->text = "horizontal lines";
    hor_line->callbackData = HORIZONTAL_LINE_SIGNAL;
    line1.push_back(hor_line);

    InlineKeyboardButton::Ptr ver_line(new InlineKeyboardButton);
    ver_line->text = "vertical lines";
    ver_line->callbackData = VERTICAL_LINE_SIGNAL;
    line1.push_back(ver_line);
    keyboard->inlineKeyboard.push_back(line1);

    return keyboard;
}

string tgbot::get_unique_filename_in_dir(string directory, string extension) {
    string filename;
    int i = 0;
    do {
        filename = directory + "/file" + std::to_string(i++) + "." + extension;
    } while (std::filesystem::exists(filename));
    return filename;
}

string tgbot::process_photo(string todo, string fn) {
    string tmp_fn = get_unique_filename_in_dir("tmp", fn.substr(fn.find_last_of(".") + 1, fn.length()));
    try {
        Image img = Image(fn);
        if (todo == VINTAGE_IMAGE_SIGNAL)
            img.vintage_image();
        else if (todo == VERTICAL_LINE_SIGNAL)
            img.stripe_image(VERTICAL_STRIPES);
        else if (todo == HORIZONTAL_LINE_SIGNAL)
            img.stripe_image(HORIZONTAL_STRIPES);
        else if (todo == PIXELIZE_IMAGE_SIGNAL)
            img.pixelize_image(10);
        img.save_image(tmp_fn);
    } catch (std::exception &e) {
        std::cout << e.what() << "\n";
    }
    return tmp_fn;
}

void tgbot::download_file(Bot &bot, Message::Ptr message, unordered_map<int, string> &mp) {
    mp[message->chat->id] = get_unique_filename_in_dir(DIR_FOR_RECIEVED_FILES, message->document->fileName.substr(message->document->fileName.find_last_of(".") + 1, message->document->fileName.length()));
    File::Ptr file = bot.getApi().getFile(message->document->fileId);
    ofstream myfile;
    myfile.open(mp[message->chat->id]);
    myfile << bot.getApi().downloadFile(file->filePath);
    myfile.close();
}