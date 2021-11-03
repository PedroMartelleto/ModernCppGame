#include "GameData.h"

nlohmann::json GameData::s_mobData;
nlohmann::json GameData::s_gameSettings;
std::unordered_map<std::string, int> GameData::s_mobActions;

void GameData::Create(const std::string& folderPath)
{
	s_mobData = Utils::LoadJSON(folderPath + "MobData.json");
	s_gameSettings = Utils::LoadJSON(folderPath + "GameSettings.json");

	const auto& playerActions = s_gameSettings.at("input").at("actions");
	for (uint32_t i = 0; i < playerActions.size(); ++i)
	{
		s_mobActions[playerActions[i]] = i;
	}
}

PlayerInputComponent GameData::CreateDefaultBinding(int playerID)
{
	auto input = PlayerInputComponent();
	const auto& bindings = s_gameSettings.at("input").at("bindings")[playerID];

	for (uint32_t i = 0; i < bindings.size(); ++i)
	{
		input.inputCodes[i] = bindings[i];
	}

	return input;
}

MobComponent GameData::GetMobData(const std::string& mobName)
{
	auto mobComponent = MobComponent();
	Serialization::from_json(s_mobData.at(mobName), mobComponent);
	return mobComponent;
}