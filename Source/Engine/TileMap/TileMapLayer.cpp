#include "TileMapLayer.h"
#include "../Render/Render2D.h"

TileMapLayer::TileMapLayer(const TileID* tiles, int width, int height, const std::string& name, Ref<Tileset> tileset) :
	m_tiles(tiles),
	width(width),
	height(height),
	name(name),
	tileset(tileset) {}

TileMapLayer::~TileMapLayer()
{
	if (m_tiles == nullptr) return;
	delete[] m_tiles;
	m_tiles = nullptr;
}

TileID TileMapLayer::GetTile(int x, int y) const
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return 0;
	}

	return m_tiles[x + y * width];
}

void TileMapLayer::Render(const Vec2f& position, float scale, int zIndex)
{
	auto tileSize = tileset->TileSize() * scale;
	auto zero = Vec2f(0, 0);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			auto id = m_tiles[x + y * width];
			if (id > 0)
			{
				auto region = tileset->GetTileRegion(id);
				auto drawPos = position + Vec2f((float)x, (float)y) * tileSize;
				Render2D::DrawRect(drawPos, tileSize, zIndex, region, tileset->texture, Colors::WHITE);
			}
		}
	}
}