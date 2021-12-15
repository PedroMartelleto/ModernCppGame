#pragma once

#include "../../Engine/Engine.h"
#include "../ECS/Components.h"
#include "../Physics/WorldSnapshotEvent.h"

struct RemoveMobsEvent
{
	std::vector<MobID> mobIDs;
};

struct MapDataEvent
{
	std::string mapXMLContents;
};

struct SpawnPlayersEvent
{
	std::vector<MobID> mobIDs;
	std::vector<HostType> types;
	std::vector<std::string> charNames;

	/// <summary>
	/// If host type is SERVER, then positions are randomly selected. Otherwise, this variable needs to be specified.
	/// </summary>
	std::vector<std::pair<float, float>> positions;	
};

struct MobInputsEvent
{
	json bitBuffers;
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapDataEvent, mapXMLContents)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpawnPlayersEvent, mobIDs, types, charNames, positions)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobInputsEvent, bitBuffers)
}

namespace Utils
{
	template<typename T>
	inline nlohmann::json ToJSON(const T& t)
	{
		nlohmann::json dest;
		Serialization::to_json(dest, t);
		return dest;
	}

	template<typename T>
	inline std::shared_ptr<T> RefFromJSON(const nlohmann::json& j)
	{
		T value;
		Serialization::from_json(j, value);
		return std::make_shared<T>(value);
	}

	template<typename T>
	inline T FromJSON(const nlohmann::json& j)
	{
		T value;
		Serialization::from_json(j, value);
		return value;
	}
}