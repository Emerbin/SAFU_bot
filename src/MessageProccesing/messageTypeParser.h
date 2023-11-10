#pragma once
#include "pch.h"

using namespace std;
using namespace TgBot;

class messageTypeParser 
{
public:
	messageTypeParser(Message::Ptr message);
	vector<string> getMessageTypes();
	vector<string> getMessageText();
private:
	Message::Ptr message;
	vector<string> m_messageText{};
	vector<string> m_messageTypes{};
	unordered_map<string, void*> messageTypeMap() noexcept;
	void setupMessageText();
};
