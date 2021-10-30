#pragma once

#include "TileMapLayer.h"
#include "Tileset.h"

class TextureManager;
class Core;
class CollisionMask;

class TileMap
{
public:
	TileMap(b2World& physicsWorld, float mapScale, const std::string& fileName, TextureManager* textureManager);
	~TileMap();

	TileID GetTile(int layer, int x, int y) const;

	void Destroy();
public:
	const float mapScale;

	std::vector<TileMapLayer*> layers;
	Tileset* tileset;
	
	Vec2f scaledTileSize;

	/// <summary>
	/// One over the scaled tile size.
	/// </summary>
	Vec2f scaledInvTileSize;

	b2Body* body;
};