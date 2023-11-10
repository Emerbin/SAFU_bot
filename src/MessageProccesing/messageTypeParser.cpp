#include "MessageTypeParser.h"

messageTypeParser::messageTypeParser(Message::Ptr message) : message(message)
{
	for (auto c : messageTypeMap())
	{
		if (c.second != nullptr)
		{
			m_messageTypes.push_back(c.first);
		}
	}
}

unordered_map<string, void*> messageTypeParser::messageTypeMap() noexcept
{
	auto types = unordered_map<string, void*>{};

	types["animation"] = message->animation.get();
	types["audio"] = message->audio.get();
	types["contact"] = message->contact.get();
	types["document"] = message->document.get();
	types["photo"] = message->photo.data();
	types["sticker"] = message->sticker.get();
	types["video"] = message->video.get();
	types["videoNote"] = message->videoNote.get();
	types["voice"] = message->voice.get();
	types["text"] = message->text.data();

	return types;
}


void messageTypeParser::setupMessageText()
{
	if (m_messageTypes.back() == "animation")
		m_messageText.push_back(message->animation->fileId);
	else if (m_messageTypes.back() == "audio")
		m_messageText.push_back(message->audio->fileId);
	else if (m_messageTypes.back() == "contact")
		m_messageText.push_back(message->contact->firstName);
	else if (m_messageTypes.back() == "document")
		m_messageText.push_back(message->document->fileId);
	else if (m_messageTypes.back() == "photo")
		for_each(message->photo.begin(), message->photo.end(), [this](PhotoSize::Ptr& photo) {m_messageText.push_back(photo->fileId); });
	else if (m_messageTypes.back() == "sticker")
		m_messageText.push_back(message->sticker->fileId);
	else if (m_messageTypes.back() == "video")
		m_messageText.push_back(message->video->fileId);
	else if (m_messageTypes.back() == "videoNote")
		m_messageText.push_back(message->videoNote->fileId);
	else if (m_messageTypes.back() == "voice")
		m_messageText.push_back(message->voice->fileId);
	else if (m_messageTypes.back() == "text")
		m_messageText.push_back(message->text);
}

vector<string> messageTypeParser::getMessageTypes()
{
	return m_messageTypes;
}

vector<string> messageTypeParser::getMessageText()
{
	setupMessageText();
	return m_messageText;
}