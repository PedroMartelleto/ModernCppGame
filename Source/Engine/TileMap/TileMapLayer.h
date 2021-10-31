#pragma once

#include "../Commons.h"
#include "Tileset.h"

class TileMapLayer
{
public:
	TileMapLayer(const TileID* tiles, int width, int height, const std::string& name, Ref<Tileset> tileset);
	~TileMapLayer();

	TileID GetTile(int x, int y) const;

	void Render(const Vec2f& position, float scale, int zIndex);
public:
	const std::string name;
	Ref<Tileset> tileset;
	const int width;
	const int height;
private:
	const TileID* m_tiles;
};

