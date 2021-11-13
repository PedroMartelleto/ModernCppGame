#pragma once

#include "../Engine/Engine.h"
#include "ECS/Components.h"

class GameCore;
struct FixtureUserData;

enum class SensorPlacement
{
	Top, Bottom
};

namespace Spawner
{
	b2Body* CreateDynamicBoxBody(GameCore* gameCore, float density, const Vec2f& position, const Vec2f& size, const SensorPlacement& sensorPlacement,
								 const Vec2f& sensorRatio, FixtureUserData* fixtureUserData);

	entt::entity SpawnMob(GameCore* gameCore, MobID mobID, const std::string& charName, const Vec2f& tilePos);

	entt::entity SpawnEnemyMob(GameCore* gameCore, MobID mobID, const std::string& charName, const Vec2f& tilePos);

	entt::entity SpawnPlayer(GameCore* gameCore, MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal);

	entt::entity SpawnProjectile(GameCore* gameCore, const Vec2f& pos, const ProjectileData& projectileData);
};

