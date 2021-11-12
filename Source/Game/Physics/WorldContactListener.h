#pragma once

#include "../ECS/Components.h"

struct FixtureUserData
{
	SensorComponent* sensor = nullptr;

	/// <summary>
	/// If this fixture belongs to a projectile, this variable points to the projectile component
	/// </summary>
	ProjectileComponent* projectileComponent = nullptr;

	/// <summary>
	/// If this fixture belongs to a mob, this variable points to the mob component
	/// </summary>
	MobComponent* mobComponent = nullptr;
};

struct WorldContactListener : public b2ContactListener
{
	GameCore* gameCore;

	WorldContactListener(GameCore* gameCore) : gameCore(gameCore) {}

	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
};
