#pragma once

#include "../Engine/Engine.h"
#include "ECS/Components.h"

class GameCore;

namespace Spawner
{
	b2Body* CreateDynamicBoxBody(GameCore* gameCore, const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, SensorComponent* groundDetectionComponent);

	void SpawnPlayer(GameCore* gameCore, MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal);
};

