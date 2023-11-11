#include "pch.h"
#include "cBot.cpp"

//#include "Reminder.cpp"
//#include "HelperFunctions.h"

int main() {
    cBot("6085779942:AAHGc2LDchFWEOC4j6KWhp8qwffCg06DRW8");
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