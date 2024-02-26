#pragma once
#include "pch.h"

using namespace std;
using namespace TgBot;

class team
{
	private:
		string teamName;
		set<int64_t> users{};
		nlohmann::json jsonBlock;
	public:
		team();
		team(const string& teamName);
		set<int64_t> getUsers();
		string getTeamName();
		nlohmann::json getJson();
		void setUsers(const vector<int64_t>& users);
		void setTeamName(const string& teamName);
		void setJson(const nlohmann::json& json);
		void appendUser(int64_t id);
		bool checkUser(int64_t id);
};

void to_json(nlohmann::json& j, team& team)
{
	j["TeamName"] = team.getTeamName();
	j["Users"].push_back(team.getUsers());
	printf("[Team]Team successfully converted to json\n");
}

void from_json(const nlohmann::json& json, team& team)
{
	team.setTeamName(json.at("TeamName"));
	team.setUsers(json.at("Users"));
	printf("[Team]Team successfully converted from json\n");
}