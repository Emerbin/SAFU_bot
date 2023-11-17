#include "pch.h"
#include "cBot.cpp"

//#include "Reminder.cpp"
//#include "HelperFunctions.h"

int main() {
    std::wstring tokenenv = _wgetenv(L"TgToken");
    std::string token(tokenenv.begin(), tokenenv.end());
    cBot c(token);
    return 0;
/*

    bot.getEvents().onCommand("create", [&bot, &creatingBlock,&messagesToEdit, &personalKeyboard](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Choose what block you want to create:", false, 0, personalKeyboard);
            creatingBlock = true;
            return;
        });

    
   */
}
