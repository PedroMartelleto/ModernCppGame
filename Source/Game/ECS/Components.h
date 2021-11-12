#pragma once

#include "../../Engine/Engine.h"
#include "../Networking/BitBuffer.h"
#include "../Physics/Projectile.h"

// We are using ECS, so we need to define components: minimal pieces of data used in the game

struct PhysicsBodyComponent
{
	b2Body* body;

	PhysicsBodyComponent() = default;
	PhysicsBodyComponent(b2Body* body) : body(body) {}

	inline operator const b2Body* () const { return body; }
	inline operator b2Body* () { return body; }
};

struct TextureRegionComponent
{
	Vec2f texPos;
	Vec2f texSize;
	bool allowFlip;

	TextureRegionComponent(const Vec2f& texPos, const Vec2f& texSize) : texPos(texPos), texSize(texSize), allowFlip(true) {}
	TextureRegionComponent(const Rect2D& rect) : texPos(rect.pos()), texSize(rect.size()), allowFlip(true) {}

	inline operator Rect2D () const { return Rect2D(texPos, texSize); }
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

struct MovementComponent
{
	float impulse;
	float maxSpeed;
	Vec2f axis;

	MovementComponent(const Vec2f& axis, float horizontalDir, float impulse, float maxSpeed) : axis(axis), impulse(impulse), maxSpeed(maxSpeed) {}
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

struct SensorComponent
{
	static const int GroundSensor = 0;

	int contactCount[1] = { 0 };

	SensorComponent() {}
	
	inline bool IsGrounded()
	{
		return contactCount[SensorComponent::GroundSensor] > 0;
	}
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
	std::string name = "";

	float density = 0.0f;
	float horizontalImpulse = 0.0f;
	float maxHorizontalSpeed = 0.0f;
	float horizontalDragForce = 0.0f;
	float horizontalMoveDir = 0.0f;
	float jumpHeight = 0.0f;

	ProjectileDirection shootDirection;
	
	bool isPlayer = false;
	bool wantsToJump = false;
	bool wantsToShoot = false;
	bool readyToShoot = false;
	double lastShotTime = 0.0;
	nlohmann::json aabb;

	BitBuffer8 CreateEventBitBuffer() const;

	inline Rect2D GetAABB() const { return Rect2D(aabb["x"], aabb["y"], aabb["width"], aabb["height"]); }
};

#define MAX_PLAYER_INPUTS 8

struct LocalInputComponent
{
	int inputCodes[MAX_PLAYER_INPUTS];
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobComponent, isPlayer, name, density, horizontalImpulse, maxHorizontalSpeed, horizontalDragForce, jumpHeight, aabb)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LocalInputComponent, inputCodes)
}