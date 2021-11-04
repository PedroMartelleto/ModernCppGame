#pragma once

#include "../Engine/Commons.h"
#include "ECS/Components.h"

class GameData
{
public:
	static void Create(const std::string& folderPath);
	
	static std::string GetMobNameFromTypeID(uint8_t mobTypeID);
	static MobComponent GetMobData(const std::string& mobName, MobID mobID);
	static LocalInputComponent CreateDefaultBinding(int playerID);

	inline static int GetMobActionBit(const std::string& actionName)
	{
		return s_mobActions[actionName];
	}
private:
	static nlohmann::json s_mobData;
	static nlohmann::json s_gameSettings;
	static std::vector<std::string> s_mobNames;
	static std::unordered_map<std::string, int> s_mobActions;
};

