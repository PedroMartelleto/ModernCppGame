#pragma once

#include "../../Engine/Engine.h"

struct WorldSnapshotEvent
{
public:
	json positions = json();
	json velocities = json();

	void Save(GameCore* gameCore);
	void Load(GameCore* gameCore) const;
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorldSnapshotEvent, positions, velocities)
}