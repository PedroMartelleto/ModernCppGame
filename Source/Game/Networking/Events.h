#pragma once

#include "../../Engine/Engine.h"
#include "../ECS/Components.h"

struct MapDataEvent
{
	std::string mapXMLContents;
};

struct SpawnPlayerEvent
{
	MobID mobID;
	HostType type;
	uint16_t tileX;
	uint16_t tileY;
	std::string charName;
};

struct MobPositionsEvent
{
	json pos;
	json vel;
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapDataEvent, mapXMLContents)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpawnPlayerEvent, mobID, type, tileX, tileY, charName)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobPositionsEvent, pos, vel)
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