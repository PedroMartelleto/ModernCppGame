#include "Projectile.h"
#include "../Spawner.h"
#include "../GameCore.h"

// TODO: Stick to walls, Sensor, damage
// TODO: Small input delay for removing direction
// TODO: Handle looping of non-fixed rotation projectiles

namespace Projectile
{
	void SpawnAndShoot(GameCore* gameCore, const entt::entity& owner, const ProjectileData& projectile)
	{
		auto& registry = gameCore->registry;
		auto& mob = registry.get<MobComponent>(owner);
		auto& body = registry.get<PhysicsBodyComponent>(owner).body;
		auto map = gameCore->map;

		auto projEntity = Spawner::SpawnProjectile(gameCore, Vec2fFromB2(body->GetPosition()) * Game::METERS_TO_PIXELS, projectile);
		auto& projBody = registry.get<PhysicsBodyComponent>(projEntity).body;
		auto initialDir = mob.shootDirection.AsVector();
		
		float initialAngle = mob.shootDirection.AsAngle() + glm::radians(projectile.baseAngle);
		auto projOffset = Vec2fToB2((initialDir * (mob.GetAABB().size() / 2.0f + Vec2f(8.0f, 8.0f)) * map->mapScale * Game::PIXELS_TO_METERS));

		projBody->SetTransform(projBody->GetPosition() + projOffset, initialAngle);
		projBody->SetFixedRotation(false);
		projBody->SetLinearDamping(projectile.linearDamping);
		projBody->SetAngularDamping(projectile.angularDamping);
		projBody->ApplyLinearImpulse(Vec2fToB2(initialDir * projectile.initialSpeed), projBody->GetPosition(), true);
	}
}

void ProjectileDirection::Reset()
{
	up = false;
	down = false;
	left = false;
	right = false;
}


std::string ProjectileDirection::ToString() const
{
	auto vec = AsVector();
	std::stringstream stream;

	if (vec.y > 0.0f)
	{
		stream << "U";
	}
	else if (vec.y < 0.0f)
	{
		stream << "D";
	}

	if (vec.x > 0.0f)
	{
		stream << "R";
	}
	else if (vec.x < 0.0f)
	{
		stream << "L";
	}

	return stream.str();
}

bool ProjectileDirection::IsNone() const
{
	return !up && !down && !left && !right;
}

Vec2f ProjectileDirection::AsVector() const
{
	float yDir = up ? -1.0f : 0.0f;
	if (down) yDir += 1.0f;

	float xDir = right ? 1.0f : 0.0f;
	if (left) xDir -= 1.0f;

	return Vec2f(xDir, yDir);
}

float ProjectileDirection::AsAngle() const
{
	auto dir = AsVector();
	return atan2f(dir.y, dir.x);
}