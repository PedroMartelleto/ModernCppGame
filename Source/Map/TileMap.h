#pragma once

#include "../Core/GameObject.h"
#include "TileMapLayer.h"
#include "Tileset.h"
#include "MapCoords.h"

class TextureManager;
class Core;
class CollisionMask;

class TileMap
{
public:
	TileMap(Core* core, float mapScale, const std::string& fileName, TextureManager* textureManager);
	~TileMap();

	unsigned char GetTile(int layer, int x, int y) const;

	void Destroy();
public:
	const float mapScale;

	Core* core;
	std::vector<TileMapLayer*> layers;
	Tileset* tileset;
	
	Vec2f scaledTileSize;

	/// <summary>
	/// One over the scaled tile size.
	/// </summary>
	Vec2f scaledInvTileSize;

	b2Body* body;
};