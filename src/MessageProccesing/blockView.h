#pragma once
#include "pch.h"
#include "message.h"

using namespace std;
using namespace TgBot;
using json = nlohmann::json;
using namespace nlohmann::literals;

class blockView // Proccesing block from json, sending it and deleting if needed
{
private:
	json m_jsonBlock;
	vector<message> m_messagesToSend{};
	vector<int32_t> m_messages{};
	vector<future<void>> m_futures{};
	unordered_map<string, function<Message::Ptr(string)>> m_messageType{};
	int64_t m_chatId{};
	uint32_t m_blockNumber{1};

	InlineKeyboardMarkup::Ptr blockControl{new InlineKeyboardMarkup};

	InlineKeyboardButton::Ptr next{new InlineKeyboardButton};
	InlineKeyboardButton::Ptr previous{new InlineKeyboardButton};
	InlineKeyboardButton::Ptr select{new InlineKeyboardButton};
	[[maybe_unused]] InlineKeyboardButton::Ptr infoDetailed{new InlineKeyboardButton};

	vector<InlineKeyboardButton::Ptr> row0;
	[[maybe_unused]] vector<InlineKeyboardButton::Ptr> row1;

	void sendMessage(Bot& bot, message _message);
	void initializingMap(Bot& bot);
	void initializingInline();
public:
	blockView();
	blockView(json info, Message::Ptr _message, uint32_t blockNumber);
	void deleteBlock(Bot& bot);
	void sendBlock(Bot& bot);
	uint32_t getBlockNumber();
};
