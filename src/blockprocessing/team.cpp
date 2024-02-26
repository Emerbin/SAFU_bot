#include "pch.h"
#include "team.h"

team::team()
{
	printf("[Team]Empty team initialized\n");
}

team::team(const string& teamName) : teamName(teamName) 
{
	printf("[Team]Team initialized\n");
}

string team::getTeamName()
{
	return teamName;
}

nlohmann::json team::getJson()
{
	return jsonBlock;
}

set<int64_t> team::getUsers()
{
	return users;
}

void team::setTeamName(const string& teamName)
{
	this->teamName = teamName;
}

void team::setJson(const nlohmann::json& json)
{
	jsonBlock = json;
}

void team::setUsers(const vector<int64_t>& users)
{
	for (auto& c : users)
		this->users.insert(c);
}

void team::appendUser(int64_t id)
{
	users.insert(id);
}

bool team::checkUser(int64_t id)
{
	return users.contains(id);
}
