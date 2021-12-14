#pragma once

#include "../../Engine/Engine.h"
#include "../Networking/BitBuffer.h"
#include "../Physics/Projectile.h"

// We are using ECS, so we need to define components: minimal pieces of data used in the game

struct PhysicsBodyComponent
{
	b2Body* body;
	float dragMultiplier;

	PhysicsBodyComponent() = default;
	PhysicsBodyComponent(b2Body* body, float dragMultiplier) : body(body), dragMultiplier(dragMultiplier) {}

	inline operator const b2Body* () const { return body; }
	inline operator b2Body* () { return body; }
};

struct PathfindingComponent
{
	WorldNodeID currentNode;
	WorldNodeID destNode;
};

struct ProjectileComponent
{
	ProjectileData projectileData;
	bool hasHitAnything = false;
};

struct AnimationComponent
{
	Ref<TextureAtlas> atlas;
	std::string regionName;
	uint16_t tickRate = 10;
	bool allowFlip = true;
	bool isFlipped = false;
	uint8_t frame = 0;

	AnimationComponent(const Ref<TextureAtlas>& atlas, const std::string& regionName, bool allowFlip = true) :
		atlas(atlas), regionName(regionName), allowFlip(allowFlip) {}

	inline Rect2D GetRect() const
	{
		return atlas->GetAnimFrameRegion(regionName, frame);
	}

	/// <summary>
	/// Vector that represents the direction of the entity that has this component.
	/// </summary>
	inline Vec2f GetFaceDir() const { return Vec2f(isFlipped ? -1.0f : 1.0f, 0.0f);  }
};

struct SpriteComponent
{
	Ref<Texture2D> texture;
	float zIndex;
	Vec2f pos = Vec2f(0, 0);
	Vec2f size = Vec2f(0, 0);
	Color4f tint = Colors::WHITE;

	SpriteComponent(const Ref<Texture2D>& texture, float zIndex, const Vec2f& pos, const Vec2f& size, const Color4f& tint = Colors::WHITE) :
		texture(texture), pos(pos), size(size), tint(tint), zIndex(zIndex) {}
};

struct DEBUG_PhysicsBodyDrawComponent
{
	bool drawAABB;
	bool drawPoly;
	Color4f aabbColor;
	Color4f polyColor;
	
	DEBUG_PhysicsBodyDrawComponent(bool drawAABB, bool drawPoly,
		const Color4f& aabbColor = Colors::Alpha(Colors::GREEN, 0.3f),
		const Color4f& polyColor = Colors::BLACK) :
		drawAABB(drawAABB), drawPoly(drawPoly), aabbColor(aabbColor), polyColor(polyColor) {}
};

using MobID = uint32_t;

struct ProjectileInventoryComponent
{
	std::vector<ProjectileData> projectiles;
};

struct MobComponent
{
public:
	/// <summary>
	/// General config (mob id, atlas, name...)
	/// </summary>
	MobID mobID = 0;
	ResourceID atlas = 0;
	std::string name = "";
	int8_t playerIndex = -1;
	nlohmann::json aabb;

	/// <summary>
	/// Tick rates for the animations.
	/// </summary>
	uint16_t idleTickRate = 10;
	uint16_t runTickRate = 10;

	/// <summary>
	/// Movement-related variables.
	/// </summary>
	float density = 0.0f;
	float horizontalImpulse = 0.0f;
	float maxHorizontalSpeed = 0.0f;
	float dragMultiplier = 0.0f;
	float horizontalMoveDir = 0.0f;
	float jumpHeight = 0.0f;

	bool wantsToJump = false;
	bool wantsToShoot = false;

	/// <summary>
	/// As long as this value is greater than 0, this mob cannot take damage.
	/// </summary>
	int16_t invencibilityTicks = 0;
	int16_t maxInvencibilityTicks = 30;

	/// <summary>
	/// Projectile-related.
	/// </summary>
	ProjectileDirection shootDirection;
	bool readyToShoot = false;
	Timestamp lastShotTime = 0.0;
	std::string projectile = "";
	uint16_t ammo = 0;

	/// <summary>
	/// Health-related.
	/// </summary>
	float maxHealth = 3;
	float health = 3.0f;
	float contactDamage = 0.0f;

	BitBuffer8 CreateEventBitBuffer() const;

	inline Rect2D GetAABB() const { return Rect2D(aabb["x"], aabb["y"], aabb["width"], aabb["height"]); }
	inline bool IsPlayer() const { return playerIndex >= 0; }

	inline void Damage(float dmg)
	{
		assert(dmg >= 0.0f);
		if (invencibilityTicks <= 0)
		{
			health -= dmg;
		}

		// If the mob actually took damage, it should not receive damage again in the subsequent frames
		if (dmg > 0.0f)
		{
			invencibilityTicks = maxInvencibilityTicks;
		}
	}
};


struct SensorComponent
{
	static const int GroundSensor = 0;

	int contactCount[1] = { 0 };

	SensorComponent() {}

	inline bool IsColliding()
	{
		return contactCount[SensorComponent::GroundSensor] > 0;
	}
};

#define MAX_PLAYER_INPUTS 8

struct LocalInputComponent
{
	int inputCodes[MAX_PLAYER_INPUTS];
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobComponent, projectile, ammo, maxInvencibilityTicks, contactDamage, maxHealth, idleTickRate, runTickRate, atlas, health, name, density, horizontalImpulse, maxHorizontalSpeed, dragMultiplier, jumpHeight, aabb)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LocalInputComponent, inputCodes)
}