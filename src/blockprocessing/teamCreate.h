#pragma once
#include "pch.h"

using namespace std;
using namespace TgBot;

class teamCreate
{
	private:
		string teamName;
		vector<int64_t> users{};
	public:
		teamCreate(Message::Ptr message);
		void appendUser(Message::Ptr message);
		
};