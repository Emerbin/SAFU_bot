#include "timeProcessor.h"

timeProcessor::timeProcessor(Message::Ptr message) : m_message(message)
{
	for (auto format : m_formats)
	{
		m_futures.push_back(async(launch::async, &timeProcessor::parsingMessage,this,m_message->text,format));
	}
}

void timeProcessor::parsingMessage(string dateTime, string format)
{
	stringstream dateStream(dateTime);
	dateStream >> chrono::parse(format, m_parsedTime);
	if (dateStream.fail())
	{
		cerr << "Parsing failed\n";
		return;
	}
	printf("Parsed successful\n");
	cout << m_parsedTime;
}

void timeProcessor::creatingReminder()
{
}

void timeProcessor::savingReminder()
{
}
