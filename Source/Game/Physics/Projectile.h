#pragma once

#include "../../Engine/Engine.h"

struct ProjectileDirection
{
	bool up;
	bool right;
	bool down;
	bool left;

	void Reset();

	Vec2f AsVector(const Vec2f& defaultDir) const;
	float AsAngle(const Vec2f& defaultDir) const;
	std::string ToString(const Vec2f& defaultDir) const;
};

struct ProjectileData
{
	ResourceID atlas = 0;
	std::string flyingSpriteName;
	std::string readySpriteName;

	float initialSpeed;
	float density;
	float baseAngle;
	float linearDamping;
	float angularDamping;
	float dragMultiplier;
	float damage;

	nlohmann::json aabb;
	
	entt::entity owner = entt::null;

	inline Rect2D GetAABB() const
	{
		return Rect2D(aabb["x"], aabb["y"], aabb["width"], aabb["height"]);
	}
};

namespace Projectile
{
	void SpawnAndShoot(GameCore* gameCore, const entt::entity& owner, const ProjectileData& projectile);
}

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectileData, atlas, initialSpeed, density, flyingSpriteName, readySpriteName, baseAngle, aabb, damage, dragMultiplier)
}