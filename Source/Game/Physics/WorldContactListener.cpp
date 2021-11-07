#include "WorldContactListener.h"

FixtureUserData* GetFixtureUserData(b2Fixture* fixture)
{
	if (fixture == nullptr || fixture->GetUserData().pointer == NULL) return nullptr;
	return reinterpret_cast<FixtureUserData*>(fixture->GetUserData().pointer);
}

void WorldContactListener::BeginContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

	if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
	{
		auto* userDataA = GetFixtureUserData(fixtureA);
		auto* userDataB = GetFixtureUserData(fixtureB);

		if (userDataA != nullptr) userDataA->sensor->contactCount[SensorComponent::GroundSensor] += 1;
		if (userDataB != nullptr) userDataB->sensor->contactCount[SensorComponent::GroundSensor] += 1;
	}
}

void WorldContactListener::EndContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();

	if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

	if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
	{
		auto* userDataA = GetFixtureUserData(fixtureA);
		auto* userDataB = GetFixtureUserData(fixtureB);

		if (userDataA != nullptr) userDataA->sensor->contactCount[SensorComponent::GroundSensor] -= 1;
		if (userDataB != nullptr) userDataB->sensor->contactCount[SensorComponent::GroundSensor] -= 1;
	}
}