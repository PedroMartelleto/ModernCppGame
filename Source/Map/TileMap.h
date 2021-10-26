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
	TileMap(Core* core, float mapScale, int collisionLayerIndex, const std::string& fileName, const std::string& maskPath, TextureManager* textureManager);
	~TileMap();

	MapCoords WorldToMapPos(const Vec2f& worldPos) const;

	unsigned char GetTile(int layer, int x, int y) const;

	bool IsPixelSolid(const MapCoords& mapCoords) const;
		
	/// <summary>
	/// Calculates the corner points of the given rectangle in map coords.
	/// </summary>
	/// <param name="pos">The position of the rectangle.</param>
	/// <param name="size">The size of the rectangle.</param>
	/// <returns>The corners in map coords.</returns>
	std::array<MapCoords, 4> GetRectCorners(const Vec2f& pos, const Vec2f& size) const;

	/// <summary>
	/// Determines whether each point given is overlapping a solid pixel.
	/// </summary>
	/// <param name="pos">The map coords of the points to check.</param>
	/// <returns>Returns an array with a boolean for each corner of the rectangle.</returns>
	std::array<bool, 4> GetOverlaps(const std::array<MapCoords, 4>& corners) const;

	void Destroy();
public:
	const int collisionLayerIndex;
	const float mapScale;

	Core* core;
	std::vector<TileMapLayer*> layers;
	CollisionMask* collisionMask;
	Tileset* tileset;
	
	Vec2f scaledTileSize;

	/// <summary>
	/// One over the scaled tile size.
	/// </summary>
	Vec2f scaledInvTileSize;
};