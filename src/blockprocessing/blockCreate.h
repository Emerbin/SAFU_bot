#pragma once
#include "pch.h"
#include "messageTypeParser.h"
#include "message.h"

using namespace std;
using namespace TgBot;
using json = nlohmann::json;
using namespace nlohmann::literals;

class blockCreate
{
private:
	json m_jsonBlock;
	// We will store our messages in vector and then process it. Also we can make it async and store into another vector
	vector<Message::Ptr> m_messages;
	vector<string> m_captions;
public:
	blockCreate();
	string saveBlock();
	json getJson();
	void setMessage(Message::Ptr);
	void setCaption(string caption);
	void processMessages();
};