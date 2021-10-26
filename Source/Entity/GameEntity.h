#pragma once

#include "../Commons.h"

class TextureAtlas;
class Core;
class MapCoords;

enum CollisionState
{
	Falling, Grounded, Jumping
};

class GameEntity : GameObject
{
public:
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

	Vec2f size;

	Vec2f position;
	Vec2f velocity;
	Vec2f acceleration;

	bool flipX;
	bool flipY;

	float angle;
	float angularVelocity;
	float angularAcceleration;

	bool checksForMapCollisions;
private:
	CollisionState ResolveMapCollisions();

	Vec2f YAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB);
	Vec2f XAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB);
	Vec2f MinAxisSolve(int xDir, int yDir, const MapCoords& coords);
};

