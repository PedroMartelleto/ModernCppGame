#pragma once

#include "../ECS/Components.h"
#include "WorldInteractionHandler.h"

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
	WorldInteractionHandler worldInteractionHandler;

	WorldContactListener(GameCore* gameCore) :
		gameCore(gameCore),
		worldInteractionHandler(gameCore) {}

	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
private:
	void OnInteraction(InteractionFlag flag, FixtureUserData* userDataA, FixtureUserData* userDataB,
					   b2Fixture* fixtureA, b2Fixture* fixtureB, b2Contact* contact);
};
