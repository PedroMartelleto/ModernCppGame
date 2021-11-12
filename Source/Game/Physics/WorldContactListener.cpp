#include "WorldContactListener.h"
#include "WorldInteractionHandler.h"

FixtureUserData* GetFixtureUserData(b2Fixture* fixture)
{
	if (fixture == nullptr || fixture->GetUserData().pointer == NULL) return nullptr;
	return reinterpret_cast<FixtureUserData*>(fixture->GetUserData().pointer);
}

void OnBeginContact(FixtureUserData* userData)
{
	if (userData == nullptr) return;
	userData->sensor->contactCount[SensorComponent::GroundSensor] += 1;
}

void OnEndContact(FixtureUserData* userData)
{
	if (userData == nullptr) return;
	userData->sensor->contactCount[SensorComponent::GroundSensor] -= 1;
}

struct InteractionActors
{
	std::vector<MobComponent*> mobs;
	std::vector<ProjectileComponent*> projectiles;
};

void AddActorsToInteraction(FixtureUserData* userData, InteractionActors& actors)
{
	if (userData->mobComponent != nullptr) actors.mobs.push_back(userData->mobComponent);
	if (userData->projectileComponent != nullptr) actors.projectiles.push_back(userData->projectileComponent);
}

void OnInteraction(InteractionFlag flag, FixtureUserData* userDataA, FixtureUserData* userDataB)
{
	InteractionActors actors;

	AddActorsToInteraction(userDataA, actors);
	AddActorsToInteraction(userDataB, actors);

	if (actors.projectiles.size() == 1 && actors.mobs.size() == 1)
	{
		WorldInteractionHandler::OnMobProjectileInteraction(flag, actors.mobs[0], actors.projectiles[0]);
	}
	else if (actors.mobs.size() == 2)
	{
		WorldInteractionHandler::OnMobMobInteraction(flag, actors.mobs[0], actors.mobs[1]);
	}
}

void WorldContactListener::BeginContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

	auto* userDataA = GetFixtureUserData(fixtureA);
	auto* userDataB = GetFixtureUserData(fixtureB);

	if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
	{
		OnBeginContact(userDataA);
		OnBeginContact(userDataB);
	}

	if (userDataA != nullptr && userDataB != nullptr) OnInteraction(InteractionFlag::BEGIN, userDataA, userDataB);
}

void WorldContactListener::EndContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

	auto* userDataA = GetFixtureUserData(fixtureA);
	auto* userDataB = GetFixtureUserData(fixtureB);

	if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
	{
		OnEndContact(userDataA);
		OnEndContact(userDataB);
	}

	if (userDataA != nullptr && userDataB != nullptr) OnInteraction(InteractionFlag::END, userDataA, userDataB);
}