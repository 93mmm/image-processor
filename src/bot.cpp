#include "bot.h"
#include "image_processor.h"

#include <fstream>

using std::string, std::ofstream;

TgBot::Bot tgbot::init_bot(string token) {
    TgBot::Bot bot(token);
    init_events(bot);
    return bot;
}

void tgbot::init_events(TgBot::Bot &bot) {
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hello! This bot can process your image!");
        bot.getApi().sendMessage(message->chat->id, "Send photo as file to me and I will give you an answer with processed photo");
    });

    bot.getEvents().onCommand("stop", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Bye-bye!");
    });

    bot.getEvents().onCommand("what", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Send me an image and I will give you an answer with processed photo");
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/start"))
            return;
        else if (StringTools::startsWith(message->text, "/stop"))
            return;
        else if (StringTools::startsWith(message->text, "/what"))
            return;
        if (message->document != nullptr)
            get_convert_and_send_photo(bot, message);
        else
            bot.getApi().sendMessage(message->chat->id, "Type /what to see what can I do");
    });
}

int tgbot::start_polling(TgBot::Bot &bot) {
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true)
            longPoll.start();

    } catch (TgBot::TgException &e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}

void tgbot::get_convert_and_send_photo(TgBot::Bot &bot, TgBot::Message::Ptr message) {
    TgBot::File::Ptr file = bot.getApi().getFile(message->document->fileId);
    std::cout << file->filePath << "\n";
    string filename = file->filePath;
    ofstream myfile;
    myfile.open(filename);
    myfile << bot.getApi().downloadFile(filename);
    myfile.close();
    string temportary_filename = "tmp_" + filename;
        std::cout << temportary_filename << "\n";
    try {
        Image img = Image(filename);
        img.pixelize_image(10);
        std::cout << "aa";
        img.save_image(temportary_filename);
    } catch (std::exception &e) {
        std::cout << e.what() << "\n";
    }
    bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(temportary_filename, ""));
}