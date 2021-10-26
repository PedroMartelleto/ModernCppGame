#pragma once

#include "../Commons.h"

class Core;

/// <summary>
/// Position in the world relative to a tile.
/// </summary>
class MapCoords
{
public:
	MapCoords(const Vec2f& tilePos, const Vec2f& inTileOffset);
	MapCoords(const MapCoords& other);

	void DEBUG_Draw(Core* core, Color color, const Vec2f& size = Vec2f(4, 4)) const;
	void DEBUG_DrawTile(Core* core, Color color) const;

	MapCoords& operator=(const MapCoords& other);
	bool operator==(const MapCoords& other) const;
public:
	/// Tile position in the world (whole number).
	Vec2f tilePos;
	/// Pixel offset inside the tile (whole number).
	Vec2f inTileOffset;
};

