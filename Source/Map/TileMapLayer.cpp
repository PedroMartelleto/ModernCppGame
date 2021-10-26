#include "TileMapLayer.h"

TileMapLayer::TileMapLayer(const TileID* tiles, int width, int height, const std::string& name, Tileset* tileset) :
	m_tiles(tiles),
	m_width(width),
	m_height(height),
	m_name(name),
	m_tileset(tileset)
{
}

TileMapLayer::~TileMapLayer()
{
	Destroy();
}

void TileMapLayer::Destroy()
{
	if (m_tiles == nullptr) return;
	delete[] m_tiles;
	m_tiles = nullptr;
}

TileID TileMapLayer::GetTile(int x, int y) const
{
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
	{
		return 0;
	}

	return m_tiles[x + y * m_width];
}

Tileset* TileMapLayer::GetTileset()
{
	return m_tileset;
}

void TileMapLayer::SetTileset(Tileset* tileset)
{
	m_tileset = tileset;
}

void TileMapLayer::Render(const Vec2f& position, float scale)
{
	auto tileSize = m_tileset->TileSize() * scale;
	auto zero = Vec2f(0, 0).raylib();

	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			auto id = m_tiles[x + y * m_width];
			if (id > 0)
			{
				auto region = m_tileset->GetTileRegion(id);
				auto dest = CreateRectangle(position + Vec2f(x, y).Multiply(tileSize), tileSize);
				DrawTexturePro(m_tileset->texture, region, dest, zero, 0.0f, WHITE);
			}
		}
	}
}

int TileMapLayer::GetWidth() const
{
	return m_width;
}

int TileMapLayer::GetHeight() const
{
	return m_height;
}