#include "MapCoords.h"
#include "../Core/Core.h"
#include "../Map/TileMap.h"

MapCoords::MapCoords(const Vec2f& tilePos, const Vec2f& inTileOffset)
{
	this->tilePos = tilePos;
	this->inTileOffset = inTileOffset;
}

MapCoords::MapCoords(const MapCoords& other)
{
	tilePos = Vec2i(other.tilePos);
	inTileOffset = Vec2i(other.inTileOffset);
}

void MapCoords::DebugDraw(Core* core, Color color, const Vec2f& size) const
{
	core->DrawDebugRect((tilePos * (float)core->map->tileset->tileWidth + inTileOffset) * core->map->mapScale, Vec2f::one * 4, color);
}

void MapCoords::DebugDrawTile(Core* core, Color color) const
{
	core->DrawDebugRect((tilePos * (float)core->map->tileset->tileWidth) * core->map->mapScale, core->map->scaledTileSize, ColorAlpha(color, 0.3f));
}

MapCoords& MapCoords::operator=(const MapCoords& other)
{
	tilePos = Vec2i(other.tilePos);
	inTileOffset = Vec2i(other.inTileOffset);
	return *this;
}

bool MapCoords::operator==(const MapCoords& other) const
{
	return tilePos == other.tilePos && inTileOffset == other.inTileOffset;
}