#pragma once

#include "../../Commons.h"

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
	TextureRegionComponent(const Rectangle& rect) : texPos(rect.x, rect.y), texSize(rect.width, rect.height), allowFlip(true) {}

	inline operator Rectangle () const { return CreateRectangle(texPos, texSize); }
};

struct SpriteComponent
{
	Texture2D texture;
	int zIndex;
	Vec2f pos = Vec2f::zero;
	Vec2f size = Vec2f::zero;
	Color tint = WHITE;
	float scale = 1.0f;

	SpriteComponent(const Texture2D& texture, int zIndex, const Vec2f& pos, const Vec2f& size, const Color& tint = WHITE, float scale = 1.0f) :
		texture(texture), pos(pos), size(size), tint(tint), zIndex(zIndex), scale(scale) {}
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
	Color aabbColor;
	Color polyColor;

	DEBUG_PhysicsBodyDrawComponent(bool drawAABB, bool drawPoly, const Color& aabbColor = ColorAlpha(GREEN, 0.3f), const Color& polyColor = BLACK) :
		drawAABB(drawAABB), drawPoly(drawPoly), aabbColor(aabbColor), polyColor(polyColor) {}
};

struct GroundDetectionComponent
{
	bool isGrounded = false;

	GroundDetectionComponent() {}
};

struct MobComponent
{
	std::string name = "";
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
		density(density) {}
};

struct PlayerInputComponent
{
	int JUMP;
	int MOVE_RIGHT;
	int MOVE_LEFT;
};

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MobComponent, name, horizontalImpulse, maxHorizontalSpeed, horizontalDragForce, jumpHeight, density)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerInputComponent, JUMP, MOVE_RIGHT, MOVE_LEFT)
}