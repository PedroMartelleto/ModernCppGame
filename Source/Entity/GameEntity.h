#pragma once

#include "../Commons.h"
#include "../Core/Core.h"
#include "../Map/TileMap.h"
#include "../Render/TextureAtlas.h"

class Core;

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

	void CreateBody(b2BodyDef* bodyDef, b2FixtureDef* fixtureDef);

	virtual void Create() {};
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Destroy() = 0;
public:
	const UID uid;

	TextureAtlas* atlas;
	Core* core;
	Rectangle atlasRegion;
	Texture2D texture;
	
	b2Body* body;
	Vec2f drawOffset;

	bool flipX;
	bool flipY;
	
	/// <summary>
	/// Set this to true to make this entity show up on the other side of the map when it leaves the bounds of the screen.
	/// </summary>
	bool loopsInScreen;

	CollisionState collisionState;
protected:
	Vec2f drawSize;
};

