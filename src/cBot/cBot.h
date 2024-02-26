#pragma once
#include "pch.h"
#include "messageTypeParser.cpp"
#include "team.cpp"
#include "blockCreate.cpp"
#include "blockView.cpp"

using namespace std;
using namespace TgBot;
using json = nlohmann::json;
using namespace nlohmann::literals;

class cBot
{
private:
	string token;

	ReplyKeyboardMarkup::Ptr personalKeyboard { new ReplyKeyboardMarkup };
	ReplyKeyboardMarkup::Ptr askUser { new ReplyKeyboardMarkup };

	//TODO Implement a new logging system
	//vector<Message::Ptr> messagesForLogs{};

	//TODO separate thread for input

	unordered_map<string, string> activeCommands{};
	unordered_map<string, blockView> activeBlockViewers{};
	unordered_map<string, blockCreate> activeBlockCreators{};
	unordered_map<string, team> activeTeam{};

	unordered_map<string, string> blocks{}; //to be removed

	//to be removed
	json testJson = R"({"array" : [{"type" : "text", "caption" : "caption1", "text" : "some text1"}, 
{"type" : "photo", "caption" : "captionP","text" : "AgACAgIAAxkBAAOkZVfnXkq_U1ZjiyEmiNUeAAH27ZODAAJw1DEb1M3ASooELXQaRQ8mAQADAgADeQADMwQ" },
{"type" : "text", "caption" : "caption2", "text" : "some text2"},
{"type" : "text", "caption" : "caption3", "text" : "some text3"},
{"type" : "text", "caption" : "caption4", "text" : "some text4"},
{"type" : "text", "caption" : "caption5", "text" : "some text5"},
{"type" : "text", "caption" : "caption6", "text" : "some text6"},
{"type" : "text", "caption" : "caption7", "text" : "some text7"},
{"type" : "text", "caption" : "caption8", "text" : "some text8"},
{"type" : "text", "caption" : "caption9", "text" : "some text9"}]})"_json;

public:
	cBot(string token);
	unordered_map<string, any> setupCallbacks() noexcept;
	unordered_map<string, any> setupCommands() noexcept;
	unordered_map<string, any> setupMessages() noexcept;

	//void sendBase();
	void createKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb);
	void createOneTimeKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb);
	void createUserKeyboard();
	void logger(Message::Ptr message);

	void startCommand(Bot& bot, Message::Ptr message);
	void createCommand(Bot& bot, Message::Ptr message);
	void blockCommand(Bot& bot, Message::Ptr message);
	void saveCommand(Bot& bot, Message::Ptr message);

	void nextBlock(Bot& bot, CallbackQuery::Ptr query);
	void selectBlock(Bot& bot, CallbackQuery::Ptr query);
	void previousBlock(Bot& bot, CallbackQuery::Ptr query);
	void selectBlock(Bot& bot, Message::Ptr query);

	void askTeam(Bot& bot, Message::Ptr message);
	void createTeam(Bot& bot, Message::Ptr message);
	void appendUser(Bot& bot, Message::Ptr message);
	void finishUserAppend(Bot& bot, Message::Ptr message);
	void appendTeamCaption(Bot& bot, Message::Ptr message);
	void appendTeamMessage(Bot& bot, Message::Ptr message);
	void viewTeamBlock(Bot& bot, Message::Ptr message);

	void createPersonalBlock(Bot& bot, Message::Ptr message);
	void viewPersonalBlock(Bot& bot, Message::Ptr message);

	void appendPersonalMessage(Bot& bot, Message::Ptr message);
	void appendPersonalCaption(Bot& bot, Message::Ptr message);
};
