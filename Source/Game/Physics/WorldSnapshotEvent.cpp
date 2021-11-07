#include "WorldSnapshotEvent.h"
#include "../GameCore.h"

void WorldSnapshotEvent::Load(GameCore* gameCore) const
{
	auto& registry = gameCore->registry;

	for (auto& entity : registry.view<PhysicsBodyComponent, MobComponent>())
	{
		auto* body = registry.get<PhysicsBodyComponent>(entity).body;
		auto& mob = registry.get<MobComponent>(entity);
		auto newPos = Utils::FromJSON<b2Vec2>(positions[std::to_string(mob.mobID)]);
		auto newVel = Utils::FromJSON<b2Vec2>(velocities[std::to_string(mob.mobID)]);
		body->SetTransform(newPos, body->GetAngle());
		body->SetLinearVelocity(newVel);
	}
}

void WorldSnapshotEvent::Save(GameCore* gameCore)
{
	auto& registry = gameCore->registry;

	positions.clear();
	velocities.clear();

	for (auto entity : registry.view<PhysicsBodyComponent, MobComponent>())
	{
		auto& body = registry.get<PhysicsBodyComponent>(entity);
		auto& mob = registry.get<MobComponent>(entity);

		positions[std::to_string(mob.mobID)] = Utils::ToJSON(body.body->GetPosition());
		velocities[std::to_string(mob.mobID)] = Utils::ToJSON(body.body->GetLinearVelocity());
	}
}