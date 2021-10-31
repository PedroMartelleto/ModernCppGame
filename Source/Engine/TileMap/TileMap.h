#pragma once

#include "TileMapLayer.h"
#include "Tileset.h"
#include "../Core/Math.h"

class TileMap
{
public:
	TileMap(b2World& physicsWorld, float mapScale, const std::string& fileName, Ref<TextureManager> textureManager);
	~TileMap();

	TileID GetTile(int layer, int x, int y) const;
	int GetWidth() const { return layers[0]->width; }
	int GetHeight() const { return layers[0]->height; }
	float WidthInPixels() const { return (float)layers[0]->width * scaledTileSize.x; }
	float HeightInPixels() const { return (float)layers[0]->height * scaledTileSize.y; }
	Vec2f SizeInPixels() const { return Vec2f((float)layers[0]->width, (float)layers[0]->height) * scaledTileSize; }
public:
	const float mapScale;

	std::vector<Ref<TileMapLayer>> layers;
	Ref<Tileset> tileset;
	
	Vec2f scaledTileSize;

	/// <summary>
	/// One over the scaled tile size.
	/// </summary>
	Vec2f scaledInvTileSize;

	b2Body* body;
};