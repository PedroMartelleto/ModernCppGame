#include "WorldContactListener.h"
#include "WorldInteractionHandler.h"

FixtureUserData* GetFixtureUserData(b2Fixture* fixture)
{
	if (fixture == nullptr || fixture->GetUserData().pointer == NULL) return nullptr;
	return reinterpret_cast<FixtureUserData*>(fixture->GetUserData().pointer);
}

void OnBeginContact(b2Fixture* fixture, FixtureUserData* userData)
{
	if (userData == nullptr || !fixture->IsSensor()) return;
	userData->sensor->contactCount[SensorComponent::GroundSensor] += 1;
}

void OnEndContact(b2Fixture* fixture, FixtureUserData* userData)
{
	if (userData == nullptr || !fixture->IsSensor()) return;
	userData->sensor->contactCount[SensorComponent::GroundSensor] -= 1;
}

struct InteractionActors
{
	std::vector<ContactData<MobComponent>> mobs;
	std::vector<ContactData<ProjectileComponent>> projectiles;
	std::vector<ContactData<void>> staticMapFixtures;
};

void AddActorsToInteraction(FixtureUserData* userData, b2Fixture* fixture, InteractionActors& actors)
{
	// Maintains lists of objects involved in some interaction (contact, post solve ...)
	if (fixture->GetBody()->GetType() == b2_staticBody)
	{
		actors.staticMapFixtures.push_back(ContactData<void>{nullptr, fixture});
	}

	if (userData == nullptr) return;

	if (userData->mobComponent != nullptr)
	{
		actors.mobs.push_back(ContactData<MobComponent>{userData->mobComponent, fixture});
	}
	
	if (userData->projectileComponent != nullptr)
	{
		actors.projectiles.push_back(ContactData<ProjectileComponent>{userData->projectileComponent, fixture});
	}
}

void WorldContactListener::OnInteraction(InteractionFlag flag, FixtureUserData* userDataA, FixtureUserData* userDataB,
										 b2Fixture* fixtureA, b2Fixture* fixtureB, b2Contact* contact)
{
	InteractionActors actors;

	AddActorsToInteraction(userDataA, fixtureA, actors);
	AddActorsToInteraction(userDataB, fixtureB, actors);

	if (actors.projectiles.size() == 1 && actors.mobs.size() == 1)
	{
		worldInteractionHandler.OnMobProjectileInteraction(flag, actors.mobs[0], actors.projectiles[0], contact);
	}
	else if (actors.mobs.size() == 2)
	{
		worldInteractionHandler.OnMobMobInteraction(flag, actors.mobs[0], actors.mobs[1], contact);
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
		OnBeginContact(fixtureA, userDataA);
		OnBeginContact(fixtureB, userDataB);
	}

	if (userDataA != nullptr || userDataB != nullptr)
	{
		OnInteraction(InteractionFlag::BEGIN, userDataA, userDataB, fixtureA, fixtureB, contact);
	}
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
		OnEndContact(fixtureA, userDataA);
		OnEndContact(fixtureB, userDataB);
	}

	if (userDataA != nullptr || userDataB != nullptr)
	{
		OnInteraction(InteractionFlag::END, userDataA, userDataB, fixtureA, fixtureB, contact);
	}
}

void WorldContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

	auto* userDataA = GetFixtureUserData(fixtureA);
	auto* userDataB = GetFixtureUserData(fixtureB);

	if (userDataA != nullptr || userDataB != nullptr)
	{
		InteractionActors actors;

		AddActorsToInteraction(userDataA, fixtureA, actors);
		AddActorsToInteraction(userDataB, fixtureB, actors);

		if (actors.projectiles.size() == 1 && actors.staticMapFixtures.size() == 1)
		{
			worldInteractionHandler.OnPostSolveMapProjectile(actors.staticMapFixtures[0], actors.projectiles[0], contact, impulse);
		}
	}
}