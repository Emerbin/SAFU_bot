#include "cBot.h"

cBot::cBot(string token) : token(token)
{
    //Setting console encoding so it will correctly print info
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Bot bot(token);
    printf("Token: %s\n", token.c_str());

    for (auto& c : cBot::setupCommands())
    {
        /*
        Each time cheking if any command is called
        If c.first is our command then we call the following function
        */
        auto func = std::any_cast<std::_Binder <std::_Unforced, void (cBot::*)(TgBot::Bot&, Message::Ptr), cBot*, _Ph<1> const&, _Ph<2> const&>>(c.second);
        bot.getEvents().onCommand(c.first, [&bot, func](Message::Ptr message) 
            {
                printf("User: %s, called command: %s\n", message->from->username.c_str(), message->text.c_str());
                func(bot, message);
            });
    }

    bot.getEvents().onCallbackQuery([this,&bot](CallbackQuery::Ptr query)
    {
        for (auto& c : cBot::setupCallbacks())
        {
            /*
            On any CallbackQuery we are cheking if data of this query is located in our Callbacks
            If true we are calling the following function
            */
            if (c.first == query->data)
            {
                auto func = std::any_cast<std::_Binder < std::_Unforced, void (cBot::*)(TgBot::Bot&, CallbackQuery::Ptr), cBot*, _Ph<1> const&, _Ph<2> const&>>(c.second);
                printf("User: %s, called query:%s \n", query->message->from->username.c_str(), query->message->text.c_str());
                func(bot, query);
                return;
            }
        }

        bot.getApi().sendMessage(query->message->chat->id, "Unknown control");
    });

    //Creating keyboards for future use
    createOneTimeKeyboard({ {"Personal \xF0\x9F\x94\x92"},{"Team \xF0\x9F\x8F\xA2"} }, personalKeyboard);
    printf("Created reply keyboards\n");

    bot.getEvents().onAnyMessage([&bot, this](Message::Ptr message) 
        {
            thread loggerThread(&cBot::logger,this,message);
            loggerThread.detach();
            activeCommands[message->from->id] += message->text;
            for (auto& c : cBot::setupMessages())
            {
                if (StringTools::startsWith(activeCommands[message->from->id], c.first))
                {
                    auto func = std::any_cast<std::_Binder < std::_Unforced, void (cBot::*)(TgBot::Bot&, Message::Ptr), cBot*, _Ph<1> const&, _Ph<2> const&>>(c.second);
                    printf("User: %s, called messageCom:%s \n", message->from->username.c_str(), message->text.c_str());
                    func(bot, message);
                    return;
                }
            }
        });

    bot.getEvents().onUnknownCommand([&bot, this](Message::Ptr message)
        {
            activeBlockViewers[message->from->id].deleteBlock(bot);
            bot.getApi().sendMessage(message->chat->id, " You typed unknown command. \xF0\x9F\x9A\xAB \nSorry, I can't understand you. Try again");
        });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
        });

    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    try 
    {
        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e) 
    {
        printf("error: %s\n", e.what());
    }
}


/*
Method for accesing suitable function depending on Callback data
Callback data is key, function is value
*/
unordered_map<string, any> cBot::setupCallbacks() noexcept
{
    auto Callbacks = unordered_map<string, any>{};

    Callbacks["Next"] = bind(&cBot::nextBlock, this, placeholders::_1, placeholders::_2);
    Callbacks["Select"] = bind(&cBot::selectBlock, this, placeholders::_1, placeholders::_2);
    Callbacks["Previous"] = bind(&cBot::previousBlock, this, placeholders::_1, placeholders::_2);

    return Callbacks;
}


/*
Method for accesing suitable function depending on called Command
Command is key, function is value
*/
unordered_map<string, any> cBot::setupCommands() noexcept
{
    auto Commands = unordered_map<string, any>{};

    Commands["start"] = bind(&cBot::startCommand, this, placeholders::_1, placeholders::_2);
    Commands["create"] = bind(&cBot::createCommand, this, placeholders::_1, placeholders::_2);
    Commands["block"] = bind(&cBot::blockCommand, this, placeholders::_1, placeholders::_2);
    Commands["save"] = bind(&cBot::saveCommand, this, placeholders::_1, placeholders::_2);

    return Commands;
}

unordered_map<string, any> cBot::setupMessages() noexcept
{
    auto Messages = unordered_map<string, any>{};

    Messages["Select"] = bind(&cBot::select, this, placeholders::_1, placeholders::_2); // At all times it must stay at first place
    Messages["CreatePersonal"] = bind(&cBot::createPersonal, this, placeholders::_1, placeholders::_2);
    Messages["CreateTeam"] = bind(&cBot::askTeam, this, placeholders::_1, placeholders::_2);
    Messages["Create2Team"] = bind(&cBot::createTeam, this, placeholders::_1, placeholders::_2);
    Messages["ViewPersonal"]=  bind(&cBot::viewPersonal, this, placeholders::_1, placeholders::_2);
    Messages["ViewTeam"] = bind(&cBot::askTeam, this, placeholders::_1, placeholders::_2);
    Messages["View2Team"] = bind(&cBot::viewTeam, this, placeholders::_1, placeholders::_2);
    Messages["PersonalCaption"] = bind(&cBot::appendPersonalCaption, this, placeholders::_1, placeholders::_2);
    Messages["PersonalMessage"] = bind(&cBot::appendPersonalMessage, this, placeholders::_1, placeholders::_2);

    return Messages;
}

/*
Function for creating oneTimeReplykeyboard
Working using createKeyboard and then setting oneTimeKeyboard to true
*/
void cBot::createOneTimeKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb)
{
    createKeyboard(buttonLayout, kb);
    kb->oneTimeKeyboard = true;
}

/*
Function for creating replyKeyboard
*/
void cBot::createKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb)
{
    for (std::size_t i = 0; i < buttonLayout.size(); ++i)
    {
        std::vector<TgBot::KeyboardButton::Ptr> row;
        for (std::size_t j = 0; j < buttonLayout[i].size(); ++j) {
            TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
            button->text = buttonLayout[i][j];
            button->requestContact = false;
            button->requestLocation = false;
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
}

void cBot::logger(Message::Ptr message)
{
    if (StringTools::startsWith(message->text, "/") == false)
    {
        messageTypeParser type(message);
        vector<string> types = type.getMessageTypes();
        for (auto& c : types)
        {
            if (c == "text")
            {
                printf("User: %s, id = %lld, entered: %s\n", message->from->username.c_str(),message->from->id, message->text.c_str());
                return;
            }
            else
            {
                printf("User: %s, id = %lld, sended %s, fileid = %s\n", message->from->username.c_str(), message->from->id, c.c_str(), type.getMessageText().back().c_str()); // TODO: fileid of message if exist
                return;
            }
        }
    }
}

void cBot::startCommand(Bot& bot, Message::Ptr message)
{
    activeBlockViewers[message->from->id].deleteBlock(bot);
    bot.getApi().sendMessage(message->chat->id, "/create to create a new block \n/block to view blocks");
}

void cBot::createCommand(Bot& bot, Message::Ptr message)
{
    activeBlockViewers[message->from->id].deleteBlock(bot);
    bot.getApi().sendMessage(message->chat->id, "Choose what block you want to create:", false, 0, personalKeyboard);
    activeCommands[message->from->id] = "Create";
}

void cBot::blockCommand(Bot& bot, Message::Ptr message) // use unordered_map for block where userid is key and block is value so in case of existing of object it will be updated
{
    activeBlockViewers[message->from->id].deleteBlock(bot);
    //json testJson = R"({"array" : [{"type" : "text", "caption" : "caption", "text" : "some text"}, { "type" : "photo", "caption" : "caption","text" : "AgACAgIAAxkBAAIInmTbL1L44SYpat8S6ALFPbjdhXNPAALj0zEb0h3YSq4b3q520-CpAQADAgADcwADMAQ" }]})"_json;
    bot.getApi().sendMessage(message->chat->id, "Choose what blocks you want to view:", false, 0, personalKeyboard);
    activeCommands[message->from->id] = "View";
}

void cBot::saveCommand(Bot& bot, Message::Ptr message)
{
    blocks.insert(make_pair(message->from->id, activeBlockCreators[message->from->id].saveBlock()));
    bot.getApi().sendMessage(message->chat->id, "Enter /block to see your block");
    activeCommands[message->from->id] = "";
}

void cBot::nextBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[query->from->id].deleteBlock(bot);
    //Getting json with next number. Changing number in select query. Creating new blockView and replacing old. Sending base
    uint32_t blockNumber = activeBlockViewers[query->from->id].getBlockNumber() + 1;
    //json
    activeBlockViewers[query->from->id] = blockView(json::parse(blocks.at(query->from->id)), query->message, blockNumber);
    activeBlockViewers[query->from->id].sendBlock(bot);
}

void cBot::selectBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[query->from->id].deleteBlock(bot);
    bot.getApi().sendMessage(query->message->chat->id, "Enter number of block you want to select");
    activeCommands[query->from->id] = "Select";
    //TODO add number proccesing to anyMessage for this func
    //Getting json with selected number. Changing number in select query. Creating new blockView and replacing old. Sending base
}

void cBot::previousBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[query->from->id].deleteBlock(bot);
    //Getting json with previous number. Changing number in select query. Creating new blockView and replacing old. Sending base
    uint32_t blockNumber = activeBlockViewers[query->from->id].getBlockNumber() - 1;
    if (blockNumber < 1)
        return;
    //json
    activeBlockViewers[query->from->id] = blockView(json::parse(blocks.at(query->from->id)), query->message, blockNumber);
    activeBlockViewers[query->from->id].sendBlock(bot);
}

void cBot::select(Bot& bot, Message::Ptr message)
{
    //
    activeBlockViewers[message->from->id] = blockView(json::parse(blocks.at(message->from->id)), message, stoi(message->text));
    activeBlockViewers[message->from->id].sendBlock(bot);
}

void cBot::askTeam(Bot& bot, Message::Ptr message)
{
    //TODO: check if message->from->id is in team
    //      feature of adding people in team
    //      Separate team creating into class
    if (StringTools::startsWith(activeCommands[message->from->id], "ViewTeam"))
    {
        bot.getApi().sendMessage(message->chat->id, "Enter name of team you want to view");
        activeCommands[message->from->id] = "View2Team";
    }
    else
    {
        activeCommands[message->from->id] = "Create2Team";
        bot.getApi().sendMessage(message->chat->id, "Enter name of team under which you want to create block");
    }
}

void cBot::createTeam(Bot& bot, Message::Ptr message)
{
    
}

void cBot::createPersonal(Bot& bot, Message::Ptr message)
{
    activeCommands[message->from->id] = "PersonalCaption";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element");
}

void cBot::viewTeam(Bot& bot, Message::Ptr message)
{
    //getting json from db
    //TODO: Check if such team is existing
    activeCommands[message->from->id] = "";
    activeBlockViewers[message->from->id] = blockView(testJson, message, 1);
    activeBlockViewers[message->from->id].sendBlock(bot);
}

void cBot::viewPersonal(Bot& bot, Message::Ptr message)
{
    //Getting json from db
    activeCommands[message->from->id] = "";
    try
    {
        activeBlockViewers[message->from->id] = blockView(json::parse(blocks.at(message->from->id)), message, 1);
        activeBlockViewers[message->from->id].sendBlock(bot);
    }
    catch (exception& e)
    {
        printf("%s", e.what());
        return;
    }
}

void cBot::appendPersonalCaption(Bot& bot, Message::Ptr message)
{
    if (StringTools::startsWith(message->text, "/save"))
        return;
    activeBlockCreators[message->from->id].setCaption(message->text);
    activeCommands[message->from->id] = "PersonalMessage";
    bot.getApi().sendMessage(message->chat->id, "Enter element of block");
}

void cBot::appendPersonalMessage(Bot& bot, Message::Ptr message)
{
    if (StringTools::startsWith(message->text, "/save"))
        return;
    activeBlockCreators[message->from->id].setMessage(message);
    activeCommands[message->from->id] = "PersonalCaption";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element.\nTo save block enter /save");
}
