#include <blockCreate.h>

// Default constructor for empty blockCreate, so we can use it in map
blockCreate::blockCreate() : m_chatId(0)
{
	printf("Empty block creator");
}

blockCreate::blockCreate(Message::Ptr message) : m_chatId(message->chat->id)
{
	m_messages.push_back(message);
	printf("Block creation started");
}

void blockCreate::setMessage(Message::Ptr message)
{
	m_messages.push_back(message);
	printf("Message pushed backed");
}

void blockCreate::setCaption(string caption)
{
	m_captions.push_back(caption);
	printf("Caption pushed backed");
}

void blockCreate::processMessages()
{
	for (auto caption = m_captions.begin();auto& _message : { m_messages.begin()+1, m_messages.end()})
	{
		messageTypeParser mParse(*_message);
		message messageToSave{ mParse.getMessageTypes()[0], *caption++, mParse.getMessageText()[0]};
		try 
		{
			m_jsonBlock.array() = messageToSave;
		}
		catch (exception& e)
		{
			printf("%s exception at blockCreate\n", e.what());
			return;
		}
		printf("Messages proccesed successfully");
	}
}

string blockCreate::saveBlock(Bot& bot)
{
	return nlohmann::to_string(m_jsonBlock);
}