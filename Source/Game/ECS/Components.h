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

	AnimationComponent(Ref<TextureAtlas> atlas, const std::string& regionName, bool allowFlip = true) :
		atlas(atlas), regionName(regionName), allowFlip(allowFlip) {}

	inline Rect2D GetRect() const
	{
		return atlas->GetAnimFrameRegion(regionName, frame);
	}
};

struct SpriteComponent
{
	Ref<Texture2D> texture;
	int zIndex;
	Vec2f pos = Vec2f(0, 0);
	Vec2f size = Vec2f(0, 0);
	Color4f tint = Colors::WHITE;

	SpriteComponent(Ref<Texture2D> texture, int zIndex, const Vec2f& pos, const Vec2f& size, const Color4f& tint = Colors::WHITE) :
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
	MobID mobID = 0;
	ResourceID atlas = 0;
	std::string name = "";

	uint16_t idleTickRate = 10;
	uint16_t runTickRate = 10;

	float density = 0.0f;
	float horizontalImpulse = 0.0f;
	float maxHorizontalSpeed = 0.0f;
	float dragMultiplier = 0.0f;
	float horizontalMoveDir = 0.0f;
	float jumpHeight = 0.0f;

	ProjectileDirection shootDirection;
	
	float health = 3.0f;
	bool isPlayer = false;
	bool wantsToJump = false;
	bool wantsToShoot = false;
	bool readyToShoot = false;
	double lastShotTime = 0.0;
	nlohmann::json aabb;

	BitBuffer8 CreateEventBitBuffer() const;

	inline Rect2D GetAABB() const { return Rect2D(aabb["x"], aabb["y"], aabb["width"], aabb["height"]); }
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
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobComponent, idleTickRate, runTickRate, atlas, health, isPlayer, name, density, horizontalImpulse, maxHorizontalSpeed, dragMultiplier, jumpHeight, aabb)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LocalInputComponent, inputCodes)
}