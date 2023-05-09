#include "bot.h"
#include "image_processor.h"

#include <exception>
#include <fstream>
#include <chrono>
#include <thread>

#define STRIPE_IMAGE_SIGNAL "si"
#define PIXELIZE_IMAGE_SIGNAL "pi"
#define BOTH_IMAGE_SIGNAL "both"
#define VINTAGE_IMAGE_SIGNAL "vi"
#define GLITCH_IMAGE_SIGNAL "gi"

#define VERTICAL_LINE_SIGNAL "ver_line"
#define HORIZONTAL_LINE_SIGNAL "hor_line"

#define DIR_FOR_RECIEVED_FILES "docs"
#define DIR_FOR_TEMPORARY_FILES "tmp"

using std::string, std::ofstream, std::vector, std::unordered_map;
using namespace TgBot;

void tgbot::start_bot(string token) {
    Bot bot(token);

    Args args{bot};
    args.main_keyboard = get_main_keyboard();
    args.si_keyboard = get_si_keyboard();
    
    setup_reactions_for_messages(args);
    bot.getApi().setMyCommands(setup_commands(args));
    setup_callback_query(args);
    start_longpoll(args);
}

void tgbot::start_longpoll(Args &args) {
    while (true) {    
        try {
            printf("Bot username: %s\n", args.bot.getApi().getMe()->username.c_str());
            printf("Longpoll started");
            args.bot.getApi().deleteWebhook();
            TgLongPoll longPoll(args.bot);
            while (true)
                longPoll.start();
        } catch (std::exception& e) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            printf("error: %s\n", e.what());
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            printf("unexpected error");
        }
    }
}

vector<BotCommand::Ptr> tgbot::setup_commands(tgbot::Args &args) {
    // /start
    args.bot.getEvents().onCommand("start", [&args](Message::Ptr message) {
        args.bot.getApi().sendMessage(message->chat->id, "Hello! This bot can process your image!");
        args.bot.getApi().sendMessage(message->chat->id, "Send image as file to me and follow instructions to convert your image");
    });

    // /what
    args.bot.getEvents().onCommand("what", [&args](Message::Ptr message) {
        args.bot.getApi().sendMessage(message->chat->id, "Send image as file to me and follow instructions to convert your image");
    });

    vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray(new BotCommand);
    cmdArray->command = "what";
    cmdArray->description = "get info about bot";
    commands.push_back(cmdArray);

    cmdArray = BotCommand::Ptr(new BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "launch the bot";
    commands.push_back(cmdArray);
    
    return commands;
}

void tgbot::setup_callback_query(Args &args) {
    args.bot.getEvents().onCallbackQuery([&args](CallbackQuery::Ptr query) {
        bool convert_image = (StringTools::startsWith(query->data, GLITCH_IMAGE_SIGNAL) ||
                             StringTools::startsWith(query->data, BOTH_IMAGE_SIGNAL) ||
                             StringTools::startsWith(query->data, PIXELIZE_IMAGE_SIGNAL) ||
                             StringTools::startsWith(query->data, VINTAGE_IMAGE_SIGNAL) ||
                             StringTools::startsWith(query->data, VERTICAL_LINE_SIGNAL) ||
                             StringTools::startsWith(query->data, HORIZONTAL_LINE_SIGNAL)
                             );
        if (convert_image) {
            string fn = args.image_to_edit[query->message->chat->id];
            if (fn == "") {
                args.bot.getApi().sendMessage(query->message->chat->id, "an error occurred");
                return;
            }
            delete_message(args, query);
            try {
                string tmp_fn = process_photo(query->data, fn);
                args.bot.getApi().sendMessage(query->message->chat->id, "here is your image:");
                args.bot.getApi().sendDocument(query->message->chat->id, InputFile::fromFile(tmp_fn, ""));
                args.image_to_edit.erase(query->message->chat->id);
                return;
            } catch (const char *error) {
                args.bot.getApi().sendMessage(query->message->chat->id, error);
            }
        } else if (StringTools::startsWith(query->data, STRIPE_IMAGE_SIGNAL)) {
            string response = "stripe image";
            delete_message(args, query);
            args.message_to_delete[query->message->chat->id] = args.bot.getApi().sendMessage(query->message->chat->id, response, false, 0, args.si_keyboard)->messageId;
            return;
        }
    });
}

void tgbot::setup_reactions_for_messages(Args &args) {
    args.bot.getEvents().onAnyMessage([&args](Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/"))
            return;
        else if (message->document != nullptr) {
            args.message_to_delete[message->chat->id] = args.bot.getApi().sendMessage(message->chat->id, "What I will do?", false, 0, args.main_keyboard)->messageId;
            download_file(args, message);
        } else
            args.bot.getApi().sendMessage(message->chat->id, "Type \"/what\" to see what can I do");
    });
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

    vector<InlineKeyboardButton::Ptr> glitch_image;
    InlineKeyboardButton::Ptr glitch_image_btn(new InlineKeyboardButton);
    glitch_image_btn->text = "glitch image";
    glitch_image_btn->callbackData = GLITCH_IMAGE_SIGNAL;
    glitch_image.push_back(glitch_image_btn);
    keyboard->inlineKeyboard.push_back(glitch_image);
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

    vector<InlineKeyboardButton::Ptr> both;
    InlineKeyboardButton::Ptr both_btn(new InlineKeyboardButton);
    both_btn->text = "both";
    both_btn->callbackData = BOTH_IMAGE_SIGNAL;
    both.push_back(both_btn);
    keyboard->inlineKeyboard.push_back(both);

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
    string tmp_fn = get_unique_filename_in_dir(DIR_FOR_TEMPORARY_FILES, fn.substr(fn.find_last_of(".") + 1, fn.length()));
    try {
        Image img = Image(fn);
        if (todo == VINTAGE_IMAGE_SIGNAL)
            img.vintage_image();
        else if (todo == VERTICAL_LINE_SIGNAL)
            img.stripe_image(VERTICAL_STRIPES);
        else if (todo == BOTH_IMAGE_SIGNAL)
            img.stripe_image(VERTICAL_STRIPES | HORIZONTAL_STRIPES);
        else if (todo == HORIZONTAL_LINE_SIGNAL)
            img.stripe_image(HORIZONTAL_STRIPES);
        else if (todo == PIXELIZE_IMAGE_SIGNAL)
            img.pixelize_image(10);
        else if (todo == GLITCH_IMAGE_SIGNAL)
            img.glitch_image();
        img.save_image(tmp_fn);
        return tmp_fn;
    } catch (std::exception &e) {
        throw e.what();
    }
}

void tgbot::delete_message(Args &args, CallbackQuery::Ptr query) {
    if (args.message_to_delete[query->message->chat->id] != 0) {
        args.bot.getApi().deleteMessage(query->message->chat->id, args.message_to_delete[query->message->chat->id]);
        args.message_to_delete.erase(query->message->chat->id);
    }
}

void tgbot::download_file(Args &args, Message::Ptr message) {
    args.image_to_edit[message->chat->id] = get_unique_filename_in_dir(DIR_FOR_RECIEVED_FILES, message->document->fileName.substr(message->document->fileName.find_last_of(".") + 1, message->document->fileName.length()));
    File::Ptr file = args.bot.getApi().getFile(message->document->fileId);
    ofstream myfile;
    myfile.open(args.image_to_edit[message->chat->id]);
    myfile << args.bot.getApi().downloadFile(file->filePath);
    myfile.close();
}