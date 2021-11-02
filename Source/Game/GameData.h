#pragma once

#include "../Engine/Commons.h"
#include "ECS/Components.h"

class GameData
{
public:
	static void Create(const std::string& folderPath);

	static MobComponent GetMobData(const std::string& mobName);
	static PlayerInputComponent GetDefaultPlayerInput(int playerID);

	inline static int GetPlayerActionBit(const std::string& actionName)
	{
		return s_playerActions[actionName];
	}
private:
	static nlohmann::json s_mobData;
	static nlohmann::json s_gameSettings;
	static std::unordered_map<std::string, int> s_playerActions;
};

