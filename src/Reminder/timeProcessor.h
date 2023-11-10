#pragma once
#include "pch.h"

using namespace std;
using namespace TgBot;


//TODO: process a time zone from message, parse time from message. Parse time from different inputs
// Use failbit. If one of the parsers doesnt have failbit it will assign values for time
// Parse async. Use strong exception guarantee. Write own exception class inheritence
// How to know if message is reminder. Use regex. If some number and T after number then it is reminder
// savingReminder: Getting times from db and storing new into unordered_map
// Later sending reminder using lazy evalution. async launch::deffered. Calling all async with that time only when needed
class timeProcessor
{
public:
	timeProcessor(Message::Ptr message);
private:
	Message::Ptr m_message;
	void parsingMessage(string dateTime, string formatStr);
	void creatingReminder();
	void savingReminder();
	chrono::sys_time<chrono::seconds> m_parsedTime{};
	const vector<string> m_formats = { " %Y %m %d %H:%M:%S"," %Y %m %d"," %T"," %d-%m-'%Y' %R"};
	vector<future<void>> m_futures{};
};