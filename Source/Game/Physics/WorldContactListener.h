#pragma once

#include "../ECS/Components.h"

struct FixtureUserData
{
	SensorComponent* sensor;
};

struct WorldContactListener : public b2ContactListener
{
	GameCore* gameCore;

	WorldContactListener(GameCore* gameCore) : gameCore(gameCore) {}

	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
};
