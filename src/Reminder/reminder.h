#pragma once
#include "pch.h"

struct reminder 
{
	int64_t reminderTime; //using time in seconds
	std::string reminderMessage; //message, which we will send, when time is come
	int32_t timesToRepeat; //how much times we want to repeat our reminder
};