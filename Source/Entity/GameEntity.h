#pragma once

#include "../Commons.h"

class TextureAtlas;
class Core;
class MapCoords;

enum class CollisionState
{
	Undefined, Falling, Grounded, Jumping, JumpFall
};

class GameEntity : GameObject
{
public:
	static const float jumpFallGravityMultiplier;

	const static float gravity;

	GameEntity(Core* core, TextureAtlas* atlas, Texture2D texture);

	virtual void Create() = 0;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Destroy() = 0;
public:
	const UID uid;

	TextureAtlas* atlas;
	Core* core;
	Rectangle atlasRegion;
	Texture2D texture;
	
	/// <summary>
	/// Render size
	/// </summary>
	Vec2f size;
	
	/// <summary>
	/// Render position
	/// </summary>
	Vec2f position;

	Vec2f collisionOffset;
	Vec2f collisionSize;

	Vec2f velocity;
	Vec2f acceleration;

	bool flipX;
	bool flipY;

	float angle;
	float angularVelocity;
	float angularAcceleration;
	
	/// <summary>
	/// When the entity reaches this velocity during its jump, gravity increases.
	/// </summary>
	float jumpFallVelocity;

	CollisionState collisionState;

	bool checksForMapCollisions;
	bool applyGravity;
private:
	void ResolveMapCollisions();

	Vec2f YAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB);
	Vec2f XAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB);
	Vec2f MinAxisSolve(int xDir, int yDir, const MapCoords& coords);
};

