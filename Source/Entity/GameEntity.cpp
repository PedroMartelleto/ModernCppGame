#include "GameEntity.h"
#include "../Core/Core.h"
#include "../Render/TextureAtlas.h"
#include "../Map/TileMap.h"
#include "../Physics/CollisionMask.h"
#include <assert.h>

const float GameEntity::gravity = 9.8f * 100.0f;

GameEntity::GameEntity(Core* core, TextureAtlas* atlas, Texture2D texture) :
	uid(core->CreateUID()),
	checksForMapCollisions(false),
	flipX(false),
	flipY(false)
{
	this->atlas = atlas;
	this->core = core;
	this->texture = texture;
	this->position = Vec2f::zero;
	this->velocity = Vec2f::zero;
	this->acceleration = Vec2f::zero;
	this->size = Vec2f::one * 16.0f;
	this->atlasRegion = CreateRectangle(0, 0, 16, 16);
	this->angle = 0.0f;
	this->angularVelocity = 0.0f;
	this->angularAcceleration = 0.0f;
}

void GameEntity::Update(float deltaTime)
{
	position += velocity * deltaTime + acceleration * deltaTime * deltaTime / 2;
	velocity += acceleration * deltaTime;

	if (checksForMapCollisions)
	{
		auto collisionState = ResolveMapCollisions();
	}

	angle += angularVelocity * deltaTime + angularAcceleration * deltaTime * deltaTime / 2;
	angularVelocity += angularAcceleration * deltaTime;
}

void GameEntity::Render()
{
	auto renderDest = CreateRectangle(position.x, position.y, size.x, size.y);
	
	atlasRegion.width = flipX ? -fabsf(atlasRegion.width) : fabsf(atlasRegion.width);
	atlasRegion.height = flipY ? -fabsf(atlasRegion.height) : fabsf(atlasRegion.height);

	DrawTexturePro(texture, atlasRegion, renderDest, Vec2f(0, 0).raylib(), angle, WHITE);
}

const int TOP_LEFT = 0;
const int TOP_RIGHT = 1;
const int BOTTOM_LEFT = 2;
const int BOTTOM_RIGHT = 3;

Vec2f GameEntity::YAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB)
{
	auto map = core->map;

#ifdef _DEBUG
	assert(coordsA.inTileOffset.y == coordsB.inTileOffset.y);
	assert(dir == 1 || dir == -1);
	assert(map->IsPixelSolid(coordsA) && map->IsPixelSolid(coordsB));
#endif

	velocity.y = 0.0f;
	// At this point, coordsA.inTileOffset.y and coordsB.inTileOffset.y should be equal!
	int y = coordsA.inTileOffset.y;
	bool foundResolveOffset = false;

	while (!foundResolveOffset && y >= 0 && y < map->tileset->tileHeight)
	{
		auto solidA = map->IsPixelSolid(MapCoords(coordsA.tilePos, Vec2i(coordsA.inTileOffset.x, y)));
		auto solidB = map->IsPixelSolid(MapCoords(coordsB.tilePos, Vec2i(coordsB.inTileOffset.x, y)));
		
		if (!solidA && !solidB)
		{
			foundResolveOffset = true;
		}
		else
		{
			y += dir;
		}
	}

	// TODO: Visual debug this
	LOGGER_VAR(y);
	LOGGER_VAR(coordsA.inTileOffset.y - (float)y);

	return Vec2f(0, ((float)y - coordsA.inTileOffset.y) * map->mapScale);
}

Vec2f GameEntity::XAxisSolve(int dir, const MapCoords& coordsA, const MapCoords& coordsB)
{
	//velocity.x = 0.0f;
	return Vec2f::zero;
}

Vec2f GameEntity::MinAxisSolve(int xDir, int yDir, const MapCoords& coords)
{
	if (coords.inTileOffset.x < coords.inTileOffset.y)
	{
		return XAxisSolve(xDir, coords, coords);
	}
	else
	{
		return YAxisSolve(yDir, coords, coords);
	}
}

CollisionState GameEntity::ResolveMapCollisions()
{
	auto map = core->map;
	if (map == nullptr)
	{
		return CollisionState::Grounded;
	}

	// Collision resolving = offseting the entity so that it is no longer colliding after this function.
	int maxResolutions = 2;
	auto corners = map->GetRectCorners(position, size);
	auto solid = map->GetOverlaps(corners); // True if the corner is overlapping a solid pixel
	bool isColliding = solid[0] || solid[1] || solid[2] || solid[3];

	while (isColliding && maxResolutions > 0)
	{
		// If the top edge is colliding, we solve by moving the entity down...
		// All other "edge collision" cases are analagous.
		// In the case of only ONE corner colliding, or two opposite corners colliding, we solve each
		// individually by offseting the smallest axis.
		if (solid[TOP_LEFT] && solid[TOP_RIGHT])
		{
			position += YAxisSolve(1, corners[TOP_LEFT], corners[TOP_RIGHT]);
		}
		else if (solid[BOTTOM_LEFT] && solid[BOTTOM_RIGHT])
		{
			position += YAxisSolve(-1, corners[BOTTOM_LEFT], corners[BOTTOM_RIGHT]);
		}
		else if (solid[TOP_LEFT] && solid[BOTTOM_LEFT])
		{
			position += XAxisSolve(1, corners[TOP_LEFT], corners[BOTTOM_LEFT]);
		}
		else if (solid[TOP_RIGHT] && solid[BOTTOM_RIGHT])
		{
			position += XAxisSolve(-1, corners[TOP_RIGHT], corners[BOTTOM_RIGHT]);
		}
		else if (solid[TOP_LEFT])
		{
			position += MinAxisSolve(1, -1, corners[TOP_LEFT]);
		}
		else if (solid[TOP_RIGHT])
		{
			position += MinAxisSolve(-1, -1, corners[TOP_RIGHT]);
		}
		else if (solid[BOTTOM_LEFT])
		{
			position += MinAxisSolve(1, 1, corners[BOTTOM_LEFT]);
		}
		else if (solid[BOTTOM_RIGHT])
		{
			position += MinAxisSolve(-1, 1, corners[BOTTOM_RIGHT]);
		}

		maxResolutions -= 1;

		// If there are resolutions remaining...
		if (maxResolutions > 0)
		{
			corners = map->GetRectCorners(position, size);
			solid = map->GetOverlaps(corners);
			isColliding = solid[0] || solid[1] || solid[2] || solid[3];
		}
	}

	if (velocity.y < 0.0f)
	{
		return CollisionState::Jumping;
	}
	else if (velocity.y > 0.0f)
	{
		return CollisionState::Falling;
	}
	else
	{
		return CollisionState::Grounded;
	}
}