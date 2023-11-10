#pragma once
#include "pch.h"

using namespace std;
using json = nlohmann::json;

struct message
{
	string type;
	string caption;
	string text; // using text as storage for text or id of files
};

void to_json(json& j, const message& m)
{
	j = json{ {"type",m.type},{"caption",m.caption},{"text",m.text}};
}

// Method for message struct to initialize it from json
void from_json(const json& j, message& m)
{
	m.type = j.value("type", "text");
	m.caption = j.value("caption", "");
	m.text = j.value("text", "");
}