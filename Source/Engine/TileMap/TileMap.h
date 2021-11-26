#pragma once

#include "TileMapLayer.h"
#include "Tileset.h"
#include "../Core/Math.h"
#include <tinyxml2.h>

class TileMap
{
public:
	TileMap(b2World& physicsWorld, float mapScale, const std::string& mapXMLData, const Ref<ResourceManager>& resourceManager);
	~TileMap();

	Vec2f GetSpawn() const;

	/// <summary>
	/// Randomly picks spawn positions (without replacement) and returns a list of these positions.
	/// If count is greater than the number of spawns in the map, the returned spawns will have repetitions.
	/// </summary>
	/// <param name="count">Number of spawn positions to pick.</param>
	/// <returns></returns>
	std::vector<Vec2f> GetSpawns(int count) const;

	TileID GetTile(int layer, int x, int y) const;
	int GetWidth() const { return layers[0]->width; }
	int GetHeight() const { return layers[0]->height; }
	float WidthInPixels() const { return (float)layers[0]->width * scaledTileSize.x; }
	float HeightInPixels() const { return (float)layers[0]->height * scaledTileSize.y; }
	Vec2f SizeInPixels() const { return Vec2f((float)layers[0]->width, (float)layers[0]->height) * scaledTileSize; }
public:
	const float mapScale;

	std::vector<Vec2f> spawns;
	std::vector<Ref<TileMapLayer>> layers;
	Ref<Tileset> tileset;
	
	Vec2f scaledTileSize;

	/// <summary>
	/// One over the scaled tile size.
	/// </summary>
	Vec2f scaledInvTileSize;

	b2Body* body;
};