#pragma once

#include "../../Engine/Engine.h"
#include "../Networking/BitBuffer.h"

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
	TextureRegionComponent(const Rect2D& rect) : texPos(rect.pos), texSize(rect.size), allowFlip(true) {}

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

struct GroundDetectionComponent
{
	bool isGrounded = false;

	GroundDetectionComponent() {}
};

struct MobComponent
{
	static uint16_t globalMobID;

	std::string name = "";
	uint16_t mobID = 0;
	float horizontalImpulse = 0.0f;
	float maxHorizontalSpeed = 0.0f;
	float horizontalDragForce = 0.0f;
	float jumpHeight = 0.0f;
	float horizontalMoveDir = 0.0f;
	float density = 0.0f;
	bool wantsToJump = 0.0f;

	MobComponent() = default;

	MobComponent(const std::string& name, float horizontalImpulse, float maxHorizontalSpeed, float horizontalDragForce, float jumpHeight, float density) :
		name(name),
		horizontalImpulse(horizontalImpulse),
		maxHorizontalSpeed(maxHorizontalSpeed),
		horizontalDragForce(horizontalDragForce),
		jumpHeight(jumpHeight),
		horizontalMoveDir(0.0f),
		wantsToJump(false),
		density(density)
	{
		MobComponent::globalMobID += 1;
		mobID = MobComponent::globalMobID;
	}
	
	NetworkBuffer CreateEventPacket() const;
};

struct PlayerInputRequest
{
	int playerID;
	BitBuffer8 inputs;
};

#define MAX_PLAYER_INPUTS 8

struct PlayerInputComponent
{
	int inputCodes[MAX_PLAYER_INPUTS];
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobComponent, name, horizontalImpulse, maxHorizontalSpeed, horizontalDragForce, jumpHeight, density)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerInputComponent, inputCodes)
}