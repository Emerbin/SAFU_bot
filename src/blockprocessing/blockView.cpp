#include "blockView.h"

blockView::blockView()
{
	m_jsonBlock.clear();
	printf("[BlockView]Json cleared\n[BlockView]Created empty block\n");
}

//Getting block from json
blockView::blockView(json info, Message::Ptr message, uint32_t blockNumber) : m_chatId(message->chat->id), m_blockNumber(blockNumber)
{
	m_futures.push_back(async(launch::async, &blockView::initializingInline, this));
	// Pushing back messageId of command so we can delete it later
	m_messages.push_back(message->messageId);
	try
	{
		m_jsonBlock = info;
		printf("[BlockView]Json created successfully\n");
		for (auto& _message : m_jsonBlock.at("Messages"))
		{
			m_messagesToSend.push_back(_message);
		}
	}
	catch (exception& e)
	{
		//TODO: Throw exception
		printf("[Exception][At blockView] %s\n", e.what());
		return;
	}
	printf("[BlockView]Block created successfully\n");
}

//Sending all of information from message struct
void blockView::sendMessage(Bot& bot, message _message)
{
	auto it = m_messageType.find(_message.type);
	m_messages.push_back(bot.getApi().sendMessage(m_chatId, "Caption: " + _message.caption)->messageId);
	// Pushing back messageId of already sended messages, so we can delete them or edit in the future
	m_messages.push_back(it->second(_message.text)->messageId);
}

void blockView::sendBlock(Bot& bot)
{
	//Setting up functions for all of types
	initializingMap(bot);
	for (auto& _message : m_messagesToSend)
	{
		sendMessage(bot, _message);
	}
	bot.getApi().editMessageReplyMarkup(m_chatId, m_messages.back(), "", blockControl);
}

void blockView::deleteBlock(Bot& bot)
{
	for (auto& _message : m_messages)
	{
		m_futures.push_back(async(launch::async, [this, &bot, _message]() {bot.getApi().deleteMessage(m_chatId, _message); }));
	}
}

void blockView::initializingInline()
{
	//Setting up text and data of callbacks
	previous->text = "\xE2\x8F\xAA";
	previous->callbackData = "Previous";
	row0.push_back(previous);

	select->text = "1/n";
	select->callbackData = "Select";
	row0.push_back(select);

	next->text = "\xE2\x8F\xA9";
	next->callbackData = "Next";
	row0.push_back(next);

	/*infoDetailed->text = "Detailed info";
	infoDetailed->callbackData = "Detail";
	row1.push_back(infoDetailed);*/

	printf("Text and data of callbacks was set up\n");

	blockControl->inlineKeyboard.push_back(row0);
	/*blockControl->inlineKeyboard.push_back(row1);*/

	printf("Created inline keyboard\n");
}

void blockView::initializingMap(Bot& bot)
{
	m_messageType["animation"] = [&bot, this](string animation) {return bot.getApi().sendAnimation(m_chatId, animation); };
	m_messageType["audio"] = [&bot, this](string audio) {return bot.getApi().sendAudio(m_chatId, audio); };
	m_messageType["contact"] = [&bot, this](string contact) {return bot.getApi().sendMessage(m_chatId, "Contacts yet to be implemented"); };
	m_messageType["document"] = [&bot, this](string document) {return bot.getApi().sendDocument(m_chatId, document); };
	m_messageType["photo"] = [&bot, this](string photo) {return bot.getApi().sendPhoto(m_chatId, photo); };
	m_messageType["sticker"] = [&bot, this](string sticker) {return bot.getApi().sendSticker(m_chatId, sticker); };
	m_messageType["video"] = [&bot, this](string video) {return bot.getApi().sendVideo(m_chatId, video); };
	m_messageType["videoNote"] = [&bot, this](string videoNote) {return bot.getApi().sendVideoNote(m_chatId, videoNote); };
	m_messageType["voice"] = [&bot, this](string voice) {return bot.getApi().sendVoice(m_chatId, voice); };
	m_messageType["text"] = [&bot, this](string text) {return bot.getApi().sendMessage(m_chatId, text); };
	
}

uint32_t blockView::getBlockNumber()
{
	return m_blockNumber;
}
