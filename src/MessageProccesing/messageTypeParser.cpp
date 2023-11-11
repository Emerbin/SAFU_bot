#include "MessageTypeParser.h"

messageTypeParser::messageTypeParser(Message::Ptr message) : message(message)
{
	for (auto c : messageTypeMap())
	{
		if (c.second != nullptr)
		{
			m_messageTypes.push_back(c.first);
			break;
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
	auto types = unordered_map<string, function<void(void)>>{};

	types["animation"] = [this]() {return m_messageText.push_back(message->animation->fileId);};
	types["audio"] = [this]() {return m_messageText.push_back(message->audio->fileId);};
	types["contact"] = [this]() {return m_messageText.push_back(message->contact->firstName);};
	types["document"] = [this]() {return m_messageText.push_back(message->document->fileId);};
	types["photo"] = [this]() {for_each(message->photo.begin(), message->photo.end(), [this](PhotoSize::Ptr& photo) {return m_messageText.push_back(photo->fileId); });};
	types["sticker"] = [this]() {return m_messageText.push_back(message->sticker->fileId);};
	types["video"] = [this]() {return m_messageText.push_back(message->video->fileId);};
	types["videoNote"] = [this]() {return m_messageText.push_back(message->videoNote->fileId);};
	types["voice"] = [this]() {return m_messageText.push_back(message->voice->fileId);};
	types["text"] = [this]() {return m_messageText.push_back(message->text);};

	auto it = types.find(m_messageTypes.back());
	it->second();
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