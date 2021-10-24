#pragma once

#include "../Commons.h"
#include "Tileset.h"

class TileMapLayer
{
public:
	TileMapLayer(const TileID* tiles, int width, int height, const std::string& name, Tileset* tileset);
	~TileMapLayer();

	TileID GetTile(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;

	Tileset* GetTileset();
	void SetTileset(Tileset* tileset);

	void Render(const Vec2f& position, float scale);

	void Destroy();
private:
	Tileset* m_tileset;
	const std::string m_name;
	const TileID* m_tiles;
	const int m_width;
	const int m_height;
};

