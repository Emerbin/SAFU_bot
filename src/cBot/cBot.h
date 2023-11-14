#pragma once
#include "pch.h"
#include "messageTypeParser.cpp"
#include "blockCreate.cpp"
#include "blockView.cpp"

using namespace std;
using namespace TgBot;
using json = nlohmann::json;
using namespace nlohmann::literals;

class cBot
{
public:
	cBot(string token);
	unordered_map<string, any> setupCallbacks() noexcept;
	unordered_map<string, any> setupCommands() noexcept;
	unordered_map<string, any> setupMessages() noexcept;

	//void sendBase();
	void createKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb);
	[[maybe_unused]] void createOneTimeKeyboard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb);
	void logger(Message::Ptr message);

	void startCommand(Bot& bot, Message::Ptr message);
	void createCommand(Bot& bot, Message::Ptr message);
	void blockCommand(Bot& bot, Message::Ptr message);
	void saveCommand(Bot& bot, Message::Ptr message);
	void nextBlock(Bot& bot, CallbackQuery::Ptr query);
	void selectBlock(Bot& bot, CallbackQuery::Ptr query);
	void previousBlock(Bot& bot, CallbackQuery::Ptr query);
	void select(Bot& bot, Message::Ptr query);
	void askTeam(Bot& bot, Message::Ptr message);
	void createTeam(Bot& bot, Message::Ptr message);
	void createPersonal(Bot& bot, Message::Ptr message);
	void viewTeam(Bot& bot, Message::Ptr message);
	void viewPersonal(Bot& bot, Message::Ptr message);
	void appendPersonalMessage(Bot& bot, Message::Ptr message);
	void appendPersonalCaption(Bot& bot, Message::Ptr message);
private:
	string token;

	ReplyKeyboardMarkup::Ptr personalKeyboard{new ReplyKeyboardMarkup};

	//TODO Implement a new logging system
	//vector<Message::Ptr> messagesForLogs{};

	unordered_map<int64_t, string> activeCommands{};
	unordered_map<int64_t, blockView> activeBlockViewers{};
	unordered_map<int64_t, blockCreate> activeBlockCreators{};
	//To be removed
	multimap<int64_t, string> blocks{};

	json testJson = R"({"array" : [{"type" : "text", "caption" : "caption1", "text" : "some text1"}, 
{"type" : "photo", "caption" : "captionP","text" : "AgACAgIAAxkBAAIInmTbL1L44SYpat8S6ALFPbjdhXNPAALj0zEb0h3YSq4b3q520-CpAQADAgADcwADMAQ" },
{"type" : "text", "caption" : "caption2", "text" : "some text2"},
{"type" : "text", "caption" : "caption3", "text" : "some text3"},
{"type" : "text", "caption" : "caption4", "text" : "some text4"},
{"type" : "text", "caption" : "caption5", "text" : "some text5"},
{"type" : "text", "caption" : "caption6", "text" : "some text6"},
{"type" : "text", "caption" : "caption7", "text" : "some text7"},
{"type" : "text", "caption" : "caption8", "text" : "some text8"},
{"type" : "text", "caption" : "caption9", "text" : "some text9"}]})"_json;
};
