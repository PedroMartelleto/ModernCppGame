#include "GameData.h"

nlohmann::json GameData::s_mobData;
nlohmann::json GameData::s_gameSettings;
nlohmann::json GameData::s_projectileData;
std::unordered_map<std::string, int> GameData::s_mobActions;
std::vector<std::string> GameData::s_mobNames;

void GameData::Create(const std::string& folderPath)
{
	s_mobData = Utils::LoadJSON(folderPath + "MobData.json");
	s_gameSettings = Utils::LoadJSON(folderPath + "GameSettings.json");
	s_projectileData = Utils::LoadJSON(folderPath + "ProjectileData.json");

	for (auto it = s_mobData.begin(); it != s_mobData.end(); ++it)
	{
		s_mobNames.push_back((*it)["name"]);
	}

	const auto& playerActions = s_gameSettings.at("input").at("actions");
	for (uint32_t i = 0; i < playerActions.size(); ++i)
	{
		s_mobActions[playerActions[i]] = i;
	}
}

ProjectileData GameData::GetProjectileData(const std::string& projectileName)
{
	auto projectileData = ProjectileData();
	Serialization::from_json(s_projectileData.at(projectileName), projectileData);
	return projectileData;
}

std::string GameData::GetMobNameFromTypeID(uint8_t typeID)
{
	return s_mobNames[typeID];
}

LocalInputComponent GameData::CreateDefaultBinding(int playerID)
{
	auto input = LocalInputComponent();
	const auto& bindings = s_gameSettings.at("input").at("bindings")[playerID];

	for (uint32_t i = 0; i < bindings.size(); ++i)
	{
		input.inputCodes[i] = bindings[i];
	}

	return input;
}

MobComponent GameData::GetMobData(const std::string& mobName, MobID mobID)
{
	auto mobComponent = MobComponent();
	Serialization::from_json(s_mobData.at(mobName), mobComponent);
	mobComponent.mobID = mobID;
	return mobComponent;
}