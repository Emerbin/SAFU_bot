#include <blockCreate.h>

// Default constructor for empty blockCreate, so we can use it in map
blockCreate::blockCreate()
{
	printf("Empty block creator\n");
}

void blockCreate::setMessage(Message::Ptr message)
{
	m_messages.push_back(message);
	printf("Message pushed backed\n");
}

void blockCreate::setCaption(string caption)
{
	m_captions.push_back(caption);
	printf("Caption pushed backed\n");
}

void blockCreate::processMessages()
{
	for (auto caption = m_captions.begin(); auto& st_message : m_messages)
	{
		messageTypeParser mParse(st_message);
		message messageToSave{ mParse.getMessageTypes()[0], *caption++, mParse.getMessageText()[0]};
		try 
		{
			m_jsonBlock["array"].push_back(messageToSave);
		}
		catch (exception& e)
		{
			printf("%s exception at blockCreate\n", e.what());
			return;
		}
	}
	printf("Messages proccesed successfully\n");
}

string blockCreate::saveBlock()
{
	//TODO: Make call to database
	processMessages();
	return m_jsonBlock.dump();
}