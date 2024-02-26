#include "cBot.h"

cBot::cBot(string token) : token(token)
{
    //Setting console encoding so it will correctly print info
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Bot bot(token);
    printf("[cBot]Token: %s\n", token.c_str());

    for (auto& c : cBot::setupCommands())
    {
        /*
        Each time cheking if any command is called
        If c.first is our command then we call the following function
        */
        auto func = std::any_cast<std::_Binder <std::_Unforced, void (cBot::*)(TgBot::Bot&, Message::Ptr), cBot*, _Ph<1> const&, _Ph<2> const&>>(c.second);
        bot.getEvents().onCommand(c.first, [&bot, func](Message::Ptr message) 
            {
                printf("[cBot]User: %s, called command: %s\n", message->from->username.c_str(), message->text.c_str());
                func(bot, message);
                return;
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
                printf("[cBot]User: %s, called query:%s \n", query->message->from->username.c_str(), query->message->text.c_str());
                func(bot, query);
                return;
            }
        }

        bot.getApi().sendMessage(query->message->chat->id, "Unknown control");
    });

    bot.getEvents().onAnyMessage([&bot, this](Message::Ptr message)
        {
            thread loggerThread(&cBot::logger, this, message);
            loggerThread.detach();
        });

    bot.getEvents().onNonCommandMessage([&bot, this](Message::Ptr message) 
        {
            activeCommands[to_string(message->from->id)] += message->text;
            for (auto& c : cBot::setupMessages())
            {
                if (StringTools::startsWith(activeCommands[to_string(message->from->id)], c.first))
                {
                    auto func = std::any_cast<std::_Binder < std::_Unforced, void (cBot::*)(TgBot::Bot&, Message::Ptr), cBot*, _Ph<1> const&, _Ph<2> const&>>(c.second);
                    printf("[cBot]User: %s, called messageCom:%s \n", message->from->username.c_str(), message->text.c_str());
                    func(bot, message);
                    return;
                }
            }
            bot.getApi().sendMessage(message->chat->id, "You have not entered any command");
        });

    //Creating keyboards for future use
    createOneTimeKeyboard({ {"Personal \xF0\x9F\x94\x92"},{"Team \xF0\x9F\x8F\xA2"} }, personalKeyboard);
    createUserKeyboard();
    printf("[cBot]Created reply keyboards\n");

    bot.getEvents().onUnknownCommand([&bot, this](Message::Ptr message)
        {
            activeBlockViewers[to_string(message->from->id)].deleteBlock(bot);
            bot.getApi().sendMessage(message->chat->id, " You typed unknown command. \xF0\x9F\x9A\xAB \nSorry, I can't understand you. Try again");
        });

    signal(SIGINT, [](int s) {
        printf("[cBot]SIGINT got\n");
        exit(0);
        });

    printf("[cBot]Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    try
    {
        bot.getApi().deleteWebhook(true);
        TgLongPoll longPoll(bot);
        while (true) {
            printf("[cBot]Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e) 
    {
        printf("[Exception][At cBot][TgException]: %s\n", e.what());
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
    Callbacks["Select"] = bind(static_cast<void(cBot::*)(Bot&, CallbackQuery::Ptr)>(&cBot::selectBlock), this, placeholders::_1, placeholders::_2);
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

    Messages["Select"] = bind(static_cast<void(cBot::*)(Bot&, Message::Ptr)>(&cBot::selectBlock), this, placeholders::_1, placeholders::_2); // At all times it must stay at first place
    Messages["CreatePersonal"] = bind(&cBot::createPersonalBlock, this, placeholders::_1, placeholders::_2);
    Messages["CreateTeam"] = bind(&cBot::askTeam, this, placeholders::_1, placeholders::_2);
    Messages["Create2Team"] = bind(&cBot::createTeam, this, placeholders::_1, placeholders::_2);
    Messages["AppendUserFinish"] = bind(&cBot::finishUserAppend, this, placeholders::_1, placeholders::_2);
    Messages["AppendUser"] = bind(&cBot::appendUser, this, placeholders::_1, placeholders::_2);
    Messages["ViewPersonal"]=  bind(&cBot::viewPersonalBlock, this, placeholders::_1, placeholders::_2);
    Messages["ViewTeam"] = bind(&cBot::askTeam, this, placeholders::_1, placeholders::_2);
    Messages["View2Block"] = bind(&cBot::viewTeamBlock, this, placeholders::_1, placeholders::_2);
    Messages["TeamCaption"] = bind(&cBot::appendTeamCaption, this, placeholders::_1, placeholders::_2);
    Messages["TeamMessage"] = bind(&cBot::appendTeamMessage, this, placeholders::_1, placeholders::_2);
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

void cBot::createUserKeyboard()
{
    KeyboardButton::Ptr button{ new TgBot::KeyboardButton };
    KeyboardButton::Ptr button1{ new TgBot::KeyboardButton };
    std::vector<TgBot::KeyboardButton::Ptr> row;
    std::vector<TgBot::KeyboardButton::Ptr> row1;
    button->text = "Add user";
    button->requestContact = false;
    button->requestLocation = false;
    KeyboardButtonRequestUser::Ptr req{ new KeyboardButtonRequestUser };
    req->userIsBot = false;
    req->userIsPremium = false;
    button->requestUser = req;
    row.push_back(button);
    askUser->keyboard.push_back(row);
    button1->text = "Finish";
    button1->requestContact = false;
    button1->requestLocation = false;
    row1.push_back(button1);
    askUser->keyboard.push_back(row1);
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
                printf("[cBot]User: %s, id = %lld, entered: %s\n", message->from->username.c_str(),message->from->id, message->text.c_str());
                return;
            }
            else
            {
                printf("[cBot]User: %s, id = %lld, sended %s, fileid = %s\n", message->from->username.c_str(), message->from->id, c.c_str(), type.getMessageText().back().c_str()); // TODO: fileid of message if exist
                return;
            }
        }
    }
}

void cBot::startCommand(Bot& bot, Message::Ptr message)
{
    activeBlockViewers[to_string(message->from->id)].deleteBlock(bot);
    bot.getApi().sendMessage(message->chat->id, "/create to create a new block \n/block to view blocks");
}

void cBot::createCommand(Bot& bot, Message::Ptr message)
{
    activeBlockViewers[to_string(message->from->id)].deleteBlock(bot);
    bot.getApi().sendMessage(message->chat->id, "Choose what block you want to create:", false, 0, personalKeyboard);
    activeCommands[to_string(message->from->id)] = "Create";
}

void cBot::blockCommand(Bot& bot, Message::Ptr message)
{
    activeBlockViewers[to_string(message->from->id)].deleteBlock(bot);
    bot.getApi().sendMessage(message->chat->id, "Choose what blocks you want to view:", false, 0, personalKeyboard);
    activeCommands[to_string(message->from->id)] = "View";
}

void cBot::saveCommand(Bot& bot, Message::Ptr message)
{
    activeBlockCreators[to_string(message->from->id)].processMessages();
    bot.getApi().sendMessage(message->chat->id, "Enter /block to see your block");
    if (StringTools::startsWith(activeCommands[to_string(message->from->id)], "Team"))
        activeTeam[to_string(message->from->id)].setJson(activeBlockCreators[to_string(message->from->id)].getJson());
    else
        blocks[to_string(message->from->id)] = activeBlockCreators[to_string(message->from->id)].saveBlock();
    activeCommands[to_string(message->from->id)] = "";
}

void cBot::nextBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[to_string(query->from->id)].deleteBlock(bot);
    //Getting json with next number. Changing number in select query. Creating new blockView and replacing old. Sending base
    uint32_t blockNumber = activeBlockViewers[to_string(query->from->id)].getBlockNumber() + 1;
    //json
    activeBlockViewers[to_string(query->from->id)] = blockView(json::parse(blocks.at(to_string(query->from->id))), query->message, blockNumber);
    activeBlockViewers[to_string(query->from->id)].sendBlock(bot);
}

void cBot::selectBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[to_string(query->from->id)].deleteBlock(bot);
    bot.getApi().sendMessage(query->message->chat->id, "Enter number of block you want to select");
    activeCommands[to_string(query->from->id)] += "Select";
    //Getting json with selected number. Changing number in select query. Creating new blockView and replacing old. Sending base
}

void cBot::previousBlock(Bot& bot, CallbackQuery::Ptr query)
{
    activeBlockViewers[to_string(query->from->id)].deleteBlock(bot);
    //Getting json with previous number. Changing number in select query. Creating new blockView and replacing old. Sending base
    uint32_t blockNumber = activeBlockViewers[to_string(query->from->id)].getBlockNumber() - 1;
    if (blockNumber < 1)
        return;
    //json
    activeBlockViewers[to_string(query->from->id)] = blockView(json::parse(blocks.at(to_string(query->from->id))), query->message, blockNumber);
    activeBlockViewers[to_string(query->from->id)].sendBlock(bot);
}

void cBot::selectBlock(Bot& bot, Message::Ptr message)
{
    try
    {
        if (StringTools::startsWith(activeCommands[to_string(message->from->id)], "Team"))
            activeBlockViewers[to_string(message->from->id)] = blockView(activeTeam[to_string(message->from->id)].getJson(), message, stoi(message->text));
        else
            activeBlockViewers[to_string(message->from->id)] = blockView(json::parse(blocks.at(to_string(message->from->id))), message, stoi(message->text));
        activeBlockViewers[to_string(message->from->id)].sendBlock(bot);
    }
    catch (exception& e)
    {
        printf("[Exception][At cBot]%s\n", e.what());
        return;
    }
    activeCommands[to_string(message->from->id)] = "";
}

void cBot::askTeam(Bot& bot, Message::Ptr message)
{
    //TODO: check if message->from->id is in team
    //      feature of adding people in team
    //      Separate team creating into class
    if (StringTools::startsWith(activeCommands[to_string(message->from->id)], "ViewTeam"))
    {
        bot.getApi().sendMessage(message->chat->id, "Enter name of team you want to view");
        activeCommands[to_string(message->from->id)] = "View2Block";
    }
    else
    {
        bot.getApi().sendMessage(message->chat->id, "Enter name of team under which you want to create block");
        activeCommands[to_string(message->from->id)] = "Create2Team";
    }
}

void cBot::createTeam(Bot& bot, Message::Ptr message)
{
    activeCommands[to_string(message->from->id)] = "AppendUser";
    activeTeam[to_string(message->from->id)] = team(message->text);
    activeTeam[to_string(message->from->id)].appendUser(message->from->id);
    bot.getApi().sendMessage(message->chat->id, "Select user to add from list",false,0,askUser);
}

void cBot::appendUser(Bot& bot, Message::Ptr message)
{
    activeCommands[to_string(message->from->id)] = "AppendUser";
    if (message->userShared == nullptr)
    {
        bot.getApi().sendMessage(message->chat->id, "Please enter user", false, 0, askUser);
        return;
    }
    activeTeam[to_string(message->from->id)].appendUser(message->userShared->userId);
    bot.getApi().sendMessage(message->chat->id, "Select another user to add from list\nTo end press button finish", false, 0, askUser);
}

void cBot::finishUserAppend(Bot& bot, Message::Ptr message)
{
    activeCommands[to_string(message->from->id)] = "CreateTeamBlock";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element");
    activeCommands[to_string(message->from->id)] = "TeamCaption";
}

void cBot::appendTeamCaption(Bot& bot, Message::Ptr message)
{
    /*if (StringTools::startsWith(message->text, "/save"))
        return;*/
    activeBlockCreators[to_string(message->from->id)].setCaption(message->text);
    activeCommands[to_string(message->from->id)] = "TeamMessage";
    bot.getApi().sendMessage(message->chat->id, "Enter element of block:");
}

void cBot::appendTeamMessage(Bot& bot, Message::Ptr message)
{
    /*if (StringTools::startsWith(message->text, "/save"))
        return;*/
    activeBlockCreators[to_string(message->from->id)].setMessage(message);
    activeCommands[to_string(message->from->id)] = "TeamCaption";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element:\nIf you want to save block enter /save");
}

void cBot::viewTeamBlock(Bot& bot, Message::Ptr message)
{
    /*getting json from db
     activeTeam[message->from->id] = json::parse(res from db);
     if(activeTeam[message->from->id].checkUser(message->from->id))
     {
     bot.getApi().sendMessage();
     activeTeam[message->from->id] = team("");
     }
     else
     return;*/
    //TODO: Check if such team is existing
    activeCommands[to_string(message->from->id)] = "Team";
    for (auto& c : activeTeam)
    {
        if (c.second.getTeamName() == message->text)
            if (c.second.checkUser(message->from->id))
            {
                activeBlockViewers[to_string(message->from->id)] = blockView(activeTeam[to_string(message->from->id)].getJson(), message, 1);
                activeBlockViewers[to_string(message->from->id)].sendBlock(bot);
                printf("[cBot]Successfully output team block\n");
            }
            else
                bot.getApi().sendMessage(message->chat->id, "You don't have access to this team");
        else
            bot.getApi().sendMessage(message->chat->id, "There is no such team");

    }
}

void cBot::createPersonalBlock(Bot& bot, Message::Ptr message)
{
    activeCommands[to_string(message->from->id)] = "PersonalCaption";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element");
}

void cBot::viewPersonalBlock(Bot& bot, Message::Ptr message)
{
    //Getting json from db
    activeCommands[to_string(message->from->id)] = "";
    try
    {
        activeBlockViewers[to_string(message->from->id)] = blockView(json::parse(blocks.at(to_string(message->from->id))), message, 1);
        activeBlockViewers[to_string(message->from->id)].sendBlock(bot);
    }
    catch (exception& e)
    {
        printf("[Exception][At cBot]%s\n", e.what());
        bot.getApi().sendMessage(message->from->id, "Firstly, create a block"); // to be deleted
        return;
    }
}

void cBot::appendPersonalCaption(Bot& bot, Message::Ptr message)
{
    /*if (StringTools::startsWith(message->text, "/save"))
        return;*/
    activeBlockCreators[to_string(message->from->id)].setCaption(message->text);
    activeCommands[to_string(message->from->id)] = "PersonalMessage";
    bot.getApi().sendMessage(message->chat->id, "Enter element of block:");
}

void cBot::appendPersonalMessage(Bot& bot, Message::Ptr message)
{
    /*if (StringTools::startsWith(message->text, "/save"))
        return;*/
    activeBlockCreators[to_string(message->from->id)].setMessage(message);
    activeCommands[to_string(message->from->id)] = "PersonalCaption";
    bot.getApi().sendMessage(message->chat->id, "Enter caption of element:\nIf you want to save block enter /save");
}
